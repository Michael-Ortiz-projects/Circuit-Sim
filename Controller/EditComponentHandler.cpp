#include "EditComponentHandler.h"

EditComponentHandler::EditComponentHandler(UI_Manager& ui, Circuit& c)
	: ui(ui), schematicComponents(c.getSchematicComponents()), wires(c.getWires()), circuit(c) { }

void EditComponentHandler::onKeyPress(const sf::Event::KeyEvent& event) {
    if (event.code == sf::Keyboard::Enter)
        update();
    if (event.code == sf::Keyboard::R && !ui.hasActiveDialog())
        rotateTarget();
}

void EditComponentHandler::onMousePress(const sf::Vector2f&) {
    update();
}

void EditComponentHandler::onMouseRelease(const sf::Vector2f&) {

    update();
}

bool EditComponentHandler::shouldRelease() const {
    return finished;
}

void EditComponentHandler::setTarget(NetlistComponent* target) {
    std::cout << "setTarget Called\n";
    component = target;
    schemComp = circuit.getSchematicComponent(*component);
    for (auto& c : schematicComponents) {
        if (c.componentID == target->id) schemComp = &c;
    }
    finished = false;

}

void EditComponentHandler::openEditDialog() {
    ui.openEditDialog(component);
}

void EditComponentHandler::update() {
    UICommand cmd;
    while (ui.pollCommand(cmd)) {
        EditDialogResult EditDialog;
        Debug::UICommand(cmd);
        switch (cmd) {
        case UICommand::ApplyEdit:
            std::cout << "this ran\n";
            EditDialog = ui.getEditDialogText();
            std::cout << "got edit dialog\n";
            double parsedValue;
            if (!parseValueWithSuffix(EditDialog.valueText, parsedValue)) {
                std::cout << "[EditComponentHandler] Invalid value: " << EditDialog.valueText << "\n";
                break;
            }

            std::cout << "setting values\n";
            component->value = parsedValue;
            component->label = EditDialog.labelText;
            
            std::cout << "Component Value = " << component->value;
            std::cout << "\nClosing Edit dialog\n";

            ui.closeEditDialog();
            finished = true;
            break;

        case UICommand::CancelEdit:
            std::cout << "closing Dialog Ran\n";
            ui.closeEditDialog();
            finished = true;
            break;

        default:
            std::cout << "Default case taken in update()\n";
            break;
        }        
    }
}

bool EditComponentHandler::parseValueWithSuffix(const std::string& input, double& outValue) {
    if (input.empty())
        return false;

    static const std::unordered_map<char, double> suffixMap = {
        {'p', 1e-12},
        {'n', 1e-9},
        {'u', 1e-6},
        {'m', 1e-3},
        {'k', 1e3},
        {'M', 1e6},
        {'G', 1e9}
    };

    char lastChar = input.back();
    double multiplier = 1.0;
    std::string numberString = input;

    // check for suffix
    if (std::isalpha(lastChar)) {
        auto it = suffixMap.find(lastChar);
        if (it == suffixMap.end())
            return false;

        multiplier = it->second;
        numberString.pop_back();
    }

    try {
        size_t idx;
        double baseValue = std::stod(numberString, &idx);

        // reject invalid strings
        if (idx != numberString.size())
            return false;

        outValue = baseValue * multiplier;
        return true;
    }
    catch (...) {
        return false;
    }
}

void EditComponentHandler::rotateTarget() { //rotation is bad
    float prevRotation = schemComp->getRotation();
    float newRotation = prevRotation - 90.0f;
    newRotation = std::fmod(newRotation, 360.0f);
    if (newRotation < 0)
        newRotation += 360.0f;
    schemComp->setRotation(newRotation);


    for (auto& T : schemComp->schematicTerminals) {
        Debug::printVector2f(T.offset);
        sf::Vector2f rotatedOffset = rotatePoint(schemComp->getPosition() + T.offset, schemComp->getPosition(),-90);
        T.offset = rotatedOffset - schemComp->getPosition();
        if (T.wireNodeReference.isValid()) {
            int wireID = T.wireNodeReference.wireID;
            int nodeID = T.wireNodeReference.nodeID;
            Wire* W = circuit.getWire(wireID);
            W->moveNode(nodeID, rotatedOffset);
        }
    }

    finished = true;

}

sf::Vector2f EditComponentHandler::rotatePoint(const sf::Vector2f& point, const sf::Vector2f& center, float angleDegrees) {
    // Translate point to origin
    float s = std::sin(angleDegrees * 3.14159265f / 180.0f);
    float c = std::cos(angleDegrees * 3.14159265f / 180.0f);

    sf::Vector2f p = point - center;

    float xnew = p.x * c - p.y * s;
    float ynew = p.x * s + p.y * c;

    // Translate back
    return sf::Vector2f(xnew, ynew) + center;
}