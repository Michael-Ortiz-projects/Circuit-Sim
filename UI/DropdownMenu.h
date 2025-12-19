#pragma once
#include <iostream>
#include "Button.h"

enum class MenuID {
	Place
};

class DropdownMenu {
public:
	Button parentButton;
	std::vector<Button> options;
	bool open = false;
	sf::RectangleShape panel;
	float panelMargin = 4;
	float rowGap = 3;

	float margin = 4;
	float maxWidth = 0;

	DropdownMenu(sf::Font& fnt, const std::string& label, const sf::Vector2f& pos, const sf::Vector2f& size);

	void addOption(const std::string& label, UICommand cmd);

	bool onMousePress(const sf::Vector2f& point);

	bool onMouseMove(const sf::Vector2f& point);

	bool onMouseRelease(const sf::Vector2f& point);	

	bool poll(UICommand& outputCommand);

	void updateMenuLayout();

	void draw(sf::RenderWindow& window);

	bool contains(const sf::Vector2f& point);
};

