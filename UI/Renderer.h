#pragma once
#include "AssetManager.h"

class Renderer {
public:
    Renderer(sf::RenderWindow& Window, AssetManager& Assets);

    void setViews();

    sf::View& getCanvasView() { return canvasView; }

    sf::View& getUIView() { return UIView; }

    sf::RenderWindow& getWindow() { return window; }

    AssetManager& getAssets() { return assets; }
private:

    AssetManager& assets;
    sf::RenderWindow& window;
    sf::View canvasView;
    sf::View UIView;
};
