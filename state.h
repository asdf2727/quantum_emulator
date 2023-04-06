#pragma once

#include <complex>

class transform;

class state {
private:
	unsigned int no_qubits;
	std::vector <std::complex <float>> state_vector;
	float norm_factor;

public:
	state(unsigned int get_qubits);
	state(std::complex <float> alpha, std::complex <float> beta);
	unsigned int size() const;

	std::vector <std::complex <float>> getState() const;

	bool measure(int id);

	friend state operator*(const transform &modify, const state &ini);
};