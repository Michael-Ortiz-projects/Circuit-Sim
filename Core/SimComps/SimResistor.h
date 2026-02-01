#pragma once
#include "SimulationComponent.h"


class SimResistor : public SimulationComponent {
public:
	SimResistor(int a, int b, double r, int id)
		: SimulationComponent(id), n1(a), n2(b), R(r) { }

	std::vector<int> getNodes() const override { return { n1, n2 }; }

	void stamp(MNASystem& sys) override {
		std::cout << "stamp ran\n";
		if (n1 != 0) sys.addToA(n1, n1, 1 / R);
		if (n2 != 0) sys.addToA(n2, n2, 1 / R);
		if (n1 != 0 && n2 != 0) {
			sys.addToA(n1, n2, -1 / R);
			sys.addToA(n2, n1, -1 / R);
		}
	}

private:
	int n1, n2;
	double R;
};
