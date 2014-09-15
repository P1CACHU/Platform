#ifndef __Platform__Level__
#define __Platform__Level__

#include "cocos2d.h"
#include "Player.h"

USING_NS_CC;
using namespace std;

class Level : public Layer {
public:

	static cocos2d::Scene * createScene();
	virtual bool init();
	CREATE_FUNC(Level);
	
private:
	
	TMXTiledMap * _map;
	Player * _player;
	
	void update(float dt);
	Point tileCoordForPosition(Point position);
	Rect tileRectFromTileCoords(Point tileCoords);

	struct Dictionary {
		int gid;
		float x;
		float y;
		Point tilePos;
	};
	
	vector<Level::Dictionary> getSurroundingTilesAtPosition(Point position, TMXLayer * layer);
};

#endif /* defined(__Platform__Level__) */
