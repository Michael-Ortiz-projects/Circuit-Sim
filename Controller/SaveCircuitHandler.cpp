#include "SaveCircuitHandler.h"

SaveCircuitHandler::SaveCircuitHandler(EditorUI_Manager& ui, SaveManager& saver, Circuit& Circuit, AssetManager& Assets, std::string& workingFilePath)
	: ui(ui), saveManager(saver), circuit(Circuit), assets(Assets), workingFilePath(workingFilePath) { }


void SaveCircuitHandler::onMousePress(const sf::Vector2f& worldPos) {

}

void SaveCircuitHandler::onMouseRelease(const sf::Vector2f& worldPos) {

}

void SaveCircuitHandler::onMouseMove(const sf::Vector2f& worldPos) {

}

void SaveCircuitHandler::onScroll(const sf::Event::MouseWheelScrollEvent& event) {

}

void SaveCircuitHandler::onKeyPress(const sf::Event::KeyEvent& event) {

}

void SaveCircuitHandler::saveDialog() {
    std::string path = selectSaveFile();
    if (path.empty()) return;
   
    if (!saveManager.saveCircuitToFile(circuit, path)) std::cout << "Failed to save circuit to path: " << path << std::endl;
    workingFilePath = path;
}

std::string SaveCircuitHandler::selectSaveFile() {
    char fileName[MAX_PATH] = "";

    OPENFILENAMEA ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = "Circuit Files (*.ckt)\0*.ckt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = "ckt";

    if (GetSaveFileNameA(&ofn))
        return fileName;

    return "";
}

void SaveCircuitHandler::loadDialog() {
    std::string path = selectLoadFile();
    if (path.empty()) return;
    CircuitData data;
    if (!saveManager.loadCircuitFromFile(data, path, assets)) {
       std::cout << "Failed to load circuit from path: " << path << std::endl;
    }
    circuit.setCircuitData(data);

    workingFilePath = path;
}

std::string SaveCircuitHandler::selectLoadFile() {
    char filename[MAX_PATH] = "";

    OPENFILENAMEA ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = "Circuit Files (*.ckt)\0*.ckt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    ofn.lpstrDefExt = "ckt";

    if (GetOpenFileNameA(&ofn)) {
        std::cout << filename;
        return filename;
    }

    return "";
}

void SaveCircuitHandler::saveCurrentWorkingFile() {
    saveManager.saveCircuitToFile(circuit, workingFilePath);
}

CircuitData SaveCircuitHandler::loadFromFile(const std::string& path) {
    CircuitData data;
    if (!saveManager.loadCircuitFromFile(data, path, assets)) std::cout << "SaveCircuitHandler::loadFromFile() failed\n";
    workingFilePath = path;
    return data;
}