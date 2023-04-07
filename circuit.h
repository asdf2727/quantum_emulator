#pragma once

#include <vector>

template <unsigned int no_qubits>
class circuit {
private:
	std::vector <std::vector <char>> gates;
	std::vector <unsigned int> gate_stops;
	std::vector <std::vector <double>> data;
	std::vector <unsigned int> last_gate;

	unsigned int getSpot(unsigned int pos);
	unsigned int getSpot(unsigned int pos1, unsigned int pos2);

	unsigned int controlSetup(unsigned int posC, unsigned int posT);
	unsigned int controlSetup(std::vector <unsigned int> posC, unsigned int posT);

	bool up_to_date = false;
	transform *total = nullptr;
	void Calculate();
public:
	circuit();

	void Bar();

	void H(unsigned int pos);

	void X(unsigned int pos);
	void Y(unsigned int pos);
	void Z(unsigned int pos);
	void RX(unsigned int pos, double phase);
	void RY(unsigned int pos, double phase);
	void RZ(unsigned int pos, double phase);

	void CX(unsigned int posC, unsigned int posX);
	void CY(unsigned int posC, unsigned int posY);
	void CZ(unsigned int posC, unsigned int posZ);
	void CCX(const std::vector <unsigned int> &posC, unsigned int posX);
	void CCY(const std::vector <unsigned int> &posC, unsigned int posY);
	void CCZ(const std::vector <unsigned int> &posC, unsigned int posZ);
	void CRX(unsigned int posC, unsigned int posX, double phase);
	void CRY(unsigned int posC, unsigned int posY, double phase);
	void CRZ(unsigned int posC, unsigned int posZ, double phase);
	void CCRX(const std::vector <unsigned int> &posC, unsigned int posX, double phase);
	void CCRY(const std::vector <unsigned int> &posC, unsigned int posY, double phase);
	void CCRZ(const std::vector <unsigned int> &posC, unsigned int posZ, double phase);

	void Apply(state <no_qubits>&init);
};

template <unsigned int no_qubits>
circuit <no_qubits>::circuit () : gates(), data(), last_gate(no_qubits, 0)  {}

template <unsigned int no_qubits>
unsigned int circuit <no_qubits>::getSpot (unsigned int pos) {
	if (pos >= no_qubits) {
		throw std::runtime_error("Qubit index not in range!\n");
	}
	up_to_date = false;
	while (gates.size() <= last_gate[pos]) {
		gates.emplace_back(no_qubits, '-');
		data.emplace_back(no_qubits, 0.0f);
		gate_stops.emplace_back();
	}
	last_gate[pos]++;
	return last_gate[pos] - 1;
}
template <unsigned int no_qubits>
unsigned int circuit <no_qubits>::getSpot (unsigned int pos1, unsigned int pos2) {
	if (pos1 > pos2 || pos2 >= no_qubits) {
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
		gate_stops.emplace_back();
	}
	for (int pos = pos1; pos <= pos2; pos++) {
		last_gate[pos] = maxi + 1;
	}
	return maxi;
}

template <unsigned int no_qubits>
unsigned int circuit <no_qubits>::controlSetup(unsigned int posC, unsigned int posT) {
	int start = std::min(posC, posT), stop = std::max(posC, posT);
	int depth = getSpot(start, stop);
	for (int pos = start; pos <= stop; pos++) {
		gates[depth][pos] = '0';
	}
	gates[depth][posC] = 'c';
	gate_stops[depth] |= 1 << stop;
	return depth;
}
template <unsigned int no_qubits>
unsigned int circuit <no_qubits>::controlSetup(std::vector <unsigned int> posC, unsigned int posT) {
	int start = posT, stop = posT;
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
	gate_stops[depth] |= 1 << stop;
	return depth;
}

template <unsigned int no_qubits>
void circuit <no_qubits>::Bar () {
	getSpot(0, no_qubits - 1);
	gates.back()[0] = '|';
}

template <unsigned int no_qubits>
void circuit <no_qubits>::H (unsigned int pos) {
	int depth = getSpot(pos);
	gates[depth][pos] = 'H';
}

template <unsigned int no_qubits>
void circuit <no_qubits>::X (unsigned int pos) {
	int depth = getSpot(pos);
	gates[depth][pos] = 'X';
	data[depth][pos] = M_PI;
}
template <unsigned int no_qubits>
void circuit <no_qubits>::Y (unsigned int pos) {
	int depth = getSpot(pos);
	gates[depth][pos] = 'Y';
	data[depth][pos] = M_PI;
}
template <unsigned int no_qubits>
void circuit <no_qubits>::Z (unsigned int pos) {
	int depth = getSpot(pos);
	gates[depth][pos] = 'Z';
	data[depth][pos] = M_PI;
}
template <unsigned int no_qubits>
void circuit <no_qubits>::RX (unsigned int pos, double phase) {
	int depth = getSpot(pos);
	gates[depth][pos] = 'X';
	data[depth][pos] = phase;
}
template <unsigned int no_qubits>
void circuit <no_qubits>::RY (unsigned int pos, double phase) {
	int depth = getSpot(pos);
	gates[depth][pos] = 'Y';
	data[depth][pos] = phase;
}
template <unsigned int no_qubits>
void circuit <no_qubits>::RZ (unsigned int pos, double phase) {
	int depth = getSpot(pos);
	gates[depth][pos] = 'Z';
	data[depth][pos] = phase;
}

