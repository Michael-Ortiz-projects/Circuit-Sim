#include "../Core/Circuit.h"
#include "../UI/SimulationUI_Manager.h"
#include "DialogHandler.h"
#include <fstream>


class SimulationController {
public:
    DialogHandler dialogHandler;
	SimulationController(Circuit& C, SimulationUI_Manager& ui);

	void handleEvent(const sf::Event& event);

    void onMousePress(const sf::Event::MouseButtonEvent& event);

    void onMouseMove(const sf::Event::MouseMoveEvent& event);

    void onMouseRelease(const sf::Event::MouseButtonEvent& event);

    void onKeyPress(const sf::Event::KeyEvent& event);

    void onKeyRelease(const sf::Event::KeyEvent& event);

    void onScroll(const sf::Event::MouseWheelScrollEvent& event);

    sf::FloatRect& getSelectionRect() { return selectionRect; }
    void setHandler(InputHandler* handler, SimulationUICommand cmd);

    void runSimulation();

private:

	Circuit& circuit;
    SimulationUI_Manager& UI;
    InputHandler* currentHandler;
    SimulationType simType = SimulationType::None;
    Config simParameters;

    bool dragging;
    sf::FloatRect selectionRect;
    sf::Vector2f startSelectionPos;
    sf::Vector2f endSelectionPos;
    sf::Vector2i startSelectionPixelPos;
	
    bool parseValueWithSuffix(const std::string& input, double& outValue);
    void exportToCSV(const std::vector<Eigen::VectorXd>& results, const std::vector<double> timeVector, const std::string& filename);
};