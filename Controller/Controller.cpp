#include "Controller.h"
#include "InputHandler.h"

Controller::Controller(Circuit& Circuit, std::vector<SchematicComponent>& Components, sf::RenderWindow& Window, AssetManager& Assets, Renderer& Renderer)
	: circuit(Circuit), components(Components), cameraController(Window, Renderer.getCanvasView()), dragHandler(Components), placeHandler(Components, Circuit, Assets), currentHandler(nullptr),
	command(UICommand::None), window(Window), renderer(Renderer), assets(Assets) { }

void Controller::handleEvent(const sf::Event& event) {
	switch (event.type) {

	case sf::Event::MouseButtonPressed: {

		if (event.mouseButton.button == sf::Mouse::Right) {
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

		if (currentHandler) currentHandler->onMouseMove(worldPos);
		break;
	}

	case sf::Event::MouseWheelScrolled:
		cameraController.onScroll(event.mouseWheelScroll);

		if (currentHandler) currentHandler->onScroll(event.mouseWheelScroll);
		break;

	case sf::Event::MouseButtonReleased: {
		sf::Vector2f worldPos = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y }, renderer.getCanvasView());

		if (event.mouseButton.button == sf::Mouse::Right) {
			cameraController.onMouseRelease();
		}

		if (currentHandler) {
			currentHandler->onMouseRelease(worldPos);
			if (currentHandler->shouldRelease()) currentHandler = nullptr;
		}
		break;
	}

	case sf::Event::KeyPressed:
		cameraController.onKeyPress(event.key);

		if (currentHandler) currentHandler->onKeyPress(event.key);
		
		break;

	default:
		break;
	}
}

void Controller::onMousePress(const sf::Event::MouseButtonEvent& event) {
	sf::Vector2f worldMousePosition = window.mapPixelToCoords({ event.x, event.y }, renderer.getCanvasView());
	if (event.button == sf::Mouse::Button::Left) {
		if (!currentHandler) {
			Component* clickedComponent = findComponentAt(worldMousePosition);
			if (clickedComponent) {
				currentHandler = &dragHandler;
				Debug::setHandler("Drag Handler");
				dragHandler.setDraggedComponent(*clickedComponent);
			}
			else {
				std::cout << "No clicked component\n";
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
		if (comp.selected) currentHandler = &placeHandler;

	if (currentHandler) {
		currentHandler->onKeyPress(event);
		if (currentHandler->shouldRelease()) {
			currentHandler = nullptr;
		}
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
		c.setTexture(assets.getTexture(comp.type));
		components.push_back(c);
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
