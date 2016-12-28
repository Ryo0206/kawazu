#pragma once
#include "cocos2d.h"
#include "Player.h"

class Stage :
	public cocos2d::Layer
{
protected:
	Stage();
	virtual ~Stage();
	bool initWithLevel(int level);
public:
	void update(float dt) override;

	//�^�C���̎��
	enum class TileType {
		//��
		WALL = 1 << 0,
		//�v���C���[
		PLAYER = 1 << 1,
		//�G�L����
		ENEMY = 1 << 2,
		//�R�C��
		COIN = 1 << 3
	};
	CC_SYNTHESIZE_READONLY(int, _level, Level);
	//�^�C���}�b�v
	CC_SYNTHESIZE_RETAIN(cocos2d::TMXTiledMap *, _tiledMap, TiledMap);
	//�v���C���[
	CC_SYNTHESIZE_RETAIN(Player *, _player, Player);

	static Stage * createWithLevel(int level);

	CREATE_FUNC(Stage);
private:
	//�w��̃��C���[�̓���̈ʒu�̃^�C���ɍ��̂�t����
	cocos2d::Sprite* addPhysicsBody(cocos2d::TMXLayer *layer, cocos2d::Vec2&coordinate);
};

