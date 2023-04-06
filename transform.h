#pragma once

#include <complex>

class state;

class transform {
private:
	unsigned int no_qubits;
	std::vector <std::vector <std::complex <float>>> matrix;
	float norm_factor;

public:
	explicit transform(int get_qubits, char gate);
	explicit transform(char gate);

	friend state operator*(const transform &modify, const state &ini);

	void operator*=(const transform &next);
	friend transform operator*(const transform &first, const transform &second);

	friend transform operator|(const transform &first, const transform &second);
};
