#include "Logics.hpp"
#include "Actions.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <Action/Actions/sge_action_move.hpp>
#include <IO/KeyboardState/sge_keyboard_state.hpp>
#include <Utils/Timing/sge_fps_limiter.hpp>
#include <Game/Director/sge_director.hpp>

#include "ZombieScene.hpp"

void MoveAwayFromObstacle::performLogic()
{
	for(SGE::WorldElement& w: *this->obstacles)
	{
		SGE::Shape obShape = *w.getShape();
		switch(obShape.getType())
		{
		case SGE::ShapeType::Circle:
		{
			this->movers.clear();
			this->world->getNeighbours(this->movers, w.getPosition(), obShape.getRadius() + 1.f);		
			if(this->movers.empty()) continue;
			for(MovingObject* mo : this->movers)
			{
				SGE::Shape moShape = *mo->getShape();
				b2Vec2 toMover = mo->getPosition() - w.getPosition();
				float dist = toMover.Length();
				float radius = moShape.getRadius() + obShape.getRadius();
				if(dist < radius)
				{
					toMover *= (radius - dist) / dist;
					mo->setPosition(mo->getPosition() + toMover);
				}
			}
			break;
		}
		case SGE::ShapeType::Rectangle:
		{
			break;
		}
		case SGE::ShapeType::None: break;
		default: ;
		}
	}
	auto ob = this->world->getObstacles(this->player, 4.f*this->player->getShape()->getRadius());
	for(SGE::Object* o : ob)
	{
		SGE::Shape obShape = *o->getShape();
		switch(obShape.getType())
		{
		case SGE::ShapeType::Circle:
		{
			SGE::Shape moShape = *this->player->getShape();
			b2Vec2 toMover = this->player->getPosition() - o->getPosition();
			float dist = toMover.Length();
			float radius = moShape.getRadius() + obShape.getRadius();
			if(dist < radius)
			{
				toMover *= (radius - dist) / dist;
				this->player->setPosition(this->player->getPosition() + toMover);
			}
			break;
		}
		case SGE::ShapeType::Rectangle:
		{
			break;
		}
		case SGE::ShapeType::None: break;
		default:;
		}
	}
}

SnapCamera::SnapCamera(const float speed, const SGE::Key up, const SGE::Key down, const SGE::Key left, const SGE::Key right, const SGE::Key snapKey, SGE::Object* snapTo, SGE::Camera2d* cam): Logic(SGE::LogicPriority::Highest), speed(speed), up(up), down(down), left(left), right(right), snapKey(snapKey), snapTo(snapTo), cam(cam)
{}

void SnapCamera::performLogic()
{
	this->snapped = SGE::isPressed(snapKey); //We need to be able to send signals to actions, like sending actions to objects
	glm::vec2 move = {0, 0};
	if(!this->snapped)
	{
		move = this->snapTo->getPositionGLM();
		this->cam->setPositionGLM(move.x, move.y); //Replace with action, i.e. GoTo
	}
	else
	{
		if(SGE::isPressed(this->up)) move.y += this->speed;
		if(SGE::isPressed(this->down)) move.y -= this->speed;
		if(SGE::isPressed(this->right)) move.x += this->speed;
		if(SGE::isPressed(this->left)) move.x -= this->speed;
		this->sendAction(new SGE::ACTION::Move(this->cam, move.x, move.y, true));
	}
}

Timer::Timer(float time, SGE::Action* action): Logic(SGE::LogicPriority::Low), time(time), action(action)
{}

void Timer::performLogic()
{
	if(this->time > .0f)
	{
		this->time -= SGE::delta_time;
	}
	else
	{
		this->isOn = false;
		this->sendAction(this->action);
	}
}

OnKey::OnKey(SGE::Key key, SGE::Scene* scene): Logic(SGE::LogicPriority::Low), key(key), scene(scene)
{}

void OnKey::performLogic()
{
	if(SGE::isPressed(this->key))
	{
		this->sendAction(new Load(scene));
	}
}

Aim::Aim(World* world, SGE::Object* aimer, SGE::MouseObject* mouse, SGE::Camera2d* cam, std::size_t& counter, float range)
	: Logic(SGE::LogicPriority::High), world(world), aimer(aimer), mouse(mouse), cam(cam), range(range), counter(counter)
{
}

void Aim::aim(b2Vec2 pos, b2Vec2 target)
{
	//Raycast
}

void Aim::performLogic()
{
	if(reload > 0.f)
	{
		reload -= SGE::delta_time;
	}
	auto dir = this->cam->screenToWorld(this->mouse->getMouseCoords()) - this->aimer->getPositionGLM();
	b2Vec2 direction{dir.x, dir.y};
	direction.Normalize();
	this->aimer->setOrientation(direction.Orientation());
	//std::cout << direction.x << ", " << direction.y << std::endl;
	b2Vec2 pos{this->aimer->getXGLM() / 64.f, this->aimer->getYGLM() / 64.f};
	b2Vec2 target = pos + this->range* 1000.f * direction;
	aim(pos, target);
}

WinCondition::WinCondition(size_t& zombies, size_t& killedZombies, SGE::Scene* endGame, Player* player)
	: Logic(SGE::LogicPriority::Low), zombies(zombies), killedZombies(killedZombies), endGame(endGame), player(player)
{}

void WinCondition::performLogic()
{
	if(zombies == killedZombies || this->player->getHP() == 0);
	{
		this->sendAction(new Load(endGame));
	}
}
