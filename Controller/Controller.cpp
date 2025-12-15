#include "Controller.h"
#include "InputHandler.h"

Controller::Controller(Circuit& Circuit, std::vector<SchematicComponent>& Components, sf::RenderWindow& Window, UI_Manager& UI, AssetManager& Assets)
	: circuit(Circuit), components(Components), dragHandler(Components), cameraHandler(Window), placeHandler(Components, Circuit, Assets), currentHandler(nullptr),
	command(UICommand::None), window(Window), ui(UI)
	{ }


void Controller::onMousePress(const sf::Event::MouseButtonEvent& event) {
	sf::Vector2f worldMousePosition = window.mapPixelToCoords({ event.x, event.y });

	if (event.button == sf::Mouse::Button::Left) {
		if (!currentHandler) {
			SchematicComponent* clickedComponent = findComponentAt(worldMousePosition);
			if (clickedComponent) {
				currentHandler = &dragHandler;
				Debug::setHandler("Drag Handler");
				dragHandler.setDraggedComponent(*clickedComponent);
			}
			else std::cout << "No clicked component\n";
		}
	}

	if (event.button == sf::Mouse::Button::Right) {
		if (!currentHandler) {
			currentHandler = &cameraHandler;
			Debug::setHandler("Camera Handler");
		}
	}


	if (currentHandler) currentHandler->onMousePress(worldMousePosition);
}

void Controller::onMouseMove(const sf::Event::MouseMoveEvent& event) {
	sf::Vector2f worldMousePosition = window.mapPixelToCoords({ event.x, event.y });
	if (currentHandler) currentHandler->onMouseMove(worldMousePosition);
}

void Controller::onScroll(const sf::Event::MouseWheelScrollEvent& event) {
	if (!currentHandler) {
		currentHandler = &cameraHandler;
		Debug::setHandler("Camera Handler");
	}
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
	if (currentHandler) currentHandler->onKeyPress(event);

	else if (event.code == sf::Keyboard::Escape) {
		window.close();
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

			case UICommand::ToggleMenu:
				break;
		}
	}
	currentHandler = handler;
}

InputHandler* Controller::getHandler() {
	return currentHandler;
}
SchematicComponent* Controller::findComponentAt(const sf::Vector2f point) {
	for (auto& comp : components) {
		if (comp.spriteContainsPoint(point)) {
			return &comp;
		}
	}
	return nullptr;
}

sf::View Controller::getView() {
	return cameraHandler.GetView();
}