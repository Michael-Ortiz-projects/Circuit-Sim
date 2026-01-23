#pragma once
#include <vector>
#include "Button.h"
#include "DropdownMenu.h"
#include "AssetManager.h"
#include <optional>
#include "Renderer.h"
#include "../Config.h"
#include <queue>
#include "Dialog.h"

class UI_Manager
{
public:

	std::unordered_map<MenuID, DropdownMenu> menu_map;

	UI_Manager(Renderer& rend);

	void initialize(AssetManager& assets);

	bool handleEvent(const sf::Event& event);

	bool pollCommand(UICommand& outputCommand);

	bool onMousePress(const sf::Vector2f& pixelPos);

	bool onMouseMove(const sf::Vector2f& pixelPos);

	bool onMouseRelease(const sf::Vector2f& pixelPos);

	void draw();
	

	// modal control

	void openEditDialog(Component* target);
	void closeEditDialog();

	bool hasActiveDialog() const;
	const std::string getEditDialogText() const;

private:
	std::unique_ptr<Dialog> activeDialog;


	Renderer& renderer;
	bool modalActive = false;
	std::queue<UICommand> modalCommands;


};

