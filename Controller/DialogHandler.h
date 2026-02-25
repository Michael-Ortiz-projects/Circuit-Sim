#include "InputHandler.h"
#include "../Core/Simulator.h"
#include "../UI/SimulationUI_Manager.h"

class DialogHandler : public InputHandler {
public:
	DialogHandler(SimulationUI_Manager& ui, SimulationType& simType, Config& simulationParameters);

    void onKeyPress(const sf::Event::KeyEvent& event) override;

    void onMousePress(const sf::Vector2f&) override;

    void onMouseRelease(const sf::Vector2f&) override;

    bool shouldRelease() const override;

    void update(SimulationUICommand& cmd, Circuit& circuit);

private:
    SimulationUI_Manager& ui;
    bool release = false;
    SimulationType& simType;
    Config& simParameters;
    
    bool parseValueWithSuffix(const std::string& input, double& outValue);


};