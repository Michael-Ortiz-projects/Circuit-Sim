#include "Entity.h"


Entity::Entity() {}

Entity::Entity(Component& comp) {
    component = comp;
    
    lead1 = { 0, 0 };
    lead2 = { 0, 0 };

    switch (comp.type) {
        case ComponentType::VoltageSource: {
            idle_symbol.setTexture(assets.getTexture("normal_VS"));
            highlight_symbol.setTexture(assets.getTexture("highlight_VS"));
            break;
        }
        
        case ComponentType::Resistor: {
            idle_symbol.setTexture(assets.getTexture("normal_R"));
            highlight_symbol.setTexture(assets.getTexture("highlight_R"));
            break;
        }

        case ComponentType::CurrentSource: {
            idle_symbol.setTexture(assets.getTexture("normalCS"));
            highlight_symbol.setTexture(assets.getTexture("highlightedCS"));
            break;
        }

        case ComponentType::Capacitor: {
            idle_symbol.setTexture(assets.getTexture("normal_CAP"));
            highlight_symbol.setTexture(assets.getTexture("highlighted_CAP"));
            break;
        }

        case ComponentType::Inductor: {
            idle_symbol.setTexture(assets.getTexture("normal_L"));
            highlight_symbol.setTexture(assets.getTexture("highlighted_L"));
            break;
        }

        case ComponentType::Switch: {
            if (!comp.isClosed) {
                idle_symbol.setTexture(assets.getTexture("normal_SO"));
                highlight_symbol.setTexture(assets.getTexture("highlighted_SO"));
            }
            else {
                idle_symbol.setTexture(assets.getTexture("normal_SC"));
                highlight_symbol.setTexture(assets.getTexture("highlighted_SC"));
            }
            break;
        }
    }
    idle_symbol.setOrigin(60, 30);
    highlight_symbol.setOrigin(60, 30);


    
    selection_box.setSize(sf::Vector2f(110, 50));
    selection_box.setOrigin(55, 30);
    
    
    sf::Text label(comp.label, mainFont, 20);
    label.setFillColor(sf::Color(223, 208, 184));
    label.setScale(1, -1);
    label.setOrigin(label.getLocalBounds().width / 2, 0);
    IDBox.text = label;
    IDBox.setTextBoundingBox();
    IDBox.outline_color = sf::Color(100, 150, 255);
    ValueBox.outline_color = sf::Color(100, 150, 255);

    rotation = 0;

    component.fromNode = -1;
    component.toNode = -1;


    setValueText();
}

void Entity::handleMouseEvents(CursorState& cursorState, std::vector<Wire>& wires, std::unordered_map<int, Entity>& entities, sf::Vector2f mousePosition, int entityID) {
    
    sf::Vector2f snappedPos = snapToGrid(mousePosition);

    bool clickedOnLeftLead = (snappedPos == lead1);
    bool clickedOnRightLead = (snappedPos == lead2);
    int nextWireIndex = wires.size();

    if (((clickedOnLeftLead && component.fromNode == -1) || (clickedOnRightLead && component.toNode == -1)) && cursorState == CursorState::Default) {
        //std::cout << "wire Created\n";
        wires.push_back(Wire(snappedPos));

        wires.back().editing = true;
        cursorState = CursorState::PlacingWire;
        
        if (clickedOnLeftLead) {
            component.fromNode = nextWireIndex;
            wires.back().connected_entity_ids.emplace_back(entityID, 0);
        }
        else if (clickedOnRightLead) {
            component.toNode = nextWireIndex;
            wires.back().connected_entity_ids.emplace_back(entityID, 1);
        }
        std::cout << nextWireIndex;
        wires.back().anchorNodes.push_back(0);
    }


    else if ((clickedOnLeftLead || clickedOnRightLead) && cursorState == CursorState::PlacingWire) {
        int leadType = clickedOnLeftLead ? 0 : 1;
        int& existingWireIndex = clickedOnLeftLead ? component.fromNode : component.toNode;
        sf::Vector2f leadPosition = clickedOnLeftLead ? lead1 : lead2;

        wires.back().AddFullPreview();
        //std::cout << "Existing Wire Index: " << existingWireIndex << ", wires.size() - 1: " << wires.size() - 1 << " Next Wire Index: " << nextWireIndex;
        if (existingWireIndex != wires.size() - 1 && existingWireIndex != -1) {
            //std::cout << "debug";
            std::optional<std::pair<int, int>> insertion_segment = wires[existingWireIndex].findClickedSegment(snappedPos, 15.0f);
            
            if (insertion_segment.has_value()) {
                auto [a, b] = insertion_segment.value();
                

                int joiningID = wires[existingWireIndex].insertWireBetween(a, b, wires.back());
                
                if (clickedOnLeftLead) {
                    component.fromNode = existingWireIndex;
                }
                else {
                    component.toNode = existingWireIndex;
                }

                for (const auto& pair : wires.back().connected_entity_ids) {

                    //checks if pair is in the existing wire's entity indexes
                    if (std::find(wires[existingWireIndex].connected_entity_ids.begin(), wires[existingWireIndex].connected_entity_ids.end(), pair) == wires[existingWireIndex].connected_entity_ids.end()) {
                        wires[existingWireIndex].connected_entity_ids.push_back(pair);
                    }
                }

                for (const auto& [id, lead] : wires.back().connected_entity_ids) {
                    
                    if (lead) {
                        entities[id].component.toNode = existingWireIndex;
                    }
                    else {
                        entities[id].component.fromNode = existingWireIndex;
                    }
                }
                
                wires.pop_back();
            }
        }

        else {
            if (clickedOnLeftLead) {
                component.fromNode = nextWireIndex - 1;
                wires.back().connected_entity_ids.emplace_back(entityID, 0);
            }
            else {
                component.toNode = nextWireIndex - 1;
                wires.back().connected_entity_ids.emplace_back(entityID, 1);
            }
            wires.back().anchorNodes.push_back(wires.back().graph.size() - 1);
            wires.back().editing = false;
        }
        
        cursorState = CursorState::Default;
    }
    

    if (IDBox.box.getGlobalBounds().contains(mousePosition))
        setSelectingID(true);

    else {
        setSelectingID(false);
    }

    if (ValueBox.box.getGlobalBounds().contains(mousePosition))
        setSelectingValue(true);

    else {
        setSelectingValue(false);
    }
}