template <unsigned int no_qubits>
void circuit <no_qubits>::CX (unsigned int posC, unsigned int posX) {
	int depth = controlSetup(posC, posX);
	gates[depth][posX] = 'x';
	data[depth][posX] = M_PI;
}
template <unsigned int no_qubits>
void circuit <no_qubits>::CY (unsigned int posC, unsigned int posY) {
	int depth = controlSetup(posC, posY);
	gates[depth][posY] = 'y';
	data[depth][posY] = M_PI;
}
template <unsigned int no_qubits>
void circuit <no_qubits>::CZ (unsigned int posC, unsigned int posZ) {
	int depth = controlSetup(posC, posZ);
	gates[depth][posZ] = 'z';
	data[depth][posZ] = M_PI;
}
template <unsigned int no_qubits>
void circuit <no_qubits>::CCX (const std::vector <unsigned int> &posC, unsigned int posX) {
	int depth = controlSetup(posC, posX);
	gates[depth][posX] = 'x';
	data[depth][posX] = M_PI;
}
template <unsigned int no_qubits>
void circuit <no_qubits>::CCY (const std::vector <unsigned int> &posC, unsigned int posY) {
	int depth = controlSetup(posC, posY);
	gates[depth][posY] = 'y';
	data[depth][posY] = M_PI;
}
template <unsigned int no_qubits>
void circuit <no_qubits>::CCZ (const std::vector <unsigned int> &posC, unsigned int posZ) {
	int depth = controlSetup(posC, posZ);
	gates[depth][posZ] = 'z';
	data[depth][posZ] = M_PI;
}
template <unsigned int no_qubits>
void circuit <no_qubits>::CRX (unsigned int posC, unsigned int posX, double phase) {
	int depth = controlSetup(posC, posX);
	gates[depth][posX] = 'x';
	data[depth][posX] = phase;
}
template <unsigned int no_qubits>
void circuit <no_qubits>::CRY (unsigned int posC, unsigned int posY, double phase) {
	int depth = controlSetup(posC, posY);
	gates[depth][posY] = 'y';
	data[depth][posY] = phase;
}
template <unsigned int no_qubits>
void circuit <no_qubits>::CRZ (unsigned int posC, unsigned int posZ, double phase) {
	int depth = controlSetup(posC, posZ);
	gates[depth][posZ] = 'z';
	data[depth][posZ] = phase;
}
template <unsigned int no_qubits>
void circuit <no_qubits>::CCRX (const std::vector <unsigned int> &posC, unsigned int posX, double phase) {
	int depth = controlSetup(posC, posX);
	gates[depth][posX] = 'x';
	data[depth][posX] = phase;
}
template <unsigned int no_qubits>
void circuit <no_qubits>::CCRY (const std::vector <unsigned int> &posC, unsigned int posY, double phase) {
	int depth = controlSetup(posC, posY);
	gates[depth][posY] = 'y';
	data[depth][posY] = phase;
}
template <unsigned int no_qubits>
void circuit <no_qubits>::CCRZ (const std::vector <unsigned int> &posC, unsigned int posZ, double phase) {
	int depth = controlSetup(posC, posZ);
	gates[depth][posZ] = 'z';
	data[depth][posZ] = phase;
}

template <unsigned int no_qubits>
void circuit <no_qubits>::Calculate () {
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
				dyn = new transform(gateI);
			}
			else if (layer[ind] == 'H') {
				dyn = new transform(gateH);
			}
			else if (layer[ind] == 'X') {
				dyn = new transform(gateRX, data[depth][ind]);
			}
			else if (layer[ind] == 'Y') {
				dyn = new transform(gateRY, data[depth][ind]);
			}
			else if (layer[ind] == 'Z') {
				dyn = new transform(gateRZ, data[depth][ind]);
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
					if (gate_stops[depth] & (1 << ind)) {
						break;
					}
					ind++;
				}
				switch (type) {
				case 'x':
					dyn = new transform(gateCRX, len, target, ctls, data[depth][ind]);
					break;
				case 'y':
					dyn = new transform(gateCRY, len, target, ctls, data[depth][ind]);
					break;
				case 'z':
					dyn = new transform(gateCRZ, len, target, ctls, data[depth][ind]);
					break;
				default:
					throw std::runtime_error("Invalid gate found!\n");
				}
			}
			*temp |= *dyn;
			delete dyn;
		}
		*total *= *temp;
#ifdef DEBUG
		temp->Show();
		total->Show();
#endif
		delete temp;
	}
	up_to_date = true;
}

template <unsigned int no_qubits>
void circuit <no_qubits>::Apply(state <no_qubits> &init) {
	if (!up_to_date) {
		Calculate();
	}
	init = *total * init;
}