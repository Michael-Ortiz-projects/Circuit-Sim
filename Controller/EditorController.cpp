#include "EditorController.h"
#include "InputHandler.h"

EditorController::EditorController(Circuit& Circuit, AssetManager& Assets, Renderer& Renderer, EditorUI_Manager& UI, std::string& workingFilePath)
	: circuit(Circuit), window(Renderer.getWindow()), cameraController(Renderer.getWindow(), Renderer.getCanvasView()), command(EditorUICommand::None), renderer(Renderer), assets(Assets),
	dragHandler(Circuit.getSchematicComponents(), Circuit.getWires()), placeHandler(Circuit, Assets),	wireHandler(Circuit, Circuit.getSchematicComponents()),
	selectionBoxHandler(Circuit.getSchematicComponents(), Circuit, selection, shiftHeld), deleteHandler(Circuit, selection), editComponentHandler(UI, Circuit),
	saveCircuitHandler(UI, saveManager, Circuit, Assets, workingFilePath), currentHandler(nullptr), workingFilePath(workingFilePath) { }

void EditorController::handleEvent(const sf::Event& event) {
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

		cameraController.onMouseMove({ event.mouseMove.x, event.mouseMove.y });

		onMouseMove(event.mouseMove);
		break;
	}

	case sf::Event::MouseWheelScrolled:
		cameraController.onScroll(event.mouseWheelScroll);

		if (currentHandler) currentHandler->onScroll(event.mouseWheelScroll);
		break;

	case sf::Event::MouseButtonReleased: {

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

void EditorController::onMousePress(const sf::Event::MouseButtonEvent& event) {
	if (event.button != sf::Mouse::Left) return;
	sf::Vector2f worldMousePosition = window.mapPixelToCoords({ event.x, event.y }, renderer.getCanvasView());
	if (currentHandler != &editComponentHandler) {
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
			selectionBoxHandler.updateSelection(worldMousePosition);
			break;

		case HitResult::Type::None:
			if (currentHandler == &wireHandler || currentHandler == &dragHandler || currentHandler == &placeHandler) {

				// let the current handler handle the empty click
				wireHandler.setHitResult(hit);
				currentHandler->onMousePress(worldMousePosition);
				return;
			}
			else {
				if (!shiftHeld) selection.clear();		// If shift is held, continue adding to selection; otherwise start fresh

				currentHandler = &selectionBoxHandler;
				selectionBoxHandler.onMousePress(worldMousePosition);
				Debug::setHandler("SelectionBoxHandler");
				return;
			}

		}
	}
	if (currentHandler) currentHandler->onMousePress(worldMousePosition);
}

void EditorController::onMouseMove(const sf::Event::MouseMoveEvent& event) {
	sf::Vector2f worldMousePosition = window.mapPixelToCoords({ event.x, event.y }, renderer.getCanvasView());
	if (currentHandler) currentHandler->onMouseMove(worldMousePosition);

}

void EditorController::onScroll(const sf::Event::MouseWheelScrollEvent& event) {

	currentHandler->onScroll(event);

	if (currentHandler) {
		currentHandler->onScroll(event);
		if (currentHandler->shouldRelease()) {
			currentHandler = nullptr;
			Debug::setHandler("None");
		}
	}
}

