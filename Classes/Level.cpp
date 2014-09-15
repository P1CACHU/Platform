#include "Level.h"

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
	
	_player = (Player *)Sprite::create("koalio_stand.png");
	_player->setVelocity(Point(0.0, 0.0));
	_player->setPosition(Point(100, 50));
	_map->addChild(_player, 15);
	
//	walls = [map layerNamed:@"walls"];
//	hazards = [map layerNamed:@"hazards"];
	
	scheduleUpdate();
	
	return true;
}

void Level::update(float dt)
{
	_player->update(dt);
	/*
	if (gameOver) {
		return;
	}
	[player update:dt];
	[self checkForWin];
	[self checkForAndResolveCollisions:player];
	[self handleHazardCollisions:player];
	[self setViewpointCenter:player.position];
	*/
}

Point Level::tileCoordForPosition(Point position)
{
	float x = floor(position.x / _map->getTileSize().width);
	float levelHeightInPixels = _map->getTileSize().height * _map->getTileSize().height;
	float y = floor((levelHeightInPixels - position.y) / _map->getTileSize().height);
	return Point(x, y);
}

Rect Level::tileRectFromTileCoords(Point tileCoords)
{
	float levelHeightInPixels = _map->getMapSize().height * _map->getTileSize().height;
	Point origin = Point(tileCoords.x * _map->getTileSize().width, levelHeightInPixels - ((tileCoords.y + 1) * _map->getTileSize().height));
	return Rect(origin.x, origin.y, _map->getTileSize().width, _map->getTileSize().height);
}

vector<Level::Dictionary> Level::getSurroundingTilesAtPosition(Point position, TMXLayer * layer)
{
	Point plPos = this->tileCoordForPosition(position);
	
	vector<Level::Dictionary> gids;
	
	for (int i = 0; i < 9; i++) {
		int c = i % 3;
		int r = (int)(i / 3);
		Point tilePos = Point(plPos.x + (c - 1), plPos.y + (r - 1));
		if (tilePos.y > (_map->getMapSize().height - 1)) {
			//fallen in a hole
//			gameOver(0);
			return gids;
		}
		int tgid = layer->getTileGIDAt(tilePos);
		Rect tileRect = this->tileRectFromTileCoords(tilePos);
		Level::Dictionary tileDict;
		tileDict.gid = tgid;
		tileDict.x = tileRect.origin.x;
		tileDict.y = tileRect.origin.y;
		tileDict.tilePos = tilePos;
		gids.push_back(tileDict);
	}
	/*
	
	[gids removeObjectAtIndex:4]; //7
	[gids insertObject:[gids objectAtIndex:2] atIndex:6];
	[gids removeObjectAtIndex:2];
	[gids exchangeObjectAtIndex:4 withObjectAtIndex:6];
	[gids exchangeObjectAtIndex:0 withObjectAtIndex:4];
	
	return (NSArray *)gids;*/
	return gids;
}
