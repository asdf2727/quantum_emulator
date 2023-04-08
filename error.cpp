#include <iostream>
#include <cmath>
#include <vector>
#include <random>

#include "circuit.h"

int main () {
	circuit <3> transmit;
	transmit.RX(0, 2 * M_PI / 8);
	transmit.Bar();

	transmit.CX(0, 1);
	transmit.CX(0, 2);
	transmit.Bar();

	//transmit.X(0);
	transmit.X(1);
	//transmit.X(2);
	transmit.Bar();

	transmit.CX(0, 1);
	transmit.CX(0, 2);
	transmit.CCX({1, 2}, 0);
	transmit.Bar();

	transmit.RX(0, -2 * M_PI / 8);

	transmit.Draw(std::cout);

	std::vector <int> cnt(2);
	for(int ind = 0; ind < 100000; ind++) {
		state <3> now;
		transmit.Apply(now);
		cnt[now.measure(0)]++;
	}
	for (int val : cnt) {
		std::cout << val << '\n';
	}
	return 0;
}
