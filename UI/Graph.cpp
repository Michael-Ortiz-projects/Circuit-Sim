#include "Graph.h"

void Graph::setData(const std::vector<sf::Vector2f>& points) {
    data = points;
}

void Graph::setBounds(float minX_, float maxX_, float minY_, float maxY_) {
    minX = minX_;
    maxX = maxX_;
    minY = minY_;
    maxY = maxY_;
    updateView();
}

GraphBounds Graph::getBounds() {
    return { minX, maxX, minY, maxY };
}


void Graph::autoScale() {
    if (data.empty()) return;

    minX = maxX = data[0].x;
    minY = maxY = data[0].y;

    for (const auto& p : data) {
        minX = std::min(minX, p.x);
        maxX = std::max(maxX, p.x);
        minY = std::min(minY, p.y);
        maxY = std::max(maxY, p.y);
    }

    // Force origin to be included
    minX = std::min(minX, 0.f);
    maxX = std::max(maxX, 0.f);
    minY = std::min(minY, 0.f);
    maxY = std::max(maxY, 0.f);

    float rangeX = maxX - minX;
    float rangeY = maxY - minY;

    if (rangeX == 0.f) {
        minX -= 1.f;
        maxX += 1.f;
        rangeX = maxX - minX;
    }

    if (rangeY == 0.f) {
        minY -= 1.f;
        maxY += 1.f;
        rangeY = maxY - minY;
    }
    const float paddingPercent = 0.05f; // 5%

    float padX = rangeX * paddingPercent;
    float padY = rangeY * paddingPercent;

    minX -= padX;
    maxX += padX;
    minY -= padY;
    maxY += padY;

    updateView();
}

void Graph::setArea(const sf::FloatRect& area, const sf::Vector2u& windowSize) {
    drawArea = area;
    std::cout
        << drawArea.left / windowSize.x << " "
        << drawArea.top / windowSize.y << " "
        << drawArea.width / windowSize.x << " "
        << drawArea.height / windowSize.y << "\n";
    view.setViewport({
        drawArea.left / windowSize.x,
        drawArea.top / windowSize.y,
        drawArea.width / windowSize.x,
        drawArea.height / windowSize.y
        });

    updateView();

    sliderMinX = area.left;
    currentX_pixel = sliderMinX;
    sliderMaxX = area.left + area.width;
    sliderY = area.top - 30;
    sliderHandle.setOrigin(10, 10);
    sliderHandle.setPosition(sliderMinX, sliderY);
    sliderHandle.setSize({ 20, 20 });
    sliderHandle.setFillColor(sf::Color::White);

}



void Graph::handleEvent(const sf::Event& event, sf::RenderWindow& window) {


    


    if (cursorXOutputTextBox.handleEvent(event)) {
        //do reverse lookup to set cursor
        std::cout << "X text Enter Pressed\n";
    }
    if (cursorYOutputTextBox.handleEvent(event)) {
        //do reverse lookup to set cursor
        std::cout << "Y text Enter Pressed\n";
    }
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mouse(
            (float)event.mouseButton.x,
            (float)event.mouseButton.y
        );
        cursorXOutputTextBox.setActive(cursorXOutputTextBox.contains(mouse));
        cursorYOutputTextBox.setActive(cursorYOutputTextBox.contains(mouse));
        if (sliderHandle.getGlobalBounds().contains(mouse))
        {
            sliderDragging = true;
            std::cout << "sliderDragging set true\n";
        }
    }

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sliderDragging = false;
        std::cout << "sliderDragging set false\n";

    }

    if (event.type == sf::Event::MouseMoved && sliderDragging) {
        sf::Vector2i pixel(
            event.mouseMove.x,
            event.mouseMove.y
        );


        currentX_pixel = std::clamp((float)pixel.x, sliderMinX, sliderMaxX);
        
        std::cout << "\n";
        sliderHandle.setPosition(currentX_pixel, sliderY);
          
        float X_Val = pixelToGraphX(currentX_pixel);
        float Y_Val = getInterpolatedY(X_Val);
        currentY_pixel = graphYtoPixel(Y_Val);

        std::cout << "X = " << X_Val << "    Y = " << Y_Val << "\n";
        std::cout << "Pixel Y = " << graphYtoPixel(Y_Val) << "\n";

        cursorXOutputTextBox.setText(std::to_string(X_Val));
        cursorYOutputTextBox.setText(std::to_string(Y_Val));
        
    }
}



