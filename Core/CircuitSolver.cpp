#include "CircuitSolver.h"

void CircuitSolver::AddComponent(Component& component) {
    int index = components.size();
    components.push_back(&component);
    adjacencyList[component.nodeA].emplace_back(component.nodeB, index);
    adjacencyList[component.nodeB].emplace_back(component.nodeA, index);
}


int CircuitSolver::ConfigureCircuit() {
    if (adjacencyList.empty()) {
        std::cout << "No components in the circuit.\n";
        return -1;
    }

    int bestReferenceNode = -1;
    int maxDegree = -1;
    int mostNegativeVoltages = -1;

    for (const auto& [node, adjacentNodes] : adjacencyList) {
        int degree = adjacentNodes.size();
        if (degree < maxDegree) continue;

        int negativeVoltages = 0;
        for (const auto& [neighboringNode, componentID] : adjacentNodes) {
            const Component* comp = components[componentID];
            if (comp->type == ComponentType::VoltageSource && comp->nodeA == node) {
                negativeVoltages++;
            }
        }

        if (degree > maxDegree || (degree == maxDegree && negativeVoltages > mostNegativeVoltages)) {
            bestReferenceNode = node;
            maxDegree = degree;
            mostNegativeVoltages = negativeVoltages;
        }
    }
    referenceNode = bestReferenceNode;

    int nodeindex = 0;
    for (const auto& [node, adjacentNodes] : adjacencyList) {
        if (node == bestReferenceNode) continue;
        nodeToIndex[node] = nodeindex++;
    }

    int voltageSourceCount = 0;
    int inductorCount = 0;
    int switchCount = 0;

    for (const Component* c : components) {
        if (c->type == ComponentType::VoltageSource) voltageSourceCount++;
        if (c->type == ComponentType::Inductor) inductorCount++;
        if (c->type == ComponentType::Switch) switchCount++;
    }

    int matrixSize = nodeindex + voltageSourceCount + inductorCount + switchCount;
    A = Eigen::MatrixXd::Zero(matrixSize, matrixSize);
    b = Eigen::VectorXd::Zero(matrixSize);

    variables.clear();
    for (int i = 0; i < nodeToIndex.size(); i++) {
        variables.push_back("v" + std::to_string(i));
    }

    

    return bestReferenceNode;
}

