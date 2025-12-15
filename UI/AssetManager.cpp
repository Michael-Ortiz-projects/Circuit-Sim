#include "AssetManager.h"

AssetManager::AssetManager() {


	loadTexture("highlight_VS", "UI/Assets/highlightedVS.png");
	loadTexture("normal_VS", "UI/Assets/normalVS.png");

	loadTexture("highlight_R", "UI/Assets/highlighted_R.png");
	loadTexture("normal_R", "UI/Assets/normal_R.png");

	loadTexture("normalCS", "UI/Assets/normalCS.png");
	loadTexture("highlightedCS", "UI/Assets/highlightedCS.png");

	loadTexture("normal_CAP", "UI/Assets/normalCAP.png");
	loadTexture("highlighted_CAP", "UI/Assets/highlighted_CAP.png");

	loadTexture("normal_L", "UI/Assets/normalL.png");
	loadTexture("highlighted_L", "UI/Assets/highlightedL.png");

	loadTexture("normal_SO", "UI/Assets/normal_SO.png");
	loadTexture("highlighted_SO", "UI/Assets/highlighted_SO.png");

	loadTexture("normal_SC", "UI/Assets/normal_SC.png");
	loadTexture("highlighted_SC", "UI/Assets/highlighted_SC.png");
	std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	mainFont.loadFromFile("UI/Fonts/RobotoSlab-Regular.ttf");
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

sf::Texture& AssetManager::getTexture(ComponentType type) {
	switch (type) {
		case ComponentType::Resistor: {
			return m_textures.at("normal_R");
		}
		case ComponentType::Capacitor: {
			return m_textures.at("normal_CAP");
		}
		case ComponentType::CurrentSource: {
			return m_textures.at("normalCS");
		}
		case ComponentType::Inductor: {
			return m_textures.at("normal_L");
		}
		case ComponentType::VoltageSource: {
			return m_textures.at("normal_VS");
		}
		case ComponentType::Switch: {
			return m_textures.at("normal_SO");
		}
	}
}