void Entity::handleTextInput(const sf::Event& event, const sf::Vector2f& mousePos, CursorState& cursorState) {
    if (IDBox.selecting) {

        if (event.type == sf::Event::TextEntered) {
            if (event.text.unicode == 8) { // Backspace
                if (!input.empty()) input.pop_back();
            }

            else if (event.text.unicode >= 32 && event.text.unicode < 128) {
                input += static_cast<char>(event.text.unicode);
            }
            IDBox.text.setString(input);
            component.label = input;

            if (event.text.unicode == 13) { //enter is pressed
                setSelectingID(false);
                input.clear();
            }
        }
    }


    if (ValueBox.selecting) { //set ComponentValue when changing value
        if (event.type == sf::Event::TextEntered) {
            if (event.text.unicode == 8) { // Backspace
                if (!input.empty()) input.pop_back();
            }

            else if (event.text.unicode >= 32 && event.text.unicode < 128) {
                char entered = static_cast<char>(event.text.unicode);

                if (std::isdigit(entered)) {
                    input += entered;
                }

                else if (entered == '.' && input.find('.') == std::string::npos) {
                    input += entered;
                }

                else if (entered == '-' && input.empty()) {
                    input += entered;
                }
            }
            ValueBox.text.setString(input);

            if (event.text.unicode == 13) {
                setSelectingValue(false);
                cursorState = CursorState::Default;
                if (!input.empty()) {
                    component.value = std::stod(input);
                    std::cout << component.value;
                }

                std::cout << component.value;

                setValueText();

                input.clear();
            }
        }
    }
}

void Entity::setPosition(sf::Vector2f newPosition, std::vector<Wire>& wires, int dr) {
    std::cout << newPosition.x << ", " << newPosition.y << std::endl;
    int leftNode = -1;
    int rightNode = -1;
    if (component.fromNode != -1) {
        Wire& fromWire = wires[component.fromNode];
        for (const auto& node : fromWire.anchorNodes) {
            if (fromWire.graph[node].position == lead1) {
                leftNode = node;
            }
        }
    }
    if (component.toNode != -1) {
        Wire& toWire = wires[component.toNode];
        for (const auto& node : toWire.anchorNodes) {
            if (toWire.graph[node].position == lead2) {
                rightNode = node;
            }
        }
    }
    position = newPosition;
    rotation += dr;

    idle_symbol.setPosition(position);
    idle_symbol.setRotation(rotation);
    highlight_symbol.setPosition(position);
    highlight_symbol.setRotation(rotation);
    selection_box.setPosition(position);

    lead1 = sf::Vector2f(-60 * cos(rotation * PI / 180) + position.x, -60 * sin(rotation * PI / 180) + position.y);
    lead2 = sf::Vector2f( 60 * cos(rotation * PI / 180) + position.x,  60 * sin(rotation * PI / 180) + position.y);
    //std::cout << "\nLead1 (" << lead1.x << ", " << lead1.y << ")\nLead2 (" << lead2.x << ", " << lead2.y << ")\n";

    if (component.fromNode != -1 && leftNode != -1) {
        wires[component.fromNode].moveNode(leftNode, lead1);
    }
    if (component.toNode != -1 && rightNode != -1) {
        wires[component.toNode].moveNode(rightNode, lead2);
    }

    sf::Vector2f iBoxPosition;
    sf::Vector2f vBoxPosition;
    float moddedRotation = rotation % 360;
    bool vertical = ((moddedRotation >= 45 && moddedRotation <= 135) || (moddedRotation >= 225 && moddedRotation <= 315));
    

    vertical ? iBoxPosition = sf::Vector2f(position.x + 60, position.y + 35) : iBoxPosition = sf::Vector2f(position.x, position.y + 60);
    vertical ? vBoxPosition = sf::Vector2f(position.x + 60, position.y - 15) : vBoxPosition = sf::Vector2f(position.x, position.y - 60);
    IDBox.text.setPosition(iBoxPosition);
    IDBox.setTextBoundingBox();

    ValueBox.text.setPosition(vBoxPosition);
    ValueBox.setTextBoundingBox();
}

