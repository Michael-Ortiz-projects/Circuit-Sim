#pragma once
#include <iostream>
#include "SimulationComponent.h"

class SimCurrentSource : public SimulationComponent {
public:
	SimCurrentSource(int a, int b, double i, int id)
		: SimulationComponent(id), n1(a), n2(b), I(i) { }

	std::vector<int> getNodes() const override { return { n1, n2 }; }

	void stamp(SimulationType type, MNASystem& sys, double deltaT) override {
		//std::cout << "Current Source Stamp running()\n";
		if (n1 != 0) sys.addTob(n1, -I);

		if (n2 != 0) sys.addTob(n2, I);
	}

private:
	int n1, n2;
	double I;
};