#ifndef ZOMBIE_SCENE
#define ZOMBIE_SCENE

#include <Game/sge_game.hpp>
#include <Scene/sge_scene.hpp>
#include "MovingObject.hpp"
#include "World.hpp"

namespace SGE
{
	class RealSpriteBatch;
}

class ZombieScene: public SGE::Scene
{
protected:
	World world;
	SGE::Game* game = nullptr;
	std::string path;
	std::vector<MovingObject> movers;
	std::size_t zombieCount = 0;
	std::size_t killCount = 0;
	Player* player = nullptr;
	static bool init();
public:
	SGE::Scene* endScene = nullptr;
	static const b2Vec2 trianglePoints[3];
	static SGE::RealSpriteBatch* zombieBatch;
	static SGE::RealSpriteBatch* deadZombieBatch;

	ZombieScene(SGE::Game* game, const char* path);

	virtual void loadScene() override;
	virtual void unloadScene() override;

	virtual ~ZombieScene();

	virtual void finalize() override;

	virtual void onDraw() override;
};

#endif
