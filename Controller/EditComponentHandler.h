#pragma once
#include "InputHandler.h"
#include "../UI/UI_Manager.h"

class EditComponentHandler : public InputHandler {
public:
	EditComponentHandler(UI_Manager& ui, std::vector<SchematicComponent>& schemComponents);

    void onKeyPress(const sf::Event::KeyEvent& event) override;

    void onMousePress(const sf::Vector2f&) override;
    
    void onMouseRelease(const sf::Vector2f&) override;

    bool shouldRelease() const override;

    void setTarget(Component* target);

    void openEditDialog();

    void update();

private:

    Component* component;
    SchematicComponent* schemComp = nullptr;
    std::vector<SchematicComponent>& schematicComponents;
    UI_Manager& ui;
    bool finished = false;

    bool parseValueWithSuffix(const std::string& input, double& outValue);

    void rotateTarget();
};