#include "Stage.h"

USING_NS_CC;

const char* STAGE_FILE_FORMAT = "map/stage%d.tmx";

Stage::Stage()
:_tiledMap(nullptr)
,_player(nullptr)
,_level(0)
{
}


Stage::~Stage()
{
	CC_SAFE_RELEASE_NULL(_tiledMap);
	CC_SAFE_RELEASE_NULL(_player);
}

bool Stage::initWithLevel(int level) {
	if (!Layer::init()) {
		return false;
	}

	//ステージ番号を格納
	_level = level;

	//マップからノードを作成する
	//auto map = TMXTiledMap::create("map/stage1.tmx");
	auto stageFile = StringUtils::format(STAGE_FILE_FORMAT, level);
	auto map = TMXTiledMap::create(stageFile);
	this->addChild(map);
	this->setTiledMap(map);

	auto terrainLayer = map->getLayer("Terrain");
	auto objectLayer = map->getLayer("Object");

	//マップサイズ
	auto mapSize = map->getMapSize();
	for (int x = 0; x < mapSize.width; ++x) {
		for (int y = 0; y < mapSize.height; ++y) {
			auto coordinate = Vec2(x, y);

			this->addPhysicsBody(terrainLayer, coordinate);
			this->addPhysicsBody(objectLayer, coordinate);
		}
	}

	//プレイヤーの作成
	auto player = Player::create();
	player->setPosition(Vec2(40, 160));
	this->addChild(player);
	this->setPlayer(player);

	//カメラの追従
	auto winSize = Director::getInstance()->getWinSize();
	this->runAction(Follow::create(player, Rect(0, 0, _tiledMap->getContentSize().width, _tiledMap->getContentSize().height)));

	this->scheduleUpdate();

	return true;
}

Stage * Stage::createWithLevel(int level) {
	Stage *ret = new Stage();
	if (ret->initWithLevel(level)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

void Stage::update(float dt) {

}

Sprite* Stage::addPhysicsBody(cocos2d::TMXLayer *layer, cocos2d::Vec2 &coordinate) 
{
	auto sprite = layer->getTileAt(coordinate);
	if (sprite) {
		//剛体用のマテリアルを作成
		auto material = PhysicsMaterial();
		//ひっかからないように摩擦を0にする
		material.friction = 0;
		//剛体を設置する	
		auto physicsBody = PhysicsBody::createBox(sprite->getContentSize(), material);
		//剛体を固定する
		physicsBody->setDynamic(false);
		//剛体を付けるSpriteのアンカーポイントを中心にする
		sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		//タイルのIDを取り出す
		auto gid = layer->getTileGIDAt(coordinate);
		//タイルのプロパティをmapで取り出す
		auto properties = _tiledMap->getPropertiesForGID(gid).asValueMap();

		//categoryというプロパティが存在しているかチェック
		if(properties.count("category") > 0){
		//プロパティの中からcategoryの値をintとして取り出す
		auto category = properties.at("category").asInt();
		//剛体にカテゴリをセットする
		physicsBody->setCategoryBitmask(category);
		//剛体と接触判定を取るカテゴリを指定する
		physicsBody->setContactTestBitmask(static_cast<int>(TileType::PLAYER));
		//剛体と衝突を取るカテゴリを指定する
		physicsBody->setCollisionBitmask(static_cast<int>(TileType::PLAYER));
		}
		//剛体をSpriteにつける
		sprite->setPhysicsBody(physicsBody);

		return sprite;
	}
	return nullptr;
}