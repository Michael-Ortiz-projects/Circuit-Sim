#pragma once
#include "InputHandler.h"
#include "../UI/UI_Manager.h"

class EditComponentHandler : public InputHandler {
public:
	EditComponentHandler(UI_Manager& ui, std::vector<SchematicComponent>& schemComponents);

    void onKeyPress(const sf::Event::KeyEvent&) override;

    void onMousePress(const sf::Vector2f&) override;
    
    void onMouseRelease(const sf::Vector2f&) override;

    bool shouldRelease() const override;

    void begin(Component* target);

    void update();

    bool parseValueWithSuffix(const std::string& input, double& outValue);
private:

    Component* component;
    SchematicComponent* schemComp = nullptr;
    std::vector<SchematicComponent>& schematicComponents;
    UI_Manager& ui;
    bool finished = false;
};