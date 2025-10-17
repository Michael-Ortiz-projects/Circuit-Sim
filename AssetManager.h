#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>

class AssetManager {
public:
	AssetManager();
	~AssetManager();

	bool loadTexture(const std::string& textureName, const std::string& filePath);

	sf::Texture& getTexture(const std::string textureName);


private:
	std::map<std::string, sf::Texture> m_textures;
};