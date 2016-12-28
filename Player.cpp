#include "Player.h"
#include "Stage.h"

USING_NS_CC;

//�A�j���[�V���������t���[�����邩
const int FRAME_COUNT = 4;
//�������̑��x�̍ő�l
const int VELOCITY_LIMIT = 40;
//�����W�F�b�g�����x
const Vec2 INITAL_ACCELERATION = Vec2(200, 0);


Player::Player()
{
}


Player::~Player()
{
}


bool Player::init() {
	if (!Sprite::initWithFile("image/player.png")) {
		return false;
	}
	//1�t���[���̉摜�T�C�Y���擾����
	auto frameSize = Size(this->getContentSize().width / FRAME_COUNT, this->getContentSize().height);
	//�e�N�X�`���̑傫����1�t���[�����ɂ���
	this->setTextureRect(Rect(0, 0, frameSize.width, frameSize.height));

	Vector<SpriteFrame *> frames;
	for (int i = 0; i < FRAME_COUNT; i++) {
		//�P�R�}���A�j���[�V�������쐬����
		auto frame = SpriteFrame::create("image/player.png", Rect(frameSize.width * i, 0, frameSize.width, frameSize.height));
		frames.pushBack(frame);
	}
	auto animation = Animation::createWithSpriteFrames(frames);
	animation->setDelayPerUnit(0.05);
	this->runAction(RepeatForever::create(Animate::create(animation)));

	auto body = PhysicsBody::createCircle(this->getContentSize().width / 2.0);
	//���̂̉�]�𖳌��ɂ���
	body->setRotationEnable(false);

	//�J�e�S����PLAYER�ɃZ�b�g����
	body->setCategoryBitmask(static_cast<int>(Stage::TileType::PLAYER));

	//�ǂƂ̏Փ˂���
	body->setCollisionBitmask(static_cast<int>(Stage::TileType::WALL));

	//���ׂĂ̍��̂�ڐG������s��
	body->setContactTestBitmask(INT_MAX);

	this->setPhysicsBody(body);

	//�������x��ݒ肷��
	_acceleration = INITAL_ACCELERATION;

	this->scheduleUpdate();

	return true;
}


void Player::update(float dt) {
	//���t���[�����킸����ɐ��i�͂�^����
	this->getPhysicsBody()->applyImpulse(_acceleration);
	//�������̑��x��VELLOCIY_LIMIT�ȏ�ɂȂ����璴���Ȃ��悤�ɂ���
	auto v = this->getPhysicsBody()->getVelocity();
	if (v.x > VELOCITY_LIMIT) {
		v.x = VELOCITY_LIMIT;
		this->getPhysicsBody()->setVelocity(v);
	}
}
