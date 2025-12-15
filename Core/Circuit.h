#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include "SFML/Graphics.hpp"

#include "Component.h"
class Circuit
{
public:
	std::unordered_map<int, std::vector<std::pair<int, int>>> adjacencyList; // key = node ID, vector = list of (neighborNode, componentID)
	bool isSimulating = false;
	std::vector<Component> components;


	int AddComponent(const ComponentType& type);

	Component* GetComponentByID(int id);

	void RemoveComponent(int id);

	void ComputeTopology();
private:
	int nextComponentID = 0;
};

