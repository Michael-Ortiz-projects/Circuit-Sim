#include "HelperFunctions.h"
#include <sstream>
#include <iomanip>
#include "global_variables.h"

std::string to_string(CursorState state) {
    switch (state) {
    case CursorState::Default: return "Default";
    case CursorState::CreatingResistor: return "CreatingResistor";
    case CursorState::CreatingVoltageSource: return "CreatingVoltageSource";
    case CursorState::CreatingCurrentSource: return "CreatingCurrentSource";
    case CursorState::PlacingResistor: return "PlacingResistor";
    case CursorState::PlacingVoltageSource: return "PlacingVoltageSource";
    case CursorState::PlacingCurrentSource: return "PlacingCurrentSource";
    case CursorState::CreatingWire: return "CreatingWire";
    case CursorState::PlacingWire: return "PlacingWire";
    case CursorState::SelectingComponent: return "SelectingComponent";
    default: return "Unknown";
    }
}

bool VectorForwardPairCheck(const std::vector<int>& vector, int x, int y) {

    if (vector.size() < 2) return false;

    for (int i = 0; i < vector.size() - 1; ++i) {
        if (vector[i] == x && vector[i + 1] == y) {
            return true;   // Forward order found
        }
        if (vector[i] == y && vector[i + 1] == x) {
            return false;  // Reversed order found, stop checking and return false
        }
    }

    // Wrap-around check
    if (vector[vector.size() - 1] == x && vector[0] == y) {
        return true;
    }
    if (vector[vector.size() - 1] == y && vector[0] == x) {
        return false;
    }

    return false;
}

void drawGrid(sf::RenderWindow& window) {
    for (float x = 0; x < SCR_WIDTH; x += GRIDSIZE) {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(x, 0), sf::Color(40, 40, 48)),
            sf::Vertex(sf::Vector2f(x, SCR_HEIGHT), sf::Color(40, 40, 48))
        };
        window.draw(line, 2, sf::Lines);
    }

    for (float y = 0; y < SCR_HEIGHT; y += GRIDSIZE) {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(0, y), sf::Color(40, 40, 48)),
            sf::Vertex(sf::Vector2f(SCR_WIDTH, y), sf::Color(40, 40, 48))
        };
        window.draw(line, 2, sf::Lines);
    }
}


bool alphanum_compare_string(const std::string& a, const std::string& b) {
    int indexA = 0;
    int indexB = 0;

    while (indexA < a.size() && indexB < b.size()) {
        if (std::isdigit(a[indexA]) && std::isdigit(b[indexB])) {
            int startA = indexA;
            int startB = indexB;

            while (indexA < a.size() && std::isdigit(a[indexA])) {
                indexA++;
            }

            while (indexB < b.size() && std::isdigit(b[indexB])) {
                indexB++;
            }
            int numA = std::stoi(a.substr(startA, indexA - startA));
            int numB = std::stoi(b.substr(startB, indexB - startB));

            if (numA != numB) {
                return numA < numB;
            }

        }
        else {
            if (a[indexA] != b[indexB]) {
                return a[indexA] < b[indexB];
            }

            indexA++;
            indexB++;
        }
    }

    return a.size() < b.size();
}

int FindStringIndex(std::string target, std::vector<std::string> vector) {
    for (int i = 0; i < vector.size(); i++) {
        if (target == vector[i]) return i;
    }
    return -1;
}

void printMatrix(std::vector<std::vector<float>>& matrix) {
    int colWidth = 0;

    for (const auto& row : matrix) {
        for (double val : row) {
            int len = std::to_string(static_cast<int>(std::abs(val))).length();
            if (val < 0) len++; //account for negative sign
            colWidth = std::max(colWidth, len);
        }
    }
    colWidth += 4; //for spacing

    for (const auto& row : matrix) {
        for (double val : row) {
            std::cout << std::setw(colWidth) << std::fixed << std::setprecision(2) << val;
        }
        std::cout << "\n";
    }
}

const float EPSILON = 1e-6;

void rref(std::vector<std::vector<float>>& matrix) {
    int rowCount = matrix.size();
    if (rowCount == 0) return;
    int colCount = matrix[0].size();

    int lead = 0;
    for (int r = 0; r < rowCount; ++r) {
        if (lead >= colCount)
            return;

        int i = r;
        while (std::fabs(matrix[i][lead]) < EPSILON) {
            ++i;
            if (i == rowCount) {
                i = r;
                ++lead;
                if (lead == colCount)
                    return;
            }
        }

        std::swap(matrix[i], matrix[r]);

        float lv = matrix[r][lead];
        if (std::fabs(lv) > EPSILON) {
            for (int j = 0; j < colCount; ++j)
                matrix[r][j] /= lv;
        }

        for (int i = 0; i < rowCount; ++i) {
            if (i != r) {
                float lv2 = matrix[i][lead];
                for (int j = 0; j < colCount; ++j)
                    matrix[i][j] -= lv2 * matrix[r][j];
            }
        }

        ++lead;
    }
}


sf::Text ValuetoWideStringLiteral(double value, const sf::Font& font, unsigned int size, sf::Color color) {
    std::wostringstream woss;
    if (std::floor(value) == value) {
        woss << static_cast<int>(value);  // No decimals
    }
    else {
        woss << std::fixed << std::setprecision(2) << value;
    }
    
    woss << L" \u03A9";
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setString(woss.str());
    text.setScale(1, -1);
    //std::wcout << woss.str() << "\n";

    return text;
}

std::string doubleToString(double value) {
    std::ostringstream out;
    out << std::defaultfloat << value;
    return out.str();
}

std::string floatToString(float value) {
    std::ostringstream out;
    out << std::defaultfloat << value;
    return out.str();
}


bool isPointNearSegment(sf::Vector2f mouse, sf::Vector2f a, sf::Vector2f b, float max_distance) {
    sf::Vector2f ab = b - a; //vector of segment
    sf::Vector2f ap = mouse - a; //vector of mouse

    float dotSquared = ab.x * ab.x + ab.y * ab.y;
    if (dotSquared == 0.0f) //if a == b
        return (std::hypot(ap.x, ap.y) <= max_distance);

    float t = std::max(0.f, std::min(1.f, (ap.x * ab.x + ap.y * ab.y) / dotSquared));
    sf::Vector2f closest = a + t * ab; //point closest to mouse
    sf::Vector2f difference = mouse - closest;
    float dist = std::hypot(difference.x, difference.y);

    return dist <= max_distance;
}

sf::Vector2f snapToGrid(const sf::Vector2f& pos) {
    return {
        std::round(pos.x / GRIDSIZE) * GRIDSIZE,
        std::round(pos.y / GRIDSIZE) * GRIDSIZE
    };
}

bool isCollinear(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c) {
    return (a.x == b.x && b.x == c.x) || (a.y == b.y && b.y == c.y);
}