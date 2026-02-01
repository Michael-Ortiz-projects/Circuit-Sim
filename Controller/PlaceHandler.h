#pragma once
#include "SFML/Graphics.hpp"
#include "../UI/SchematicComponent.h"
#include "../Core/circuit.h"
#include "InputHandler.h"
#include <vector>
#include "../UI/AssetManager.h"

enum class EditMode {
	Place,
	Delete
};

class PlaceHandler : public InputHandler {
public:
	Circuit& circuit;
	AssetManager& assets;
	ComponentType type;

	PlaceHandler(Circuit& Circuit, AssetManager& Assets);

	void onMousePress(const sf::Vector2f& worldPos) override;

	void onMouseMove(const sf::Vector2f& worldPos) override;

	void onKeyPress(const sf::Event::KeyEvent& event) override;

	bool shouldRelease() const override;


	void placeComponent(const sf::Vector2f& worldPos);

	void setComponentType(ComponentType comp_type);
};