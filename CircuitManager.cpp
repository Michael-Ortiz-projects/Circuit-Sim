#include "CircuitManager.h"

void CircuitManager::markForDeletion() {
    for (auto& wire : wires) {
        if (wire.selected) {
            for (const auto& [entityID, leadType] : wire.connected_entity_ids) {
                if (!entities.count(entityID)) continue;

                if (leadType == 0) {
                    entities[entityID].component.fromNode = -1;
                }
                else {
                    entities[entityID].component.toNode = -1;
                }
            }
            wire.deleted = true;
        }
    }

    // Mark entities for deletion
    for (auto& [id, entity] : entities) {
        if (entity.selected) {
            entity.deleted = true;
        }
    }
}

void CircuitManager::remapWireIndices() {
    std::unordered_map<int, int> wireIndexMap;
    int newIndex = 0;
    for (int i = 0; i < wires.size(); ++i) {
        if (!wires[i].deleted) {
            wireIndexMap[i] = newIndex++;
        }
    }

    for (auto& [id, entity] : entities) {
        if (entity.component.fromNode != -1)
            entity.component.fromNode = wireIndexMap.count(entity.component.fromNode) ? wireIndexMap[entity.component.fromNode] : -1;
        if (entity.component.toNode != -1)
            entity.component.toNode = wireIndexMap.count(entity.component.toNode) ? wireIndexMap[entity.component.toNode] : -1;
    }
}

void CircuitManager::eraseDeletedWires() {
    wires.erase(std::remove_if(wires.begin(), wires.end(),
        [](const Wire& w) { return w.deleted; }), wires.end());
}

void CircuitManager::disconnectEntityFromWire(int wireIndex, int entityId, int port) {
    if (wireIndex < 0 || wireIndex >= wires.size()) return;
    auto& wire = wires[wireIndex];

    auto& anchorNodes = wire.anchorNodes;
    auto& connected_ids = wire.connected_entity_ids;

    auto it = std::find(connected_ids.begin(), connected_ids.end(), std::make_pair(entityId, port));
    if (it != connected_ids.end()) {
        size_t index = std::distance(connected_ids.begin(), it);
        if (index < anchorNodes.size()) {
            anchorNodes.erase(anchorNodes.begin() + index);
        }
        connected_ids.erase(it);

        wire.pruneCollinearNodes();
        wire.pruneDeadEnds();

        if (anchorNodes.size() <= 1)
            wire.deleted = true;
    }
}

void CircuitManager::deleteSelected() {
    remapWireIndices();
    eraseDeletedWires();

    for (auto it = entities.begin(); it != entities.end(); ) {
        Entity& entity = it->second;
        if (entity.deleted) {
            int fromNode = entity.component.fromNode;
            int toNode = entity.component.toNode;

            if (fromNode != -1)
                disconnectEntityFromWire(fromNode, entity.component.identification_number, 0);
            if (toNode != -1)
                disconnectEntityFromWire(toNode, entity.component.identification_number, 1);

            it = entities.erase(it);
        }
        else {
            ++it;
        }
    }
}

void CircuitManager::createEntityFromCursorState(CursorState& cursorState) {
    Component temp_component;
    std::string prefix;

    switch (cursorState) {
    case CursorState::CreatingResistor:
        temp_component = Component(-1, -1, ComponentType::Resistor, "R_" + std::to_string(nextEntityID), 500);
        cursorState = CursorState::PlacingResistor;
        break;

    case CursorState::CreatingVoltageSource:
        temp_component = Component(-1, -1, ComponentType::VoltageSource, "V_" + std::to_string(nextEntityID), 5);
        cursorState = CursorState::PlacingVoltageSource;
        break;

    case CursorState::CreatingCurrentSource:
        temp_component = Component(-1, -1, ComponentType::CurrentSource, "I_" + std::to_string(nextEntityID), 1);
        cursorState = CursorState::PlacingCurrentSource;
        break;

    case CursorState::CreatingCapacitor:
        temp_component = Component(-1, -1, ComponentType::Capacitor, "C_" + std::to_string(nextEntityID), 0.00001);
        cursorState = CursorState::PlacingCapacitor;
        break;

    case CursorState::CreatingInductor:
        temp_component = Component(-1, -1, ComponentType::Inductor, "L_" + std::to_string(nextEntityID), 0.00001);
        cursorState = CursorState::PlacingInductor;
        break;

    case CursorState::CreatingSwitch:
        temp_component = Component(-1, -1, ComponentType::Switch, "S_" + std::to_string(nextEntityID), 0);
        temp_component.controlKey = sf::Keyboard::S;
        cursorState = CursorState::PlacingSwitch;
        break;

    default:
        return;
    }

    Entity newEntity(temp_component);

    newEntity.uniqueID = nextEntityID;
    newEntity.component.identification_number = nextEntityID;

    entities[nextEntityID] = newEntity;
    draggedEntity = &entities[nextEntityID];

    ++nextEntityID;
    std::cout << draggedEntity->component.label;
}
