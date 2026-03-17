#include "SimulationComponent.h"

class CurrentControlledVoltageSource : public SimulationComponent {

public:
    CurrentControlledVoltageSource(int a, int b, int c, int d, double k, int id, std::string Label)
        : SimulationComponent(id, Label), n1(a), n2(b), n3(c), n4(d), k(k) {
    }

    std::vector<int> getNodes() const override { return { n1, n2, n3, n4 }; }

    int extraVariables() const override { return 2; } // 1: I_control, 2: I_DVS

    void setExtraVariableIndex(int startIndex) {
        for (int i = 0; i < extraVariables(); i++) {
            extraVarIndices.push_back(startIndex + i);
        }
    }

    void stamp(SimulationType type, MNASystem& sys, double deltaT, double t) override {

        if (type == SimulationType::DC || type == SimulationType::Transient) {

            int I_control = extraVarIndices[0];  // current through sensing source
            int I_Vs = extraVarIndices[1];

            // sensing voltage source between n4 (+) and n3 (-)

            if (n3 != 0) sys.addToA(n3, I_control, -1);
            if (n4 != 0) sys.addToA(n4, I_control, 1);

            if (n3 != 0) sys.addToA(I_control, n3, 1);
            if (n4 != 0) sys.addToA(I_control, n4, -1);

            // voltage source current addition n2 - n1 = k * I_control

            if (n1 != 0) sys.addToA(n1, I_Vs, -1);
            if (n2 != 0) sys.addToA(n2, I_Vs, 1);
            if (n1 != 0) sys.addToA(I_Vs, n1, -1);
            if (n2 != 0) sys.addToA(I_Vs, n2, 1);
            sys.addToA(I_Vs, I_control, -k);


            
        }
    }

    void addGraphVariables(std::vector<TransientGraphVariable>& vars, const std::unordered_map<int, int>& eNodeToMNA) const override {
        int mna1 = (n1 == 0) ? -1 : n1;
        int mna2 = (n2 == 0) ? -1 : n2;
        int mna3 = (n3 == 0) ? -1 : n3;
        int mna4 = (n4 == 0) ? -1 : n4;
        int I_control = extraVarIndices[0];


        std::string compLabel = label;

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
            [I_control]
            (const TransientSimulationState& state)
            {
                return state.resultsVector[I_control];
            }
            });
    }


private:
    int n1, n2, n3, n4;
    double k;
};