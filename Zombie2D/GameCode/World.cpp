#include "World.hpp"

World::Ray::RayIterator& World::Ray::RayIterator::operator++()
{
	if (tMaxX < tMaxY)
	{
		tMaxX += tDeltaX;
		X += deltaX;
	}
	else
	{
		tMaxY += tDeltaY;
		Y += tDeltaX;
	}
	return *this;
}

size_t World::Ray::RayIterator::operator*() const
{
	return this->X + partitionX * Y;
}

bool World::Ray::RayIterator::operator==(const RayIterator& other) const
{
	return this->X < partitionX && this->Y < partitionY;
}

bool World::Ray::RayIterator::operator!=(const RayIterator& other) const
{
	return !this->operator==(other);
}

World::World(float width, float height): width(width), height(height), movers(width, height), obstacles(width, height), walls(4u)
{}

std::vector<SGE::Object*> World::getObstacles(MovingObject* const mover, float radius)
{
	std::vector<SGE::Object*> obs;
	obs.reserve(5u);
	this->obstacles.CalculateNeighbours(obs, mover->getPosition(), radius);
	return obs;
}

std::vector<SGE::Object*> World::getObstacles(MovingObject* const mover)
{
	return this->getObstacles(mover, 10.f);
}

void World::getNeighbours(std::vector<MovingObject*>& res, MovingObject* const mover)
{
	return this->getNeighbours(res, mover, 10.f);
}

void World::getNeighbours(std::vector<MovingObject*>& res, MovingObject* const mover, float radius)
{
	this->getNeighbours(res, mover->getPosition(), radius);
	res.erase(std::remove(res.begin(), res.end(), mover),res.end());
}

void World::getNeighbours(std::vector<MovingObject*>& res, b2Vec2 position, float radius)
{
	this->movers.CalculateNeighbours(res, position, radius);
}

std::vector<std::pair<SGE::Object*, Wall>>& World::getWalls()
{
	return this->walls;
}

void World::AddObstacle(SGE::Object* ob)
{
	this->obstacles.AddEntity(ob);
}

void World::AddMover(MovingObject* mo)
{
	this->movers.AddEntity(mo);
}

void World::UpdateMover(MovingObject* mo, b2Vec2 oldPos)
{
	this->movers.UpdateEntity(mo, oldPos);
}

void World::AddWall(SGE::Object* wall, Wall::WallEdge edge)
{
	const b2Vec2 pos = wall->getPosition();
	const float w = wall->getShape()->getWidth();
	const float h = wall->getShape()->getHeight();
	b2Vec2 from, to;
	switch (edge)
	{
	case Wall::Left:
		from = b2Vec2{pos.x - 0.5f * w, pos.y - 0.5f * h};
		to = b2Vec2{pos.x - 0.5f * w, pos.y + 0.5f * h};
		break;
	case Wall::Right:
		from = b2Vec2{pos.x + 0.5f * w, pos.y + 0.5f * h};
		to = b2Vec2{pos.x + 0.5f * w, pos.y - 0.5f * h};
		break;
	case Wall::Top:
		from = b2Vec2{pos.x - 0.5f * w, pos.y + 0.5f * h};
		to = b2Vec2{pos.x + 0.5f * w, pos.y + 0.5f * h};
		break;
	case Wall::Bottom:
		from = b2Vec2{pos.x + 0.5f * w, pos.y - 0.5f * h};
		to = b2Vec2{pos.x - 0.5f * w, pos.y - 0.5f * h};
		break;
	default: ;
	}
	this->walls.emplace_back(wall, Wall(from, to, edge));
}

void World::clear()
{
	this->movers.ClearCells();
	this->obstacles.ClearCells();
	this->walls.clear();
}

bool World::Raycast(b2Vec2 from, b2Vec2 direction, b2Vec2& hit)
{
	float tMaxX = 0.f, tMaxY = 0.f, tDeltaX = 0.f, tDeltaY = 0.f;
	const size_t deltaX = size_t(std::copysign(1.f, direction.x));
	const size_t deltaY = size_t(std::copysign(1.f, direction.y));
	size_t X = size_t(std::floor(from.x / this->width));
	size_t Y = size_t(std::floor(from.y / this->height));
	size_t CellIdx = X + partitionX * Y;
}

