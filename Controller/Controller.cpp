#include "Controller.h"
#include "InputHandler.h"

Controller::Controller(Circuit& Circuit, std::vector<SchematicComponent>& Components, sf::RenderWindow& Window, AssetManager& Assets, Renderer& Renderer)
	: circuit(Circuit), components(Components), cameraController(Window, Renderer.getCanvasView()), dragHandler(Components, Circuit.getWires()), placeHandler(Components, Circuit, Assets),
	wireHandler(Circuit, Components), currentHandler(nullptr), command(UICommand::None), window(Window), renderer(Renderer), assets(Assets) { }

void Controller::handleEvent(const sf::Event& event) {
	switch (event.type) {

	case sf::Event::MouseButtonPressed: {

		if (event.mouseButton.button == sf::Mouse::Middle) {
			cameraController.onMousePress({ event.mouseButton.x, event.mouseButton.y });
		}

		if (event.mouseButton.button == sf::Mouse::Left) {
			onMousePress(event.mouseButton);
		}
		break;
	}

	case sf::Event::MouseMoved: {
		sf::Vector2f worldPos = window.mapPixelToCoords({ event.mouseMove.x, event.mouseMove.y }, renderer.getCanvasView());

		cameraController.onMouseMove({ event.mouseMove.x, event.mouseMove.y });

		onMouseMove(event.mouseMove);
		break;
	}

	case sf::Event::MouseWheelScrolled:
		cameraController.onScroll(event.mouseWheelScroll);

		if (currentHandler) currentHandler->onScroll(event.mouseWheelScroll);
		break;

	case sf::Event::MouseButtonReleased: {
		sf::Vector2f worldPos = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y }, renderer.getCanvasView());

		if (event.mouseButton.button == sf::Mouse::Middle) {
			cameraController.onMouseRelease();
		}

		onMouseRelease(event.mouseButton);
		break;
	}

	case sf::Event::KeyPressed:
		cameraController.onKeyPress(event.key);

		onKeyPress(event.key);
		break;

	default:
		break;
	}
}

void Controller::onMousePress(const sf::Event::MouseButtonEvent& event) {
	sf::Vector2f worldMousePosition = window.mapPixelToCoords({ event.x, event.y }, renderer.getCanvasView());

	if (event.button == sf::Mouse::Button::Left) {
		ElectricalConnection clickedLead = findClickedLead(sf::Vector2f(event.x, event.y));
		WireNodeReference clickedNodeReference = findClickedNode(sf::Vector2f(event.x, event.y));

		WireInteraction interaction = { clickedLead, clickedNodeReference };
		if (interaction.hasLead() || interaction.hasNode()) {
			std::cout << "Clicked Component " << clickedLead.componentID << ", Lead "
				<< Debug::lead_to_string(clickedLead.lead) << std::endl;
			std::cout << "Clicked Wire " << clickedNodeReference.wireID << ", Node " << clickedNodeReference.nodeID << std::endl;
			currentHandler = &wireHandler;
			Debug::setHandler("Wire Handler");
			wireHandler.setInteractionContext(interaction);
		}

		

		else if (!currentHandler) {
			Component* clickedComponent = findComponentAt(worldMousePosition);
			if (clickedComponent) {
				currentHandler = &dragHandler;
				Debug::setHandler("Drag Handler");
				dragHandler.setDraggedComponent(*clickedComponent);
			
			}
			else {
				for (auto& c : circuit.getComponents()) {
					c.selected = false;
				}
			}

		}
	}
	if (currentHandler) currentHandler->onMousePress(worldMousePosition);
}

void Controller::onMouseMove(const sf::Event::MouseMoveEvent& event) {
	sf::Vector2f worldMousePosition = window.mapPixelToCoords({ event.x, event.y }, renderer.getCanvasView());
	if (currentHandler) currentHandler->onMouseMove(worldMousePosition);
}

void Controller::onScroll(const sf::Event::MouseWheelScrollEvent& event) {

	currentHandler->onScroll(event);

	if (currentHandler->shouldRelease()) {
		currentHandler = nullptr;
		Debug::setHandler("None");

	}
}

void Controller::onMouseRelease(const sf::Event::MouseButtonEvent& event) {
	sf::Vector2f worldMousePosition = window.mapPixelToCoords({ event.x, event.y });

	if (currentHandler) {
		currentHandler->onMouseRelease(worldMousePosition);
	}
	if (currentHandler) {
		if (currentHandler->shouldRelease()) {
			currentHandler = nullptr;
			Debug::setHandler("None");
		}
	}
}

