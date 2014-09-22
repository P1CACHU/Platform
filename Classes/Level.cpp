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
	
	LayerColor * sky = LayerColor::create(Color4B(100, 100, 250, 255));
	this->addChild(sky);
	
	_map = TMXTiledMap::create("level1.tmx");
	this->addChild(_map);
	
	_player = (Player *)Sprite::create("koalio_stand.png");
	_player->setVelocity(Point(0.0, 0.0));
	_player->setPosition(Point(100, 50));
	_map->addChild(_player, 15);
	
	_walls = _map->getLayer("walls");
	_coins = _map->getLayer("coins");
	
	_score = 0;
	
	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = CC_CALLBACK_2(Level::onTouchesBegan, this);
	listener->onTouchesMoved = CC_CALLBACK_2(Level::onTouchesMoved, this);
	listener->onTouchesEnded = CC_CALLBACK_2(Level::onTouchesEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	
	scheduleUpdate();
	
	return true;
}

void Level::update(float dt)
{
	if (_gameOver) {
		return;
	}
	
	_player->updatePosition(dt);
	checkForWin();
	checkForAndResolveCollisions(_player);
	handleCollectCoins(_player);
	setViewpointCenter(_player->getPosition());
}

Point Level::tileCoordForPosition(Point position)
{
	float x = floor(position.x / _map->getTileSize().width);
	float levelHeightInPixels = _map->getMapSize().height * _map->getTileSize().height;
	float y = floor((levelHeightInPixels - position.y) / _map->getTileSize().height);
	return Point(x, y);
}

Rect Level::tileRectFromTileCoords(Point tileCoords)
{
	float levelHeightInPixels = _map->getMapSize().height * _map->getTileSize().height;
	Point origin = Point(tileCoords.x * _map->getTileSize().width, levelHeightInPixels - ((tileCoords.y + 1) * _map->getTileSize().height));
	return Rect(origin.x, origin.y, _map->getTileSize().width, _map->getTileSize().height);
}

__Array * Level::getSurroundingTilesAtPosition(Point position, TMXLayer * layer)
{
	__Array * gids = __Array::create();
	Point plPos = tileCoordForPosition(position);
	
	for (int i = 0; i < 9; i++) {
		int c = i % 3;
		int r = (int)(i / 3);
		Point tilePos = Point(plPos.x + (c - 1), plPos.y + (r - 1));
		if (tilePos.y > (_map->getMapSize().height - 1)) {
			gameOver(0);
			return gids;
		}
		int tgid = layer->getTileGIDAt(tilePos);
		Rect tileRect = this->tileRectFromTileCoords(tilePos);
		
		Gid * tileDict = new Gid(tgid, tileRect.origin.x, tileRect.origin.y, tilePos);
		gids->addObject(tileDict);
	}
	gids->removeObjectAtIndex(4);
	gids->insertObject(gids->getObjectAtIndex(2), 6);
	gids->removeObjectAtIndex(2);
	gids->exchangeObjectAtIndex(4, 6);
	gids->exchangeObjectAtIndex(0, 4);

	return gids;
}

void Level::checkForAndResolveCollisions(Player * p)
{
	__Array * tiles = getSurroundingTilesAtPosition(p->getPosition(), _walls);
	
	if (_gameOver) {
		return;
	}
	p->setOnGround(false);
	
	Ref* obj = nullptr;
	CCARRAY_FOREACH(tiles, obj)
	{
		Gid * dic = dynamic_cast<Gid*>(obj);
		Rect pRect = p->collisionBoundingBox();
		
		int gid = dic->_gid;
		
		if (gid) {
			Rect tileRect = Rect(dic->_x, dic->_y, _map->getTileSize().width, _map->getTileSize().height); //5
			if (pRect.intersectsRect(tileRect)) {
				
				Rect intersection = Rect(std::max(pRect.getMinX(), tileRect.getMinX()), std::max(pRect.getMinY(), tileRect.getMinY()), 0, 0);
				intersection.size.width = std::min(pRect.getMaxX(), tileRect.getMaxX()) - intersection.getMinX();
				intersection.size.height = std::min(pRect.getMaxY(), tileRect.getMaxY()) - intersection.getMinY();

				int tileIndx = tiles->getIndexOfObject(dic);
				
				if (tileIndx == 0) {
					//tile is directly below player
					p->_desiredPosition = Point(p->_desiredPosition.x, p->_desiredPosition.y + intersection.size.height);
					p->_velocity = Point(p->_velocity.x, 0.0);
					p->setOnGround(true);
				} else if (tileIndx == 1) {
					//tile is directly above player
					p->_desiredPosition = Point(p->_desiredPosition.x, p->_desiredPosition.y - intersection.size.height);
					p->_velocity = Point(p->_velocity.x, 0.0);
				} else if (tileIndx == 2) {
					//tile is left of player
					p->_desiredPosition = Point(p->_desiredPosition.x + intersection.size.width, p->_desiredPosition.y);
				} else if (tileIndx == 3) {
					//tile is right of player
					p->_desiredPosition = Point(p->_desiredPosition.x - intersection.size.width, p->_desiredPosition.y);
				} else {
					if (intersection.size.width > intersection.size.height) {
						//tile is diagonal, but resolving collision vertially
						p->_velocity = Point(p->_velocity.x, 0.0);
						float resolutionHeight;
						if (tileIndx > 5) {
							resolutionHeight = -intersection.size.height;
							p->setOnGround(true);
						} else {
							resolutionHeight = intersection.size.height;
						}
						
						p->_desiredPosition = Point(p->_desiredPosition.x, p->_desiredPosition.y + resolutionHeight );
						
					} else {
						float resolutionWidth;
						if (tileIndx == 6 || tileIndx == 4) {
							resolutionWidth = intersection.size.width;
						} else {
							resolutionWidth = -intersection.size.width;
						}
						p->_desiredPosition = Point(p->_desiredPosition.x + resolutionWidth , p->_desiredPosition.y);
						
					}
				}
			}
		}
	}
	p->setPosition(p->_desiredPosition);
}

