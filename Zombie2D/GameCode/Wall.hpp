#pragma once
#include "Box2D/Common/b2Math.h"

class Wall
{
	b2Vec2 from;
	b2Vec2 to;
public:
	Wall() = default;
	constexpr Wall(const Wall&) = default;
	constexpr Wall(b2Vec2 from, b2Vec2 to): from(from), to(to)
	{}
	constexpr b2Vec2 From() const
	{
		return from;
	}
	constexpr b2Vec2 To() const
	{
		return to;
	}
	b2Vec2 Normal() const
	{
		auto norm = (to - from).Skew();
		norm.Normalize();
		return norm;
	}
};
