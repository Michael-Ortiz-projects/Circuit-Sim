#pragma once
#include <iostream>
#include "SimulationComponent.h"

class SimCurrentSource : public SimulationComponent {
public:
	SimCurrentSource(int a, int b, double r, int id)
		: SimulationComponent(id), n1(a), n2(b), I(r) { }

	std::vector<int> getNodes() const override { return { n1, n2 }; }

	void stamp(MNASystem& sys) override {
		std::cout << "Current Source Stamp running()\n";
		std::cout << "n1 = " << n1;
		if (n1 != 0) sys.addTob(n1, -I);
		std::cout << "n2 = " << n2;

		if (n2 != 0) sys.addTob(n2, I);
	}

private:
	int n1, n2;
	double I;
};