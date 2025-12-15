#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "SchematicComponent.h"
#include "../UI/AssetManager.h"

class ComponentRenderer {
public:
    ComponentRenderer(AssetManager& assets);

    void drawComponents(sf::RenderWindow& window, std::vector<SchematicComponent>& components);

private:
    AssetManager& m_assets;
};

