#pragma once
#include "SimulationComponent.h"


class SimInductor : public SimulationComponent { // not complete, need to fix stamping
public:
	SimInductor(int a, int b, double l, int id)
		: SimulationComponent(id), n1(a), n2(b), L(l) {
	}

	std::vector<int> getNodes() const override { return { n1, n2 }; }

	int extraVariables() const override { return 1; } // one extra variable for inductor current



	void stamp(SimulationType type, MNASystem& sys, double deltaT) override {
        //std::cout << "SimInductor stamp ran\n";

        if (type == SimulationType::DC) {
            // DC inductor behaves like a 0V voltage source

            // KCL contributions
            if (n1 != 0) sys.addToA(n1, extraVarIndex, 1);   // +I_L into node n1
            if (n2 != 0) sys.addToA(n2, extraVarIndex, -1);  // -I_L into node n2

            // Voltage constraint row
            if (n1 != 0) sys.addToA(extraVarIndex, n1, 1);   // Vn1 - Vn2 = 0
            if (n2 != 0) sys.addToA(extraVarIndex, n2, -1);

            // RHS = 0
            sys.addTob(extraVarIndex, 0.0);
        }

        else if (type == SimulationType::Transient) {
            if (n1 != 0) {
                sys.addToA(n1, extraVarIndex, 1);
                sys.addToA(extraVarIndex, n1, 1);
            }

            if (n2 != 0) {
                sys.addToA(n2, extraVarIndex, -1);
                sys.addToA(extraVarIndex, n2, -1);
            }

            sys.addToA(extraVarIndex, extraVarIndex, -L / deltaT);
            sys.addTob(extraVarIndex, - (L / deltaT) * current);
        }
	}

    std::vector<ExtraVarInfo> getExtraVarInfo() const override {
        return {
            { extraVarIndex, ExtraVarType::Current, ID, "I(" }
        };
    }
private:
	int n1, n2;
	double L;
};
