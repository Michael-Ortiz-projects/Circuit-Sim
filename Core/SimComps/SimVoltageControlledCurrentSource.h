#include "SimulationComponent.h"

class VoltageControlledCurrentSource : public SimulationComponent {

public:
    VoltageControlledCurrentSource(int a, int b, int c, int d, double k, int id, std::string Label)
        : SimulationComponent(id, Label), n1(a), n2(b), n3(c), n4(d), k(k) {
    }

    std::vector<int> getNodes() const override { return { n1, n2, n3, n4 }; }

    void stampStatic(SimulationType type, MNASystem& sys, double deltaT, double t) {
        if (type == SimulationType::DC || type == SimulationType::Transient) {

            if (n1 != 0 && n4 != 0) sys.addToAStatic(n1, n4, k);
            if (n1 != 0 && n3 != 0) sys.addToAStatic(n1, n3, -k);

            if (n2 != 0 && n4 != 0) sys.addToAStatic(n2, n4, -k);
            if (n2 != 0 && n3 != 0) sys.addToAStatic(n2, n3, k);
        }
    }

    void stamp(SimulationType type, MNASystem& sys, double deltaT, double t) override {

        if (type == SimulationType::DC || type == SimulationType::Transient) {

            if (n1 != 0 && n4 != 0) sys.addToA(n1, n4, k);
            if (n1 != 0 && n3 != 0) sys.addToA(n1, n3, -k);

            if (n2 != 0 && n4 != 0) sys.addToA(n2, n4, -k);
            if (n2 != 0 && n3 != 0) sys.addToA(n2, n3, k);

        }
    }

    void addGraphVariables(std::vector<TransientGraphVariable>& vars, const std::unordered_map<int, int>& eNodeToMNA) const override {
        int mna1 = (n1 == 0) ? -1 : n1;
        int mna2 = (n2 == 0) ? -1 : n2;
        int mna3 = (n3 == 0) ? -1 : n3;
        int mna4 = (n4 == 0) ? -1 : n4;
        int multiplier = k;

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
            [mna3, mna4, multiplier]
            (const TransientSimulationState& state)
            {
                double cv_negative = (mna3 == -1) ? 0.0 : state.resultsVector[mna3];
                double cv_positive = (mna4 == -1) ? 0.0 : state.resultsVector[mna4];
                return (cv_positive - cv_negative) * multiplier;
            }
            });
    }


private:
    int n1, n2, n3, n4;
    double k;
};