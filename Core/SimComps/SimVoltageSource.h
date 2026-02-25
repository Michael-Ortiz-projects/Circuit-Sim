#pragma once
#include <iostream>
#include "SimulationComponent.h"

class SimVoltageSource : public SimulationComponent {
public:
	SimVoltageSource(int a, int b, double v, int id)
		: SimulationComponent(id), n1(a), n2(b), V(v) {
	}

	std::vector<int> getNodes() const override { return { n1, n2 }; }

    int extraVariables() const override { return 1; }

	void stamp(SimulationType type, MNASystem& sys, double deltaT) override {
        //std::cout << "SimVoltageSource stamp ran\n";

        int iIdx = extraVarIndex; // index for source current

        if (type == SimulationType::DC || type == SimulationType::Transient) {
            // KCL contributions
            if (n1 != 0) sys.addToA(n1, iIdx, 1);  // +I_s into n1
            if (n2 != 0) sys.addToA(n2, iIdx, -1); // -I_s into n2

            // Voltage constraint row
            if (n1 != 0) sys.addToA(iIdx, n1, -1);
            if (n2 != 0) sys.addToA(iIdx, n2, 1);

            // RHS = V_s
            sys.addTob(iIdx, V);
        }
	}

    std::vector<ExtraVarInfo> getExtraVarInfo() const override {
        return { ExtraVarInfo(-1, ExtraVarType::Current, ID, "I(") };
    }
private:
	int n1, n2;
	double V;
};