#pragma once
#include <vector>
#include "../../UI/ElementStateDisplay.h"
#include "../MNASystem.h"

enum class SimulationType {
    None,
    DC,
    Transient,
};

struct TransientSimulationState {
    Eigen::VectorXd resultsVector;
    Eigen::VectorXd previousResultsVector;
    double time;
    double deltaT;
};

struct TransientGraphVariable {
    std::string label;
    std::function<double(const TransientSimulationState&)> evaluator;
};


class SimulationComponent {
public:
    SimulationComponent(int id = -1, std::string Label = "Default Component Label Set In SimulationComponent.h") : ID(id), label(Label) {}
    virtual ~SimulationComponent() = default;

    virtual std::vector<int> getNodes() const = 0;
    virtual int getID() const { return ID; }
    virtual int extraVariables() const { return 0; }
    virtual void setExtraVariableIndex(int startIndex) { return;/* extraVarIndices startIndex; */} // make this function default to do nothing, but in each simComponent define how many variables and their indices

    virtual void setCurrent(double C) { current = C; }
    virtual void setVoltage(double V) { voltage = V; }

    virtual void stamp(SimulationType type, MNASystem& sys, double deltaT = 0) = 0;

    virtual ElementState getState(MNASystem& sys) const { 
        ElementState disp;
        return disp; 
    }

    virtual std::vector<ExtraVarInfo> getExtraVarInfo() const { return {}; }

    virtual void addGraphVariables(std::vector<TransientGraphVariable>& vars, const std::unordered_map<int, int>& eNodeToMNA) const { return; }

   

protected:
    std::vector<int> extraVarIndices;
    int ID = -1;
    std::string label;
    double voltage = 0;
    double current = 0;
};