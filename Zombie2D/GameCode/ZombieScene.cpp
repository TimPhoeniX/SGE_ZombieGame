#include <fstream>
#include <set>
#include <functional>

#include "ZombieScene.hpp"
#include "Logics.hpp"
#include "Actions.hpp"

#include <Logic/Logics/Movers/sge_logic_move.hpp>
#include <Logic/Logics/Camera/sge_logic_camera_zoom.hpp>
#include <Game/Director/sge_director.hpp>
#include "Object/Shape/sge_shape.hpp"
#include <algorithm>
#include "MovingObject.hpp"
#include "Utilities.hpp"
#include <random>
#include "PlayerMove.hpp"
#include "SteeringBehavioursUpdate.hpp"

SGE::GLTexture ZombieScene::zombieTexture;
SGE::GLTexture ZombieScene::deadZombieTexture;
const b2Vec2 ZombieScene::trianglePoints[3] = {b2Vec2(-0.5f,-0.5f), b2Vec2(-0.5f,0.5f), b2Vec2(0.5f,0.f)};

bool ZombieScene::init()
{
	return true;
}

ZombieScene::ZombieScene(SGE::Game* game, const char* path): Scene(), game(game),
path([game](const char* path)
{
	return game->getGamePath() + path;
}(path))
{
	static bool initialized = init();
	this->movers.reserve(80);
}

void ZombieScene::loadScene()
{
	this->killCount = 0;
	this->zombieCount = 0;
	this->level = SGE::Level();
	auto& world = this->level.getWorld();

	//Boundaries
	SGE::Shape* horizontal = SGE::Shape::Rectangle(60.f, 1.f, false);
	SGE::Shape* vertical = SGE::Shape::Rectangle(1.f, 61.f, false);

	world.emplace_back(-30.5f, 0.f, game->getGamePath() + "Resources/Textures/light_bricks.png");
	world.back().setShape(vertical);
	world.emplace_back(30.5f, 0.f, game->getGamePath() + "Resources/Textures/light_bricks.png");
	world.back().setShape(vertical);
	world.emplace_back(0.f, 30.f, game->getGamePath() + "Resources/Textures/light_bricks.png");
	world.back().setShape(horizontal);
	world.emplace_back(0.f, -30.f, game->getGamePath() + "Resources/Textures/light_bricks.png");
	world.back().setShape(horizontal);

	for(SGE::WorldElement& e : this->getLevel().getWorld())
	{
		//Create Walls
	}

	SGE::Object* Dummy1 = new Image(-1000, -1000);
	game->textureObject(Dummy1, "Resources/Textures/deadzombie.png");
	deadZombieTexture = Dummy1->getTexture();
	this->addObject(Dummy1);

	SGE::Camera2d* camera = game->getCamera();
	camera->setScale(0.5f);
	SGE::MouseObject* mouse = game->getMouse();

	this->player = new MovingObject({1.f,1.f}, getCircle(), &this->world);
	this->addObject(this->player);

	auto PlayerMoveLogic = new PlayerMove(player, 3, SGE::Key::W, SGE::Key::S, SGE::Key::A, SGE::Key::D);

	auto camLogic = new SnapCamera(8, SGE::Key::Up, SGE::Key::Down, SGE::Key::Left, SGE::Key::Right, SGE::Key::O, player, camera);
	auto camZoom = new SGE::Logics::CameraZoom(camera, 0.1f, 0.5f, 0.15f, SGE::Key::Q, SGE::Key::E);

	this->addLogic(PlayerMoveLogic);
	this->addLogic(camLogic);
	this->addLogic(camZoom);

	std::set<std::pair<float, float>> free;

	const int humans = 100;
	srand(time(NULL));

	for(int i = 0; i < humans; i++)
	{
		free.emplace((rand() % (48 / 6) * 6.f - 24 + rand() % 4 - 2), (rand() % (48 / 6) * 6.f - 24 + rand() % 4 - 2));
	}

	std::vector<std::pair <float, float>> freeList(free.begin(), free.end());

	std::random_shuffle(freeList.begin(), freeList.end());

	int pillars = 20;
	auto randf = std::bind(std::uniform_real_distribution<float>(-1.f, 1.f), std::default_random_engine());

	for(auto pos : freeList)
	{
		if(pillars > 0)
		{
			world.emplace_back(pos.first, pos.second, game->getGamePath() + "Resources/Textures/pillar.png");
			float radius = rand() % 10 / 10.f + 1.f;
			world.back().setVisible(true);
			world.back().setDrawable(true);
			world.back().setShape(SGE::Shape::Circle(radius, false));
			--pillars;
			continue;
		}
		b2Vec2 heading{randf(),randf()};
		heading.Normalize();
		this->movers.emplace_back(b2Vec2{pos.first,pos.second}, getCircle(), &this->world, heading);
		auto mover = &this->movers.back();
		game->textureObject(mover, "Resources/Textures/zombie.png");
		mover->setVisible(true);
		mover->setDrawable(true);
		mover->getSteering()->setPlayer(this->player);
		++zombieCount;
	}
	for(MovingObject& mo: this->movers)
	{
		this->addObject(&mo);
	}
	
	//AddMovement here
	this->addLogic(new SteeringBehavioursUpdate(&this->movers));

	this->addLogic(new Aim(&this->world, player, mouse, camera, this->killCount, 8.f));
	this->addLogic(new WinCondition(this->zombieCount, this->killCount, endScene));

	//Puts player on top
	game->textureObject(player, "Resources/Textures/player.png");
	player->setVisible(true);
	//Generalize this after finals
	for(auto& e :level.getWorld())
	{
		e.setVisible(true);
		e.setDrawable(true);
	}
}

void ZombieScene::unloadScene()
{
	this->finalize();
}

ZombieScene::~ZombieScene()
{
	std::cout << "~MainScene" << std::endl;
}

template<typename Vec>
void vec_clear(Vec& vec)
{
	for(auto h : vec)
	{
		delete h;
	}
	vec.clear();
}

void ZombieScene::finalize()
{
	this->level.clear();
	this->movers.clear();
	delete this->player;
	vec_clear(this->getLogics());
	vec_clear(this->getActions());
}

void ZombieScene::onDraw()
{}