void Controller::onKeyPress(const sf::Event::KeyEvent& event) {
	cameraController.onKeyPress(event);

	for (auto& comp : components)
		if (comp.selected) {
			currentHandler = &placeHandler;
			Debug::setHandler("PlaceHandler");
		}

	if (currentHandler) {
		currentHandler->onKeyPress(event);
		if (currentHandler->shouldRelease()) {
			currentHandler = nullptr;
		}
	}

	if (event.code == sf::Keyboard::F1) {
		for (const auto& w : circuit.getWires())
			Debug::debugPrintWire(w.second);
	}
}

void Controller::setHandler(InputHandler* handler, UICommand cmd) {
	command = cmd;
	if (command != UICommand::None) {
		switch (command) {
		case UICommand::PlaceVoltageSource:
			placeHandler.setComponentType(ComponentType::VoltageSource);
			break;

		case UICommand::PlaceResistor:
			placeHandler.setComponentType(ComponentType::Resistor);
			break;

		case UICommand::PlaceCurrentSource:
			placeHandler.setComponentType(ComponentType::CurrentSource);
			break;

		case UICommand::PlaceCapacitor:
			placeHandler.setComponentType(ComponentType::Capacitor);
			break;

		case UICommand::PlaceInductor:
			placeHandler.setComponentType(ComponentType::Inductor);
			break;

		case UICommand::PlaceSwitch:
			placeHandler.setComponentType(ComponentType::Switch);
			break;

		case UICommand::ToggleMenu:
			break;

		case UICommand::None:
			break;
		}
	}
	currentHandler = handler;
}

void Controller::rebuildSchematicComponents() {
	components.clear();

	for (const auto& comp : circuit.getComponents()) {
		SchematicComponent c(comp);
		sf::Vector2f target = comp.position;
		sf::Vector2f snapped(
			std::round(target.x / gridSize) * gridSize,
			std::round(target.y / gridSize) * gridSize
		);
		c.setPosition(snapped);
		c.setTexture(assets.getTexture(comp.type));
		components.emplace_back(c);

	}
}

InputHandler* Controller::getHandler() {
	return currentHandler;
}
Component* Controller::findComponentAt(const sf::Vector2f point) {
	for (auto& comp : components) {
		if (comp.hitBoxContainsPoint(point)) {
			return circuit.getComponent(comp.componentID);
		}
	}
	return nullptr;
}

ElectricalConnection Controller::findClickedLead(const sf::Vector2f point) { // parameter is in pixel space, converts lead position to pixel space
	for (const auto c : components) {
		sf::Vector2i pixelPosA = window.mapCoordsToPixel(c.getLeadPositionA(), renderer.getCanvasView());

		sf::Vector2f distanceA = sf::Vector2f(pixelPosA) - point;
		if (distanceA.x * distanceA.x + distanceA.y * distanceA.y <= nodeSelectionRadius * nodeSelectionRadius) {
			return { c.componentID, Lead::A };
		}

		sf::Vector2i pixelPosB = window.mapCoordsToPixel(c.getLeadPositionB(), renderer.getCanvasView());

		sf::Vector2f distanceB = sf::Vector2f(pixelPosB) - point;
		if (distanceB.x * distanceB.x + distanceB.y * distanceB.y <= nodeSelectionRadius * nodeSelectionRadius) {
			
			return { c.componentID, Lead::B };
		}
	}
	return { -1, Lead::Null };
}

WireNodeReference Controller::findClickedNode(const sf::Vector2f point) {// parameter is in pixel space, converts node position to pixel space, returns wireID, nodeID
	for (const auto& w : circuit.getWires()) {
		for (const auto& n : w.second.getGraph()) {
			sf::Vector2i pixelPos = window.mapCoordsToPixel(n.second.position, renderer.getCanvasView());
			sf::Vector2f distance = sf::Vector2f(pixelPos) - point;

			if (distance.x * distance.x + distance.y * distance.y <= nodeSelectionRadius * nodeSelectionRadius) {
				return { n.second.belongsTo, n.first };
			}
		}
	}
	std::cout << "findClickedNode returned NULL\n\n";
	return { -1, -1 };
}

