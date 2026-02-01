#pragma once
#include "InputHandler.h"
#include "../UI/UI_Manager.h"
#include "SaveManager.h"
#include <Windows.h>
#include <commdlg.h>
#include "../Core/Circuit.h"

class SaveCircuitHandler : public InputHandler {
public:
	SaveCircuitHandler(UI_Manager& ui, SaveManager& save, Circuit& Circuit, AssetManager& Assets, std::string& workingFilePath);

    void onMousePress(const sf::Vector2f& worldPos) override;
    void onMouseRelease(const sf::Vector2f& worldPos) override;
    void onMouseMove(const sf::Vector2f& worldPos) override;
    void onScroll(const sf::Event::MouseWheelScrollEvent& event) override;
    void onKeyPress(const sf::Event::KeyEvent& event) override;



    bool shouldRelease() const { return false; }

    void saveCurrentWorkingFile();
    void saveDialog();
    std::string selectSaveFile();

    void loadDialog();
    std::string selectLoadFile();

    CircuitData loadFromFile(const std::string& path);
   
private:
    UI_Manager& ui;
    SaveManager& saveManager;
    AssetManager& assets;
    Circuit& circuit;
    std::string& workingFilePath;
    bool finished = false;
};