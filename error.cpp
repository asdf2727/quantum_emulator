#include <iostream>
#include <cmath>
#include <vector>
#include <random>

#include "all.h"

int main () {
	circuit ini_gates(3);
	ini_gates.RX(0, 0 * M_PI / 8);
	circuit fin_gates(3);

	circuit encode(3);
	encode.CX(0, 1);
	encode.CX(0, 2);
	circuit X0(3);
	X0.X(0);
	circuit X1(3);
	X1.X(1);
	circuit X2(3);
	X2.X(2);
	circuit decode(3);
	decode.CX(0, 1);
	decode.CX(0, 2);
	decode.CCX({1, 2}, 0);

	std::vector <int> cnt(2);
	for(int ind = 0; ind < 100000; ind++) {
		state now(3);
		ini_gates.Simulate(now);

		encode.Simulate(now);
		if (rand() % 10 == 0) {
			X0.Simulate(now);
		}
		if (rand() % 10 == 0) {
			X1.Simulate(now);
		}
		if (rand() % 10 == 0) {
			X2.Simulate(now);
		}
		decode.Simulate(now);

		fin_gates.Simulate(now);
		cnt[now.measure(0)]++;
	}
	for (int val : cnt) {
		std::cout << val << '\n';
	}
	return 0;
}
