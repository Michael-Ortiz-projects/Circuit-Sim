#include <sstream>
#include <iomanip>
#include "global_variables.h"
#include "Component.h"

Component::Component(int from, int to, const ComponentType& ty, std::string name, double val) {
    fromNode = from;
    toNode = to;
    type = ty;
    label = name;
    value = val;
    current = 0;
    voltage = 0;
    switch (ty) {
        case ComponentType::VoltageSource: {
            voltage = val;
            break;
        }

        case ComponentType::CurrentSource: {
            current = val;
            break;
        }
        
        case ComponentType::Switch: {
            isClosed = false;
            break;
        }
    }
    
}

Component::Component() {

}


void Component::printData(TextBox& detailBox, bool updateBox) {
    std::string detailBoxString = "Component Data:\n";
    detailBoxString += ("ID: " + label + "\nType: " + to_string(type) + "\nValue: " + doubleToString(value) + "\nFrom, To nodes: " + std::to_string(fromNode) + ", " + std::to_string(toNode));
    if (!updateBox)
        std::cout << detailBoxString << "\nIdentification Number" << identification_number << "\n";

    if (updateBox) {
        detailBox.text.setString(detailBoxString);
        detailBox.setTextBoundingBox();
    }
}

LoopComponent::LoopComponent(const Component& c, bool direction) {
    comp = c;
    forward = direction;
}

std::string to_string(ComponentType type) { //converts componentType to string
    switch (type) {
    case (ComponentType::Resistor):
        return "Resistor";

    case (ComponentType::VoltageSource):
        return "Voltage Source";

    case (ComponentType::CurrentSource):
        return "Current Source";

    case (ComponentType::Capacitor):
        return "Capacitor";

    case (ComponentType::Inductor):
        return "Inductor";
    case (ComponentType::Switch):
        return "Switch";
    default:
        return "Unknown";
    }
}

bool alphanum_compare_component(const Component& x, const Component& y) {
    std::string a = x.label;
    std::string b = y.label;
    int indexA = 0;
    int indexB = 0;

    while (indexA < a.size() && indexB < b.size()) {

        // If both characters are digits, compare the full numbers
        if (std::isdigit(a[indexA]) && std::isdigit(b[indexB])) {

            // Start positions of the number substrings
            int startA = indexA;
            int startB = indexB;

            // Move forward until we reach the end of the digit sequence
            while (indexA < a.size() && std::isdigit(a[indexA])) {
                indexA++;
            }

            while (indexB < b.size() && std::isdigit(b[indexB])) {
                indexB++;
            }

            // Convert the substrings into integers
            int numA = std::stoi(a.substr(startA, indexA - startA));
            int numB = std::stoi(b.substr(startB, indexB - startB));

            // If the numbers are different, return the result of their comparison
            if (numA != numB) {
                return numA < numB;
            }

        }
        else {
            // If characters are not digits, compare them directly
            if (a[indexA] != b[indexB]) {
                return a[indexA] < b[indexB];
            }

            // Move to the next characters
            indexA++;
            indexB++;
        }
    }

    // If one string is a prefix of the other, the shorter one is "less"
    return a.size() < b.size();
}