void CircuitSolver::SolveCircuit(float deltaTime) {
    int n = nodeToIndex.size();

    int voltageSourceCount = 0;
    int inductorCount = 0;
    for (const auto& c : components) {
        if (c->type == ComponentType::VoltageSource) voltageSourceCount++;
        if (c->type == ComponentType::Inductor) inductorCount++;
    }

    A.setZero();
    b.setZero();

    int voltageCurrentIndex = 0;
    int inductorCurrentIndex = 0;
    int switchIndex = 0;

    for (Component* c : components) {
        int i = (c->nodeA == referenceNode) ? -1 : nodeToIndex[c->nodeA];
        int j = (c->nodeB == referenceNode) ? -1 : nodeToIndex[c->nodeB];

        switch (c->type) {
        case ComponentType::Resistor: {
            float G = 1.0f / c->value;
            if (i != -1) A(i, i) -= G;
            if (j != -1) A(j, j) -= G;
            if (i != -1 && j != -1) {
                A(i, j) += G;
                A(j, i) += G;
            }
            break;
        }

        case ComponentType::VoltageSource: {
            int vsRow = n + voltageCurrentIndex;

            if (i != -1) {
                A(i, vsRow) -= 1;
                A(vsRow, i) -= 1;
            }
            if (j != -1) {
                A(j, vsRow) += 1;
                A(vsRow, j) += 1;
            }
            b(vsRow) += c->value;

            voltageCurrentIndex++;
            break;
        }

        case ComponentType::CurrentSource: {
            if (i != -1) b(i) += c->value;
            if (j != -1) b(j) -= c->value;
            break;
        }

        case ComponentType::Capacitor: {
            float G = c->value / deltaTime;
            if (i != -1) {
                A(i, i) -= G;
                b(i) += -G * c->voltage;
            }
            if (j != -1) {
                A(j, j) -= G;
                b(j) -= -G * c->voltage;
            }
            if (i != -1 && j != -1) {
                A(i, j) += G;
                A(j, i) += G;
            }
            break;
        }

        case ComponentType::Inductor: {
            int indRow = n + voltageSourceCount + inductorCurrentIndex;

            float R = c->value / deltaTime;
            if (i != -1) {
                A(i, indRow) -= 1;
                A(indRow, i) += 1;
            }
            if (j != -1) {
                A(j, indRow) += 1;
                A(indRow, j) -= 1;
            }
            A(indRow, indRow) -= R;
            b(indRow) += -c->value * c->current / deltaTime;

            inductorCurrentIndex++;
            break;
        }

        case ComponentType::Switch: {
            if (!c->isClosed) break;
            int row = n + voltageSourceCount + inductorCount + switchIndex;

            if (i != -1) {
                A(i, row) -= 1;
                A(row, i) += 1;
            }
            if (j != -1) {
                A(j, row) += 1;
                A(row, j) -= 1;
            }
            switchIndex++;
        }
        }
    }

    x = A.colPivHouseholderQr().solve(b);


    voltageCurrentIndex = 0;
    inductorCurrentIndex = 0;
    switchIndex = 0;

    for (Component* c : components) {
        int i = (c->nodeA == referenceNode) ? -1 : nodeToIndex[c->nodeA];
        int j = (c->nodeB == referenceNode) ? -1 : nodeToIndex[c->nodeB];

        switch (c->type) {
        case ComponentType::Resistor: {
            float voltage = 0;
            if (i != -1) voltage += x(i);
            if (j != -1) voltage -= x(j);
            c->voltage = voltage;
            c->current = voltage / c->value;
            break;
        }

        case ComponentType::VoltageSource: {
            int vsIndex = n + voltageCurrentIndex;
            c->current = x(vsIndex);
            voltageCurrentIndex++;
            break;
        }

        case ComponentType::CurrentSource: {
            float voltage = 0;
            if (i != -1) voltage += x(i);
            if (j != -1) voltage -= x(j);
            c->voltage = voltage;
            break;
        }

        case ComponentType::Capacitor: {
            float voltage = 0;
            if (i != -1) voltage += x(i);
            if (j != -1) voltage -= x(j);
            c->current = (c->value / deltaTime) * (voltage - c->voltage);
            c->voltage = voltage;
            break;
        }

        case ComponentType::Inductor: {
            int inductorIndex = n + voltageSourceCount + inductorCurrentIndex;
            float current = x(inductorIndex);
            c->voltage = (c->value / deltaTime) * (current - c->current);
            c->current = current;
            inductorCurrentIndex++;
            break;
        }

        case ComponentType::Switch: {
            if (!c->isClosed) {
                c->voltage = 0;
                c->current = 0;
                break;
            }

            int switchCurrentIndex = n + voltageSourceCount + inductorCount + switchIndex;
            c->current = x(switchCurrentIndex);

            float voltage = 0;
            if (i != -1) voltage += x(i);
            if (j != -1) voltage -= x(j);
            c->voltage = voltage;

            switchIndex++;
            break;
        }
        }
    }
}


void CircuitSolver::ResetCircuit() {
    for (Component* c : components) {
        switch (c->type) {
        case ComponentType::Resistor: {
            c->voltage = 0;
            c->current = 0;
            break;
        }
        case ComponentType::VoltageSource: {
            c->current = 0;
            break;
        }
        case ComponentType::CurrentSource: {
            c->voltage = 0;
            break;
        }
        case ComponentType::Capacitor: {
            c->voltage = 0;
            c->current = 0;
            break;
        }
        case ComponentType::Inductor: {
            c->voltage = 0;
            c->current = 0;
            break;
        }
        case ComponentType::Switch: {
            c->voltage = 0;
            c->current = 0;
            c->isClosed = false;
            break;
        }
        }
    }

    adjacencyList.clear();
    components.clear();
    referenceNode = -1;
    A.setZero();
    x.setZero();
    b.setZero();
    variables.clear();
}