void EditorController::onMouseRelease(const sf::Event::MouseButtonEvent& event) {

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

void EditorController::onKeyPress(const sf::Event::KeyEvent& event) {
	cameraController.onKeyPress(event);
	if (currentHandler != &editComponentHandler) {
		switch (event.code) {
		case sf::Keyboard::R:
			if (circuit.getNetlistComponents().empty()) return;
			for (auto& comp : circuit.getSchematicComponents()) {
				if (comp.selected) {
					currentHandler = &editComponentHandler;
					Debug::setHandler("EditComponentHandler");
					editComponentHandler.setTarget(circuit.getNetlistComponent(comp));
					break;
				}
			}
			break;

		case sf::Keyboard::LShift:
		case sf::Keyboard::RShift:
			shiftHeld = true;
			break;

		case sf::Keyboard::Delete:
			currentHandler = &deleteHandler;
			Debug::setHandler("DeleteHandler");
			break;

		case sf::Keyboard::Enter:
			if (circuit.getNetlistComponents().empty()) return;
			for (SchematicComponent& comp : circuit.getSchematicComponents()) {
				
				if (comp.selected) {
					currentHandler = &editComponentHandler;
					Debug::setHandler("EditComponentHandler");
					editComponentHandler.setTarget(circuit.getNetlistComponent(comp));
					editComponentHandler.openEditDialog();
					break;
				}
			}
			break;

		case sf::Keyboard::W:
			currentHandler = &wireHandler;
			Debug::setHandler("WireHandler");
			currentHandler->onKeyPress(event);
			return;

		case sf::Keyboard::F1:
			for (auto& w : circuit.getWires())
				Debug::debugPrintWire(w.second);
			break;

		case sf::Keyboard::F2:
			for (auto& [id, eNode] : circuit.getElectricalNodes()) {
				Debug::printElectricalNode(eNode);
			}
			break;

		case sf::Keyboard::F3:
			for (auto& c : circuit.getNetlistComponents()) {
				Debug::componentData(c);
			}
			break;

		case sf::Keyboard::F4:
			circuit.getSimulator().setSystem(circuit.getNetlistComponents(), circuit.getElectricalNodes());
			circuit.getSimulator().runDC(true);
			std::cout << "Controller called circuit.getSimulator().buildMNAMap()\n";
			return;
		}

	}

	else if (event.code == sf::Keyboard::Enter) {

	}
	if (currentHandler) {
		currentHandler->onKeyPress(event);
		if (currentHandler->shouldRelease()) {
			currentHandler = nullptr;
			Debug::setHandler("None");
		}
	}
}

void EditorController::onKeyRelease(const sf::Event::KeyEvent& event) {
	if (event.code == sf::Keyboard::LShift || event.code == sf::Keyboard::RShift)
		shiftHeld = false;
	if (event.code == sf::Keyboard::Delete) {
		currentHandler = nullptr;
		Debug::setHandler("None");
	}
}


void EditorController::setHandler(InputHandler* handler, EditorUICommand cmd) {
	command = cmd;
	if (command != EditorUICommand::None) {
		switch (command) {
		case EditorUICommand::PlaceVoltageSource:
			placeHandler.setComponentType(ComponentType::VoltageSource);
			break;

		case EditorUICommand::PlaceResistor:
			placeHandler.setComponentType(ComponentType::Resistor);
			break;

		case EditorUICommand::PlaceCurrentSource:
			placeHandler.setComponentType(ComponentType::CurrentSource);
			break;
		case EditorUICommand::PlaceVCVS:
			placeHandler.setComponentType(ComponentType::VCVS);
			break;

		case EditorUICommand::PlaceVCCS:
			placeHandler.setComponentType(ComponentType::VCCS);
			break;

		case EditorUICommand::PlaceCCVS:
			placeHandler.setComponentType(ComponentType::CCVS);
			break;

		case EditorUICommand::PlaceCCCS:
			placeHandler.setComponentType(ComponentType::CCCS);
			break;

		case EditorUICommand::PlaceCapacitor:
			placeHandler.setComponentType(ComponentType::Capacitor);
			break;

		case EditorUICommand::PlaceInductor:
			placeHandler.setComponentType(ComponentType::Inductor);
			break;

		case EditorUICommand::PlaceSwitch:
			placeHandler.setComponentType(ComponentType::Switch);
			break;

		case EditorUICommand::PlaceGround:
			placeHandler.setComponentType(ComponentType::Ground);
			break;

		case EditorUICommand::ToggleMenu:
			break;

		case EditorUICommand::None:
			break;
		}
	}
	currentHandler = handler;
}

void EditorController::rebuildSchematicComponents() {
	for (const auto& comp : circuit.getNetlistComponents()) {
		SchematicComponent* c = circuit.getSchematicComponent(comp);
		c->setValue(comp.value);
		c->setLabel(comp.label);
	}
}

InputHandler* EditorController::getHandler() {
	return currentHandler;
}


HitResult EditorController::hitTest(const sf::Vector2f& mousePixel) {
	HitResult result;

	if (auto lead = findClickedLead(mousePixel); lead.componentID != -1) {
		result.type = HitResult::Type::Lead;
		result.lead = lead;
		std::cout << "hitTest() returned Lead hit on component " << lead.componentID << " and Terminal " << lead.terminalID << "\n";
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
		//std::cout << "hitTest() returned component hit\n";
		return result;
	}

	//std::cout << "hitTest() returned no hit\n";
	return result;
}

ElectricalConnection EditorController::findClickedLead(const sf::Vector2f mousePixel) { // parameter is in pixel space, converts lead position to pixel space

	for (const auto& c : circuit.getSchematicComponents()) {
		for (const auto& T : c.schematicTerminals) {
			sf::Vector2f terminalWorldPos = c.getPosition() + T.offset;
			sf::Vector2i terminalPixelPosition = window.mapCoordsToPixel(terminalWorldPos, renderer.getCanvasView());
			sf::Vector2f distance = sf::Vector2f(terminalPixelPosition) - mousePixel;
			if (distance.x * distance.x + distance.y * distance.y <= nodeSelectionRadius * nodeSelectionRadius) {
				return { c.componentID, T.terminalID };
			}
		}
	}
	return { -1, -1 };
}

WireNodeReference EditorController::findClickedNode(const sf::Vector2f mousePixel) {// parameter is in pixel space, converts node position to pixel space, returns wireID, nodeID
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

WireHit EditorController::findClickedSegment(const sf::Vector2f mousePixel) {
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

SchematicComponent* EditorController::findComponentAt(const sf::Vector2f mousePixel) {
	sf::Vector2f mouseWorld = window.mapPixelToCoords(sf::Vector2i(mousePixel), renderer.getCanvasView());

	for (auto& comp : circuit.getSchematicComponents()) {
		if (comp.hitBoxContainsPoint(mouseWorld)) {
			return &comp;
		}
	}
	return nullptr;
}