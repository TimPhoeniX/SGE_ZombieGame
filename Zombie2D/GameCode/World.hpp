#pragma once
#include "CellSpacePartition.hpp"
#include "MovingObject.hpp"
#include "Object/WorldElement/sge_world_element.hpp"
#include "Wall.hpp"

namespace
{
	constexpr size_t partitionX = 8u;
	constexpr size_t partitionY = 8u;
}

class World
{
	CellSpacePartition<MovingObject, partitionX, partitionY> movers;
	CellSpacePartition<SGE::Object, partitionX, partitionY> obstacles;
	std::vector<std::pair<SGE::Object*, Wall>> walls;
public:
	World(float width, float height);

	std::vector<SGE::Object*> getObstacles(MovingObject* const mover, float radius);

	std::vector<SGE::Object*> getObstacles(MovingObject* const mover);

	void getNeighbours(std::vector<MovingObject*>& res, MovingObject* const mover);

	void getNeighbours(std::vector<MovingObject*>& res, MovingObject* const mover, float radius);

	void getNeighbours(std::vector<MovingObject*>& res, b2Vec2 position, float radius);

	std::vector<std::pair<SGE::Object*, Wall>>& getWalls();

	void AddObstacle(SGE::Object* ob)
	{
		this->obstacles.AddEntity(ob);
	}

	void AddMover(MovingObject* mo)
	{
		this->movers.AddEntity(mo);
	}

	void UpdateMover(MovingObject* mo, b2Vec2 oldPos)
	{
		this->movers.UpdateEntity(mo, oldPos);
	}

	void AddWall(SGE::Object* wall, Wall::WallEdge edge)
	{
		const b2Vec2 pos = wall->getPosition();
		const float w = wall->getShape()->getWidth();
		const float h = wall->getShape()->getHeight();
		b2Vec2 from, to;
		switch(edge)
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
		default:;
		}
		this->walls.emplace_back(wall, Wall(from, to, edge));
	}

	void clear()
	{
		this->movers.ClearCells();
		this->obstacles.ClearCells();
		this->walls.clear();
	}
};
