#include <iostream>
#include <cmath>
#include <vector>

#include "circuit.h"

/*
 * This is an example that uses the circuit and state classes to emulate the teleportation of a quantum cirucit.
 * Q0 is transferred to Q2 with Q1 as an intermediary.
 */

int main () {

	circuit <3> teleport;
	teleport.RY(0, M_PI / 4);
	teleport.H(1);
	teleport.CX(1, 2);
	teleport.Bar();
	teleport.CX(0, 1);
	teleport.H(0);
	std::cout << "Teleporter:\n" << teleport;

	circuit <3> X;
	X.X(2);
	circuit <3> Z;
	Z.Z(2);

	std::vector <int> cnt(2);
	bool doX, doZ;
	for(int ind = 0; ind < 100000; ind++) {
		state <3> now;
		teleport.Apply(now);
		std::vector <bool> send = now.measure({0, 1});
		if (send[0]) {
			Z.Apply(now);
		}
		if (send[1]) {
			X.Apply(now);
		}
		cnt[now.measure(2)]++;
	}
	for (int val : cnt) {
		std::cout << val << '\n';
	}
	return 0;
}
