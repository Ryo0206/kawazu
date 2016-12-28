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

	//��ʂ��^�b�v����Ă��邩�ǂ���
	CC_SYNTHESIZE(bool, _isPress, IsPress);
	//���݂̎擾�R�C����
	CC_SYNTHESIZE(int, _coin, Coin);
	//�X�e�[�W
	CC_SYNTHESIZE_RETAIN(Stage *, _stage, Stage);
	//�R�C�������\���p���x��
	CC_SYNTHESIZE_RETAIN(cocos2d::Label *, _coinlabel, CoinLabel);
	//�w�i�pParallaxNode
	CC_SYNTHESIZE_RETAIN(cocos2d::ParallaxNode *, _parallaxNode, ParallaxNode);
	//CREATE_FUNC(MainScene);

private:
	void onGameOver();
};

