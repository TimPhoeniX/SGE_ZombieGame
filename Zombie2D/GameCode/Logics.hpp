#ifndef ZOMBIEGAME_LOGICS
#define ZOMBIEGAME_LOGICS

#include <vector>

#include <Logic/sge_logic.hpp>
#include <Box2D/Dynamics/b2WorldCallbacks.h>
#include <IO/Key/sge_key.hpp>
#include <Object/Camera2d/sge_camera2d.hpp>
#include <IO/Mouse/sge_mouse.hpp>

#include "MovingObject.hpp"
#include "World.hpp"

namespace SGE
{
	class Scene;
}

class SnapCamera: public SGE::Logic
{
	const float speed = 0;
	const SGE::Key up, down, left, right, snapKey;
	bool snapped = true;
	SGE::Object* snapTo = nullptr;
	SGE::Camera2d* cam = nullptr;

public:
	SnapCamera(const float specamed, const SGE::Key up, const SGE::Key down, const SGE::Key left, const SGE::Key right, const SGE::Key snapKey, SGE::Object* snapTo, SGE::Camera2d* cam);

	~SnapCamera() = default;

	void performLogic() override;
};

class Timer: public SGE::Logic
{
	float time = .0f;
	SGE::Action* action = nullptr;
public:
	Timer(float time, SGE::Action* action);
	void performLogic() override;
};

class OnKey: public SGE::Logic
{
	SGE::Key key;
	SGE::Scene* scene = nullptr;
public:
	OnKey(SGE::Key key, SGE::Scene* scene);
	void performLogic() override;
};

class Aim: public SGE::Logic
{
protected:
	World* world;
	SGE::Object* aimer;
	SGE::MouseObject* mouse;
	SGE::Camera2d* cam;
	float range;
	float reload = -1.f;
	std::size_t& counter;
	void aim(b2Vec2 pos, b2Vec2 target);
public:
	Aim(World* world, SGE::Object* aimer, SGE::MouseObject* mouse, SGE::Camera2d* cam, std::size_t& counter, float range);
	void performLogic() override;
};

namespace SGE
{
	class Scene;
}

class WinCondition: public SGE::Logic
{
protected:
	volatile size_t& zombies;
	volatile size_t& killedZombies;
	SGE::Scene* endGame = nullptr;
public:
	WinCondition(size_t& zombies, size_t& killedZombies, SGE::Scene* endGame);
	virtual void performLogic() override;
};
#endif
