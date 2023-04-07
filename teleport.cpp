#include <iostream>
#include <cmath>
#include <vector>

#include "all.h"

int main () {
	circuit ini_gates(3);
	ini_gates.RX(0, 3 * M_PI / 8);
	circuit fin_gates(3);
	fin_gates.RX(2, -3 * M_PI / 8);

	circuit teleport(3);
	teleport.H(1);
	teleport.CX(1, 2);
	teleport.CX(0, 1);
	teleport.H(0);

	circuit X(3);
	X.X(2);
	circuit Z(3);
	Z.Z(2);

	std::vector <int> cnt(2);
	bool doX, doZ;
	for(int ind = 0; ind < 100000; ind++) {
		state now(3);
		ini_gates.Simulate(now);

		teleport.Simulate(now);
		if (now.measure(0)) {
			Z.Simulate(now);
		}
		if (now.measure(1)) {
			X.Simulate(now);
		}

		fin_gates.Simulate(now);
		cnt[now.measure(2)]++;
	}
	for (int val : cnt) {
		std::cout << val << '\n';
	}
	return 0;
}
