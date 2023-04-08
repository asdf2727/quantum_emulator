#pragma once

#include <vector>
#include <iostream>
#include <string>

#include "state.h"

/*
 * Circuit class. Manages gate placement, drawing and simplifying to a 'circuit matrix'. This optimization allows the
 * circuit to be calculated a single time and then reused for different states.
 * This class is unable to handle measurements. Please call them from the state class instead.
 */
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

	/*
	 * Various gates. Each one is applied to the end of the circuit and cannot be removed.
	 * Applying a gate does NOT invalidate the 'Apply' method.
	 */
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

	/*
	 * Runs a given state through the circuit. If necessary recalculates the circuit.
	 */
	void Apply(state <no_qubits>&init);

	/*
	 * Draws a schematic of the current circuit to the given output buffer. Uses only extended ascii characters.
	 */
	void Draw(std::ostream &out);
	std::ostream &operator<<(std::ostream &out);
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
	data[depth][pos] = M_PI;
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
				unsigned int type, target;
				std::vector <unsigned int> ctls;
				while (ind < layer.size()) {
					if (layer[ind] == 'c') {
						ctls.push_back(len);
					}
					else if (layer[ind] != '0') {
						type = ind;
						target = len;
					}
					len++;
					if (gate_stops[depth] & (1 << ind)) {
						break;
					}
					ind++;
				}
				switch (layer[type]) {
				case 'x':
					dyn = new transform(gateCRX, len, target, ctls, data[depth][type]);
					break;
				case 'y':
					dyn = new transform(gateCRY, len, target, ctls, data[depth][type]);
					break;
				case 'z':
					dyn = new transform(gateCRZ, len, target, ctls, data[depth][type]);
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

#define DBar (char)186
#define VBar (char)179
#define HBar (char)196
#define upT (char)193
#define leftT (char)180
#define downT (char)194
#define rightT (char)195
#define LUcorn (char)218
#define LDcorn (char)192
#define RUcorn (char)191
#define RDcorn (char)217
#define cross (char)197
#define fill (char)219

template <unsigned int no_qubits>
void circuit <no_qubits>::Draw(std::ostream &out) {
	if (!gates.empty()) {
		static std::string HBar7(7, HBar);
		static std::string upEdge = {LUcorn, HBar, HBar, HBar, HBar, HBar, RUcorn};
		static std::string downEdge = {LDcorn, HBar, HBar, HBar, HBar, HBar, RDcorn};
		static std::string upEdgeNotch = {LUcorn, HBar, HBar, upT, HBar, HBar, RUcorn};
		static std::string downEdgeNotch = {LDcorn, HBar, HBar, downT, HBar, HBar, RDcorn};
		static std::string sideEdges = {leftT, ' ', ' ', ' ', ' ', ' ', rightT};
		std::vector <std::string> result(no_qubits * 3);
		for(int ind = 0; ind < no_qubits; ind++) {
			result[3 * ind] = ' ';
			result[3 * ind + 1] += HBar;
			result[3 * ind + 2] += ' ';
		}
		for(int depth = 0; depth < gates.size(); depth++) {
			if (gates[depth][0] == '|') {
				for(int ind = 0; ind < no_qubits; ind++) {
					result[3 * ind] += { DBar, ' ', };
					result[3 * ind + 1] += { DBar, HBar, };
					result[3 * ind + 2] += { DBar, ' ', };
				}
			}
			else {
				int ctrl_last = false, ctrl_now, ctrl_stop;
				for(int ind = 0; ind < no_qubits; ind++) {
					ctrl_stop = gate_stops[depth] & (1 << ind);
					if (gates[depth][ind] == '-') {
						result[3 * ind] +=     "       ";
						result[3 * ind + 1] += HBar7;
						result[3 * ind + 2] += "       ";
						ctrl_last = false;
					}
					else if (gates[depth][ind] == 'X' || gates[depth][ind] == 'Y' || gates[depth][ind] == 'Z' || gates[depth][ind] == 'H') {
						result[3 * ind] += upEdge;
						result[3 * ind + 1] += sideEdges;
						result[3 * ind + 2] += downEdge;
						if(data[depth][ind] == M_PI) {
							result[3 * ind + 1][result[3 * ind + 1].size() - 4] = gates[depth][ind];
						}
						else {
							result[3 * ind + 1][result[3 * ind + 1].size() - 4] = 'R';
							result[3 * ind + 1][result[3 * ind + 1].size() - 3] = gates[depth][ind];
						}
						ctrl_last = false;
					}
					else if (gates[depth][ind] == '0' || gates[depth][ind] == 'c') {
						result[3 * ind] +=     "       ";
						result[3 * ind + 1] += HBar7;
						result[3 * ind + 2] += "       ";
						if (ctrl_last) {
							result[3 * ind][result[3 * ind].size() - 4] = VBar;
							result[3 * ind + 1][result[3 * ind + 1].size() - 4] = downT;
						}
						if (!ctrl_stop) {
							result[3 * ind + 2][result[3 * ind + 2].size() - 4] = VBar;
							result[3 * ind + 1][result[3 * ind + 1].size() - 4] = ctrl_last ? cross : upT;
						}
						if (gates[depth][ind] == 'c') {
							result[3 * ind + 1][result[3 * ind + 1].size() - 4] = fill;
						}
						ctrl_last = !ctrl_stop;
					}
					else if (gates[depth][ind] == 'x' || gates[depth][ind] == 'y' || gates[depth][ind] == 'z') {
						result[3 * ind] +=     ctrl_last ? upEdgeNotch : upEdge;
						result[3 * ind + 1] += sideEdges;
						result[3 * ind + 2] += ctrl_stop ? downEdge : downEdgeNotch;
						result[3 * ind + 1][result[3 * ind + 1].size() - 5] = 'C';
						if(data[depth][ind] == M_PI) {
							result[3 * ind + 1][result[3 * ind + 1].size() - 4] = gates[depth][ind] - 32;
						}
						else {
							result[3 * ind + 1][result[3 * ind + 1].size() - 4] = 'R';
							result[3 * ind + 1][result[3 * ind + 1].size() - 3] = gates[depth][ind] - 32;
						}
						ctrl_last = !ctrl_stop;
					}
					else {
						throw std::runtime_error("Invalid gate found!\n");
					}
					result[3 * ind] +=     ' ';
					result[3 * ind + 1] += HBar;
					result[3 * ind + 2] += ' ';
				}
			}
		}
		for(std::string &line : result) {
			out << line << '\n';
		}
		out << std::endl;
	}
}
template <unsigned int no_qubits>
std::ostream &circuit <no_qubits>::operator<<(std::ostream &out) {
	Draw(out);
	return out;
}

#undef DBar
#undef VBar
#undef HBar
#undef upT
#undef leftT
#undef downT
#undef rightT
#undef LUcorn
#undef LDcorn
#undef RUcorn
#undef RDcorn
#undef cross

#undef fill
#undef gate0
#undef gateI
#undef gateH
#undef gateRX
#undef gateRY
#undef gateRZ
#undef gateCRX
#undef gateCRY
#undef gateCRZ