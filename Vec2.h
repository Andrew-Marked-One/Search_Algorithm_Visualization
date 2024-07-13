#pragma once
#include <cmath>
#include "SFML/Graphics.hpp"

class Vec2 {
public:
	float x = 0, y = 0;
	Vec2();
	Vec2(const float xin, const float yin);
	Vec2(const sf::Vector2f& vec);
	Vec2(const sf::Vector2i& vec);
	Vec2(const sf::Vector2u& vec);

	bool operator == (const Vec2& rhs) const;
	bool operator != (const Vec2& rhs) const;
	Vec2 operator + (const Vec2& rhs) const;
	Vec2 operator - (const Vec2& rhs) const;
	Vec2 operator / (const float val) const;
	Vec2 operator / (const Vec2& rhs) const;
	Vec2 operator * (const float val) const;
	Vec2 operator * (const Vec2& rhs) const;

	void operator += (const Vec2& rhs);
	void operator -= (const Vec2& rhs);
	void operator *= (const float rhs);
	void operator /= (const float rhs);
	float distMag() const;
	Vec2& normalize();
	Vec2& round();
	Vec2& abs();
	float min() const;
	float max() const;
	std::string print();
};