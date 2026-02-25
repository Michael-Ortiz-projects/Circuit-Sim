#include "ElementStateDisplay.h"

ElementStateDisplay::ElementStateDisplay(sf::Vector2f position, sf::Vector2f size) {
	box.setPosition(position);
	box.setSize(size);
	box.setOutlineThickness(1);
	box.setOutlineColor(sf::Color(120, 120, 120));
	box.setFillColor(sf::Color(40, 40, 50));
}

void ElementStateDisplay::setState(const ElementState& state, sf::Font& font) {
	std::cout << "stateDisplay.setState() running\n";
	textBoxes.clear();
	switch (state.type) {
	case ComponentType::Resistor:
	case ComponentType::VoltageSource:
	case ComponentType::CurrentSource:
	case ComponentType::Capacitor:
	case ComponentType::Inductor:
		std::cout << "case path taken\n";
		for (int i = 0; i < 5; i++) {
			TextBox tb(font, sf::Vector2f(box.getPosition().x, box.getPosition().y + 25 * i), sf::Vector2f(100, 20), 12, "", false);
			textBoxes.push_back(tb);
		}
		std::cout << "setText loop started\n";
		textBoxes[0].setText("Type: " + Debug::ComponentType_to_String(state.type));
		textBoxes[1].setText("Label: " + state.label);
		textBoxes[2].setText("Value: " + std::to_string(state.value));
		textBoxes[3].setText("Current: " + std::to_string(state.current));
		textBoxes[4].setText("Voltage Delta : " + std::to_string(state.voltageDelta));
		std::cout << "setText loop finished\n";
		break;
	case ComponentType::Ground:
		TextBox tb(font, sf::Vector2f(box.getPosition().x, box.getPosition().y + 25), sf::Vector2f(100, 20), 12, "", false);
		textBoxes.push_back(tb);
		textBoxes[0].setText("Type: Ground");
		break;
	}
}

void ElementStateDisplay::setPosition(sf::Vector2f position) {
	box.setPosition(position);
	for (int i = 0; i < textBoxes.size(); i++) {
		textBoxes[i].setPosition(sf::Vector2f(position.x, position.y + 25 * i));
	}
}

void ElementStateDisplay::draw(sf::RenderWindow& window) {
	window.draw(box);
	for (auto& T : textBoxes) {
		T.draw(window);
	}
}
