#pragma once
#include "SimulationComponent.h"


class SimResistor : public SimulationComponent {
public:
	SimResistor(int a, int b, double r, int id, std::string Label)
		: SimulationComponent(id, Label), n1(a), n2(b), R(r) { }

	std::vector<int> getNodes() const override { return { n1, n2 }; }

    void stampStatic(SimulationType type, MNASystem& sys, double deltaT, double t) {
        if (n1 != 0) sys.addToAStatic(n1, n1, 1 / R);
        if (n2 != 0) sys.addToAStatic(n2, n2, 1 / R);
        if (n1 != 0 && n2 != 0) {
            sys.addToAStatic(n1, n2, -1 / R);
            sys.addToAStatic(n2, n1, -1 / R);
        }
    }


	void stamp(SimulationType type, MNASystem& sys, double deltaT, double t) override {
		//std::cout << "stamp ran\n";
		if (n1 != 0) sys.addToADynamic(n1, n1, 1 / R);
		if (n2 != 0) sys.addToADynamic(n2, n2, 1 / R);
		if (n1 != 0 && n2 != 0) {
			sys.addToADynamic(n1, n2, -1 / R);
			sys.addToADynamic(n2, n1, -1 / R);
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

	void addGraphVariables(std::vector<TransientGraphVariable>& vars, const std::unordered_map<int, int>& eNodeToMNA) const override { 
        //std::cout << "[n1, n2] = [" << n1 << ", " << n2 << "]\n";


        int mna1 = (n1 == 0) ? -1 : n1;
        int mna2 = (n2 == 0) ? -1 : n2;

        std::string compLabel = label; // inherited from SimulationComponent

        // -------------------------
        // Voltage across resistor
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
        // Current through resistor
        // -------------------------
        double resistance = R;

        vars.push_back({
            "I(" + compLabel + ")",
            [mna1, mna2, resistance](const TransientSimulationState& state)
            {
                double v1 = (mna1 == -1) ? 0.0 : state.resultsVector[mna1];
                double v2 = (mna2 == -1) ? 0.0 : state.resultsVector[mna2];
                return (v2 - v1) / resistance;
            }
            });

        // -------------------------
        // Power dissipated
        // -------------------------
        /*
        vars.push_back({
            "P(" + compLabel + ")",
            [mna1, mna2, resistance](const TransientSimulationState& state)
            {
                double v1 = (mna1 == -1) ? 0.0 : state.resultsVector[mna1];
                double v2 = (mna2 == -1) ? 0.0 : state.resultsVector[mna2];
                double v = v2 - v1;
                return v * (v / resistance);
            }
            });
        */
        

	}

private:
	int n1, n2;
	double R;
};
