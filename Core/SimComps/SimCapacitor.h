#pragma once
#include <iostream>
#include "SimulationComponent.h"

class SimCapacitor : public SimulationComponent {// not complete, need to fix stamping
public:
	SimCapacitor(int a, int b, double c, int id)
		: SimulationComponent(id), n1(a), n2(b), C(c) {
	}

	std::vector<int> getNodes() const override { return { n1, n2 }; }

	void stamp(SimulationType type, MNASystem& sys, double deltaT) override {
		//std::cout << "SimCapacitor Stamp running()\n";
		switch (type) {
		case SimulationType::DC:
			break;
		case SimulationType::Transient:
			double G = C / deltaT;
			double V_prev = sys.getx()(n1) - sys.getx()(n2);
			if (n1 != 0) {
				sys.addToA(n1, n1, G);
				sys.addTob(n1, G * V_prev);
			}
			if (n2 != 0) {
				sys.addToA(n2, n2, G);
				sys.addTob(n2, -G * V_prev);
			}
			if (n1 != 0 && n2 != 0) {
				sys.addToA(n1, n2, -G);
				sys.addToA(n2, n1, -G);
			}

			break;
		}


	}

private:
	int n1, n2;
	double C;
};