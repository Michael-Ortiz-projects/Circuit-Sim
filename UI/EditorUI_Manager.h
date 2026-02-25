#pragma once
#include <vector>
#include "DropdownMenu.h"
#include <optional>
#include "Renderer.h"
#include "../Config.h"
#include "Dialog.h"
#include <sstream>
#include <iomanip>

class EditorUI_Manager
{
public:

	std::unordered_map<MenuID, DropdownMenu<EditorUICommand>> menu_map;

	EditorUI_Manager(Renderer& rend);

	void initialize();

	bool handleEvent(const sf::Event& event);

	bool pollCommand(EditorUICommand& outputCommand);

	bool onMousePress(const sf::Vector2f& pixelPos);

	bool onMouseMove(const sf::Vector2f& pixelPos);

	bool onMouseRelease(const sf::Vector2f& pixelPos);

	void draw();
	

	// modal control

	void openEditDialog(NetlistComponent* target);
	void closeEditDialog();

	std::string formatValue(double value);

	std::string formatValueWithSuffix(double value);


	bool hasActiveDialog() const;
	const EditDialogResult getEditDialogText() const;

private:
	std::unique_ptr<Dialog<EditorUICommand>> activeDialog;


	Renderer& renderer;
	bool modalActive = false;
};

