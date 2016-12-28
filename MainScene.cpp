#include "MainScene.h"

USING_NS_CC;

//�d�͂�\���x�N�g��
const Vec2 GRAVITY_ACCELERATION = Vec2(0, -3);
//�X�e�[�W
const Vec2 IMPULSE_ACCELERATION = Vec2(0, 180);

MainScene::MainScene()
	:_isPress(false)
	, _coin(0)
	, _coinlabel(nullptr)
	, _stage(nullptr)
	, _parallaxNode(nullptr)
{
}


MainScene::~MainScene()
{
	CC_SAFE_RELEASE_NULL(_stage);
	CC_SAFE_RELEASE_NULL(_coinlabel);
	CC_SAFE_RELEASE_NULL(_parallaxNode);
}


Scene* MainScene::createSceneWithLevel(int level) {
	//�����G���W����L���ɂ����V�[�����쐬����
	auto scene = Scene::createWithPhysics();

	//������Ԃ����o��
	auto world = scene->getPhysicsWorld();

	//�d�͂�ݒ肷��
	world->setGravity(GRAVITY_ACCELERATION);

	//�f�o�b�O�r���h�̎�
#if COCOS2D_DEBUG > 0;
	//world->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
#endif

	//������Ԃ̃X�s�[�h��ݒ肷��
	world->setSpeed(6.0);
	//���C���[�̍쐬
	//auto layer = MainScene::create();
	auto layer = new MainScene();
	if (layer && layer->initWithLevel(level)) {
		layer->autorelease();
	}
	else {
		CC_SAFE_DELETE(layer);
	}
	scene->addChild(layer);

	return scene;
}

