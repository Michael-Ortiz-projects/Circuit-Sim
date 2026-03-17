#pragma once
#include <iostream>
#include "SimulationComponent.h"

class SimCapacitor : public SimulationComponent {
public:
	SimCapacitor(int a, int b, double c, int id, std::string Label)
		: SimulationComponent(id, Label), n1(a), n2(b), C(c) {
	}

	std::vector<int> getNodes() const override { return { n1, n2 }; }

	void stamp(SimulationType type, MNASystem& sys, double deltaT, double t) override {
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


    void addGraphVariables(std::vector<TransientGraphVariable>& vars, const std::unordered_map<int, int>& eNodeToMNA) const override {
        int mna1 = (n1 == 0) ? -1 : n1;
        int mna2 = (n2 == 0) ? -1 : n2;

        std::string compLabel = label;
        double capacitance = C;

        // -------------------------
        // Voltage across capacitor
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
        // Current through capacitor
        // I = C * dV/dt
        // Using backward Euler:
        // dV/dt = (V - V_prev) / dt
        // -------------------------
        vars.push_back({
            "I(" + compLabel + ")",
            [mna1, mna2, capacitance]
            (const TransientSimulationState& state)
            {
                double v1 = (mna1 == -1) ? 0.0 : state.resultsVector[mna1];
                double v2 = (mna2 == -1) ? 0.0 : state.resultsVector[mna2];

                double v1_prev = (mna1 == -1) ? 0.0 : state.previousResultsVector[mna1];
                double v2_prev = (mna2 == -1) ? 0.0 : state.previousResultsVector[mna2];

                double v = v2 - v1;
                double v_prev = v2_prev - v1_prev;

                double dVdt = (v - v_prev) / state.deltaT;

                return capacitance * dVdt;
            }
            });

        // -------------------------
        // Energy stored
        // E = 1/2 C V^2
        // -------------------------

        /*
        vars.push_back({
            "E(" + compLabel + ")",
            [mna1, mna2, capacitance]
            (const TransientSimulationState& state)
            {
                double v1 = (mna1 == -1) ? 0.0 : state.resultsVector[mna1];
                double v2 = (mna2 == -1) ? 0.0 : state.resultsVector[mna2];

                double v = v2 - v1;

                return 0.5 * capacitance * v * v;
            }
            });
        */
        
    }
private:
	int n1, n2;
	double C;
};