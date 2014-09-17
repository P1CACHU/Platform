#ifndef __Platform__Level__
#define __Platform__Level__

#include "cocos2d.h"
#include "Player.h"

USING_NS_CC;
using namespace std;

class Gid : public Ref
{
public:
	Gid(int gid, float x, float y, Point tilePos)
	{
		_gid = gid;
		_x = x;
		_y = y;
		_tilePos = tilePos;
	}
	
	int _gid;
	float _x;
	float _y;
	Point _tilePos;
};

class Level : public Layer {
public:

	static cocos2d::Scene * createScene();
	virtual bool init();
	CREATE_FUNC(Level);
	
private:
	
	TMXTiledMap * _map;
	TMXLayer * _walls;
	TMXLayer * _coins;
	Player * _player;
	bool _gameOver;
	int _score;
	
	void update(float dt);
	Point tileCoordForPosition(Point position);
	Rect tileRectFromTileCoords(Point tileCoords);
	
	void onTouchesBegan(const std::vector<Touch *> &touches, cocos2d::Event *unused_event);
	void onTouchesMoved(const std::vector<Touch *> &touches, cocos2d::Event *unused_event);
	void onTouchesEnded(const std::vector<Touch *> &touches, cocos2d::Event *unused_event);
	
	void gameOver(bool won);
	void checkForWin();
	void setViewpointCenter(Point position);
	void restart();
	
	void handleCollectCoins(Player * p);
	__Array * getSurroundingTilesAtPosition(Point position, TMXLayer * layer);
	void checkForAndResolveCollisions(Player * p);
};

#endif /* defined(__Platform__Level__) */
