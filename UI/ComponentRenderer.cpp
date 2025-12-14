#include "ComponentRenderer.h"

ComponentRenderer::ComponentRenderer(AssetManager& assets)
    : m_assets(assets)
{
}



void ComponentRenderer::drawComponents(sf::RenderWindow& window, std::vector<SchematicComponent>& components){
    for (auto& comp : components) {
        sf::Texture& texture = m_assets.getTexture(comp.getType()); //should probably put texture changing logic in another function

        comp.getSprite().setTexture(texture, true);

        window.draw(comp.getSprite());
    }
}