void Level::handleCollectCoins(Player * p)
{
	__Array * tiles = getSurroundingTilesAtPosition(p->getPosition(), _coins);
	Ref* obj = nullptr;
	CCARRAY_FOREACH(tiles, obj)
	{
		Gid * dic = dynamic_cast<Gid*>(obj);
		Rect tileRect = Rect(dic->_x, dic->_y, _map->getTileSize().width, _map->getTileSize().height);
		Rect pRect = p->collisionBoundingBox();
		
		if (dic->_gid && pRect.intersectsRect(tileRect)) {
			_coins->removeTileAt(dic->_tilePos);
			_score += 1;
		}
	}
}

void Level::onTouchesBegan(const std::vector<Touch *> &touches, cocos2d::Event *unused_event)
{
	Size winSize = Director::getInstance()->getWinSize();
	
	for (Touch * t : touches) {
		Point touchLocation = convertTouchToNodeSpace(t);
		if (touchLocation.x > (winSize.width / 2)) {
			_player->setMightAsWellJump(true);
		} else {
			_player->setForwardMarch(true);
		}
	}
}

void Level::onTouchesMoved(const std::vector<Touch *> &touches, cocos2d::Event *unused_event)
{
	Size winSize = Director::getInstance()->getWinSize();
	
	for (Touch * t : touches) {
		Point touchLocation = convertTouchToNodeSpace(t);
		
		Point previousTouchLocation = t->getPreviousLocationInView();
		Size screenSize = Director::getInstance()->getWinSize();
		previousTouchLocation = Point(previousTouchLocation.x, screenSize.height - previousTouchLocation.y);
		
		if (touchLocation.x > (winSize.width / 2) && previousTouchLocation.x <= (winSize.width / 2)) {
			_player->setForwardMarch(false);
			_player->setMightAsWellJump(true);
		} else if (previousTouchLocation.x > (winSize.width / 2) && touchLocation.x <= (winSize.width / 2)) {
			_player->setForwardMarch(true);
			_player->setMightAsWellJump(false);
		}
	}
}

void Level::onTouchesEnded(const std::vector<Touch *> &touches, cocos2d::Event *unused_event)
{
	Size winSize = Director::getInstance()->getWinSize();
	
	for (Touch * t : touches) {
		Point touchLocation = convertTouchToNodeSpace((Touch *)t);
		if (touchLocation.x < (winSize.width / 2)) {
			_player->setForwardMarch(false);
		} else {
			_player->setMightAsWellJump(false);
		}
	}
}

void Level::gameOver(bool won)
{
	Size winSize = Director::getInstance()->getWinSize();
	Point centerOfScreen = Point(winSize.width / 2, winSize.height / 2);
	
	_gameOver = true;
	string gameText;
	
	if (won) {
		gameText = "You Won!";
	} else {
		gameText = "You Lose!";
		CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("hurt.wav");
	}
	
	Label * diedLabel = Label::createWithSystemFont(gameText, "Helvetica", 40.0f);
	diedLabel->setPosition(centerOfScreen + Point(0, 100));
	
	Label * scoreLabel = Label::createWithSystemFont(StringUtils::format("Your score is: %d", _score), "Helvetica", 40.0f);
	scoreLabel->setPosition(centerOfScreen);
	
	MoveBy * slideIn = MoveBy::create(0.2f, Point(0, winSize.height / 2 - 100));
	
	MenuItemImage * replay = MenuItemImage::create("replay.png", "replay.png", CC_CALLBACK_0(Level::restart, this));
	
	Menu * menu = Menu::create(replay, NULL);
	menu->setPosition(Point(winSize.width / 2, 0));
	
	addChild(menu);
	addChild(diedLabel);
	addChild(scoreLabel);
	
	menu->runAction(slideIn);
}

void Level::checkForWin()
{
	if (_player->getPosition().x > 3130.0) {
		gameOver(1);
	}
}

void Level::setViewpointCenter(Point position)
{
	Size winSize = Director::getInstance()->getWinSize();
	
	int x = MAX(position.x, winSize.width / 2);
	int y = MAX(position.y, winSize.height / 2);
	x = MIN(x, (_map->getMapSize().width * _map->getTileSize().width) - winSize.width / 2);
	y = MIN(y, (_map->getMapSize().height * _map->getTileSize().height) - winSize.height / 2);
	Point actualPosition = Point(x, y);
	
	Point centerOfView = Point(winSize.width / 2, winSize.height / 2);
	Point viewPoint = centerOfView - actualPosition;
	_map->setPosition(viewPoint - Point(0, winSize.height / 2));
}

void Level::restart()
{
	_score = 0;
	Director::getInstance()->replaceScene(Level::createScene());
}
