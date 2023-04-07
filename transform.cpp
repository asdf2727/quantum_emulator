#include <vector>
#include <cmath>
#include <complex>

#include "all.h"

using namespace std::complex_literals;

transform::transform (char gate) : matrix(2)  {
	no_qubits = 1;
	norm_factor = 1;
	matrix[0].resize(2);
	matrix[1].resize(2);
	switch (gate) {
	case gateI:
		matrix[0][0] = 1;
		matrix[1][1] = 1;
		break;
	case gateX:
		matrix[0][1] = 1;
		matrix[1][0] = 1;
		break;
	case gateY:
		matrix[0][1] = -1i;
		matrix[1][0] = 1i;
		break;
	case gateZ:
		matrix[0][0] = 1;
		matrix[1][1] = -1;
		break;
	case gateH:
		norm_factor = 2;
		matrix[0][0] = 1;
		matrix[0][1] = 1;
		matrix[1][0] = 1;
		matrix[1][1] = -1;
		break;
	case gate0:
		break;
	default:
		throw std::runtime_error("Gate not recognised!\n");
	}
}
transform::transform (char gate, float phase) : matrix(2) {
	no_qubits = 1;
	norm_factor = 1;
	phase /= 2;
	matrix[0].resize(2);
	matrix[1].resize(2);
	switch (gate) {
	case gateRX:
		matrix[0][0] = std::cos(phase);
		matrix[0][1] = -1if * std::sin(phase);
		matrix[1][0] = -1if * std::sin(phase);
		matrix[1][1] = std::cos(phase);
		break;
	case gateRY:
		matrix[0][0] = std::cos(phase);
		matrix[0][1] = -std::sin(phase);
		matrix[1][0] = std::sin(phase);
		matrix[1][1] = std::cos(phase);
		break;
	case gateRZ:
		matrix[0][0] = std::cos(phase) - 1if * std::sin(phase);
		matrix[0][1] = 0;
		matrix[1][0] = 0;
		matrix[1][1] = std::cos(phase) + 1if * std::sin(phase);
		break;
	default:
		throw std::runtime_error("Gate not recognised!\n");
	}
}

transform::transform(char gate, unsigned int size) : matrix(1 << size) {
	no_qubits = size;
	norm_factor = 1;
	switch (gate) {
	case gateI:
		for (int mask = 0; mask < (1 << no_qubits); mask++) {
			matrix[mask].resize(1 << no_qubits, 0);
			matrix[mask][mask] = 1;
		}
	case gate0:
		for (int mask = 0; mask < (1 << no_qubits); mask++) {
			matrix[mask].resize(1 << no_qubits, 0);
		}
		break;
	default:
		throw std::runtime_error("Gate not recognised!\n");
	}
}

void transform::setControlGates (char gate, unsigned int ctl_mask, unsigned int mod_mask) {
	int tot_mask = ctl_mask | mod_mask;
	if (ctl_mask & mod_mask) {
		throw std::runtime_error("A qubit cannot be both a control and a target one!\n");
	}
	transform *base;
	switch (gate) {
	case gateCX:
		base = new transform(gateX);
		break;
	case gateCY:
		base = new transform(gateY);
		break;
	case gateCZ:
		base = new transform(gateZ);
		break;
	default:
		throw std::runtime_error("Gate not recognised!\n");
	}
	for(int mask = 0; mask < (1 << no_qubits); mask++) {
		matrix[mask].resize(1 << no_qubits, 0);
		if ((mask & ctl_mask) == ctl_mask) {
			if (mask & mod_mask) {
				matrix[mask][mask] = base->matrix[1][1];
				matrix[mask][mask ^ mod_mask] = base->matrix[1][0];
			}
			else {
				matrix[mask][mask ^ mod_mask] = base->matrix[0][1];
				matrix[mask][mask] = base->matrix[0][0];
			}
		}
		else {
			matrix[mask][mask] = 1;
		}
	}
}
transform::transform (char gate, unsigned int size, unsigned int target, unsigned int ctl) : matrix(1 << size) {
	no_qubits = size;
	norm_factor = 1;
	int ctl_mask = 1 << ctl, mod_mask = 1 << target;
	setControlGates(gate, ctl_mask, mod_mask);
}
transform::transform (char gate, unsigned int size, unsigned int target, std::vector <unsigned int> ctl) : matrix(1 << size) {
	no_qubits = size;
	norm_factor = 1;
	int ctl_mask = 0, mod_mask = 1 << target;
	for (int pos : ctl) {
		ctl_mask |= 1 << pos;
	}
	setControlGates(gate, ctl_mask, mod_mask);
}

void transform::operator*=(const transform &next) {
	*this = next * (*this);
}
transform operator*(const transform &lhs, const transform &rhs) {
	if(lhs.no_qubits != rhs.no_qubits) {
		throw std::runtime_error("Cannot multiply 2 transformation matrices of different sizes!\n");
	}
	transform ans(gate0, lhs.no_qubits);
	ans.norm_factor = lhs.norm_factor * rhs.norm_factor;
	for(int mask1 = 0; mask1 < lhs.matrix.size(); mask1++) {
		for(int mask2 = 0; mask2 < lhs.matrix.size(); mask2++) {
			for(int mask3 = 0; mask3 < lhs.matrix.size(); mask3++) {
				ans.matrix[mask1][mask2] += lhs.matrix[mask1][mask3] * rhs.matrix[mask3][mask2];
			}
		}
	}
	return ans;
}

void transform::operator|=(const transform &next) {
	*this = next | (*this);
}
transform operator| (const transform &lhs, const transform &rhs) {
	transform ans(gate0, rhs.no_qubits + lhs.no_qubits);
	ans.norm_factor = rhs.norm_factor * lhs.norm_factor;
	for(int mask1 = 0; mask1 < ans.matrix.size(); mask1++) {
		for(int mask2 = 0; mask2 < ans.matrix.size(); mask2++) {
			ans.matrix[mask1][mask2] = rhs.matrix[mask1 & ((1 << rhs.no_qubits) - 1)][mask2 & ((1 << rhs.no_qubits) - 1)] * lhs.matrix[mask1 >> rhs.no_qubits][mask2 >> rhs.no_qubits];
		}
	}
	return ans;
}