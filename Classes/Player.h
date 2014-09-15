#ifndef __Platform__Player__
#define __Platform__Player__

#include "cocos2d.h"
USING_NS_CC;

class Player : public Sprite
{
public:
	
	inline void setVelocity(Point value) { _velocity = value; }
	
	void update(float dt);
	cocos2d::Rect collisionBoundingBox();
	
private:
	
	Point _velocity;
	Point _desiredPosition;
	bool _onGround;
	bool _forwardMarch;
	bool _mightAsWellJump;
};

#endif /* defined(__Platform__Player__) */
