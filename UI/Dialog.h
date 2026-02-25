#pragma once
#include "Renderer.h"
#include "TextBox.h"
#include "Button.h"
#include "DropdownMenu.h"
#include "../Debug.h"

struct EditDialogResult {
    std::string valueText;
    std::string labelText;
};

struct EditSimulationDialogResult {
    // TRANSIENT PARAMETER STRINGS
    std::string timeStep_text;
    std::string tStart_text;
    std::string tEnd_text;
};

struct GraphDataDialogResult {
    int MNAIndex;
};

struct GraphSettingsDialogResult {
    std::string minX, maxX, minY, maxY;
};

template <typename CommandT>
class Dialog {
public:
    Dialog(sf::Vector2f position, sf::Vector2f size, Renderer& r)
        : renderer(r) {
        panel.setSize(size);
        panel.setFillColor(sf::Color(30, 30, 40));
        panel.setOutlineThickness(2.f);
        panel.setOutlineColor(sf::Color(150, 150, 150));
        panel.setPosition(position);
    }

    void open() {
        active = true;
        std::cout << "Dialog open\n";
    }

    void close() {
        std::cout << "Dialog is closing\n";
        active = false;
    }

    bool isOpen() const {
        //std::cout << "dialog.open = " << active << std::endl;
        return active;
    }

    void addLabel(const std::string& str, const sf::Vector2f& pos, unsigned int size) {
        sf::Text t;
        t.setFont(renderer.getAssets().mainFont);
        t.setString(str);
        t.setCharacterSize(size);
        t.setFillColor(sf::Color::White);
        t.setPosition(panel.getPosition() + pos);
        labels.push_back(t);
    }

    void addTextBox(const std::string& initial, const sf::Vector2f& pos, const sf::Vector2f& size, int textSize, bool outline) {
        textboxes.emplace_back(renderer.getAssets().mainFont, panel.getPosition() + pos, size, textSize, initial, outline);
    }

    void addButton(const std::string& label, CommandT cmd, const sf::Vector2f& pos, const sf::Vector2f& size) {
        buttons.emplace_back(renderer.getAssets().mainFont, label, panel.getPosition() + pos, size, cmd);
    }

    void addDropdown(MenuID ID, sf::Font& fnt, const std::string& label, const sf::Vector2f& pos, const sf::Vector2f& size, CommandT cmd) {
        menu_map.try_emplace(ID, fnt, label, panel.getPosition() + pos, size, cmd);
    }


    void handleEvent(const sf::Event& event) {
        sf::Vector2f eventVector;
        //std::cout << "\neventVector = ";
        if (event.type == sf::Event::MouseMoved) {
            eventVector = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
            //Debug::printVector2f(eventVector);
        }

        else if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
            eventVector = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
            //Debug::printVector2f(eventVector);
        }
        if (!active) return;

        auto& window = renderer.getWindow();

        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2f mouse = window.mapPixelToCoords({ eventVector }, renderer.getUIView());
            //Debug::printVector2f(mouse);
            for (auto& tb : textboxes) {
                bool contain = tb.contains(mouse);

                tb.setActive(contain);
            }
        }

        for (auto& tb : textboxes)
            tb.handleEvent(event);
        

        if (event.type == sf::Event::MouseMoved ||
            event.type == sf::Event::MouseButtonPressed ||
            event.type == sf::Event::MouseButtonReleased)
        {  


            for (auto& b : buttons) {
                b.onMouseMove(eventVector);
                if (event.type == sf::Event::MouseButtonPressed)
                    b.onMousePress(eventVector);
                if (event.type == sf::Event::MouseButtonReleased)
                    b.onMouseRelease(eventVector);
            }
            for (auto& [id, menu] : menu_map) {
                
                menu.onMouseMove(eventVector);
                if (event.type == sf::Event::MouseButtonPressed)
                    menu.onMousePress(eventVector);
                if (event.type == sf::Event::MouseButtonReleased)
                    menu.onMouseRelease(eventVector);
            }
                
        }
    }

    bool poll(CommandT& out) {
        if (buttonPressed(out)) return true;
        for (auto& [id, menu] : menu_map) {
            if (menu.active) {
                if (menu.poll(out)) return true;
            }
        }
        return false;
    }

    bool buttonPressed(CommandT& outCmd) {
        for (auto& b : buttons) {
            if (b.consumed(outCmd))
                return true;
        }
        return false;
    }

    std::string getText(int index) const {
        if (index >= textboxes.size()) return " default text";
        return textboxes[index].getText();
    }

    DropdownMenu<CommandT>& getMenu(MenuID id) {
        return menu_map.at(id);
    }


    void addOptionToDropdown(MenuID ID, std::string label, CommandT cmd, int value = -1) {
        menu_map.at(ID).addOption(label, cmd, value);
    }

    void setMenuLabel(MenuID ID, std::string label) {
        menu_map.at(ID).setLabel(label);
    }

    void draw() {
        if (!active) return;

        auto& window = renderer.getWindow();

        window.draw(panel);

        for (auto& l : labels)
            window.draw(l);

        for (auto& tb : textboxes)
            tb.draw(window);

        for (auto& b : buttons)
            b.draw(window);
        
        for (auto& [id, m] : menu_map) {
            if (m.active)
                m.draw(window);
        }
    }


private:
    Renderer& renderer;
    sf::RectangleShape panel;
    std::vector<Button<CommandT>> buttons;
    std::map<MenuID, DropdownMenu<CommandT>> menu_map;
    
    std::vector<sf::Text> labels;
    std::vector<TextBox> textboxes;

    bool active = false;
};
