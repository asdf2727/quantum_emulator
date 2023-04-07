#include <cmath>
#include <vector>

#include "all.h"

circuit::circuit (int size) : gates(), data(), last_gate(size, 0) {
	no_qubits = size;
}

int circuit::getSpot (unsigned int pos) {
	if (0 > pos || pos >= no_qubits) {
		throw std::runtime_error("Qubit index not in range!\n");
	}
	up_to_date = false;
	while (gates.size() <= last_gate[pos]) {
		gates.emplace_back(no_qubits, '-');
		data.emplace_back(no_qubits, 0.0f);
	}
	last_gate[pos]++;
	return last_gate[pos] - 1;
}
int circuit::getSpot (unsigned int pos1, unsigned int pos2) {
	if (0 > pos1 || pos1 > pos2 || pos2 >= no_qubits) {
		throw std::runtime_error("Qubit index not in range!\n");
	}
	up_to_date = false;
	unsigned int maxi = 0;
	for (int pos = pos1; pos <= pos2; pos++) {
		maxi = std::max(maxi, last_gate[pos]);
	}
	while (gates.size() <= maxi) {
		gates.emplace_back(no_qubits, '-');
		data.emplace_back(no_qubits, 0.0f);
	}
	for (int pos = pos1; pos <= pos2; pos++) {
		last_gate[pos] = maxi + 1;
	}
	return maxi;
}

void circuit::Bar () {
	getSpot(0, no_qubits - 1);
	gates.back()[0] = '|';
}

void circuit::X (unsigned int pos) {
	int depth = getSpot(pos);
	gates[depth][pos] = 'X';
	data[depth][pos] = M_PI;
}
void circuit::Y (unsigned int pos) {
	int depth = getSpot(pos);
	gates[depth][pos] = 'Y';
	data[depth][pos] = M_PI;
}
void circuit::Z (unsigned int pos) {
	int depth = getSpot(pos);
	gates[depth][pos] = 'Z';
	data[depth][pos] = M_PI;
}
void circuit::RX (unsigned int pos, float phase) {
	int depth = getSpot(pos);
	gates[depth][pos] = 'X';
	data[depth][pos] = phase;
}
void circuit::RY (unsigned int pos, float phase) {
	int depth = getSpot(pos);
	gates[depth][pos] = 'Y';
	data[depth][pos] = phase;
}
void circuit::RZ (unsigned int pos, float phase) {
	int depth = getSpot(pos);
	gates[depth][pos] = 'Z';
	data[depth][pos] = phase;
}
void circuit::H (unsigned int pos) {
	int depth = getSpot(pos);
	gates[depth][pos] = 'H';
}

