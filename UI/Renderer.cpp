#include "Renderer.h"



Renderer::Renderer(sf::RenderWindow& Window, AssetManager& Assets)
    : assets(Assets), window(Window) {
    sf::Vector2f windowSize(window.getSize());

    canvasView.setSize(windowSize);
    canvasView.setCenter(windowSize * 0.5f);
    canvasView.zoom(defaultZoom);

    UIView.setSize(windowSize);
    UIView.setCenter(windowSize * 0.5f);
}

void Renderer::setViews() {
    sf::Vector2f windowSize(window.getSize());

    canvasView.setSize(windowSize);
    canvasView.setCenter(windowSize * 0.5f);
    canvasView.zoom(defaultZoom);

    UIView.setSize(windowSize);
    UIView.setCenter(windowSize * 0.5f);
}