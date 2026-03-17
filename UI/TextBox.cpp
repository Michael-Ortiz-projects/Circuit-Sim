#include "TextBox.h"

TextBox::TextBox(sf::Font& font, const sf::Vector2f& pos, const sf::Vector2f& boxSize, int textSize, const std::string& initialText, bool outline)
    : value(initialText) {
    box.setPosition(pos);
    box.setSize(boxSize);
    if (outline) {
        box.setFillColor(sf::Color(40, 40, 50));
        box.setOutlineThickness(1.f);
        box.setOutlineColor(sf::Color(120, 120, 120));
    }
    else {
        box.setFillColor(sf::Color::Transparent);
    }
 
    text.setFont(font);
    text.setCharacterSize(textSize * fontFactor);
    text.setScale(sf::Vector2f(1.0f/ fontFactor, 1.0f/ fontFactor));
    text.setFillColor(sf::Color::White);
    text.setPosition(pos.x + 5.f, pos.y + 5.f);
    text.setString(value);
}

void TextBox::setActive(bool a) {
    active = a;
    box.setOutlineColor(active ? sf::Color::Yellow
        : sf::Color(120, 120, 120));
}

bool TextBox::isActive() const {
    return active;
}

void TextBox::setFont(sf::Font& font) {
    text.setFont(font);
}

bool TextBox::handleEvent(const sf::Event& event) { 
    if (!active) return false;

    if (event.type == sf::Event::TextEntered) {
        uint32_t c = event.text.unicode;

        if (c == 8) { // backspace
            if (!value.empty())
                value.pop_back();
        }
        else if (c == 13) { // enter
            std::cout << "enter Pressed\n";
            return true;
        }
        else if (c >= 32 && c < 127) {
            value += static_cast<char>(c);
        }

        text.setString(value);
    }
    return false;
}

void TextBox::draw(sf::RenderWindow& window) {
    window.draw(box);
    window.draw(text);
}

const std::string& TextBox::getText() const {
    return value;
}

void TextBox::setText(const std::string& str) {
    value = str;
    text.setString(value);
}

bool TextBox::contains(const sf::Vector2f& point) const {
    return box.getGlobalBounds().contains(point);
}

void TextBox::setPosition(const sf::Vector2f& pos) {
    box.setPosition(pos);
    text.setPosition(pos.x, pos.y);
}

void TextBox::setSize(const sf::Vector2f& size) {
    box.setSize(size);
}

sf::RectangleShape TextBox::getTextBoundsRect()
{
    sf::FloatRect bounds = text.getGlobalBounds();

    sf::RectangleShape rect;
    rect.setPosition(bounds.left, bounds.top);
    rect.setSize({ bounds.width, bounds.height });

    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(sf::Color::Red);
    rect.setOutlineThickness(1.f);

    return rect;
}


ScrollTextBox::ScrollTextBox(const sf::Font& font, unsigned int charSize, sf::Vector2f position, sf::Vector2f size) {
    background.setPosition(position);
    background.setSize(size);
    background.setFillColor(sf::Color(30, 30, 30));
    background.setOutlineThickness(1.f);
    background.setOutlineColor(sf::Color::White);

    text.setFont(font);
    text.setCharacterSize(charSize);
    text.setFillColor(sf::Color::White);
    text.setPosition(position.x + 5.f, position.y + 5.f);

    viewRect = sf::FloatRect(position.x, position.y, size.x, size.y);
    view.reset(viewRect);

    updateView();
}

ScrollTextBox::ScrollTextBox() {
    background.setFillColor(sf::Color(30, 30, 30));
    background.setOutlineThickness(1.f);
    background.setOutlineColor(sf::Color::White);
    text.setFillColor(sf::Color::White);
}

void ScrollTextBox::setString(const std::string& str) {
    text.setString(str);
}

void ScrollTextBox::appendString(const std::string str) {
    text.setString(text.getString() + str);
}


void ScrollTextBox::setPosition(sf::Vector2f pos) {
    background.setPosition(pos);
    text.setPosition(pos.x + 5.f, pos.y + 5.f);

    viewRect.left = pos.x;
    viewRect.top = pos.y;
    updateView();
}

void ScrollTextBox::setSize(sf::Vector2f size) {
    background.setSize(size);
    viewRect.width = size.x;
    viewRect.height = size.y;
    updateView();
}

void ScrollTextBox::setFont(sf::Font& font) {
    text.setFont(font);
}

void ScrollTextBox::setCharSize(unsigned int size) {
    text.setCharacterSize(size);
}

void ScrollTextBox::updateView() {
    view.reset(viewRect);
}

float ScrollTextBox::getContentHeight() const {
    sf::FloatRect bounds = text.getLocalBounds();
    return bounds.height + 10.f; // padding
}

void ScrollTextBox::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
            //sf::Vector2f mouseWorld = window.mapPixelToCoords( { event.mouseWheelScroll.x, event.mouseWheelScroll.y } );
            sf::Vector2f mouseWorld = sf::Vector2f(event.mouseWheelScroll.x, event.mouseWheelScroll.y);
            sf::FloatRect b = background.getGlobalBounds();
            std::cout << "mouseWorld = (" << mouseWorld.x << ", " << mouseWorld.y << ")\n";
            std::cout
            << "Point: (" << mouseWorld.x << ", " << mouseWorld.y << ")\n"
            << "Bounds: ["
            << "L=" << b.left
            << ", T=" << b.top
            << ", R=" << b.left + b.width
            << ", B=" << b.top + b.height
            << "]\n"
            << "Contains: " << b.contains(mouseWorld)
            << "\n\n";
            // Only scroll if mouse is over the box
            if (background.getGlobalBounds().contains(mouseWorld)) {
                float delta = -event.mouseWheelScroll.delta * scrollSpeed;
                scrollOffset += delta;
                std::cout << "scroll offset = " << scrollOffset << "\ndelta = " << delta << "\n\n";

                float contentHeight = getContentHeight();
                float boxHeight = background.getSize().y;
                std::cout << "contentHeight = " << contentHeight << "\nboxHeight = " << boxHeight << "\n\n";

                float maxScroll = std::max(0.f, contentHeight - boxHeight);
                scrollOffset = std::clamp(scrollOffset, 0.f, maxScroll);

                // Move text vertically
                sf::Vector2f pos = background.getPosition();
                text.setPosition(pos.x + 5.f, pos.y + 5.f - scrollOffset);
            }
        }
    }
}

void ScrollTextBox::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // Draw background
    target.draw(background, states);

    // Save old view
    sf::View oldView = target.getView();

    // Set clipping view
    sf::View clipped = view;
    // Convert world rect to viewport (0..1)
    sf::Vector2f targetSize(target.getSize());
    clipped.setViewport(sf::FloatRect(
        viewRect.left / targetSize.x,
        viewRect.top / targetSize.y,
        viewRect.width / targetSize.x,
        viewRect.height / targetSize.y
    ));

    target.setView(clipped);
    target.draw(text, states);
    target.setView(oldView);
}
