#pragma once
#include "SFML/Graphics.hpp"
class InputHandler {
public:
    virtual ~InputHandler() = default;

    virtual void onMousePress(const sf::Vector2f& worldPos) {}
    virtual void onMouseRelease(const sf::Vector2f& worldPos) {}
    virtual void onMouseMove(const sf::Vector2f& worldPos) {}
    virtual void onScroll(const sf::Event::MouseWheelScrollEvent& event) {}
    virtual void onKeyPress(const sf::Event::KeyEvent& event) {}
    virtual bool consumesEmptyClicks() const { return true; }


    virtual bool shouldRelease() const { return false; }
};

