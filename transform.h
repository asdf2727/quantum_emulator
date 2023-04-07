#pragma once

#include <complex>

class state;

class transform {
private:
	unsigned int no_qubits;
	std::vector <std::vector <std::complex <float>>> matrix;
	float norm_factor;

	void setControlGates(char id, unsigned int ctl_mask, unsigned int mod_mask);
public:
	explicit transform(char gate);
	explicit transform(char gate, float phase);
	explicit transform(char gate, unsigned int size);
	explicit transform(char gate, unsigned int size, unsigned int targt, unsigned int ctl);
	explicit transform(char gate, unsigned int size, unsigned int targt, std::vector <unsigned int> ctls);

	friend state operator*(const transform &modify, const state &ini);

	void operator*=(const transform &next);
	friend transform operator*(const transform &first, const transform &second);

	void operator|=(const transform &next);
	friend transform operator|(const transform &first, const transform &second);
};
