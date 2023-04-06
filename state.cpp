#include <vector>
#include <random>
#include <cmath>
#include <complex>

#include "all.h"

state::state(unsigned int get_qubits) : state_vector(1 << get_qubits, 0) {
	no_qubits = get_qubits;
	norm_factor = 1;
	state_vector[0] = 1;
}
state::state(std::complex <float> alpha, std::complex <float> beta) : state_vector(2) {
	state_vector[0] = alpha;
	state_vector[1] = beta;
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
	std::default_random_engine rand_generator;
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
			norm_factor += std::abs(state_vector[chosen_state] * state_vector[chosen_state]);
		}
		else {
			state_vector[mask] = 0;
		}
	}
	return rez_bit;
}