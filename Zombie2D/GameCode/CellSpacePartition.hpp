#pragma once
#include "Box2D/Common/b2Math.h"
#include <vector>
#include <array>

struct AABB
{
	b2Vec2 low;
	b2Vec2 high;
	inline bool isOverlapping(const AABB& other) const
	{
		return !((other.high.y < this->low.y)
				 || (other.low.y > this->high.y)
				 || (other.low.x > this->high.x)
				 || (other.high.x < this->low.x));
	}
};

template<typename T>
struct Cell
{
	std::vector<T*> Entities;
	AABB aabb;
	Cell():Entities(10u)
	{}
	Cell(const Cell&) = default;
	Cell(Cell&&) = default;
	Cell(b2Vec2 low, b2Vec2 high): aabb{low, high}
	{}
	explicit Cell(AABB aabb): aabb(aabb)
	{}
};

template<typename T, size_t X, size_t Y>
class CellSpacePartition
{
	class AABBQuery
	{
		size_t beginX, beginY, endX, endY;
		explicit AABBQuery(const CellSpacePartition*const partition, const AABB& query)
		{
			beginX = size_t(X * query.low.x / partition->width);
			beginY = size_t(Y * query.low.y / partition->height);
			endX = size_t(X * query.high.x / partition->width);
			endY = size_t(Y * query.high.y / partition->height);
		}
		class iterator: std::iterator<std::input_iterator_tag, size_t>
		{
			const AABBQuery* owner;
			size_t x, y;
		public:
			iterator(size_t x, size_t y, const AABBQuery* owner): owner(owner), x(x), y(y)
			{}
			bool operator==(const iterator& other) const
			{
				return this->x == other.x && this->y == other.y;
			}
			bool operator!=(const iterator& other) const
			{
				return !this->operator==(other);
			}
			iterator& operator++()
			{
				if(this->x == this->owner->endX)
				{
					this->x = this->owner->beginX;
					++this->y;
				}
				else
				{
					++this->x;
				}
				return *this;
			}
			size_t operator*() const
			{
				return x*y;
			}
		};
	};
	std::array<Cell<T>, X*Y> cells;
	const float width;
	const float height;
	const float cellWidth;
	const float cellHeight;
public:
	CellSpacePartition(float width, float height): width(width), height(height), cellWidth(width / X), cellHeight(height / Y)
	{
		for(size_t y = 0u; y < Y; ++y)
		{
			for(size_t x = 0u; x < X; ++x)
			{
				b2Vec2 low{x * this->cellWidth, y * this->cellHeight};
				b2Vec2 high{low.x + this->cellWidth, low.y + this->cellHeight};
				this->cells[x*y].aabb = AABB{low,high};

			}
		}
	}

	void ClearCells()
	{
		for(Cell<T>& c : cells)
		{
			c.Entities.clear();
		}
	}

	void CalculateNeighbours(std::vector<T*>& res, b2Vec2 pos, const float radius)
	{
		res.clear();
		AABB query{pos - b2Vec2{radius,radius}, pos + b2Vec2{radius, radius}};
		for(Cell<T>& cell : this->cells)
		{
			if(!cell.Entities.empty() && cell.aabb.isOverlapping(query))
			{
				for(T* en : cell.Entities)
				{
					if(b2DistanceSquared(en->getPosition(), pos), radius*radius)
					{
						res.push_back(en);
					}
				}
			}
		}
		/*AABBQuery list(this, query);
		for(size_t it: list)
		{
			auto& cell = this->cells[it];
			if(!cell.Entities.empty() && cell.aabb.isOverlapping(query))
			{
				for(T* en : cell.Entities)
				{
					if(b2DistanceSquared(en->getPosition(), pos), radius*radius)
					{
						res.push_back(en);
					}
				}
			}
		}*/
	}

	size_t PosToIndex(b2Vec2 pos) const
	{
		size_t id = size_t(X * pos.x / this->width) + (size_t(Y*pos.y / this->height) * X);
		return id > X*Y ? X*Y - 1u : id;
	}

	void AddEntity(T* e)
	{
		if(!e) return;
		this->cells[this->PosToIndex(e->getPosition())].Entities.push_back(e);
	}

	void UpdateEntity(T* e, b2Vec2 oldPos)
	{
		size_t oldI = this->PosToIndex(oldPos);
		size_t newI = this->PosToIndex(e->getPosition());
		if(oldI == newI) return;
		this->cells[oldI].Entities.erase(std::find(this->cells[oldI].Entities.begin(), this->cells[oldI].Entities.end(),e));
		this->cells[newI].Entities.push_back(e);
	}
};

