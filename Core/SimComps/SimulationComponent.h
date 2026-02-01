#pragma once
#include <vector>
#include <iostream>
#include <Eigen/Dense>
class MNASystem;

class SimulationComponent {
public:
    SimulationComponent(int id = -1) : ID(id) {}
    virtual ~SimulationComponent() = default;

    virtual std::vector<int> getNodes() const = 0;
    virtual int getID() const { return ID; }
    virtual int extraVariables() const { return 0; }
    virtual void setExtraVariableIndex(int startIndex) { extraVarIndex = startIndex; }

    virtual void stamp(MNASystem& sys) = 0;


protected:
    int extraVarIndex = -1;
    int ID = -1;
};