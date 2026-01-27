#pragma once
#include "InputHandler.h"
#include "../UI/UI_Manager.h"
#include "SaveManager.h"
#include <Windows.h>
#include <commdlg.h>

class SaveCircuitHandler : public InputHandler {
public:
	SaveCircuitHandler(UI_Manager& ui, SaveManager& save, Circuit& Circuit, std::string& workingFilePath);

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
   
private:
    UI_Manager& ui;
    SaveManager& saveManager;
    Circuit& circuit;
    std::string& workingFilePath;
    bool finished = false;
};