#include "Component.h"

Component::Component(int A, int B, const ComponentType& ty, double val) {
    nodeA = A;
    nodeB = B;
    type = ty;
    value = val;
    current = 0;
    voltage = 0;
    switch (ty) {
    case ComponentType::VoltageSource: {
        voltage = val;
        break;
    }

    case ComponentType::CurrentSource: {
        current = val;
        break;
    }

    case ComponentType::Switch: {
        isClosed = false;
        break;
    }
    }

}

Component::Component() {

}