float Graph::niceFraction(float value) {
    float exponent = std::floor(std::log10(value));
    float fraction = value / std::pow(10.f, exponent);

    float niceFraction;

    if (fraction < 1.5f)      niceFraction = 1.f;
    else if (fraction < 3.f)  niceFraction = 2.f;
    else if (fraction < 7.f)  niceFraction = 5.f;
    else                      niceFraction = 10.f;

    return niceFraction * std::pow(10.f, exponent);
}

float Graph::pixelToGraphX(float pixelX) {
    float t = (pixelX - sliderMinX) / (sliderMaxX - sliderMinX);

    t = std::clamp(t, 0.f, 1.f);

    return minX + t * (maxX - minX);
}

int Graph::graphYtoPixel(float y) {
    float t = (y - minY) / (maxY - minY);

    t = std::clamp(t, 0.f, 1.f);
    

    return drawArea.top + drawArea.height - t * drawArea.height;
}

float Graph::getInterpolatedY(float x)
{
    if (data.empty())
        return 0.f;

    auto it = std::lower_bound(
        data.begin(),
        data.end(),
        x,
        [](const sf::Vector2f& p, float x)
        {
            return p.x < x;
        });

    if (it == data.begin())
        return it->y;

    if (it == data.end())
        return data.back().y;

    auto p2 = *it;
    auto p1 = *(it - 1);

    float t = (x - p1.x) / (p2.x - p1.x);

    return p1.y + t * (p2.y - p1.y);
}

void Graph::draw(sf::RenderWindow& window) {
    // Save previous view
    sf::View oldView = window.getView();
    drawBorder(window);
    window.setView(view);
    drawGrid(window);
    drawAxes(window);
    drawData(window);
    drawSelectionBox(window);
    window.setView(window.getDefaultView());
    drawLabels(window);
    drawSlider(window);

    cursorXOutputTextBox.draw(window);
    cursorYOutputTextBox.draw(window);
    window.setView(oldView);

}

void Graph::updateView() {
    view.setCenter(
        (minX + maxX) / 2.f,
        (minY + maxY) / 2.f
    );

    // Negative height flips Y so +Y is up
    view.setSize(
        (maxX - minX),
        -(maxY - minY)
    );

    float X_Val = pixelToGraphX(currentX_pixel);
    float Y_Val = getInterpolatedY(X_Val);
    currentY_pixel = graphYtoPixel(Y_Val);

    std::cout << "X = " << X_Val << "    Y = " << Y_Val << "\n";
    std::cout << "Pixel Y = " << graphYtoPixel(Y_Val) << "\n";
    cursorXOutputTextBox.setText(std::to_string(X_Val));
    cursorYOutputTextBox.setText(std::to_string(Y_Val));
}

void Graph::drawData(sf::RenderTarget& target) {
    if (data.size() < 2) return;

    sf::VertexArray lines(sf::LineStrip, data.size());

    for (size_t i = 0; i < data.size(); ++i) {
        lines[i].position = data[i];
        lines[i].color = sf::Color::Green;
    }

    target.draw(lines);
}

void Graph::drawGrid(sf::RenderTarget& target) {
    spacingX = niceFraction((maxX - minX) / 20.f);
    spacingY = niceFraction((maxY - minY) / 20.f);

    firstX = std::ceil(minX / spacingX) * spacingX;
    tickCountX = std::floor((maxX - firstX) / spacingX);

    for (int i = 0; i <= tickCountX; i++) {
        float x = firstX + i * spacingX;

        sf::Vertex line[] = {
            {{x, minY}, sf::Color(80,80,80)},
            {{x, maxY}, sf::Color(80,80,80)}
        };

        target.draw(line, 2, sf::Lines);
    }

    firstY = std::ceil(minY / spacingY) * spacingY;
    tickCountY = std::floor((maxY - firstY) / spacingY);
    for (int i = 0; i <= tickCountY; i++) {
        float y = firstY + i * spacingY;

        sf::Vertex line[] = {
            {{minX, y}, sf::Color(80,80,80)},
            {{maxX, y}, sf::Color(80,80,80)}
        };

        target.draw(line, 2, sf::Lines);
    }
}

