#pragma once
#include "UI/Button.h"
#include "Controller/Controller.h"
class Debug
{
public:
    static void setEnabled(bool e) { enabled = e; }

    static void UICommand(UICommand cmd) {
        if (!enabled) return;
        switch (cmd) {
        case UICommand::None: std::cout << "UI Command = None\n\n"; break;
        case UICommand::ToggleMenu: std::cout << "UI Command = ToggleMenu\n\n"; break;
        case UICommand::PlaceVoltageSource: std::cout << "UI Command = PlaceVoltageSource\n\n"; break;
        case UICommand::PlaceResistor: std::cout << "UI Command = PlaceResistor\n\n"; break;
        }
    }

    static void setHandler(const char* name) {
        std::cout << "[Controller] Handler set: " << name << "\n\n";
    }
private:
    static inline bool enabled = true; // default on
};


