#pragma once
#include "SFML/Graphics.hpp"
#include "InputHandler.h"
#include "../Core/Circuit.h"
#include "../UI/SchematicComponent.h"
#include <vector>
#include "../Config.h"

enum class WireState {
	Creating,
	Editing,
	Deleting//for now
};

class WireHandler : public InputHandler {
public:
	ElectricalConnection attemptedConnection;

	WireHandler(Circuit& Circuit, std::vector<SchematicComponent> components);

	void onMousePress(const sf::Vector2f& worldPos) override;

	void onMouseMove(const sf::Vector2f& worldPos) override;

	void onMouseRelease(const sf::Vector2f& worldPos) override;

	bool shouldRelease() const override;

	void createWire(const sf::Vector2f& worldPos);

private:

	sf::Vector2f& snapPositionToGrid(sf::Vector2f& position);
	sf::Vector2f positionOfConnection(ElectricalConnection& connection);
	Circuit& circuit;
	std::vector<SchematicComponent> schematicComponents;
	Wire* activeWire;

};