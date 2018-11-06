#pragma once
#include "CellSpacePartition.hpp"
#include "MovingObject.hpp"
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
	float width;
	float height;
public:
	class Ray
	{
		class RayIterator
		{
			const float tDeltaX = 0.f , tDeltaY = 0.f;
			const size_t deltaX = 0u, deltaY = 0u;
			float tMaxX = 0.f, tMaxY = 0.f;
			size_t X = 0u, Y = 0u;
		public:
			constexpr RayIterator() = default;
			constexpr RayIterator(float max_x, float max_y, float delta_x, float delta_y, size_t delta_x1, size_t delta_y1, size_t x, size_t y);

			RayIterator& operator++();
			size_t operator*() const;
			bool operator==(const RayIterator& other) const;
			bool operator!=(const RayIterator& other) const;

			RayIterator begin();
			RayIterator end();
		};
	};
	World(float width, float height);

	std::vector<SGE::Object*> getObstacles(MovingObject* const mover, float radius);

	std::vector<SGE::Object*> getObstacles(MovingObject* const mover);

	void getNeighbours(std::vector<MovingObject*>& res, MovingObject* const mover);

	void getNeighbours(std::vector<MovingObject*>& res, MovingObject* const mover, float radius);

	void getNeighbours(std::vector<MovingObject*>& res, b2Vec2 position, float radius);

	std::vector<std::pair<SGE::Object*, Wall>>& getWalls();

	void AddObstacle(SGE::Object* ob);

	void AddMover(MovingObject* mo);

	void UpdateMover(MovingObject* mo, b2Vec2 oldPos);

	void AddWall(SGE::Object* wall, Wall::WallEdge edge);

	void clear();

	void Raycast(b2Vec2 from, b2Vec2 direction, b2Vec2& hit);
};

constexpr World::Ray::RayIterator::RayIterator(float max_x, float max_y, float delta_x, float delta_y, size_t delta_x1, size_t delta_y1, size_t x, size_t y): tDeltaX(delta_x),
                                                                                                                                                              tDeltaY(delta_y),
                                                                                                                                                              deltaX(delta_x1),
                                                                                                                                                              deltaY(delta_y1),
                                                                                                                                                              tMaxX(max_x),
                                                                                                                                                              tMaxY(max_y),
                                                                                                                                                              X(x),
                                                                                                                                                              Y(y)
{
}
