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
#include "SteeringBehaviours.hpp"

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

WinCondition::WinCondition(size_t& zombies, size_t& killedZombies, SGE::Scene* endGame)
	: Logic(SGE::LogicPriority::Low), zombies(zombies), killedZombies(killedZombies), endGame(endGame)
{}

void WinCondition::performLogic()
{
	if(zombies == killedZombies)
	{
		this->sendAction(new Load(endGame));
	}
}