void circuit::CX (unsigned int posC, unsigned int posX) {
	int start = std::min(posC, posX), stop = std::max(posC, posX);
	int depth = getSpot(start, stop);
	for (int pos = start; pos <= stop; pos++) {
		gates[depth][pos] = '0';
	}
	gates[depth][posC] = 'c';
	gates[depth][posX] = 'x';
	data[depth][stop] = 1;
}
void circuit::CY (unsigned int posC, unsigned int posY) {
	int start = std::min(posC, posY), stop = std::max(posC, posY);
	int depth = getSpot(start, stop);
	for (int pos = start; pos <= stop; pos++) {
		gates[depth][pos] = '0';
	}
	gates[depth][posC] = 'c';
	gates[depth][posY] = 'y';
	data[depth][stop] = 1;
}
void circuit::CZ (unsigned int posC, unsigned int posZ) {
	int start = std::min(posC, posZ), stop = std::max(posC, posZ);
	int depth = getSpot(start, stop);
	for (int pos = start; pos <= stop; pos++) {
		gates[depth][pos] = '0';
	}
	gates[depth][posC] = 'c';
	gates[depth][posZ] = 'z';
	data[depth][stop] = 1;
}
void circuit::CCX(const std::vector <unsigned int> &posC, unsigned int posX) {
	int start = no_qubits, stop = -1;
	for(int pos : posC) {
		start = std::min(start, pos);
		stop = std::max(stop, pos);
	}
	int depth = getSpot(start, stop);
	for (int pos = start; pos <= stop; pos++) {
		gates[depth][pos] = '0';
	}
	for(int pos : posC) {
		gates[depth][pos] = 'c';
	}
	gates[depth][posX] = 'x';
	data[depth][stop] = 1;
}
void circuit::CCY(const std::vector <unsigned int> &posC, unsigned int posY) {
	int start = no_qubits, stop = -1;
	for(int pos : posC) {
		start = std::min(start, pos);
		stop = std::max(stop, pos);
	}
	int depth = getSpot(start, stop);
	for (int pos = start; pos <= stop; pos++) {
		gates[depth][pos] = '0';
	}
	for(int pos : posC) {
		gates[depth][pos] = 'c';
	}
	gates[depth][posY] = 'y';
	data[depth][stop] = 1;
}
void circuit::CCZ(const std::vector <unsigned int> &posC, unsigned int posZ) {
	int start = no_qubits, stop = -1;
	for(int pos : posC) {
		start = std::min(start, pos);
		stop = std::max(stop, pos);
	}
	int depth = getSpot(start, stop);
	for (int pos = start; pos <= stop; pos++) {
		gates[depth][pos] = '0';
	}
	for(int pos : posC) {
		gates[depth][pos] = 'c';
	}
	gates[depth][posZ] = 'z';
	data[depth][stop] = 1;
}

void circuit::Calculate () {
	static transform Igate(gateI);
	static transform Xgate(gateX);
	static transform Ygate(gateY);
	static transform Zgate(gateZ);
	static transform Hgate(gateH);
	delete total;
	total = new transform(gateI, no_qubits);
	transform *temp, *dyn;
	for (int depth = 0; depth < gates.size(); depth++) {
		std::vector <char> &layer = gates[depth];
		if (layer[0] == '|') {
			continue;
		}
		temp = new transform(gateI, 0u);
		for (int ind = 0; ind < layer.size(); ind++) {
			if (layer[ind] == '-') {
				*temp |= Igate;
			}
			else if ('X' <= layer[ind] && layer[ind] <= 'Z') {
				if (data[depth][ind] == M_PI) {
					// TODO figure out float comparison
					*temp |= layer[ind] == 'X' ? Xgate : (layer[ind] == 'Y' ? Ygate : Zgate);
				}
				else {
					dyn = new transform(layer[ind] == 'X' ? gateRX : (layer[ind] == 'Y' ? gateRY : gateRZ), data[depth][ind]);
					*temp |= *dyn;
					delete dyn;
				}
			}
			else if (layer[ind] == 'H') {
				*temp |= Hgate;
			}
			else {
				unsigned int len = 0;
				unsigned int target;
				std::vector <unsigned int> ctls;
				char type;
				while (ind < layer.size()) {
					if (layer[ind] == 'c') {
						ctls.push_back(len);
					}
					else if (layer[ind] != '0') {
						type = layer[ind];
						target = len;
					}
					len++;
					if (data[depth][ind] != 0) {
						break;
					}
					ind++;
				}
				switch (type) {
				case 'x':
					dyn = new transform(gateCX, len, target, ctls);
					break;
				case 'y':
					dyn = new transform(gateCY, len, target, ctls);
					break;
				case 'z':
					dyn = new transform(gateCZ, len, target, ctls);
					break;
				default:
					throw std::runtime_error("Invalid gate found!\n");
				}
				*temp |= *dyn;
				delete dyn;
			}
		}
		*total *= *temp;
		delete temp;
	}
	up_to_date = true;
}

void circuit::Simulate(state &init) {
	if (!up_to_date) {
		Calculate();
	}
	init = *total * init;
}