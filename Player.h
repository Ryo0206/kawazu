#pragma once
#include "cocos2d.h"
class Player :
	public cocos2d::Sprite
{
protected:
	bool init() override;
	Player();
	~Player();
public:
	void update(float dt) override;

	//ジェットの推進力
	CC_SYNTHESIZE_PASS_BY_REF(cocos2d::Vec2, _acceleration, Acceleration);
	CREATE_FUNC(Player);

};

