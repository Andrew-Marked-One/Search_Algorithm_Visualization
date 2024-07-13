#pragma once
#include "Vec2.h"
#include "SFML/Graphics.hpp"
#include <limits>

class Component {
public:
	bool has = false;
};

class CTransform : public Component {
public:
	Vec2  pos      = { 0.0, 0.0 };

	CTransform();
	CTransform(const Vec2& p);
};

class CShape : public Component {
public:
	sf::RectangleShape shape;
	Vec2 halfSize = { 1, 1 };
	CShape();
	CShape(float sizeX, float sizeY, const sf::Color& fill, const sf::Color& outline, float thickness);
	
};

class CNode : public Component {
public:
	Vec2 action = { 0, 0 };
	Vec2 parent = {-1, -1};
	int cost = 0;
	int distCost = std::numeric_limits<int>::max();
	CNode();
	CNode(const Vec2& actionParam, const Vec2& parentParam, int costParam);
};