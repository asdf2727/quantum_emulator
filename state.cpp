#include <vector>
#include <random>
#include <cmath>
#include <complex>

#include "all.h"

state::state(unsigned int size) : state_vector(1 << size, 0) {
	no_qubits = size;
	norm_factor = 1;
	state_vector[0] = 1;
}
unsigned int state::size() const {
	return no_qubits;
}

std::vector <std::complex <float>> state::getState() const {
	std::vector <std::complex <float>> ans;
	float sqrt_norm = std::sqrt(norm_factor);
	for(std::complex <float> val : state_vector) {
		ans.push_back(val / sqrt_norm);
	}
	return ans;
}

bool state::measure(int id) {
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
	bool rez_bit = (1 << id) & chosen_state;
	norm_factor = 0;
	for (int mask = 0; mask < (1 << no_qubits); mask++) {
		if(((mask & (1 << id)) == 0) ^ rez_bit) {
			norm_factor += std::abs(state_vector[mask] * state_vector[mask]);
		}
		else {
			state_vector[mask] = 0;
		}
	}
	return rez_bit;
}

void state::operator*=(const transform &modify) {
	*this = modify * (*this);
}
state operator*(const transform &modify, const state &ini) {
	state fin(ini.no_qubits);
	if(modify.no_qubits != ini.no_qubits) {
		throw std::runtime_error("Cannot multiply a state vector and a transformation matrix of different sizes!\n");
	}
	fin.state_vector[0] = 0;
	for(int mask = 0; mask < ini.state_vector.size(); mask++) {
		for(int mask2 = 0; mask2 < ini.state_vector.size(); mask2++) {
			fin.state_vector[mask] += ini.state_vector[mask2] * modify.matrix[mask][mask2];
		}
	}
	fin.norm_factor = ini.norm_factor * modify.norm_factor;
	return fin;
}