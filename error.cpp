#include <iostream>
#include <cmath>
#include <vector>
#include <random>

#include "all.h"

int main () {
	circuit <3> ini_gates;
	ini_gates.RX(0, 0 * M_PI / 8);
	circuit <3> fin_gates;

	circuit <3> encode;
	encode.CX(0, 1);
	encode.CX(0, 2);
	circuit <3> X0;
	X0.X(0);
	circuit <3> X1;
	X1.X(1);
	circuit <3> X2;
	X2.X(2);
	circuit <3> decode;
	decode.CX(0, 1);
	decode.CX(0, 2);
	decode.CCX({1, 2}, 0);

	std::vector <int> cnt(2);
	for(int ind = 0; ind < 100000; ind++) {
		state <3> now;
		ini_gates.Apply(now);

		encode.Apply(now);
		if (rand() % 10 == 0) {
			X0.Apply(now);
		}
		if (rand() % 10 == 0) {
			X1.Apply(now);
		}
		if (rand() % 10 == 0) {
			X2.Apply(now);
		}
		decode.Apply(now);

		fin_gates.Apply(now);
		cnt[now.measure(0)]++;
	}
	for (int val : cnt) {
		std::cout << val << '\n';
	}
	return 0;
}
