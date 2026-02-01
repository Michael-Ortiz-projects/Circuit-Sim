#pragma once
#include "SFML/Graphics.hpp"
#include <functional>
#include <iostream>
#include "../Config.h"

enum class UICommand {
	None,
	ToggleMenu,

	PlaceVoltageSource,
	PlaceResistor,
	PlaceCurrentSource,
	PlaceCapacitor,
	PlaceInductor,
	PlaceSwitch,
	PlaceGround,

	OpenNewFile,
	OpenFile,
	SaveFile,
	SaveFileAs,
	ExitProgram,

	ApplyEdit,
	CancelEdit
};

class Button {
public:
	sf::Font& font;
	sf::RectangleShape box;
	sf::Text text;
	int characterSize;
	float textResolutionFactor;

	Button(sf::Font& fnt, const std::string& label, const sf::Vector2f& pos, const sf::Vector2f& size, UICommand cmd);

	void draw(sf::RenderWindow& window);

	bool contains(const sf::Vector2f mousePos);

	void onMousePress(const sf::Vector2f& point);

	void onMouseMove(const sf::Vector2f& point);

	void onMouseRelease(const sf::Vector2f& point);

	bool consumed(UICommand& outputCommand);

	void setTextResolutionFactor(float factor);

	void alignTextOnLeft(float margin);

	std::string getName();
private:
	bool clicked;
	bool triggered;
	UICommand command = UICommand::None;
	std::string name;


	sf::Color hoverColor = sf::Color(70, 70, 85);
	sf::Color hoverOutline = sf::Color(172, 172, 172);
	sf::Color normalColor = sf::Color(45, 45, 55);
};
