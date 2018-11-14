#include "IntroScene.hpp"

#include <Object/Shape/sge_shape.hpp>
#include <Game/sge_game.hpp>
#include <Game/Director/sge_director.hpp>
#include "Image.hpp"
#include "Logics.hpp"
#include "Actions.hpp"
#include "Renderer/sge_renderer.hpp"


IntroScene::IntroScene(SGE::Scene* next, const char* path): path(path), next(next)
{}

void IntroScene::loadScene()
{
	auto o = new Image(0, 0, SGE::Shape::Rectangle(1024.f / 64.f, 768.f / 64.f, true));
	auto g = SGE::Game::getGame();
	auto game = SGE::Game::getGame();
	SGE::BatchRenderer* renderer = game->getRenderer();
	auto program = renderer->getProgramID("BatchShader.vert","BatchShader.frag");
	GLuint batch = renderer->newBatch(program, path, 1, false, true);
	renderer->getBatch(batch)->addObject(o);
	o->setVisible(true);
	o->setDrawable(true);
	o->setOrientation(b2_pi);
	this->addObject(o);
	this->addLogic(new Timer(2, new Load(next)));
	g->getCamera()->setPositionGLM(0, 0);
	g->getCamera()->setCameraScale(1.f);
}

IntroScene::~IntroScene()
{}

void IntroScene::finalize()
{}

void IntroScene::onDraw()
{
	auto g = SGE::Game::getGame();
	g->getCamera()->setPositionGLM(0, 0);
	g->getCamera()->setCameraScale(1.f);
	SGE::Director::getDirector()->unloadScene(next);
}

void EndScene::loadScene()
{
	auto o = new Image(0, 0, SGE::Shape::Rectangle(1024.f / 64.f, 768.f / 64.f, true));
	auto game = SGE::Game::getGame();
	SGE::BatchRenderer* renderer = game->getRenderer();
	if(this->winBatch == 0)
	{
		auto program = renderer->getProgramID("BatchShader.vert", "BatchShader.frag");
		this->winBatch = renderer->newBatch(program, path, 1, false, true);
	}
	if(this->loseBatch == 0)
	{
		auto program = renderer->getProgramID("BatchShader.vert", "BatchShader.frag");
		this->loseBatch = renderer->newBatch(program, path, 1, false, true);
	}
	renderer->getBatch(this->winBatch)->addObject(o);
	o->setVisible(true);
	o->setDrawable(true);
	auto g = SGE::Game::getGame();
	this->addObject(o);
	this->addLogic(new OnKey(SGE::Key::Return, next));
	g->getCamera()->setPositionGLM(0, 0);
	g->getCamera()->setCameraScale(1.f);
}
