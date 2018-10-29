#include "World.hpp"

World::World(float width, float height): movers(width, height), obstacles(width, height), walls(4u)
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

