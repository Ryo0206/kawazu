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

	//�W�F�b�g�̐��i��
	CC_SYNTHESIZE_PASS_BY_REF(cocos2d::Vec2, _acceleration, Acceleration);
	CREATE_FUNC(Player);

};

