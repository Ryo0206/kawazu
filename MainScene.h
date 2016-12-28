#pragma once
#include "cocos2d.h"
#include "Stage.h"

class MainScene :
	public cocos2d::Layer
{
protected:
	MainScene();
	virtual ~MainScene();
	//bool init() override;
	bool initWithLevel(int level);


public:
	//static cocos2d::Scene* createScene();
	static cocos2d::Scene* createSceneWithLevel(int level);
	void update(float dt) override;

	//画面がタップされているかどうか
	CC_SYNTHESIZE(bool, _isPress, IsPress);
	//現在の取得コイン数
	CC_SYNTHESIZE(int, _coin, Coin);
	//ステージ
	CC_SYNTHESIZE_RETAIN(Stage *, _stage, Stage);
	//コイン枚数表示用ラベル
	CC_SYNTHESIZE_RETAIN(cocos2d::Label *, _coinlabel, CoinLabel);
	//背景用ParallaxNode
	CC_SYNTHESIZE_RETAIN(cocos2d::ParallaxNode *, _parallaxNode, ParallaxNode);
	//CREATE_FUNC(MainScene);

private:
	void onGameOver();
};

