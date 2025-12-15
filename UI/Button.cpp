#include "Button.h"
#include "../Config.h"


Button::Button(sf::Font& fnt, const std::string& label, const sf::Vector2f& pos, const sf::Vector2f& size, UICommand cmd)
	: font(fnt), textResolutionFactor(fontFactor), characterSize(fontSize), command(cmd), clicked(false), triggered(false), name(label)
{
	box.setPosition(pos);
	box.setSize(size);
	box.setFillColor(normalColor);
	box.setOutlineThickness(1);


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

void Button::draw(sf::RenderWindow& window) {
	window.draw(box);
	window.draw(text);
}

bool Button::contains(const sf::Vector2f mousePos) {
	return box.getGlobalBounds().contains(mousePos);
}

void Button::onMousePress(const sf::Vector2f& point) {
	clicked = contains(point);
}

void Button::onMouseMove(const sf::Vector2f& point) {
	bool hovering = contains(point);
	box.setFillColor(hovering ? hoverColor : normalColor);
	box.setOutlineColor(hovering ? hoverOutline : normalColor);
}

void Button::onMouseRelease(const sf::Vector2f& point) {
	if (clicked && contains(point)) {
		triggered = true;
	}

	clicked = false;
}

bool Button::consumed(UICommand& outputCommand) {
	if (triggered) {
		outputCommand = command;
		triggered = false;
		std::cout << name + " Button Triggered\n\n";
		return true;
	}
	return false;
}

void Button::setTextResolutionFactor(float factor) {
	textResolutionFactor = factor;
}

void Button::alignTextOnLeft(float margin) {
	text.setOrigin(0, 0);
	text.setPosition(box.getPosition().x + margin / 2,
		box.getPosition().y + (box.getSize().y - text.getLocalBounds().height) * 0.5f + margin / 2);
}

std::string Button::getName() {
	return name;
}