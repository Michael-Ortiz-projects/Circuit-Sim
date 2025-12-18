#include "Renderer.h"

Renderer::Renderer(sf::RenderWindow& Window, AssetManager& Assets, Grid& Grid, UI_Manager& Manager)
    : assets(Assets), window(Window), grid(Grid), uiManager(Manager) {
    sf::Vector2f windowSize(window.getSize());

    canvasView.setSize(windowSize);
    canvasView.setCenter(windowSize * 0.5f);
    canvasView.zoom(defaultZoom);

    UIView.setSize(windowSize);
    UIView.setCenter(windowSize * 0.5f);
}

void Renderer::drawCanvas(std::vector<SchematicComponent>& components) {
    window.setView(canvasView);
    grid.draw(window, canvasView);

    

    for (auto& c : components) {
        window.draw(c.getSprite());
    }
}

void Renderer::drawUI() {
    window.setView(UIView);
    for (auto& m : uiManager.menu_map) {
        m.second.draw(window);
    }
}

sf::View& Renderer::getCanvasView() {
    return canvasView;
}

sf::View& Renderer::getUIView() {
    return UIView;
}