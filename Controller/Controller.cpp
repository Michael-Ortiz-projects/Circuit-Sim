#include "Controller.h"
#include "InputHandler.h"

Controller::Controller(Circuit& Circuit, std::vector<SchematicComponent>& Components, sf::RenderWindow& Window, AssetManager& Assets, Renderer& Renderer)
	: circuit(Circuit), components(Components), cameraController(Window, Renderer.getCanvasView()), dragHandler(Components, Circuit.getWires()), placeHandler(Components, Circuit, Assets),
	wireHandler(Circuit, Components), selectionBoxHandler(Components, Circuit, selection, shiftHeld), deleteHandler(Circuit, selection), currentHandler(nullptr),
	command(UICommand::None), window(Window), renderer(Renderer), assets(Assets) { }

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

	case sf::Event::KeyReleased:
		onKeyRelease(event.key);
		break;

	default:
		break;
	}
}

void Controller::onMousePress(const sf::Event::MouseButtonEvent& event) {
	if (event.button != sf::Mouse::Left) return;
	sf::Vector2f worldMousePosition = window.mapPixelToCoords({ event.x, event.y }, renderer.getCanvasView());
	HitResult hit = hitTest(sf::Vector2f(event.x, event.y));
	hit.shiftHeld = shiftHeld;
	switch (hit.type) {
	case HitResult::Type::Lead:
	case HitResult::Type::WireNode:
		currentHandler = &wireHandler;
		wireHandler.setHitResult(hit);
		Debug::setHandler("WireHandler");
		break;

	case HitResult::Type::WireSegment:
		if (wireHandler.getState() == WireState::Creating) {
			currentHandler = &wireHandler;
			wireHandler.setHitResult(hit);
			Debug::setHandler("WireHandler");
			break;
		}
		currentHandler = &selectionBoxHandler;
		Debug::setHandler("selectionBoxHandler");
		break;

	case HitResult::Type::Component:
		currentHandler = &dragHandler;
		Debug::setHandler("DragHandler");
		dragHandler.setDraggedComponent(*hit.component);
		break;

	case HitResult::Type::None:
		if (currentHandler == &wireHandler || currentHandler == &dragHandler || currentHandler == &placeHandler) {
			
			// let the current handler handle the empty-space click
			wireHandler.setHitResult(hit);
			currentHandler->onMousePress(worldMousePosition);
			return;
		}
		else {
			if (!shiftHeld) selection.clear();		// If shift is held, continue adding to selection; otherwise start fresh
			
			currentHandler = &selectionBoxHandler;
			selectionBoxHandler.onMousePress(worldMousePosition);
			Debug::setHandler("SelectionBoxHandler");
			return; // SelectionBoxHandler handles dragging
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

	if (currentHandler) {
		currentHandler->onScroll(event);
		if (currentHandler->shouldRelease()) {
			currentHandler = nullptr;
			Debug::setHandler("None");
		}
	}
}

void Controller::onMouseRelease(const sf::Event::MouseButtonEvent& event) {

	sf::Vector2f worldPos =
		window.mapPixelToCoords({ event.x, event.y }, renderer.getCanvasView());

	if (currentHandler) {
		currentHandler->onMouseRelease(worldPos);

		if (currentHandler->shouldRelease()) {
			currentHandler = nullptr;
			Debug::setHandler("None");
		}
	}
}

void Controller::onKeyPress(const sf::Event::KeyEvent& event) {
	cameraController.onKeyPress(event);
	if (event.code == sf::Keyboard::LShift || event.code == sf::Keyboard::RShift)
		shiftHeld = true;
	if (event.code == sf::Keyboard::Delete) { 
		currentHandler = &deleteHandler;
		Debug::setHandler("DeleteHandler");
		currentHandler->onKeyPress(event);
		return;
	}

	if (event.code == sf::Keyboard::F1) {
		for (auto& w : circuit.getWires())
			Debug::debugPrintWire(w.second);
	}

	if (currentHandler) {
		currentHandler->onKeyPress(event);
		if (currentHandler->shouldRelease()) {
			currentHandler = nullptr;
			Debug::setHandler("None");
		}
	}

}

void Controller::onKeyRelease(const sf::Event::KeyEvent& event) {
	if (event.code == sf::Keyboard::LShift || event.code == sf::Keyboard::RShift)
		shiftHeld = false;
	if (event.code == sf::Keyboard::Delete) {
		currentHandler = nullptr;
		Debug::setHandler("None");
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


HitResult Controller::hitTest(const sf::Vector2f& mousePixel) {
	HitResult result;

	if (auto lead = findClickedLead(mousePixel); lead.lead != Lead::Null) {
		result.type = HitResult::Type::Lead;
		result.lead = lead;
		std::cout << "hitTest() returned Lead hit\n";
		return result;
	}

	if (auto node = findClickedNode(mousePixel); node.isValid()) {
		result.type = HitResult::Type::WireNode;
		result.wireNode = node;
		std::cout << "hitTest() returned Node hit\n";
		return result;
	}

	if (auto seg = findClickedSegment(mousePixel); seg.valid) {
		result.type = HitResult::Type::WireSegment;
		result.wireSegment = seg;
		std::cout << "hitTest() returned Segment hit\n";
		return result;
	}

	if (auto* comp = findComponentAt(mousePixel)) {
		result.type = HitResult::Type::Component;
		result.component = comp;
		std::cout << "hitTest() returned component hit\n";
		return result;
	}

	std::cout << "hitTest() returned no hit\n";
	return result;
}

ElectricalConnection Controller::findClickedLead(const sf::Vector2f mousePixel) { // parameter is in pixel space, converts lead position to pixel space
	for (const auto c : components) {
		sf::Vector2i pixelPosA = window.mapCoordsToPixel(c.getLeadPositionA(), renderer.getCanvasView());

		sf::Vector2f distanceA = sf::Vector2f(pixelPosA) - mousePixel;
		if (distanceA.x * distanceA.x + distanceA.y * distanceA.y <= nodeSelectionRadius * nodeSelectionRadius) {
			return { c.componentID, Lead::A };
		}

		sf::Vector2i pixelPosB = window.mapCoordsToPixel(c.getLeadPositionB(), renderer.getCanvasView());

		sf::Vector2f distanceB = sf::Vector2f(pixelPosB) - mousePixel;
		if (distanceB.x * distanceB.x + distanceB.y * distanceB.y <= nodeSelectionRadius * nodeSelectionRadius) {
			
			return { c.componentID, Lead::B };
		}
	}
	return { -1, Lead::Null };
}

WireNodeReference Controller::findClickedNode(const sf::Vector2f mousePixel) {// parameter is in pixel space, converts node position to pixel space, returns wireID, nodeID
	for (auto& w : circuit.getWires()) {
		for (const auto& n : w.second.getGraph()) {
			sf::Vector2i pixelPos = window.mapCoordsToPixel(n.second.position, renderer.getCanvasView());
			sf::Vector2f distance = sf::Vector2f(pixelPos) - mousePixel;

			if (distance.x * distance.x + distance.y * distance.y <= nodeSelectionRadius * nodeSelectionRadius) {
				return { n.second.belongsTo, n.first };
			}
		}
	}
	//std::cout << "findClickedNode returned NULL\n\n";
	return { -1, -1 };
}

WireHit Controller::findClickedSegment(const sf::Vector2f mousePixel) {
	WireHit best;
	
	sf::Vector2f mouseWorld = window.mapPixelToCoords(sf::Vector2i(mousePixel), renderer.getCanvasView());

	for (auto& [wireID, wire] : circuit.getWires()) {

		SegmentHit seg = wire.projectOntoSegment(mouseWorld);
		if (!seg.isValid())
			continue;

		sf::Vector2f snappedPixel =
			sf::Vector2f(window.mapCoordsToPixel(seg.snappedPosition, renderer.getCanvasView()));

		float pixelDist = std::hypot(mousePixel.x - snappedPixel.x, mousePixel.y - snappedPixel.y);

		if (pixelDist > wireSelectionRadius)
			continue;

		if (!best.valid || pixelDist < best.distance) {
			best.wireID = wireID;
			best.segment = seg;
			best.distance = pixelDist;
			best.valid = true;
		}
	}
	//if (best.valid) std::cout << "findClickedSegment returned True\n";
	return best;
}

Component* Controller::findComponentAt(const sf::Vector2f mousePixel) {
	sf::Vector2f mouseWorld = window.mapPixelToCoords(sf::Vector2i(mousePixel), renderer.getCanvasView());

	for (auto& comp : components) {
		if (comp.hitBoxContainsPoint(mouseWorld)) {
			return circuit.getComponent(comp.componentID);
		}
	}
	return nullptr;
}