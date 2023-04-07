#pragma once

#include <complex>

class transform;

class state {
private:
	unsigned int no_qubits;
	std::vector <std::complex <float>> state_vector;
	float norm_factor;

public:
	explicit state(unsigned int size);
	unsigned int size() const;

	std::vector <std::complex <float>> getState() const;

	bool measure(int id);

	void operator*=(const transform &modify);
	friend state operator*(const transform &modify, const state &ini);
};