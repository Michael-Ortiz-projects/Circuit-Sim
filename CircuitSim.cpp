#include "Controller/EditorController.h"
#include "Controller/SimulationController.h"
#include "UI/EditorUI_Manager.h"
#include "UI/SimulationUI_Manager.h"
#include "UI/EditorRenderer.h"
#include "UI/SimulationRenderer.h"
#include "exprtk.hpp"
#include <Eigen/Sparse>
#include <iostream>
#include <chrono>

Grid grid(gridSize);
std::string currentWorkingFilePath;
AssetManager assets;
sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
sf::RenderWindow editorWindow(desktop, "Circuit Sim", sf::Style::None);
sf::RenderWindow simulationWindow;



int main() {   
    

    int n = 3;

    Eigen::SparseMatrix<double> A(n, n);
    Eigen::VectorXd b(n), x;

    std::vector<Eigen::Triplet<double>> triplets;

    triplets.emplace_back(0, 0, 10);
    triplets.emplace_back(1, 1, 5);
    triplets.emplace_back(2, 2, 2);
    triplets.emplace_back(0, 2, 3);

    A.setFromTriplets(triplets.begin(), triplets.end());

    b << 1, 2, 3;

    Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
    solver.analyzePattern(A);
    solver.factorize(A);

    x = solver.solve(b);
    std::cout << A;
    std::cout << x;

    
    Circuit circuit(assets);
    Renderer editorRenderer(editorWindow, assets);
    Renderer simRenderer(simulationWindow, assets);
    editorWindow.setFramerateLimit(60);
    simulationWindow.setFramerateLimit(60);

    EditorRenderer schematicRenderer(editorRenderer, grid);
    SimulationRenderer simulationRenderer(simRenderer);

    EditorUI_Manager editorUI(editorRenderer);
    SimulationUI_Manager simUI(simRenderer);
    editorUI.initialize();

    EditorController editorController(circuit, assets, editorRenderer, editorUI, currentWorkingFilePath);
    SimulationController simulationController(circuit, simUI);

    //initializing circuit data
    //char filename[MAX_PATH] = "SeriesRLCDCCircuit.ckt";
    //CircuitData initializedData = editorController.saveCircuitHandler.loadFromFile(filename);
    //circuit.setCircuitData(initializedData);



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
            case EditorUICommand::PlaceVCVS:
            case EditorUICommand::PlaceVCCS:
            case EditorUICommand::PlaceCCVS:
            case EditorUICommand::PlaceCCCS:
            case EditorUICommand::PlaceCapacitor:
            case EditorUICommand::PlaceInductor:
            case EditorUICommand::PlaceSwitch:
            case EditorUICommand::PlaceGround:
            case EditorUICommand::PlaceACCurrentSource:
            case EditorUICommand::PlaceACVoltageSource:
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
                    std::cout << currentWorkingFilePath << "\n";
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

            editorWindow.display();
        }
        
        if (simulationWindow.isOpen()) {
            simulationWindow.clear(simulationWindowBackgroundColor);
            simUI.draw();
            simulationWindow.display();
        }
    }
    
    return 0;
}