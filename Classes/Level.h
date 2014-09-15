#ifndef __Platform__Level__
#define __Platform__Level__

#include "cocos2d.h"

class Level : public cocos2d::Layer {
public:

	static cocos2d::Scene * createScene();
	virtual bool init();
	CREATE_FUNC(Level);
	
private:
	
	cocos2d::TMXTiledMap * _map;
};

#endif /* defined(__Platform__Level__) */
