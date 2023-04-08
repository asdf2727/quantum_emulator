#include <iostream>
#include <cmath>
#include <vector>
#include <random>

#include "circuit.h"

/*
 * This is an example that uses the circuit and state classes to emulate a basic error correction algorithm.
 * Q0 is the corrected qubit, with Q1 and Q2 as helpers.
 */

int main () {
	circuit <3> encode;
	//encode.RX(0, 2 * M_PI / 8);
	encode.Bar();
	encode.CX(0, 1);
	encode.CX(0, 2);
	std::cout << "Encoder:\n" << encode;

	circuit <3> decode;
	decode.CX(0, 1);
	decode.CX(0, 2);
	decode.CCX({1, 2}, 0);
	decode.Bar();
	//decode.RX(0, -2 * M_PI / 8);
	std::cout << "Decoder:\n" << decode;

	circuit <3> X0, X1, X2;
	X0.X(0);
	X1.X(1);
	X2.X(2);

	std::vector <int> cnt(2);
	for(int ind = 0; ind < 100000; ind++) {
		state <3> now;
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
		cnt[now.measure(0)]++;
	}
	for (int val : cnt) {
		std::cout << val << '\n';
	}
	return 0;
}
