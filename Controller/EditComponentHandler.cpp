#include "EditComponentHandler.h"

EditComponentHandler::EditComponentHandler(UI_Manager& ui, std::vector<SchematicComponent>& schemComponents)
	: ui(ui), schematicComponents(schemComponents) { }

void EditComponentHandler::onKeyPress(const sf::Event::KeyEvent&) {
    update();
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

void EditComponentHandler::begin(Component* target) {
    std::cout << "begin Called\n";
    component = target;
    for (auto& c : schematicComponents) {
        if (c.componentID == target->id) schemComp = &c;
    }
    finished = false;

    ui.openEditDialog(component);
}

void EditComponentHandler::update() {
    UICommand cmd;
    while (ui.pollCommand(cmd)) {
        std::string dialog;
        Debug::UICommand(cmd);
        switch (cmd) {
        case UICommand::ApplyEdit:
            dialog = ui.getEditDialogText();

            double parsedValue;
            if (!parseValueWithSuffix(dialog, parsedValue)) {
                std::cout << "[EditComponentHandler] Invalid value: " << dialog << "\n";
                break;
            }

            component->value = parsedValue;
            std::cout << "Component Value = " << component->value;
            std::cout << "\nClosing Edit dialog\n";
            //schemComp->setLabel("DEFAULT LABEL");

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