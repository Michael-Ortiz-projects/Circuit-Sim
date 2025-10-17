#pragma once
#include "SFML/Graphics.hpp"
#include "TextBox.h"
#include "Component.h"
class Oscilloscope {
public:
    Component* target = nullptr;
    sf::RenderWindow window;
    sf::View view;
    std::vector<float> signal;

    float maxVoltage;
    float minVoltage;
    float timeDivision;
    int sampleSize;
    int writeIndex = 0;
    int numSamplesWritten = 0;
    int graphWidth;
    int graphHeight;
    int margin;

    float xScaleFactor;

    sf::Font& font;

    Oscilloscope(float min, float maxVoltage, float timeDivision, int sampleSize, sf::Font& font);

    void setTarget(Component* c);
    void update();
    void draw();

    
    void drawGrid();
    void drawLabels();
private:
    
};

