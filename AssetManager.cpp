#include "AssetManager.h"
#include <iostream>

AssetManager::AssetManager() {
	loadTexture("highlight_VS", "images/highlightedVS.png");
	loadTexture("normal_VS", "images/normalVS.png");
	
	loadTexture("highlight_R", "images/highlighted_R.png");
	loadTexture("normal_R", "images/normal_R.png");

	loadTexture("normalCS", "images/normalCS.png");
	loadTexture("highlightedCS", "images/highlightedCS.png");

	loadTexture("normal_CAP", "images/normalCAP.png");
	loadTexture("highlighted_CAP", "images/highlighted_CAP.png");

	loadTexture("normal_L", "images/normalL.png");
	loadTexture("highlighted_L", "images/highlightedL.png");

	loadTexture("normal_SO", "images/normal_SO.png");
	loadTexture("highlighted_SO", "images/highlighted_SO.png");

	loadTexture("normal_SC", "images/normal_SC.png");
	loadTexture("highlighted_SC", "images/highlighted_SC.png");

}

AssetManager::~AssetManager() {}

bool AssetManager::loadTexture(const std::string& textureName, const std::string& filePath) {
	sf::Texture texture;
	if (texture.loadFromFile(filePath)) {
		m_textures.emplace(textureName, std::move(texture));
		std::cout << "Loaded " << textureName << " successfully\n";
		return true;
	}
	else {
		std::cerr << "Error loading texture: " << filePath << std::endl;
		return false;
	}
}

sf::Texture& AssetManager::getTexture(const std::string textureName) {
	return m_textures.at(textureName);
}