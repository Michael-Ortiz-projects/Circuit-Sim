#include "SFML/Graphics.hpp"
#include "../Debug.h"
#include "../Config.h"
class CameraController {
public:

	CameraController(sf::RenderWindow& Window, sf::View& view);

	void onMousePress(const sf::Vector2i& pixelPos);

	void onMouseMove(const sf::Vector2i& pixelPos);

	void onMouseRelease();

	void onScroll(const sf::Event::MouseWheelScrollEvent& event);

	void setSensitivity(float s);
private:
	sf::RenderWindow& window;
	sf::View& canvasView;
	sf::Vector2i lastMousePixel;
	sf::Vector2f grabWorldPos;
	bool dragging = false;
	float zoomFactor = defaultZoom;
};