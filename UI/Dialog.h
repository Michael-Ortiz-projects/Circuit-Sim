#pragma once
#include "SFML/Graphics.hpp"
#include "Renderer.h"
#include "TextBox.h"

class Dialog {
public:
    Dialog(sf::Vector2f position, sf::Vector2f size, Renderer& renderer);

    void addButton(const std::string& label, UICommand command, const sf::Vector2f& pos, const sf::Vector2f& size);
    void addLabel(const std::string& text, const sf::Vector2f& pos, unsigned int charSize = 16);
    void addTextBox(const std::string& initialText, const sf::Vector2f& pos, const sf::Vector2f& size);

    void open();
    void close();
    bool isOpen() const;

    void handleEvent(const sf::Event& event);
    void draw();

    std::string getText(int index) const;
    bool buttonPressed(UICommand& outCommand);

private:
    Renderer& renderer;
    sf::RectangleShape panel;
    std::vector<Button> buttons;
    std::vector<sf::Text> labels;
    std::vector<TextBox> textboxes;

    bool active = false;
};