bool MainScene::initWithLevel(int level) {
	if (!Layer::init()) {
		return false;
	}
	//��ʃT�C�Y�̎��o��
	auto winSize = Director::getInstance()->getWinSize();

	//�X�e�[�W�̍쐬
	//auto stage = Stage::create();
	auto stage = Stage::createWithLevel(level);
	this->addChild(stage);
	this->setStage(stage);

	//�w�i�摜�̍쐬
	auto background = Sprite::create("image/background.png");

	//�����₷���悤�ɔw�i�̊�_�������ɐݒ肵�Ă���
	background->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

	//ParallaxNode�̐���
	auto parallaxNode = ParallaxNode::create();
	this->setParallaxNode(parallaxNode);
	this->addChild(parallaxNode);

	//�}�b�v�̉����̎擾
	auto mapWidth = stage->getTiledMap()->getContentSize().width;
	//�w�i�摜�̉����̎擾
	auto backgroundWidth = background->getContentSize().width;

	//�X�N���[���̊������v�Z����
	auto scrollRatio = (backgroundWidth - winSize.width) / mapWidth;
	parallaxNode->addChild(background, 0, Vec2(scrollRatio, 0), Vec2::ZERO);

	//�^�b�`�������Ƀ^�b�`����Ă���t���O���I���ɂ���
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [this](Touch *touch, Event *event) {
		this->setIsPress(true);
		return true;
	};
	listener->onTouchEnded = [this](Touch *touch, Event *event) {
		this->setIsPress(false);
	};
	listener->onTouchCancelled = [this](Touch *touch, Event *event) {
		this->setIsPress(false);
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	//���̂��ڐG�������Ƃ����m����EventListener
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = [this](PhysicsContact& contact) {
		//2�̍��̂̂����A�v���C���[�łȂ��ق������o��
		auto otherShape = contact.getShapeA()->getBody() == _stage->getPlayer()->getPhysicsBody() ?
			contact.getShapeB() : contact.getShapeA();
		auto body = otherShape->getBody();
		//�J�e�S������肾��
		auto category = body->getCategoryBitmask();

		if (category & static_cast<int>(Stage::TileType::ENEMY)) {
			//�Q�[���I�[�o�[
			this->onGameOver();
		}
		else if (category & (int)Stage::TileType::COIN) {
			//�R�C��
			body->getNode()->removeFromParent();//�R�C��������
			_coin += 1;
		}
		return true;
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);

	//���̂̍쐬
	auto wall = cocos2d::PhysicsBody::create();
	auto hazardWall = cocos2d::PhysicsBody::create();
	auto item = cocos2d::PhysicsBody::create();

	//�J�e�S���̒�`
	const int PhysicsCategoryWall = 1 << 0;       //�ǃJ�e�S��
	const int PhysicsCategoryHazard = 1 << 1;     //��Q���J�e�S��
	const int PhysicsCategoryItem = 1 << 2;       //�A�C�e���J�e�S��
	const int PhysicsCategoryCharacter = 1 << 3;  //�L�����N�^�[�J�e�S��

	//���̂ɃJ�e�S�����Z�b�g����
	wall->setCategoryBitmask(PhysicsCategoryWall);//��
	hazardWall->setCategoryBitmask(PhysicsCategoryWall | PhysicsCategoryHazard);//�ǂ܂��͏�Q��
	item->setCategoryBitmask(PhysicsCategoryItem);//�A�C�e��

	//�v���C���[�̍��̂��`����
	auto player = PhysicsBody::create();

	//�v���C���[��Character�J�e�S���ɓo�^����
	player->setCategoryBitmask(PhysicsCategoryCharacter);

	//�v���C���[��Wall�J�e�S���̍��̂͏Փ˂��L���ɂȂ�
	player->setCollisionBitmask(PhysicsCategoryWall);

	//�v���C���[��Hazard,Item�J�e�S���̍��̂͐ڐG���肪�L���ɂȂ�
	player->setContactTestBitmask(PhysicsCategoryHazard | PhysicsCategoryItem);

	//�ڐG���������v���C���[�Ƃ̐ڐG�����L���ɂ��Ă���
	item->setContactTestBitmask(PhysicsCategoryCharacter);
	hazardWall->setContactTestBitmask(PhysicsCategoryCharacter);

	//�R�C���̖����̕\��
	auto label = Label::createWithCharMap("image/numbers.png", 16, 18, '0');
	this->addChild(label);
	label->setPosition(Vec2(200, winSize.height - 10));
	label->enableShadow();
	this->setCoinLabel(label);

	//���t���[���Ăяo��
	this->scheduleUpdate();

	return true;
}

void MainScene::onGameOver() {
	//�v���C���[���X�e�[�W����폜����
	_stage->getPlayer()->removeFromParent();

	//��ʃT�C�Y�����o��
	auto winSize = Director::getInstance()->getWinSize();

	//GAME OVER�̕\��
	auto gameover = Sprite::create("image/gameover.png");
	gameover->setPosition(Vec2(winSize.width / 2.0, winSize.height / 1.5));
	this->addChild(gameover);

	//������x�V�ԃ{�^��
	auto menuItem = MenuItemImage::create("image/replay.png", "image/replay_pressed.png", [this]
	(Ref *sender) {
		auto scene = MainScene::createSceneWithLevel(_stage->getLevel());
		auto transition = TransitionFade::create(1.0, scene);
		Director::getInstance()->replaceScene(transition);
	});
	auto menu = Menu::create(menuItem, nullptr);
	menu->setPosition(winSize.width / 2.0, winSize.height / 3);
	this->addChild(menu);

	//�p�[�e�B�N���\��
	auto explosion = ParticleExplosion::create();
	explosion->setPosition(_stage->getPlayer()->getPosition());
	_stage->addChild(explosion);
}

void MainScene::update(float dt){
	//��ʂ��^�b�v����Ă���Ƃ�
	if (this->getIsPress()) {
		//�v���C���[�ɏ�����̐��i�͂�^����
		_stage->getPlayer()->getPhysicsBody()->applyImpulse(IMPULSE_ACCELERATION);
	}
	auto winSize = Director::getInstance()->getWinSize();
	auto position = _stage->getPlayer()->getPosition();
	const auto margin = 50;
	//�v���C���[�̈ʒu����ʊO�ɏo����Q�[���I�[�o�[���Ăяo��
	if (position.y < -margin || position.y >= winSize.height + margin) {
		//���x���Ă΂�邱�Ƃ��ӂ������߃v���C���[���X�e�[�W�ɂ��Ȃ���΂Ȃ�Ȃ�
		if (_stage->getPlayer()->getParent() != nullptr) {
			this->onGameOver();
		}
	}
	//�w�i���v���C���[�̈ʒu�ɂ���ē�����
	_parallaxNode->setPosition(_stage->getPlayer()->getPosition() * -1);
	//�R�C���ƃR�C�����x���𓯊�
	this->getCoinLabel()->setString(StringUtils::toString(_coin));
}