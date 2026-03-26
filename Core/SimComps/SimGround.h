#pragma once
#include <iostream>
#include "SimulationComponent.h"

class SimGround : public SimulationComponent {
public:
	SimGround(int a, int id)
		: SimulationComponent(id), n1(a) { }

	std::vector<int> getNodes() const override { return { n1 }; }

    void stampStatic(SimulationType type, MNASystem& sys, double deltaT, double t) {
		sys.addToAStatic(0, 0, 1);
    }

	void stamp(SimulationType type, MNASystem& sys, double deltaT, double t) override {
		//std::cout << "stamp ran\n";
		sys.addToA(0, 0, 1);
	}

private:
	int n1;
};