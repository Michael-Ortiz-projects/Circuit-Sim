#pragma once
#include <iostream>
#include "SimulationComponent.h"

class SimVoltageSource : public SimulationComponent {
public:
	SimVoltageSource(int a, int b, double v, int id, std::string Label)
		: SimulationComponent(id, Label), n1(a), n2(b), V(v) {
	}

	std::vector<int> getNodes() const override { return { n1, n2 }; }

    int extraVariables() const override { return 1; }

    void setExtraVariableIndex(int startIndex) override { 
        for (int i = 0; i < extraVariables(); i++) {
            extraVarIndices.push_back(startIndex + i);
        }
    }

    void stampStatic(SimulationType type, MNASystem& sys, double deltaT, double t) {
        int iIdx = extraVarIndices[0]; // index for source current

        if (type == SimulationType::DC || type == SimulationType::Transient) {
            // KCL contributions
            if (n1 != 0) sys.addToAStatic(n1, iIdx, 1);  // +I_s into n1
            if (n2 != 0) sys.addToAStatic(n2, iIdx, -1); // -I_s into n2

            // Voltage constraint row
            if (n1 != 0) sys.addToAStatic(iIdx, n1, -1);
            if (n2 != 0) sys.addToAStatic(iIdx, n2, 1);

            // RHS = V_s
            sys.addTobStatic(iIdx, V);
        }
    }

	void stamp(SimulationType type, MNASystem& sys, double deltaT, double t) override {
        //std::cout << "SimVoltageSource stamp ran\n";

        int iIdx = extraVarIndices[0]; // index for source current

        if (type == SimulationType::DC || type == SimulationType::Transient) {
            // KCL contributions
            if (n1 != 0) sys.addToADynamic(n1, iIdx, 1);  // +I_s into n1
            if (n2 != 0) sys.addToADynamic(n2, iIdx, -1); // -I_s into n2

            // Voltage constraint row
            if (n1 != 0) sys.addToADynamic(iIdx, n1, -1);
            if (n2 != 0) sys.addToADynamic(iIdx, n2, 1);

            // RHS = V_s
            sys.addTobDynamic(iIdx, V);
        }
	}

    void addGraphVariables(std::vector<TransientGraphVariable>& vars, const std::unordered_map<int, int>& eNodeToMNA) const override {
        int mna1 = (n1 == 0) ? -1 : n1;
        int mna2 = (n2 == 0) ? -1 : n2;

        std::string compLabel = label;
        int extraVarIndex = getExtraVarInfo()[0].index;

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
            [extraVarIndex]
            (const TransientSimulationState& state)
            {
                return state.resultsVector[extraVarIndex];
            }
            });
    }

    std::vector<ExtraVarInfo> getExtraVarInfo() const override {
        return { ExtraVarInfo(extraVarIndices[0], ExtraVarType::Current, ID, "I(")};
    }
private:
	int n1, n2;
	double V;
};