#pragma once
#include <vector>
#include "Button.h"
#include "DropdownMenu.h"
#include "AssetManager.h"
#include <optional>

enum class MenuID {
	Place
};

class UI_Manager
{
public:

	std::unordered_map<MenuID, DropdownMenu> menu_map;

	UI_Manager();

	void initialize(AssetManager& assets);

	bool pollCommand(UICommand& outputCommand);

	bool onMousePress(const sf::Vector2f& point);

	bool onMouseMove(const sf::Vector2f& point);

	bool onMouseRelease(const sf::Vector2f& point);

	void draw(sf::RenderWindow& window);
private:
};

