#pragma once
#include <iostream>
#include "SimulationComponent.h"

class SimCurrentSource : public SimulationComponent {
public:
	SimCurrentSource(int a, int b, double i, int id, std::string Label)
		: SimulationComponent(id, Label), n1(a), n2(b), I(i) { }

	std::vector<int> getNodes() const override { return { n1, n2 }; }


    void stampStatic(SimulationType type, MNASystem& sys, double deltaT, double t) {
        if (n1 != 0) sys.addTobStatic(n1, -I);

        if (n2 != 0) sys.addTobStatic(n2, I);
    }

	void stamp(SimulationType type, MNASystem& sys, double deltaT, double t) override {
		//std::cout << "Current Source Stamp running()\n";
		if (n1 != 0) sys.addTob(n1, -I);

		if (n2 != 0) sys.addTob(n2, I);
	}

    void addGraphVariables(std::vector<TransientGraphVariable>& vars, const std::unordered_map<int, int>& eNodeToMNA) const override {
        int mna1 = (n1 == 0) ? -1 : n1;
        int mna2 = (n2 == 0) ? -1 : n2;

        std::string compLabel = label;
        float current = I;
        // -------------------------
        // Voltage across source
        // -------------------------
        vars.push_back({
            "V(" + compLabel + ")",
            [mna1, mna2](const TransientSimulationState& state)
            {
                double v1 = (mna1 == -1) ? 0.0 : state.resultsVector[mna1];
                double v2 = (mna2 == -1) ? 0.0 : state.resultsVector[mna2];
                return v2 - v1;
            }
            });

        // -------------------------
        // Current through source stored in MNA matrix
        // -------------------------

        vars.push_back({
            "I(" + compLabel + ")",
            [current]
            (const TransientSimulationState& state)
            {
                return current;
            }
            });
    }


private:
	int n1, n2;
	double I;
};