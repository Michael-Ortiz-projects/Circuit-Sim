#pragma once
#include "SFML/Graphics.hpp"
#include <iostream>
#include "../Config.h"

enum class EditorUICommand {
	None,
	ToggleMenu,

	PlaceVoltageSource,
	PlaceResistor,
	PlaceCurrentSource,
	PlaceVCVS,
	PlaceVCCS,
	PlaceCCVS,
	PlaceCCCS,
	PlaceCapacitor,
	PlaceInductor,
	PlaceSwitch,
	PlaceGround,

	OpenNewFile,
	OpenFile,
	SaveFile,
	SaveFileAs,
	ExitProgram,

	OpenSimulationWindow,

	ApplyEdit,
	CancelEdit
};

enum class SimulationUICommand {
	None,
	ToggleMenu,
	NewSimulation,
	CreateSimulation,
	CancelCreateSimulation,
	EditSimulation,
	ApplyEditSimulation,
	CancelEditSimulation,
	RunSimulation,


	GraphData,
	UpdateY_AxisMenu,
	ApplyGraphData,
	CancelGraphData,

	GraphSettings,
	AutoScaleGraph,
	ApplyGraphSettings,
	CancelGraphSettings,
	

	DCOP,
	TRAN,
};

template <typename CommandT>

class Button {
public:
	sf::Font& font;
	sf::RectangleShape box;
	sf::Text text;
	int characterSize;
	float textResolutionFactor;

	Button(sf::Font& fnt, const std::string& label, const sf::Vector2f& pos, const sf::Vector2f& size, CommandT cmd)
		: font(fnt), textResolutionFactor(fontFactor), characterSize(fontSize), command(cmd), clicked(false), triggered(false), name(label)
	{
		box.setPosition(pos);
		box.setSize(size);
		box.setFillColor(normalColor);
		box.setOutlineThickness(1);
		box.setOutlineColor(normalColor);


		text.setFont(font);
		text.setString(label);
		text.setCharacterSize(characterSize * textResolutionFactor); // Resolution factor because default font resolution was fuzzy
		text.setFillColor(sf::Color(220, 220, 240));
		text.setScale({ 1 / textResolutionFactor, 1 / textResolutionFactor });
		sf::FloatRect textbox = text.getLocalBounds();
		text.setOrigin(0, 0);
		text.setPosition(
			pos.x + (size.x - textbox.width / textResolutionFactor) / 2,
			pos.y + (size.y - textbox.height / textResolutionFactor) / 2 - textbox.top / 2
		);
	}

	void draw(sf::RenderWindow& window) {
		window.draw(box);
		window.draw(text);
	}

	bool contains(const sf::Vector2f mousePos) {
		const sf::FloatRect b = box.getGlobalBounds();
		/*std::cout
			<< "Point: (" << mousePos.x << ", " << mousePos.y << ")\n"
			<< "Bounds: ["
			<< "L=" << b.left
			<< ", T=" << b.top
			<< ", R=" << b.left + b.width
			<< ", B=" << b.top + b.height
			<< "]\n"
			<< "Contains: " << b.contains(mousePos)
			<< "\n\n";*/
		return b.contains(mousePos);
	}

	void onMousePress(const sf::Vector2f& point) {
		clicked = contains(point);
		//std::cout << "Clicked = " << clicked << std::endl;
	}

	void onMouseMove(const sf::Vector2f& point) {
		bool hovering = contains(point);
		box.setFillColor(hovering ? hoverColor : normalColor);
		box.setOutlineColor(hovering ? hoverOutline : normalColor);
	}

	void onMouseRelease(const sf::Vector2f& point) {
		if (clicked && contains(point)) {
			triggered = true;
			std::cout << name << " was triggered\n";
		}

		clicked = false;
	}

	bool consumed(CommandT& outputCommand) {
		if (triggered) {
			outputCommand = command;
			triggered = false;
			std::cout << name + " Button Triggered\n\n";
			return true;
		}
		return false;
	}

	void setTextResolutionFactor(float factor) {
		textResolutionFactor = factor;
	}

	void alignTextOnLeft(float margin) {
		text.setOrigin(0, 0);
		text.setPosition(box.getPosition().x + margin / 2,
			box.getPosition().y + (box.getSize().y - text.getLocalBounds().height) * 0.5f + margin / 2);
	}

	void fitBoxWidthToText(float padding = 12.f) {
		// Get unscaled text bounds
		sf::FloatRect bounds = text.getLocalBounds();

		// Convert to actual rendered size (because you scale the text down)
		float textWidth = bounds.width / textResolutionFactor;
		float textHeight = bounds.height / textResolutionFactor;

		// Resize the box to fit text + padding
		sf::Vector2f size = box.getSize();
		size.x = textWidth + padding * 2.f;
		size.y = std::max(size.y, textHeight + padding * 2.f);
		box.setSize(size);

		// Re-center text in the resized box
		sf::Vector2f pos = box.getPosition();
		text.setPosition(
			pos.x + (size.x - textWidth) / 2.f,
			pos.y + (size.y - textHeight) / 2.f - (bounds.top / textResolutionFactor)
		);
	}
	std::string& getName() {
		return name;
	}

	void setName(std::string n) {
		name = n;
		text.setString(name);
		fitBoxWidthToText();
	}
private:
	bool clicked;
	bool triggered;
	CommandT command;
	std::string name;


	sf::Color hoverColor = sf::Color(70, 70, 85);
	sf::Color hoverOutline = sf::Color(172, 172, 172);
	sf::Color normalColor = sf::Color(45, 45, 55);
};
