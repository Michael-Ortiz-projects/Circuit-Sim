#include "Button.h"


Button::Button(const sf::Vector2f& position, const sf::Font& font, const std::string& text, std::function<void()> on_click_function) {
        textBox = TextBox(position, font);
        textBox.text.setString(text);
        textBox.setTextBoundingBox();
        click_function = on_click_function;
    }

void Button::draw(sf::RenderWindow& window) {
        textBox.box.setFillColor(isHovered ? hoverColor : normalColor);
        textBox.draw(window);
    }

bool Button::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
        sf::Vector2f mouse_position = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        if (event.type == sf::Event::MouseMoved) {
            isHovered = textBox.box.getGlobalBounds().contains(mouse_position);

        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && textBox.box.getGlobalBounds().contains(mouse_position)) {
            if (click_function) click_function();
            return true;
        }
        return false;
    }
//nothing