sf::Vector2f Entity::getPosition() {
    return position;
}

bool Entity::contains(sf::Vector2f point) {
    return selection_box.getGlobalBounds().contains(point);
}

void Entity::setSelectingID(bool state) {
    IDBox.selecting = state;
    IDBox.outline_color = state ? sf::Color::Blue : sf::Color(100, 150, 255);
    IDBox.box.setOutlineColor(state ? sf::Color::Blue : sf::Color(100, 150, 255));
}

void Entity::setSelectingValue(bool state) {
    ValueBox.selecting = state;
    ValueBox.outline_color = state ? sf::Color::Blue : sf::Color(100, 150, 255);
    ValueBox.box.setOutlineColor(state ? sf::Color::Blue : sf::Color(100, 150, 255));
}

void Entity::draw(sf::RenderWindow& window) {
    if (component.type == ComponentType::Switch) {
        if (!component.isClosed) {
            idle_symbol.setTexture(assets.getTexture("normal_SO"));
            highlight_symbol.setTexture(assets.getTexture("highlighted_SO"));
        }
        else {
            idle_symbol.setTexture(assets.getTexture("normal_SC"));
            highlight_symbol.setTexture(assets.getTexture("highlighted_SC"));
        }
    }
    
    selected ? window.draw(highlight_symbol) : window.draw(idle_symbol);
    
    IDBox.draw(window);
    ValueBox.draw(window);
    //std::cout << "Drawn";
}

void Entity::printData() {
    
}

void Entity::setValueText() {
    switch (component.type) {
        case(ComponentType::Resistor): {
            sf::Text value = ValuetoWideStringLiteral(double(component.value), mainFont, 20, sf::Color(223, 208, 184));
            value.setScale(1, -1);
            value.setPosition(sf::Vector2f(position.x + 60 * sin(rotation), position.y - 60 * cos(rotation)));
            value.setOrigin(value.getLocalBounds().width / 2, 0);
            ValueBox.text = value;
            ValueBox.setTextBoundingBox();
            break;
        }

        case(ComponentType::VoltageSource): {
            sf::Text value(doubleToString(component.value) + " V", mainFont, 20);
            value.setFillColor(sf::Color(223, 208, 184));
            value.setScale(1, -1);
            value.setPosition(sf::Vector2f(position.x, position.y - 50));
            value.setOrigin(value.getLocalBounds().width / 2, 0);
            ValueBox.text = value;
            ValueBox.setTextBoundingBox();
            break;
        }

        case(ComponentType::CurrentSource): {
            sf::Text value(doubleToString(component.value) + " A", mainFont, 20);
            value.setFillColor(sf::Color(223, 208, 184));
            value.setScale(1, -1);
            value.setPosition(sf::Vector2f(position.x, position.y - 50));
            value.setOrigin(value.getLocalBounds().width / 2, 0);
            ValueBox.text = value;
            ValueBox.setTextBoundingBox();
            break;
        }

        case(ComponentType::Capacitor): {
            sf::Text value(doubleToString(component.value) + " F", mainFont, 20);
            value.setFillColor(sf::Color(223, 208, 184));
            value.setScale(1, -1);
            value.setPosition(sf::Vector2f(position.x, position.y - 50));
            value.setOrigin(value.getLocalBounds().width / 2, 0);
            ValueBox.text = value;
            ValueBox.setTextBoundingBox();
            break;
        }

        case(ComponentType::Inductor): {
            sf::Text value(doubleToString(component.value) + " H", mainFont, 20);
            value.setFillColor(sf::Color(223, 208, 184));
            value.setScale(1, -1);
            value.setPosition(sf::Vector2f(position.x, position.y - 50));
            value.setOrigin(value.getLocalBounds().width / 2, 0);
            ValueBox.text = value;
            ValueBox.setTextBoundingBox();
            break;
        }

        case(ComponentType::Switch): {
            sf::Text value("S", mainFont, 20);
            value.setFillColor(sf::Color(223, 208, 184));
            value.setScale(1, -1);
            value.setPosition(sf::Vector2f(position.x, position.y - 50));
            value.setOrigin(value.getLocalBounds().width / 2, 0);
            ValueBox.text = value;
            ValueBox.setTextBoundingBox();
            break;
        }
    }
}