#include <vector>
#include <complex>

#include "all.h"

using namespace std::complex_literals;

transform::transform (unsigned int get_qubits, char gate) {
	no_qubits = get_qubits;
	norm_factor = 1;
	matrix.emplace_back(1 << no_qubits);
	if (gate == '-') {
		for(int mask = 0; mask < (1 << no_qubits); mask++) {
			matrix[mask].emplace_back(1 << no_qubits, 0);
			matrix[mask][mask] = 1;
		}
	}
	if (gate == 'X') {
		for(int mask = 0; mask < (1 << no_qubits); mask++) {
			matrix.emplace_back(1 << no_qubits, 0);
			matrix[mask][mask ^ ((mask & 1) << (no_qubits - 1))] = 1;
		}
	}
	if (gate == 'x') {
		for(int mask = 0; mask < (1 << no_qubits); mask++) {
			matrix.emplace_back(1 << no_qubits, 0);
			matrix[mask][mask ^ ((mask & (1 << (no_qubits - 1))) >> (no_qubits - 1))] = 1;
		}
	}
	else if (gate == '0') {
		for(int mask = 0; mask < (1 << no_qubits); mask++) {
			matrix[mask].emplace_back(1 << no_qubits, 0);
		}
	}
	else {
		throw "Gate not recognised";
	}
}
transform::transform (char gate) {
	no_qubits = 2;
	norm_factor = 1;
	matrix.emplace_back(2);
	matrix[0].emplace_back(2, 0);
	matrix[0].emplace_back(2, 0);
	if (gate == '-') {
		matrix[0][0] = 1;
		matrix[1][1] = 1;
	}
	else if (gate == 'X') {
		matrix[0][1] = 1;
		matrix[1][0] = 1;
	}
	else if (gate == 'Y') {
		matrix[0][0] = 1;
		matrix[1][1] = -1;
	}
	else if (gate == 'Z') {
		matrix[0][0] = 1i;
		matrix[1][1] = -1i;
	}
	else if (gate == 'H') {
		norm_factor = 2;
		matrix[0][0] = 1;
		matrix[0][1] = 1;
		matrix[1][0] = 1;
		matrix[1][1] = -1;
	}
	else if(gate != '0') {
		throw "Gate not recognised";
	}
}

void transform::operator*=(const transform &next) {
	if(no_qubits != next.no_qubits) {
		throw "Cannot multiply 2 transfromation matrices of different sizes!\n";
	}
	std::vector <std::vector <std::complex <float>>> ans;
	for(int mask1 = 0; mask1 < matrix.size(); mask1++) {
		ans.emplace_back();
		for(int mask2 = 0; mask2 < matrix.size(); mask2++) {
			ans.back().push_back(0);
			for(int mask3 = 0; mask3 < matrix.size(); mask3++) {
				ans.back().back() += next.matrix[mask1][mask3] * matrix[mask3][mask2];
			}
		}
	}
	matrix = ans;
}

transform operator*(const transform &first, const transform &second) {
	transform ans = first;
	ans *= second;
	return ans;
}
state operator*(const transform &modify, const state &ini) {
	state fin(ini.no_qubits);
	if(modify.no_qubits != ini.no_qubits) {
		throw "Cannot multiply a state vector and a transfromation matrix of different sizes!\n";
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
transform operator| (const transform &first, const transform &second) {
	transform ans(first.no_qubits + second.no_qubits, '0');
	ans.norm_factor = first.norm_factor * second.norm_factor;
	for(int mask1 = 0; mask1 < ans.no_qubits; mask1++) {
		for(int mask2 = 0; mask2 < ans.no_qubits; mask2++) {
			ans.matrix[mask1][mask2] =
					first.matrix[mask1 & ((1 << first.no_qubits) - 1)][mask2 & ((1 << first.no_qubits) - 1)] *
					second.matrix[mask1 >> first.no_qubits][mask2 >> first.no_qubits];
		}
	}
	return ans;
}