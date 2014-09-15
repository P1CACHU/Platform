#include "Level.h"

USING_NS_CC;

Scene * Level::createScene()
{
	auto scene = Scene::create();
	auto layer = Level::create();
	scene->addChild(layer);
	return scene;
}

bool Level::init()
{
	if (!Layer::init()) {
		return false;
	}
	
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("level1.mp3");
	
	cocos2d::LayerColor * sky = LayerColor::create(Color4B(100, 100, 250, 255));
	this->addChild(sky);
	
	_map = TMXTiledMap::create("level1.tmx");
	this->addChild(_map);
	
	return true;
}
