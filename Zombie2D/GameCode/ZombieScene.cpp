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
#include "Renderer/SpriteBatch/sge_sprite_batch.hpp"
#include "Renderer/sge_renderer.hpp"
#include <allocators>

SGE::RealSpriteBatch* ZombieScene::zombieBatch;
SGE::RealSpriteBatch* ZombieScene::deadZombieBatch;

const b2Vec2 ZombieScene::trianglePoints[3] = {b2Vec2(0.5f,0.f), b2Vec2(-0.5f,-0.5f), b2Vec2(-0.5f,0.5f)};

bool ZombieScene::init()
{
	return true;
}

constexpr float Width = 160.f;
constexpr float Height = 120.f;
constexpr size_t pillarsMax = 30;
constexpr size_t zombiesMax = 70;

ZombieScene::ZombieScene(SGE::Game* game, const char* path): Scene(), world(Width, Height), game(game),
path([game](const char* path)
{
	return game->getGamePath() + path;
}(path))
{
	static bool initialized = init();
	this->movers.reserve(zombiesMax);
}

void ZombieScene::loadScene()
{
	SGE::BatchRenderer* renderer = SGE::Game::getGame()->getRenderer();
	this->killCount = 0;
	this->zombieCount = 0;
	this->level = SGE::Level();
	GLuint basicProgram = renderer->getProgramID("BatchShader.vert", "BatchShader.frag");
	GLuint scaleUVProgram = renderer->getProgramID("BatchUVShader.vert", "BatchShader.frag");

	std::string lightBrickTexPath = "Resources/Textures/light_bricks.png";
	std::string deadZombieTexPath = "Resources/Textures/deadzombie.png";
	std::string zombieTexPath = "Resources/Textures/zombie.png";
	std::string pillarTexPath = "Resources/Textures/pillar.png";
	std::string playerTexPath = "Resources/Textures/player.png";
	std::string beamPath = "Resources/Textures/pointer.png";

	SGE::RealSpriteBatch* wallBatch = renderer->getBatch(renderer->newBatch(scaleUVProgram, lightBrickTexPath, 4, false, true));
	SGE::RealSpriteBatch* pillarsBatch = renderer->getBatch(renderer->newBatch(basicProgram, pillarTexPath, pillarsMax, false, true));
	SGE::RealSpriteBatch* playerBatch = renderer->getBatch(renderer->newBatch(basicProgram, playerTexPath, 1));
	SGE::RealSpriteBatch* beamBatch = renderer->getBatch(renderer->newBatch(basicProgram, beamPath, 1));
	deadZombieBatch = renderer->getBatch(renderer->newBatch(basicProgram, deadZombieTexPath, zombiesMax));
	zombieBatch = renderer->getBatch(renderer->newBatch(basicProgram, zombieTexPath, zombiesMax));

	GLuint IBO = zombieBatch->initializeIBO();
	GLuint sampler = zombieBatch->initializeSampler();

	for(SGE::RealSpriteBatch* b : {wallBatch,pillarsBatch,playerBatch,beamBatch, deadZombieBatch})
	{
		b->initializeIBO(IBO);
		b->initializeSampler(sampler);
	}

	this->player = new Player({0.5*Width, 0.5*Height}, SGE::Shape::Circle(0.45f,true), &this->world);
	auto& world = this->level.getWorld();
	world.reserve(4 + pillarsMax);
	//Boundaries

	SGE::Shape* horizontal = SGE::Shape::Rectangle(Width, 1.f, false);
	SGE::Shape* vertical = SGE::Shape::Rectangle(1.f, Height + 2.f, false);

	world.emplace_back(-0.5f, Height * 0.5f, lightBrickTexPath);
	world.back().setShape(vertical);
	this->world.AddWall(&world.back(), Wall::Right);
	wallBatch->addObject(&world.back());

	world.emplace_back(Width + .5f, Height * 0.5f, lightBrickTexPath);
	world.back().setShape(vertical);
	this->world.AddWall(&world.back(), Wall::Left);
	wallBatch->addObject(&world.back());

	world.emplace_back(Width * 0.5f, Height + .5f, lightBrickTexPath);
	world.back().setShape(horizontal);
	this->world.AddWall(&world.back(), Wall::Bottom);
	wallBatch->addObject(&world.back());

	world.emplace_back(Width * 0.5f, -0.5f, lightBrickTexPath);
	world.back().setShape(horizontal);
	this->world.AddWall(&world.back(), Wall::Top);
	wallBatch->addObject(&world.back());

	std::set<std::pair<size_t, size_t>> pillarsSlot;
	std::set<std::pair<size_t, size_t>> zombiesSlot;

	srand(unsigned(time(NULL)));

	constexpr size_t border = 6u;
	constexpr size_t spread = 11u;
	while(pillarsSlot.size() < pillarsMax)
	{
		pillarsSlot.emplace(border + spread * (rand() % size_t((Width - 2u * border) / spread)), border + spread * (rand() % size_t((Height - 2u * border) / spread)));
	}

	while(zombiesSlot.size() < zombiesMax)
	{
		zombiesSlot.emplace(border + spread * (rand() % size_t((Width - 2u * border) / spread)), border + spread * (rand() % size_t((Height - 2u * border) / spread)));
	}

	std::vector<std::pair <size_t, size_t>> freeList(pillarsSlot.begin(), pillarsSlot.end());

	std::random_shuffle(freeList.begin(), freeList.end());

	auto randf = std::bind(std::uniform_real_distribution<float>(-b2_pi, b2_pi), std::default_random_engine());

	for(auto pos : freeList)
	{
		world.emplace_back(float(pos.first) + randf()*0.3f, float(pos.second) + randf()*0.2f,
						   game->getGamePath() + "Resources/Textures/pillar.png");
		float radius = (rand() % 10 / 10.f) + 2.f;
		world.back().setVisible(true);
		world.back().setDrawable(true);
		world.back().setShape(SGE::Shape::Circle(radius, false));
		this->world.AddObstacle(&world.back());
		pillarsBatch->addObject(&world.back());
	}
	freeList.assign(zombiesSlot.begin(), zombiesSlot.end());
	std::random_shuffle(freeList.begin(), freeList.end());
	b2Vec2 heading{0.f,1.f};
	for(auto pos : freeList)
	{
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
		zombieBatch->addObject(&mo);
	}

	//Rest
	SGE::Camera2d* camera = game->getCamera();
	SGE::MouseObject* mouse = game->getMouse();

	this->addObject(this->player);
	playerBatch->addObject(this->player);

	auto PlayerMoveLogic = new PlayerMove(player, 3, SGE::Key::W, SGE::Key::S, SGE::Key::A, SGE::Key::D);

	auto camLogic = new SnapCamera(8, SGE::Key::Up, SGE::Key::Down, SGE::Key::Left, SGE::Key::Right, SGE::Key::O, player, camera);
	auto camZoom = new SGE::Logics::CameraZoom(camera, 0.5f, 0.5f, 0.1f, SGE::Key::Q, SGE::Key::E);

	camera->setCameraScale(0.1f);

	this->addLogic(PlayerMoveLogic);
	this->addLogic(camLogic);
	this->addLogic(camZoom);

	auto pointer = new Image(0.f, 0.f);
	pointer->setVisible(false);
	pointer->setDrawable(true);
	pointer->setShape(nullptr);
	pointer->setLayer(0.1f);
	beamBatch->addObject(pointer);
	//AddMovement here
	this->addLogic(new SteeringBehavioursUpdate(&this->movers));
	this->addLogic(new DamagePlayer(&this->world, this->player, 5));
	this->addLogic(new SeparateZombies(&this->world, player, &this->movers));
	this->addLogic(new MoveAwayFromObstacle(&this->world, player, &world));
	this->addLogic(new MoveAwayFromWall(&this->world, player, this->movers));
	auto aim = new Aim(&this->world, player, mouse, camera, this->killCount, pointer);
	this->addLogic(aim);
	this->addLogic(new WinCondition(this->zombieCount, this->killCount, endScene, player));

	game->mapAction(SGE::InputBinder(new Shoot(aim), SGE::Key::MB_Left));
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
	game->getRenderer()->deleteSceneBatch(this);
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
