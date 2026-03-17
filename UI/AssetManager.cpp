#include "AssetManager.h"

AssetManager::AssetManager() {


	loadTexture("normal_VS", "UI/Assets/Schematic_Voltage_Source.png");

	loadTexture("normal_R", "UI/Assets/Schematic_Resistor.png");

	loadTexture("normalCS", "UI/Assets/Schematic_Current_Source.png");

	loadTexture("normal_CAP", "UI/Assets/Schematic_Capacitor.png");

	loadTexture("normal_L", "UI/Assets/Schematic_Inductor.png");

	loadTexture("normal_SO", "UI/Assets/Schematic_Open_Switch.png");

	loadTexture("normal_SC", "UI/Assets/Schematic_Closed_Switch.png");

	loadTexture("normal_G", "UI/Assets/Schematic_Ground.png");

	loadTexture("dependent_VS", "UI/Assets/Schematic_Dep_Voltage_Source.png");

	loadTexture("dependent_CS", "UI/Assets/Schematic_Dep_Current_Source.png");

	loadTexture("AC_CS", "UI/Assets/AC_CurrentSource.png");

	loadTexture("AC_VS", "UI/Assets/AC_VoltageSource.png");

	std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	mainFont.loadFromFile("UI/Fonts/RobotoSlab-Regular.ttf");
}

AssetManager::~AssetManager() {}

bool AssetManager::loadTexture(const std::string& textureName, const std::string& filePath) {
	sf::Texture texture;
	if (texture.loadFromFile(filePath)) {
		texture.setSmooth(true);
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
	std::cout << "this ran\n";
	switch (type) {
		case ComponentType::Resistor:
			return m_textures.at("normal_R");
		
		case ComponentType::Capacitor:
			return m_textures.at("normal_CAP");
		
		case ComponentType::CurrentSource:
			return m_textures.at("normalCS");
		
		case ComponentType::VCVS:
		case ComponentType::CCVS:
			return m_textures.at("dependent_VS");

		case ComponentType::VCCS:
		case ComponentType::CCCS:
			return m_textures.at("dependent_CS");

		case ComponentType::ACCurrentSource:
			return m_textures.at("AC_CS");

		case ComponentType::ACVoltageSource:
			return m_textures.at("AC_VS");

		case ComponentType::Inductor:
			return m_textures.at("normal_L");
		
		case ComponentType::VoltageSource:
			return m_textures.at("normal_VS");
		
		case ComponentType::Switch:
			return m_textures.at("normal_SO");
		
		case ComponentType::Ground:
			return m_textures.at("normal_G");

		default:
			std::cout << "getTexture() failed to find texture\n";
			break;
		
	}
}