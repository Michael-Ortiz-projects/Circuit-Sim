#pragma once
#include <string>
#include <iostream>
#include "ElectricalNode.h"
#include "../Config.h"

enum class ComponentType {
    Resistor,
    VoltageSource,
    CurrentSource,
    Capacitor,
    Inductor,
    Switch,
    Ground,
    VCVS,
    VCCS,
    CCVS,
    CCCS,
    ACVoltageSource,
    ACCurrentSource,
};

enum class PinRole {
    Positive,
    Negative,
    ControlPositive,
    ControlNegative
};

struct NetlistTerminal {
    int terminalID;
    int electricalNode;
    PinRole role;
};

class NetlistComponent {
public:
    int id;
    std::string label;
    ComponentType type;
    std::vector<NetlistTerminal> terminals;
    double value;
    std::string expression;

    NetlistComponent(const ComponentType& ty, double val);
    NetlistComponent();

    void setType(ComponentType Type);
    void setValue(double Value);

    bool terminalValid(int terminalID);
};
