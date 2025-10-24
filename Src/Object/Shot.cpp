#include "Shot.h"
#include <DxLib.h>
#include "./ActorBase.h"
#include "../Manager/SceneManager.h"
#include "../Utility/AsoUtility.h"
#include "./ActorBase.h"


Shot::Shot(int handle, unsigned int color, ActorBase& actor)
	:actor_(actor)
{
	state_ = STATE::NONE;
	modelId_ = handle;
	difColor_ = color;
	pos_ = actor_.GetPosHand();
	scale_ = actor_.GetShotScale();
	radius_ = actor_.GetShotRadius();

	Init();
}


void Shot::Init(void)
{
	state_ = STATE::TRACK;
	waitTime_ = 0.0f;

	MV1SetScale(modelId_, scale_);
	MV1SetPosition(modelId_, pos_);
}

void Shot::Update(void)
{
	float delta = SceneManager::GetInstance().GetDeltaTime();

	if (state_ == STATE::TRACK)
	{
		// 手に追尾
		pos_ = actor_.GetPosHand();
	}

	else if (state_ == STATE::SHOT)
	{
		velocity_.y -= ActorBase::GRAVITY_ACC;

		if (velocity_.y < -ActorBase::GRAVITY_MAX)
		{
			velocity_.y = -ActorBase::GRAVITY_MAX;
		}

		if (pos_.y > 0.0f)
		{
			// 位置更新
			pos_ = VAdd(pos_, velocity_);
		}
		else
		{
			state_ = STATE::TRACK;
			velocity_ = AsoUtility::VECTOR_ZERO;
		}
	}
	else if (state_ == STATE::HIT)
	{
		// 待機時間減少
		waitTime_ -= ((waitTime_ > 0.0f) ? delta : 0.0f);

		if (waitTime_ <= 0.0f)
		{
			state_ = STATE::TRACK;
		}
	}
}

void Shot::Draw(void)
{
	// 位置割り当て
	MV1SetPosition(modelId_, pos_);

	if (state_ == STATE::SHOT || state_ == STATE::TRACK)
	{
		// 弾描画
		MV1DrawModel(modelId_);

#ifdef _DEBUG
		DrawSphere3D(pos_, radius_, 16,
			0x00ffff, 0xffffff, false);
#endif
	}
}

void Shot::SetShot(VECTOR& _dir, float shotSpeed, float shotY)
{
	// 待機中はfalse
	if (waitTime_ > 0.0f) { return; }

	state_ = STATE::SHOT;

	// 横の向きを正規化して取得
	dir_ = _dir;
	VECTOR normDir = VNorm(dir_);

	// 発射方向を割り当て
	velocity_ = VScale(normDir, shotSpeed);
	velocity_.y = shotY;
}

void Shot::SetIsHit(void)
{
	if (state_ == STATE::SHOT)
	{
		state_ = STATE::HIT;
		waitTime_ = HIT_WAIT;
	}
}
