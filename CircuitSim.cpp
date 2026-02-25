#include "Controller/EditorController.h"
#include "Controller/SimulationController.h"
#include "UI/EditorUI_Manager.h"
#include "UI/SimulationUI_Manager.h"
#include "UI/EditorRenderer.h"
#include "UI/SimulationRenderer.h"



//I now need to actually create the simulation for DC operating point
Grid grid(gridSize);
std::string currentWorkingFilePath;
AssetManager assets;
sf::RenderWindow editorWindow(sf::VideoMode::getDesktopMode(), "Circuit Sim", sf::Style::None);
sf::RenderWindow simulationWindow;

int main() {   
    
    Circuit circuit(assets);

    Renderer editorRenderer(editorWindow, assets);
    Renderer simRenderer(simulationWindow, assets);

    EditorRenderer schematicRenderer(editorRenderer, grid);
    SimulationRenderer simulationRenderer(simRenderer);

    EditorUI_Manager editorUI(editorRenderer);
    SimulationUI_Manager simUI(simRenderer);
    editorUI.initialize();

    EditorController editorController(circuit, assets, editorRenderer, editorUI, currentWorkingFilePath);
    SimulationController simulationController(circuit, simUI);

    //initializing circuit data
    char filename[MAX_PATH] = "SeriesRLCDCCircuit.ckt";
    CircuitData initializedData = editorController.saveCircuitHandler.loadFromFile(filename);
    circuit.setCircuitData(initializedData);


    ScrollTextBox box(assets.mainFont, 18, { 50, 1000 }, { 400, 200 });
    box.setString("Long text...\nLine 2...\nLine 3...\nLine 4...\nLine 5...\nLine 6...\nLine 7...\nLine 2...\nLine 3...\nLine 4...\nLine 5...\nLine 6...\nLine 7");

    int simColorDepth = 90;

    while (editorWindow.isOpen()) {
        sf::Event event;
        while (editorWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                editorWindow.close();                
                return 1;
            }
            editorUI.handleEvent(event);

            editorController.handleEvent(event);
            editorController.rebuildSchematicComponents();
            box.handleEvent(event, editorWindow);

        }

        // ----- Simulation window -----
        if (simulationWindow.isOpen()) {
            sf::Event simEvent;
            while (simulationWindow.pollEvent(simEvent)) {
                if (simEvent.type == sf::Event::Closed) {
                    simulationWindow.close();
                }
                simUI.handleEvent(simEvent);
                simulationController.handleEvent(simEvent);
            }
        }

        EditorUICommand cmd;

        while (editorUI.pollCommand(cmd)) {
            CircuitData data;
            switch (cmd) {
            case EditorUICommand::PlaceVoltageSource:
            case EditorUICommand::PlaceResistor:
            case EditorUICommand::PlaceCurrentSource:
            case EditorUICommand::PlaceCapacitor:
            case EditorUICommand::PlaceInductor:
            case EditorUICommand::PlaceSwitch:
            case EditorUICommand::PlaceGround:
                Debug::UICommand(cmd);
                Debug::setHandler("PlaceHandler");
                editorController.setHandler(&editorController.placeHandler, cmd);
                break;

            case EditorUICommand::OpenNewFile:
                circuit.setCircuitData(data);
                currentWorkingFilePath.clear();
                break;
            case EditorUICommand::OpenFile:
                editorController.saveCircuitHandler.loadDialog();
                break;
            case EditorUICommand::SaveFile:
                if (currentWorkingFilePath.empty())
                    editorController.saveCircuitHandler.saveDialog();
                else {
                    editorController.saveCircuitHandler.saveCurrentWorkingFile();
                }
                break;
            case EditorUICommand::SaveFileAs:
                editorController.saveCircuitHandler.saveDialog();
                break;
            case EditorUICommand::ExitProgram:
                editorWindow.close();
                break;
            case EditorUICommand::OpenSimulationWindow:
                if (!simulationWindow.isOpen()) {

                    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

                    simulationWindow.create(sf::VideoMode(desktop.width, desktop.height), "Simulation Window", sf::Style::Default);
                    circuit.getSimulator().setSystem(circuit.getNetlistComponents(), circuit.getElectricalNodes());
                    simRenderer.setViews();
                    simUI.initialize();


                }
                break;
                
            default:
                break;
            }
        }

        
        
        if (editorWindow.isOpen()) {
            schematicRenderer.drawCanvas(circuit.getSchematicComponents(), circuit.getWires(), editorController.selectionBoxHandler.getRect());
            editorUI.draw();
            editorWindow.draw(box);

            editorWindow.display();
        }
        
        // ----- Draw simulation window -----
        if (simulationWindow.isOpen()) {
            simulationWindow.clear(simulationWindowBackgroundColor);
            simUI.draw();
            simulationWindow.display();
        }
    }
    
    return 0;
}