#include "SFML/Graphics.hpp"
#include <iostream>
#include "AssetManager.h"
#include <sstream>
#include "TextBox.h"
struct GraphBounds {
    float minX, maxX, minY, maxY;
};
class Graph {
public:
    sf::FloatRect selectionRect;
    bool dragging;
    sf::FloatRect drawArea;

    float sliderMinX;
    float sliderMaxX;
    float sliderY;
    int currentX_pixel;
    int currentY_pixel;

    sf::RectangleShape sliderHandle;
    bool sliderDragging;
    TextBox cursorXOutputTextBox;
    TextBox cursorYOutputTextBox;


    Graph(AssetManager& asset, const sf::FloatRect& area) 
        : font(asset.mainFont), 
        cursorYOutputTextBox(asset.mainFont, area.getPosition() + sf::Vector2f(area.getSize().x + 180, 50), {150, 50}, 16, "Graph initializer text"),
        cursorXOutputTextBox(asset.mainFont, area.getPosition() + sf::Vector2f(area.getSize().x + 30, 50), { 150, 50 }, 16, "Graph initializer text") {}

    void setData(const std::vector<sf::Vector2f>& points);

    void setBounds(float minX_, float maxX_, float minY_, float maxY_);

    GraphBounds getBounds();

    void autoScale();

    void setArea(const sf::FloatRect& area, const sf::Vector2u& windowSize);

    void handleEvent(const sf::Event& event, sf::RenderWindow& window);

    bool dataEmpty() { return data.empty(); }

    sf::View& getView() { return view; }

    void draw(sf::RenderWindow& window);
private:
    sf::Font& font;
    std::vector<sf::Vector2f> data;

    float minX = -1.f, maxX = 1.f;
    float minY = -1.f, maxY = 1.f;

    int tickCountX, tickCountY;
    float firstX, firstY, spacingX, spacingY;

    sf::View view;
    
    void updateView();

    float niceFraction(float value);

    float pixelToGraphX(float pixelX);


    int graphYtoPixel(float y);

    float getInterpolatedY(float x);

    void drawData(sf::RenderTarget& target);

    void drawGrid(sf::RenderTarget& target);

    void drawAxes(sf::RenderTarget& target);

    void drawBorder(sf::RenderTarget& target);

    void drawLabels(sf::RenderTarget& target);

    void drawSelectionBox(sf::RenderTarget& target);

    void drawSlider(sf::RenderTarget& target);

};