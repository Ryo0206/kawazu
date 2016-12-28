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

	//�X�e�[�W�ԍ����i�[
	_level = level;

	//�}�b�v����m�[�h���쐬����
	//auto map = TMXTiledMap::create("map/stage1.tmx");
	auto stageFile = StringUtils::format(STAGE_FILE_FORMAT, level);
	auto map = TMXTiledMap::create(stageFile);
	this->addChild(map);
	this->setTiledMap(map);

	auto terrainLayer = map->getLayer("Terrain");
	auto objectLayer = map->getLayer("Object");

	//�}�b�v�T�C�Y
	auto mapSize = map->getMapSize();
	for (int x = 0; x < mapSize.width; ++x) {
		for (int y = 0; y < mapSize.height; ++y) {
			auto coordinate = Vec2(x, y);

			this->addPhysicsBody(terrainLayer, coordinate);
			this->addPhysicsBody(objectLayer, coordinate);
		}
	}

	//�v���C���[�̍쐬
	auto player = Player::create();
	player->setPosition(Vec2(40, 160));
	this->addChild(player);
	this->setPlayer(player);

	//�J�����̒Ǐ]
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
		//���̗p�̃}�e���A�����쐬
		auto material = PhysicsMaterial();
		//�Ђ�������Ȃ��悤�ɖ��C��0�ɂ���
		material.friction = 0;
		//���̂�ݒu����	
		auto physicsBody = PhysicsBody::createBox(sprite->getContentSize(), material);
		//���̂��Œ肷��
		physicsBody->setDynamic(false);
		//���̂�t����Sprite�̃A���J�[�|�C���g�𒆐S�ɂ���
		sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		//�^�C����ID�����o��
		auto gid = layer->getTileGIDAt(coordinate);
		//�^�C���̃v���p�e�B��map�Ŏ��o��
		auto properties = _tiledMap->getPropertiesForGID(gid).asValueMap();

		//category�Ƃ����v���p�e�B�����݂��Ă��邩�`�F�b�N
		if(properties.count("category") > 0){
		//�v���p�e�B�̒�����category�̒l��int�Ƃ��Ď��o��
		auto category = properties.at("category").asInt();
		//���̂ɃJ�e�S�����Z�b�g����
		physicsBody->setCategoryBitmask(category);
		//���̂ƐڐG��������J�e�S�����w�肷��
		physicsBody->setContactTestBitmask(static_cast<int>(TileType::PLAYER));
		//���̂ƏՓ˂����J�e�S�����w�肷��
		physicsBody->setCollisionBitmask(static_cast<int>(TileType::PLAYER));
		}
		//���̂�Sprite�ɂ���
		sprite->setPhysicsBody(physicsBody);

		return sprite;
	}
	return nullptr;
}