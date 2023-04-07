#include <iostream>
#include <cmath>
#include <vector>

#include "all.h"

int main () {
	circuit <3> ini_gates;
	//ini_gates.RX(0, 0 * M_PI / 8);
	//ini_gates.H(0);
	circuit <3> fin_gates;
	fin_gates.H(2);

	circuit <3> teleport;
	teleport.H(1);
	teleport.CX(1, 2);
	teleport.CX(0, 1);
	teleport.H(0);

	circuit <3> X;
	X.X(2);
	circuit <3> Z;
	Z.Z(2);

	std::vector <int> cnt(2);
	bool doX, doZ;
	for(int ind = 0; ind < 100000; ind++) {
		state <3> now;
		ini_gates.Apply(now);

		teleport.Apply(now);
		if (now.measure(0)) {
			Z.Apply(now);
		}
		if (now.measure(1)) {
			X.Apply(now);
		}

		fin_gates.Apply(now);
		cnt[now.measure(2)]++;
	}
	for (int val : cnt) {
		std::cout << val << '\n';
	}
	return 0;
}
