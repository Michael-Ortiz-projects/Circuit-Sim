#include "Oscilloscope.h"

Oscilloscope::Oscilloscope(float min, float maxVoltage, float timeDiv, int sampleSize, sf::Font& font) : 
maxVoltage(maxVoltage), timeDivision(timeDiv), sampleSize(sampleSize), font(font),
graphWidth(1280), graphHeight(720), margin(50), signal(sampleSize, 0.0f), minVoltage(min) {
    xScaleFactor = static_cast<float>(sampleSize) / graphWidth;
    window.create(sf::VideoMode(graphWidth, graphHeight + 2 * margin), "Scope");
    window.setPosition(sf::Vector2i(-1290, 360));

    view.setCenter(graphWidth / 2, graphHeight / 2);
    view.setSize(graphWidth, -graphHeight);
    window.setView(view);
    window.clear(sf::Color::Black);
    window.display();
}

void Oscilloscope::setTarget(Component* c) {
    target = c;
}

void Oscilloscope::update() {
    if (!target) return;

    float data = target->voltage;

    signal[writeIndex] = data;
    writeIndex = (writeIndex + 1) % sampleSize;
    draw();
}

void Oscilloscope::draw() {
    

    window.clear(sf::Color::Black);

    // Draw waveform
    sf::VertexArray waveform(sf::LineStrip, signal.size());
    for (int i = 0; i < sampleSize; ++i) {
        int index = (writeIndex + i) % sampleSize;
        float x = static_cast<float>(i / xScaleFactor);
        float y = margin + (signal[index] / maxVoltage) * (graphHeight - 2 * margin);
        waveform[i] = sf::Vertex(sf::Vector2f(x, y), sf::Color::Green);
    }

    drawGrid();
    drawLabels();
    window.draw(waveform);
    window.display();
}

void Oscilloscope::drawGrid() {
    sf::VertexArray timeDivisions(sf::Lines, 20);
    for (int i = 0; i < 10; i++) {
        timeDivisions[2 * i].position = sf::Vector2f((graphWidth * i / 10), margin);
        timeDivisions[2 * i].color = sf::Color(40, 40, 48);
        timeDivisions[2 * i + 1].position = sf::Vector2f((graphWidth * i / 10), graphHeight - margin);
        timeDivisions[2 * i + 1].color = sf::Color(40, 40, 48);
    }
    sf::VertexArray voltageDivisions(sf::Lines, 12);
    for (int i = 0; i < 6; i++) {
        voltageDivisions[2 * i].position = sf::Vector2f(0, margin + ((graphHeight - 2 * margin) * i / 5));
        voltageDivisions[2 * i].color = sf::Color(40, 40, 48);
        voltageDivisions[2 * i + 1].position = sf::Vector2f(graphWidth, margin + ((graphHeight - 2 * margin) * i / 5));
        voltageDivisions[2 * i + 1].color = sf::Color(40, 40, 48);
    }
    window.draw(timeDivisions);
    window.draw(voltageDivisions);
}

void Oscilloscope::drawLabels() {
    TextBox minVolt(sf::Vector2f(20, 40), mainFont);
    minVolt.text.setString("Min Voltage = " + std::to_string(minVoltage));
    minVolt.setTextBoundingBox();
    TextBox maxVolt(sf::Vector2f(20 + minVolt.box.getPosition().x + minVolt.box.getSize().x + 100, 40), mainFont);
    maxVolt.text.setString("Max Voltage = " + std::to_string(maxVoltage));
    maxVolt.setTextBoundingBox();
    TextBox timeDiv(sf::Vector2f(20 + maxVolt.box.getPosition().x + maxVolt.box.getSize().x + 100, 40), mainFont);
    timeDiv.text.setString("Time Division = " + std::to_string(timeDivision * sampleSize / 10));
    timeDiv.setTextBoundingBox();
    minVolt.draw(window);
    maxVolt.draw(window);
    timeDiv.draw(window);
}