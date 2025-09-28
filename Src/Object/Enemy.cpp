#include "Enemy.h"
#include <DxLib.h>
#include "./ActorBase.h"
#include "../Utility/AsoUtility.h"
#include "../Utility/MatrixUtility.h"
#include "./AnimationController.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/Resource.h"

Enemy::Enemy(void)
{
	modelId_ = -1;
	pos_ = {};
	angles_ = {};
}

void Enemy::InitLoad(void)
{
	modelId_ = MV1LoadModel((ResourceManager::PATH_MODEL + "Player/Player.mv1").c_str());
}

void Enemy::InitTransform(void)
{
	angles_ = { 0.0f, AsoUtility::Deg2RadF(45.0f), 0.0f };

	scales_ = { 1.0f, 1.0f, 1.0f };

	// 角度から方向に変換する
	moveDir_ = { sinf(angles_.y), 0.0f, cosf(angles_.y) };


	MATRIX mat = MatrixUtility::Multiplication(LOCAL_ANGLES, angles_);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	pos_ = {200.0f , 0.0f, 150.0f};

	// モデルの位置設定
	MV1SetPosition(modelId_, pos_);
}

void Enemy::InitAnim(void)
{
	int type = static_cast<int>(ANIM_TYPE::IDLE);
	anim_ = new AnimationController(modelId_);

	anim_->Add(static_cast<int>(ANIM_TYPE::IDLE),
		SPEED_ANIM, (ResourceManager::PATH_MODEL + "Player/Idle.mv1").c_str());

	anim_->Add(static_cast<int>(ANIM_TYPE::WALK),
		SPEED_ANIM, (ResourceManager::PATH_MODEL + "Player/Walk.mv1").c_str());

	anim_->Add(static_cast<int>(ANIM_TYPE::WALK),
		SPEED_ANIM, (ResourceManager::PATH_MODEL + "Player/Throw.mv1").c_str());

	// 歩行アニメーション有効
	ChangeAnim(ANIM_TYPE::WALK);
}

void Enemy::InitPost(void)
{
}


void Enemy::Update(void)
{
	MATRIX mat = MatrixUtility::Multiplication(LOCAL_ANGLES, angles_);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	// モデルの位置設定
	MV1SetPosition(modelId_, pos_);

	anim_->Update();
}

void Enemy::Draw(void)
{
	MV1DrawModel(modelId_);
}

void Enemy::Release(void)
{
	anim_->Release();
	delete anim_;
}


void Enemy::ChangeAnim(ANIM_TYPE type, bool isLoop)
{
	// アニメーション遷移処理
	animType_ = static_cast<int>(type);
	anim_->Play(animType_, isLoop);
}
