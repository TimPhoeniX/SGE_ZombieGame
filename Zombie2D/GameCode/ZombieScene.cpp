#include <set>
#include <functional>

#include <boost/config/suffix.hpp>
#include <Logic/Logics/Camera/sge_logic_camera_zoom.hpp>
#include <Object/Shape/sge_shape.hpp>
#include <algorithm>
#include <random>

#include "ZombieScene.hpp"
#include "Image.hpp"
#include "Logics.hpp"
#include "Actions.hpp"
#include "MovingObject.hpp"
#include "Utilities.hpp"
#include "PlayerMove.hpp"
#include "SteeringBehavioursUpdate.hpp"
#include "Game/InputHandler/sge_input_binder.hpp"

SGE::GLTexture ZombieScene::zombieTexture;
SGE::GLTexture ZombieScene::deadZombieTexture;
const b2Vec2 ZombieScene::trianglePoints[3] = {b2Vec2(0.5f,0.f), b2Vec2(-0.5f,-0.5f), b2Vec2(-0.5f,0.5f)};

bool ZombieScene::init()
{
	return true;
}

constexpr float Width = 160.f;
constexpr float Height = 120.f;

ZombieScene::ZombieScene(SGE::Game* game, const char* path): Scene(), world(Width, Height), game(game),
path([game](const char* path)
{
	return game->getGamePath() + path;
}(path))
{
	static bool initialized = init();
	this->movers.reserve(100u);
}

void ZombieScene::loadScene()
{
	this->killCount = 0;
	this->zombieCount = 0;
	this->level = SGE::Level();
	auto& world = this->level.getWorld();
	world.reserve(50u);
	//Boundaries
	SGE::Shape* horizontal = SGE::Shape::Rectangle(Width, 1.f, false);
	SGE::Shape* vertical = SGE::Shape::Rectangle(1.f, Height + 2.f, false);

	world.emplace_back(-0.5f, Height * 0.5f, game->getGamePath() + "Resources/Textures/light_bricks.png");
	world.back().setShape(vertical);
	this->world.AddWall(&world.back(), Wall::Right);
	world.emplace_back(Width + .5f, Height * 0.5f, game->getGamePath() + "Resources/Textures/light_bricks.png");
	world.back().setShape(vertical);
	this->world.AddWall(&world.back(), Wall::Left);
	world.emplace_back(Width * 0.5f, Height + .5f, game->getGamePath() + "Resources/Textures/light_bricks.png");
	world.back().setShape(horizontal);
	this->world.AddWall(&world.back(), Wall::Bottom);
	world.emplace_back(Width * 0.5f, -0.5f, game->getGamePath() + "Resources/Textures/light_bricks.png");
	world.back().setShape(horizontal);
	this->world.AddWall(&world.back(), Wall::Top);

	SGE::Object* Dummy1 = new Image(-1000, -1000);
	game->textureObject(Dummy1, "Resources/Textures/deadzombie.png");
	deadZombieTexture = Dummy1->getTexture();
	this->addObject(Dummy1);

	SGE::Camera2d* camera = game->getCamera();
	camera->setScale(0.5f);
	SGE::MouseObject* mouse = game->getMouse();

	this->player = new Player({50.f, 50.f}, getCircle(), &this->world);
	this->addObject(this->player);

	auto PlayerMoveLogic = new PlayerMove(player, 3, SGE::Key::W, SGE::Key::S, SGE::Key::A, SGE::Key::D);

	auto camLogic = new SnapCamera(8, SGE::Key::Up, SGE::Key::Down, SGE::Key::Left, SGE::Key::Right, SGE::Key::O, player, camera);
	auto camZoom = new SGE::Logics::CameraZoom(camera, 0.1f, 0.5f, 0.15f, SGE::Key::Q, SGE::Key::E);

	camera->setScale(0.05f);

	this->addLogic(PlayerMoveLogic);
	this->addLogic(camLogic);
	this->addLogic(camZoom);

	std::set<std::pair<size_t, size_t>> free;

	srand(time(NULL));

	constexpr size_t humans = 70;
	int pillars = 25;
	constexpr size_t border = 6u;
	constexpr size_t spread = 10u;
	while(free.size() < humans)
	{
		free.emplace(border + spread * (rand() % size_t((Width - 2u * border) / spread)), border + spread * (rand() % size_t((Height - 2u * border) / spread)));
	}

	std::vector<std::pair <size_t, size_t>> freeList(free.begin(), free.end());

	std::random_shuffle(freeList.begin(), freeList.end());

	auto randf = std::bind(std::uniform_real_distribution<float>(-b2_pi, b2_pi), std::default_random_engine());

	for(auto pos : freeList)
	{
		if(pillars > 0)
		{
			world.emplace_back(float(pos.first), float(pos.second), game->getGamePath() + "Resources/Textures/pillar.png");
			float radius = (rand() % 10 / 10.f) + 2.f;
			world.back().setVisible(true);
			world.back().setDrawable(true);
			world.back().setShape(SGE::Shape::Circle(radius, false));
			this->world.AddObstacle(&world.back());
			--pillars;
			continue;
		}
		b2Vec2 heading{0.f,1.f};
		heading = b2Mul(b2Rot(randf()), heading);
		this->movers.emplace_back(b2Vec2{float(pos.first),float(pos.second)}, getCircle(), &this->world, heading);
		auto mover = &this->movers.back();
		game->textureObject(mover, "Resources/Textures/zombie.png");
		mover->setVisible(true);
		mover->setDrawable(true);
		mover->getSteering()->setPlayer(this->player);
		this->world.AddMover(mover);
		++zombieCount;
	}
	for(MovingObject& mo : this->movers)
	{
		this->addObject(&mo);
	}

	//AddMovement here
	this->addLogic(new SteeringBehavioursUpdate(&this->movers));
	this->addLogic(new MoveAwayFromObstacle(&this->world, player, &world));
	this->addLogic(new DamagePlayer(&this->world, this->player, 5));
	auto aim = new Aim(&this->world, player, mouse, camera, this->killCount);
	this->addLogic(aim);
	this->addLogic(new WinCondition(this->zombieCount, this->killCount, endScene, player));

	game->mapAction(SGE::InputBinder(new Shoot(aim), SGE::Key::MB_Left));
	//Puts player on top
	game->textureObject(player, "Resources/Textures/player.png");
	player->setVisible(true);
	//Generalize this after finals
	for(auto& e : level.getWorld())
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
	this->world.clear();
	delete this->player;
	vec_clear(this->getLogics());
	vec_clear(this->getActions());
	this->getObjects().clear();
	game->unmapAll();
}

void ZombieScene::onDraw()
{}