void Graph::drawAxes(sf::RenderTarget& target) {
    if (minY <= 0 && maxY >= 0) {
        sf::Vertex xAxis[] = {
            {{minX, 0.f}, sf::Color::White},
            {{maxX, 0.f}, sf::Color::White}
        };
        target.draw(xAxis, 2, sf::Lines);
    }

    if (minX <= 0 && maxX >= 0) {
        sf::Vertex yAxis[] = {
            {{0.f, minY}, sf::Color::White},
            {{0.f, maxY}, sf::Color::White}
        };
        target.draw(yAxis, 2, sf::Lines);
    }
}

void Graph::drawBorder(sf::RenderTarget& target) {
    sf::RectangleShape border;
    border.setPosition(drawArea.left, drawArea.top);
    border.setSize({ drawArea.width, drawArea.height });
    border.setFillColor(sf::Color::Black);
    border.setOutlineThickness(1.f);
    border.setOutlineColor(sf::Color::White);
    target.draw(border);
}

void Graph::drawLabels(sf::RenderTarget& target) {
    
    for (int i = 0; i <= tickCountX; i++) {

        float x = firstX + i * spacingX;

        sf::Vector2f worldPos(x, 0.f);
        sf::Vector2i pixelPos = target.mapCoordsToPixel(worldPos, view);

        sf::Text label;
        label.setFont(font);
        label.setCharacterSize(12);
        label.setFillColor(sf::Color::White);

        std::ostringstream ss;
        ss << x;
        label.setString(ss.str());

        label.setPosition(
            pixelPos.x - label.getLocalBounds().width / 2.f,
            drawArea.top + drawArea.height + 2.f
        );

        target.draw(label);
    }

    
    for (int i = 0; i <= tickCountY; i++) {

        float y = firstY + i * spacingY;

        sf::Vector2f worldPos(0.f, y);
        sf::Vector2i pixelPos = target.mapCoordsToPixel(worldPos, view);

        sf::Text label;
        label.setFont(font);
        label.setCharacterSize(12);
        label.setFillColor(sf::Color::White);

        std::ostringstream ss;
        ss << y;
        label.setString(ss.str());

        label.setPosition(
            drawArea.left - label.getLocalBounds().width - 4.f,
            pixelPos.y - label.getLocalBounds().height / 2.f
        );
        target.draw(label);
    }
    sf::Text XcursorLabel;
    XcursorLabel.setFont(font);
    XcursorLabel.setCharacterSize(16);
    XcursorLabel.setFillColor(sf::Color::White);
    XcursorLabel.setString("X Value:");
    XcursorLabel.setPosition(cursorXOutputTextBox.getBox().getGlobalBounds().getPosition() + sf::Vector2f(0, -30));


    sf::Text YcursorLabel;
    YcursorLabel.setFont(font);
    YcursorLabel.setCharacterSize(16);
    YcursorLabel.setFillColor(sf::Color::White);
    YcursorLabel.setString("Y Value:");
    YcursorLabel.setPosition(cursorYOutputTextBox.getBox().getGlobalBounds().getPosition() + sf::Vector2f(0, -30));

    target.draw(XcursorLabel);
    target.draw(YcursorLabel);
}

void Graph::drawSelectionBox(sf::RenderTarget& target) {

    if (!dragging) return;

    sf::RectangleShape box;
    box.setPosition(selectionRect.left, selectionRect.top);
    box.setSize({ selectionRect.width, selectionRect.height });
    box.setFillColor(sf::Color(100, 100, 255, 50));
    target.draw(box);
}

void Graph::drawSlider(sf::RenderTarget& target) {
    
    sf::RectangleShape sliderTrack;
    sliderTrack.setPosition({ sliderMinX, sliderY });
    sliderTrack.setFillColor(sf::Color::Red);
    sliderTrack.setSize({ sliderMaxX - sliderMinX, 5 });
    
    target.draw(sliderTrack);
    target.draw(sliderHandle);


    sf::Vertex cursorYLine[] = {
        {{(float)currentX_pixel, sliderY}, sf::Color::Blue},
        {{(float)currentX_pixel, drawArea.top + drawArea.height}, sf::Color::Blue}
    };
    sf::Vertex cursorXLine[] = {
        {{drawArea.left, (float)currentY_pixel}, sf::Color::Blue},
        {{drawArea.left + drawArea.width, (float)currentY_pixel}, sf::Color::Blue}
    };

    target.draw(cursorYLine, 2, sf::Lines);
    target.draw(cursorXLine, 2, sf::Lines);
}
