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

	//タイルの種類
	enum class TileType {
		//壁
		WALL = 1 << 0,
		//プレイヤー
		PLAYER = 1 << 1,
		//敵キャラ
		ENEMY = 1 << 2,
		//コイン
		COIN = 1 << 3
	};
	CC_SYNTHESIZE_READONLY(int, _level, Level);
	//タイルマップ
	CC_SYNTHESIZE_RETAIN(cocos2d::TMXTiledMap *, _tiledMap, TiledMap);
	//プレイヤー
	CC_SYNTHESIZE_RETAIN(Player *, _player, Player);

	static Stage * createWithLevel(int level);

	CREATE_FUNC(Stage);
private:
	//指定のレイヤーの特定の位置のタイルに剛体を付ける
	cocos2d::Sprite* addPhysicsBody(cocos2d::TMXLayer *layer, cocos2d::Vec2&coordinate);
};

