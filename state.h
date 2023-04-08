#pragma once

#include <random>
#include <complex>

#include "transform.h"

class transform;

template <unsigned int no_qubits>
class state;

template <unsigned int no_qubits>
state <no_qubits> operator*(const transform &modify, const state <no_qubits>&ini);

/*
 * State vector class. Saves a situation of the entire quantum circuit. Operations can be applied upon it, and qubits
 * can be measured.
 */
template <unsigned int no_qubits>
class state {
private:
	std::vector <std::complex <double>> state_vector;
	double norm_factor;

	int get_random_state();
public:
	explicit state();
	unsigned int size() const;

	/*
	 * returns the current state vector as a complex vector of size 2 ^ no_qubits
	 */
	std::vector <std::complex <double>> getState() const;

	/*
	 * Measures one or more qubits. Returns the reading and modifies the state. Uses read_random_state as an intermediary
	 */
	bool measure(unsigned int id);
	std::vector <bool> measure(std::vector <unsigned int> ids);

	/*
	 * Here the '*' operator multiplies a state vector by a transformation matrix
	 */
	void operator*=(const transform &modify);
	friend state <no_qubits> operator* <>(const transform &modify, const state <no_qubits>&ini);
};

template <unsigned int no_qubits>
state <no_qubits>::state() : state_vector(1 << no_qubits, 0) {
	norm_factor = 1;
	state_vector[0] = 1;
}
template <unsigned int no_qubits>
unsigned int state <no_qubits>::size() const {
	return no_qubits;
}

template <unsigned int no_qubits>
std::vector <std::complex <double>> state <no_qubits>::getState() const {
	std::vector <std::complex <double>> ans;
	double sqrt_norm = std::sqrt(norm_factor);
	for(std::complex <double> val : state_vector) {
		ans.push_back(val / sqrt_norm);
	}
	return ans;
}

template <unsigned int no_qubits>
int state <no_qubits>::get_random_state () {
	std::random_device rand_device;
	std::default_random_engine rand_generator(rand_device());
	std::uniform_real_distribution<double> distribution(0, norm_factor);
	double chosen_num = distribution(rand_generator);
	int chosen_state = 0;
	while(chosen_state < (1 << no_qubits)) {
		chosen_num -= std::abs(state_vector[chosen_state] * state_vector[chosen_state]);
		if(chosen_num < 0) {
			break;
		}
		chosen_state++;
	}
	return chosen_state;
}
template <unsigned int no_qubits>
bool state <no_qubits>::measure (unsigned int id) {
	int chosen_state = get_random_state();
	int read_mask = 1 << id;
	int rez_mask = chosen_state & read_mask;
	norm_factor = 0;
	for (int mask = 0; mask < (1 << no_qubits); mask++) {
		if((mask & read_mask) == rez_mask) {
			norm_factor += std::abs(state_vector[mask] * state_vector[mask]);
		}
		else {
			state_vector[mask] = 0;
		}
	}
	return chosen_state & (1 << id);
}
template <unsigned int no_qubits>
std::vector <bool> state <no_qubits>::measure(std::vector <unsigned int> ids) {
	int chosen_state = get_random_state();
	int read_mask = 0;
	for(unsigned int value : ids) {
		read_mask |= 1 << value;
	}
	int rez_mask =  chosen_state & read_mask;
	norm_factor = 0;
	for (int mask = 0; mask < (1 << no_qubits); mask++) {
		if((mask & read_mask) == rez_mask) {
			norm_factor += std::abs(state_vector[mask] * state_vector[mask]);
		}
		else {
			state_vector[mask] = 0;
		}
	}
	std::vector <bool> ans(ids.size());
	for(int ind = 0; ind < ids.size(); ind++) {
		ans[ind] = chosen_state & (1 << ids[ind]);
	}
	return ans;
}

template <unsigned int no_qubits>
void state <no_qubits>::operator*=(const transform &modify) {
	*this = modify * (*this);
}
template <unsigned int no_qubits>
state <no_qubits> operator*(const transform &modify, const state <no_qubits> &ini) {
	state <no_qubits> fin;
	if(modify.no_qubits != no_qubits) {
		throw std::runtime_error("Cannot multiply a state vector and a transformation matrix of different sizes!\n");
	}
	fin.state_vector[0] = 0;
	for(int mask = 0; mask < ini.state_vector.size(); mask++) {
		for(int mask2 = 0; mask2 < ini.state_vector.size(); mask2++) {
			fin.state_vector[mask] += modify.matrix[mask][mask2] * ini.state_vector[mask2];
		}
	}
	fin.norm_factor = ini.norm_factor * modify.norm_factor;
	return fin;
}