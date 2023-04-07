#pragma once

#include <vector>

class circuit {
private:
	unsigned int no_qubits;
	std::vector <std::vector <char>> gates;
	std::vector <std::vector <float>> data;
	std::vector <unsigned int> last_gate;

	int getSpot(unsigned int pos);
	int getSpot(unsigned int pos1, unsigned int pos2);

	bool up_to_date = false;
	transform *total = nullptr;
	void Calculate();
public:
	circuit(int size);

	void Bar();

	void X(unsigned int pos);
	void Y(unsigned int pos);
	void Z(unsigned int pos);
	void RX(unsigned int pos, float phase);
	void RY(unsigned int pos, float phase);
	void RZ(unsigned int pos, float phase);
	void H(unsigned int pos);

	void CX(unsigned int posC, unsigned int posX);
	void CY(unsigned int posC, unsigned int posY);
	void CZ(unsigned int posC, unsigned int posZ);
	void CCX(const std::vector <unsigned int> &posC, unsigned int posX);
	void CCY(const std::vector <unsigned int> &posC, unsigned int posY);
	void CCZ(const std::vector <unsigned int> &posC, unsigned int posZ);

	void Apply(state &init);
};