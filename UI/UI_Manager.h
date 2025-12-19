#pragma once
#include <vector>
#include "Button.h"
#include "DropdownMenu.h"
#include "AssetManager.h"
#include <optional>
#include "../Controller/Controller.h"

class UI_Manager
{
public:

	std::unordered_map<MenuID, DropdownMenu> menu_map;

	UI_Manager(Controller& Controller);

	void initialize(AssetManager& assets);

	bool handleEvent(const sf::Event& event);

	bool pollCommand(UICommand& outputCommand);

	bool onMousePress(const sf::Vector2f& pixelPos);

	bool onMouseMove(const sf::Vector2f& pixelPos);

	bool onMouseRelease(const sf::Vector2f& pixelPos);

	void draw(sf::RenderWindow& window);
private:
	Controller& controller;
};

