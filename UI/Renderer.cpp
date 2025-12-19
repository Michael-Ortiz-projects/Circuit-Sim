#include "Renderer.h"

Renderer::Renderer(sf::RenderWindow& Window, AssetManager& Assets, Grid& Grid)
    : assets(Assets), window(Window), grid(Grid) {
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
        if (c.selected)
            window.draw(c.getHitBox());
    }
}

void Renderer::drawUI(std::unordered_map<MenuID, DropdownMenu> menu_map) {
    window.setView(UIView);
    for (auto& m : menu_map) {
        m.second.draw(window);
    }
}

sf::View& Renderer::getCanvasView() {
    return canvasView;
}

sf::View& Renderer::getUIView() {
    return UIView;
}