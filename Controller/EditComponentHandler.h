#pragma once
#include "InputHandler.h"
#include "../UI/EditorUI_Manager.h"
#include "../Core/Circuit.h"

class EditComponentHandler : public InputHandler {
public:
	EditComponentHandler(EditorUI_Manager& ui, Circuit& c);

    void onKeyPress(const sf::Event::KeyEvent& event) override;

    void onMousePress(const sf::Vector2f&) override;
    
    void onMouseRelease(const sf::Vector2f&) override;

    bool shouldRelease() const override;

    void setTarget(NetlistComponent* target);

    void openEditDialog();

    void update();

private:

    NetlistComponent* component;
    SchematicComponent* schemComp = nullptr;
    std::vector<SchematicComponent>& schematicComponents;
    EditorUI_Manager& ui;
    std::unordered_map<int, Wire>& wires;
    Circuit& circuit;
    bool finished = false;

    bool parseValueWithSuffix(const std::string& input, double& outValue);

    void rotateTarget();
    sf::Vector2f rotatePoint(const sf::Vector2f& point, const sf::Vector2f& center, float angleDegrees);

};