#ifndef __Platform__Player__
#define __Platform__Player__

#include "cocos2d.h"
USING_NS_CC;

class Player : public Sprite
{
public:
	
	inline void setVelocity(Point value) { _velocity = value; }
	inline void setForwardMarch(bool value) { _forwardMarch = value; }
	inline void setMightAsWellJump(bool value) { _mightAsWellJump = value; }
	inline void setOnGround(bool value) { _onGround = value; }
	
	void updatePosition(float dt);
	Rect collisionBoundingBox();
	
	Point _velocity;
	Point _desiredPosition;
	
private:
	
	bool _onGround = true;
	bool _forwardMarch = false;
	bool _mightAsWellJump = false;
};

#endif /* defined(__Platform__Player__) */
