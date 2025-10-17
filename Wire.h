#pragma once
#include "SFML/Graphics.hpp"
#include <iostream>
#include <algorithm> 
#include <set>
#include "HelperFunctions.h"
#include <optional>
#include <unordered_set>
#include "TextBox.h"

struct Node {
    sf::Vector2f position;
    std::vector<int> neighbors;
};

class Wire {
public:
    sf::Vector2f preview_midpoint;
    sf::Vector2f preview_endpoint;
    std::vector<sf::Vector2f> points;
    sf::VertexArray preview_wire_points;
    
    std::vector<std::pair<int, int>> connected_entity_ids; //pair = index of connected entity in entities, 0 for left, 1 for right, deleting entities needs to also effect this vector
    std::vector<int> anchorNodes;
    std::vector<int> junctionNodes;
    bool editing = false;
    bool selected = false;
    bool deleted = false;

    std::map<int, Node> graph;
    float node_voltage = NULL;

    Wire(sf::Vector2f mousePos);

    void UpdatePreview(sf::Vector2f mousePos, bool invert);

    void AddHalfPreview();

    void AddFullPreview();

    bool isPointNearWire(sf::Vector2f mouse, float max_distance);

    std::pair<bool, int> findClickedNode(sf::Vector2f mousePos);

    std::optional<std::pair<int, int>> findClickedSegment(sf::Vector2f mousePos, float max_distance);

    void insertWireAt(int joiningID, Wire& new_wire);

    int insertWireBetween(int a, int b, Wire& new_wire);

    void moveNode(int node_number, sf::Vector2f position);
    
    void mergeNodes(int a, int b);

    void pruneDeadEnds();

    void pruneCollinearNodes();
    
    void cleanGraph();

    void updateWire(std::pair<int, int>& selectedPair, CursorState& cursorState);

    void cancel();

    void draw(sf::RenderWindow& window);

    void sortConnectedEntities();

    void printData(TextBox& detailBox, bool updateBox);

};