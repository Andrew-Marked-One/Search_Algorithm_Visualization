#pragma once
#include "Components.h"

CTransform::CTransform() {}
CTransform::CTransform(const Vec2& p)
	: pos(p) {}


CShape::CShape() {}
CShape::CShape(float sizeX, float sizeY, const sf::Color& fill, const sf::Color& outline, float thickness)
		: shape(sf::Vector2f(sizeX, sizeY))
		, halfSize(sizeX / 2.f, sizeY / 2.f){
		shape.setFillColor(fill);
		shape.setOutlineColor(outline);
		shape.setOutlineThickness(thickness);
		shape.setOrigin(halfSize.x, halfSize.y);
}


CNode::CNode() {}
CNode::CNode(const Vec2& actionParam, const Vec2& parentParam, int costParam)
	: action(actionParam)
	, parent(parentParam)
	, cost(costParam) {}