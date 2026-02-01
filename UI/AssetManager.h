#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <map>
#include <string>
#include "../Core/NetlistComponent.h"

class AssetManager {
public:
	sf::Font mainFont;

	AssetManager();
	~AssetManager();

	bool loadTexture(const std::string& textureName, const std::string& filePath);

	sf::Texture& getTexture(ComponentType type);


private:
	std::map<std::string, sf::Texture> m_textures;
};


