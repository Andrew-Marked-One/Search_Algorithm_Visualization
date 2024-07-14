#pragma once
#include "Vec2.h"

Vec2::Vec2() {}
Vec2::Vec2(const float xin, const float yin)
		: x(xin), y(yin) {}
Vec2::Vec2(const sf::Vector2f& vec)
		: x(vec.x), y(vec.y) {}
Vec2::Vec2(const sf::Vector2i& vec)
		: x(static_cast<float>(vec.x)), y(static_cast<float>(vec.y)) {}
Vec2::Vec2(const sf::Vector2u& vec)
	: x(static_cast<float>(vec.x)), y(static_cast<float>(vec.y)) {}

bool Vec2::operator == (const Vec2& rhs) const {
	return (x == rhs.x && y == rhs.y);
}
bool Vec2::operator != (const Vec2& rhs) const {
	return !(x == rhs.x && y == rhs.y);
}
Vec2 Vec2::operator + (const Vec2& rhs) const {
	return Vec2(x + rhs.x, y + rhs.y);
}
Vec2 Vec2::operator - (const Vec2& rhs) const {
	return Vec2(x - rhs.x, y - rhs.y);
}
Vec2 Vec2::operator / (const float val) const {
	return Vec2(x / val, y / val);
}
Vec2 Vec2::operator / (const Vec2& rhs) const {
	return Vec2(x / rhs.x, y / rhs.y);
}
Vec2 Vec2::operator * (const float val) const {
	return Vec2(x * val, y * val);
}
Vec2 Vec2::operator * (const Vec2& rhs) const {
	return Vec2(x * rhs.x, y * rhs.y);
}

void Vec2::operator += (const Vec2& rhs) {
	x += rhs.x;
	y += rhs.y;
}
void Vec2::operator -= (const Vec2& rhs) {
	x -= rhs.x;
	y -= rhs.y;
}
void Vec2::operator *= (const float rhs) {
	x *= rhs;
	y *= rhs;
}
void Vec2::operator /= (const float rhs) {
	x /= rhs;
	y /= rhs;
}
float Vec2::distMag() const {
	return sqrt(x * x + y * y);
}
Vec2& Vec2::normalize() {
	float d = this->distMag();
	x /= d;
	y /= d;
	return *this;
}
Vec2& Vec2::round() {
	this->x = std::round(this->x);
	this->y = std::round(this->y);
	return *this;
}
Vec2& Vec2::abs() {
	this->x = std::abs(this->x);
	this->y = std::abs(this->y);
	return *this;
}
float Vec2::min() const {
	return std::min(this->x, this->y);
}
float Vec2::max() const{
	return std::max(this->x, this->y);
}
std::string Vec2::print() {
	return std::to_string(x) + " " + std::to_string(y);
}
