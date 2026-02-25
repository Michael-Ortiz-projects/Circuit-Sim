#pragma once
#include "SimulationComponent.h"


class SimResistor : public SimulationComponent {
public:
	SimResistor(int a, int b, double r, int id)
		: SimulationComponent(id), n1(a), n2(b), R(r) { }

	std::vector<int> getNodes() const override { return { n1, n2 }; }

	void stamp(SimulationType type, MNASystem& sys, double deltaT) override {
		//std::cout << "stamp ran\n";
		if (n1 != 0) sys.addToA(n1, n1, 1 / R);
		if (n2 != 0) sys.addToA(n2, n2, 1 / R);
		if (n1 != 0 && n2 != 0) {
			sys.addToA(n1, n2, -1 / R);
			sys.addToA(n2, n1, -1 / R);
		}
	}

	ElementState getState(MNASystem& sys) const override {
		ElementState state;
		state.label = "Default Resistor Label in SimComp.getState()";
		state.value = R;
		state.voltageDelta;
		double v1 = sys.getx()(n1);
		double v2 = sys.getx()(n2);
		state.voltageDelta = v2 - v1;
		state.current = state.voltageDelta / R;
		state.type = ComponentType::Resistor;
		return state;
	}
private:
	int n1, n2;
	double R;
};
