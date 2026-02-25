#pragma once
#include <vector>
#include "../../UI/ElementStateDisplay.h"
#include "../MNASystem.h"

enum class SimulationType {
    None,
    DC,
    Transient,
};


class SimulationComponent {
public:
    SimulationComponent(int id = -1) : ID(id) {}
    virtual ~SimulationComponent() = default;

    virtual std::vector<int> getNodes() const = 0;
    virtual int getID() const { return ID; }
    virtual int extraVariables() const { return 0; }
    virtual void setExtraVariableIndex(int startIndex) { extraVarIndex = startIndex; }

    virtual void setCurrent(double C) { current = C; }
    virtual void setVoltage(double V) { voltage = V; }

    virtual void stamp(SimulationType type, MNASystem& sys, double deltaT = 0) = 0;

    virtual ElementState getState(MNASystem& sys) const { 
        ElementState disp;
        return disp; 
    }

    virtual std::vector<ExtraVarInfo> getExtraVarInfo() const { return {}; }


protected:
    int extraVarIndex = -1;
    int ID = -1;
    double voltage = 0;
    double current = 0;
};