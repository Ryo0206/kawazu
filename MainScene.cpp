#include "MainScene.h"

USING_NS_CC;

//重力を表すベクトル
const Vec2 GRAVITY_ACCELERATION = Vec2(0, -3);
//ステージ
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
	//物理エンジンを有効にしたシーンを作成する
	auto scene = Scene::createWithPhysics();

	//物理空間を取り出す
	auto world = scene->getPhysicsWorld();

	//重力を設定する
	world->setGravity(GRAVITY_ACCELERATION);

	//デバッグビルドの時
#if COCOS2D_DEBUG > 0;
	//world->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
#endif

	//物理空間のスピードを設定する
	world->setSpeed(6.0);
	//レイヤーの作成
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
	//画面サイズの取り出し
	auto winSize = Director::getInstance()->getWinSize();

	//ステージの作成
	//auto stage = Stage::create();
	auto stage = Stage::createWithLevel(level);
	this->addChild(stage);
	this->setStage(stage);

	//背景画像の作成
	auto background = Sprite::create("image/background.png");

	//扱いやすいように背景の基準点を左下に設定している
	background->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

	//ParallaxNodeの生成
	auto parallaxNode = ParallaxNode::create();
	this->setParallaxNode(parallaxNode);
	this->addChild(parallaxNode);

	//マップの横幅の取得
	auto mapWidth = stage->getTiledMap()->getContentSize().width;
	//背景画像の横幅の取得
	auto backgroundWidth = background->getContentSize().width;

	//スクロールの割合を計算する
	auto scrollRatio = (backgroundWidth - winSize.width) / mapWidth;
	parallaxNode->addChild(background, 0, Vec2(scrollRatio, 0), Vec2::ZERO);

	//タッチした時にタッチされているフラグをオンにする
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

	//物体が接触したことを検知するEventListener
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = [this](PhysicsContact& contact) {
		//2つの剛体のうち、プレイヤーでないほうを取り出す
		auto otherShape = contact.getShapeA()->getBody() == _stage->getPlayer()->getPhysicsBody() ?
			contact.getShapeB() : contact.getShapeA();
		auto body = otherShape->getBody();
		//カテゴリを取りだす
		auto category = body->getCategoryBitmask();

		if (category & static_cast<int>(Stage::TileType::ENEMY)) {
			//ゲームオーバー
			this->onGameOver();
		}
		else if (category & (int)Stage::TileType::COIN) {
			//コイン
			body->getNode()->removeFromParent();//コインを消す
			_coin += 1;
		}
		return true;
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);

	//剛体の作成
	auto wall = cocos2d::PhysicsBody::create();
	auto hazardWall = cocos2d::PhysicsBody::create();
	auto item = cocos2d::PhysicsBody::create();

	//カテゴリの定義
	const int PhysicsCategoryWall = 1 << 0;       //壁カテゴリ
	const int PhysicsCategoryHazard = 1 << 1;     //障害物カテゴリ
	const int PhysicsCategoryItem = 1 << 2;       //アイテムカテゴリ
	const int PhysicsCategoryCharacter = 1 << 3;  //キャラクターカテゴリ

	//剛体にカテゴリをセットする
	wall->setCategoryBitmask(PhysicsCategoryWall);//壁
	hazardWall->setCategoryBitmask(PhysicsCategoryWall | PhysicsCategoryHazard);//壁または障害物
	item->setCategoryBitmask(PhysicsCategoryItem);//アイテム

	//プレイヤーの剛体を定義する
	auto player = PhysicsBody::create();

	//プレイヤーをCharacterカテゴリに登録する
	player->setCategoryBitmask(PhysicsCategoryCharacter);

	//プレイヤーとWallカテゴリの剛体は衝突が有効になる
	player->setCollisionBitmask(PhysicsCategoryWall);

	//プレイヤーとHazard,Itemカテゴリの剛体は接触判定が有効になる
	player->setContactTestBitmask(PhysicsCategoryHazard | PhysicsCategoryItem);

	//接触される方もプレイヤーとの接触判定を有効にしておく
	item->setContactTestBitmask(PhysicsCategoryCharacter);
	hazardWall->setContactTestBitmask(PhysicsCategoryCharacter);

	//コインの枚数の表示
	auto label = Label::createWithCharMap("image/numbers.png", 16, 18, '0');
	this->addChild(label);
	label->setPosition(Vec2(200, winSize.height - 10));
	label->enableShadow();
	this->setCoinLabel(label);

	//毎フレーム呼び出す
	this->scheduleUpdate();

	return true;
}

void MainScene::onGameOver() {
	//プレイヤーをステージから削除する
	_stage->getPlayer()->removeFromParent();

	//画面サイズを取り出す
	auto winSize = Director::getInstance()->getWinSize();

	//GAME OVERの表示
	auto gameover = Sprite::create("image/gameover.png");
	gameover->setPosition(Vec2(winSize.width / 2.0, winSize.height / 1.5));
	this->addChild(gameover);

	//もう一度遊ぶボタン
	auto menuItem = MenuItemImage::create("image/replay.png", "image/replay_pressed.png", [this]
	(Ref *sender) {
		auto scene = MainScene::createSceneWithLevel(_stage->getLevel());
		auto transition = TransitionFade::create(1.0, scene);
		Director::getInstance()->replaceScene(transition);
	});
	auto menu = Menu::create(menuItem, nullptr);
	menu->setPosition(winSize.width / 2.0, winSize.height / 3);
	this->addChild(menu);

	//パーティクル表示
	auto explosion = ParticleExplosion::create();
	explosion->setPosition(_stage->getPlayer()->getPosition());
	_stage->addChild(explosion);
}

void MainScene::update(float dt){
	//画面がタップされているとき
	if (this->getIsPress()) {
		//プレイヤーに上方向の推進力を与える
		_stage->getPlayer()->getPhysicsBody()->applyImpulse(IMPULSE_ACCELERATION);
	}
	auto winSize = Director::getInstance()->getWinSize();
	auto position = _stage->getPlayer()->getPosition();
	const auto margin = 50;
	//プレイヤーの位置が画面外に出たらゲームオーバーを呼び出す
	if (position.y < -margin || position.y >= winSize.height + margin) {
		//何度も呼ばれることをふせぐためプレイヤーがステージにいなければならない
		if (_stage->getPlayer()->getParent() != nullptr) {
			this->onGameOver();
		}
	}
	//背景をプレイヤーの位置によって動かす
	_parallaxNode->setPosition(_stage->getPlayer()->getPosition() * -1);
	//コインとコインラベルを同期
	this->getCoinLabel()->setString(StringUtils::toString(_coin));
}