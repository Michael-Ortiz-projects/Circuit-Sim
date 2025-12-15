#include "ComponentRenderer.h"

ComponentRenderer::ComponentRenderer(AssetManager& assets)
    : m_assets(assets)
{
}



void ComponentRenderer::drawComponents(sf::RenderWindow& window, std::vector<SchematicComponent>& components){
    for (auto& comp : components) {
        window.draw(comp.getSprite());
    }
}