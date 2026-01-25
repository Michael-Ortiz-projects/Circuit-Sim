#include "Dialog.h"

Dialog::Dialog(sf::Vector2f position, sf::Vector2f size, Renderer& r)
    : renderer(r)
{
    panel.setSize(size);
    panel.setFillColor(sf::Color(30, 30, 40));
    panel.setOutlineThickness(2.f);
    panel.setOutlineColor(sf::Color(150, 150, 150));
    panel.setPosition(position);
}

void Dialog::open() {
    active = true;
    std::cout << "Dialog open\n";
}

void Dialog::close() {
    std::cout << "Dialog is closing\n";
    active = false;
}

bool Dialog::isOpen() const {
    //std::cout << "dialog.open = " << active << std::endl;
    return active;
}

void Dialog::addLabel(const std::string& str, const sf::Vector2f& pos, unsigned int size) {
    sf::Text t;
    t.setFont(renderer.getAssets().mainFont);
    t.setString(str);
    t.setCharacterSize(size);
    t.setFillColor(sf::Color::White);
    t.setPosition(panel.getPosition() + pos);
    labels.push_back(t);
}

void Dialog::addTextBox(const std::string& initial, const sf::Vector2f& pos, const sf::Vector2f& size, int textSize, bool outline) {
    textboxes.emplace_back(renderer.getAssets().mainFont, panel.getPosition() + pos, size, textSize, initial, outline);
}

void Dialog::addButton(const std::string& label,
    UICommand cmd,
    const sf::Vector2f& pos,
    const sf::Vector2f& size)
{
    buttons.emplace_back(renderer.getAssets().mainFont, label, panel.getPosition() + pos, size, cmd);
}

void Dialog::handleEvent(const sf::Event& event) {
    if (!active) return;

    auto& window = renderer.getWindow();

    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mouse = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y }, renderer.getUIView());

        for (auto& tb : textboxes)
            tb.setActive(tb.contains(mouse));
    }

    for (auto& tb : textboxes)
        tb.handleEvent(event);

    if (event.type == sf::Event::MouseMoved ||
        event.type == sf::Event::MouseButtonPressed ||
        event.type == sf::Event::MouseButtonReleased)
    {
        sf::Vector2f moveMouse = window.mapPixelToCoords( { event.mouseMove.x, event.mouseMove.y }, renderer.getUIView());
        sf::Vector2f buttonMouse = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y }, renderer.getUIView());

        for (auto& b : buttons) {
            b.onMouseMove(moveMouse);
            if (event.type == sf::Event::MouseButtonPressed)
                b.onMousePress(buttonMouse);
            if (event.type == sf::Event::MouseButtonReleased)
                b.onMouseRelease(buttonMouse);
        }
    }
}

bool Dialog::buttonPressed(UICommand& outCmd) {
    for (auto& b : buttons) {
        if (b.consumed(outCmd))
            return true;
    }
    return false;
}

std::string Dialog::getText(int index) const {
    if (index >= textboxes.size()) return " default text";
    return textboxes[index].getText();
}

void Dialog::draw() {
    if (!active) return;

    auto& window = renderer.getWindow();

    window.draw(panel);

    for (auto& l : labels)
        window.draw(l);

    for (auto& tb : textboxes)
        tb.draw(window);

    for (auto& b : buttons)
        b.draw(window);
}

