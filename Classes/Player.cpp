#import "Player.h"
#import "SimpleAudioEngine.h"

void Player::updatePosition(float dt)
{
	Point jumpForce = Point(0.0, 310.0);
	float jumpCutoff = 150.0;
	
	if (_mightAsWellJump && _onGround) {
		_velocity += jumpForce;
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("jump.wav");
	} else if (!_mightAsWellJump && _velocity.y > jumpCutoff) {
		_velocity = Point(_velocity.x, jumpCutoff);
	}
	
	Point gravity = Point(0.0, -450.0);
	Point gravityStep = gravity * dt;
	
	Point forwardMove = Point(800.0, 0.0);
	Point forwardStep = forwardMove * dt;
	
	_velocity = Point(_velocity.x * 0.90, _velocity.y);
	
	if (_forwardMarch)
		_velocity += forwardStep;
	
	Point minMovement = Point(0.0, -450.0);
	Point maxMovement = Point(120.0, 250.0);
	_velocity.clamp(minMovement, maxMovement);
	
	_velocity = _velocity + gravityStep;
	
	Point stepVelocity = _velocity * dt;
	
	_desiredPosition = _position + stepVelocity;
}

Rect Player::collisionBoundingBox()
{
	return getBoundingBox();
}
