#pragma once
#include <vector>
#include "Button.h"
#include "DropdownMenu.h"
#include "AssetManager.h"
#include <optional>

class UI_Manager
{
public:
	UI_Manager();

	void initialize(AssetManager& assets);

	bool pollCommand(UICommand& outputCommand);

	bool onMousePress(const sf::Vector2f& point);

	bool onMouseMove(const sf::Vector2f& point);

	bool onMouseRelease(const sf::Vector2f& point);

	void draw(sf::RenderWindow& window);
private:
	std::optional<DropdownMenu> placeMenu;
};

