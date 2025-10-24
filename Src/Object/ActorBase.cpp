#include "ActorBase.h"
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Utility/MatrixUtility.h"
#include "./AnimationController.h"
#include "../Manager/SceneManager.h"
#include "./Shot.h"


ActorBase::ActorBase(void)
{
}

void ActorBase::Init(void)
{
	// リソースロード
	InitLoad();

	// Transform初期化
	InitTransform();

	// 大きさ、回転、座標のモデル設定
	InitTransformPost();

	// アニメーションの初期化
	InitAnim();

	// 初期化後の個別処理
	InitPost();

	radius_ = RADIUS;

	handNum_ = MV1SearchFrame(modelId_, "mixamorig:RightHandMiddle1");
	SetFrame();

}

void ActorBase::Update(void)
{
	// モデルのY軸回転
	//angles_.y += 0.01f;
	
	// 無敵時間減少
	float delta = SceneManager::GetInstance().GetDeltaTime();
	timeInv_ -= ((timeInv_ > 0.0f) ? delta : 0.0f);

	// プレイヤーの遅延回転処理
	DelayRotate();

	// 行列の合成(子, 親と指定すると親⇒子の順に適用される)
	MATRIX mat = MatrixUtility::Multiplication(localAngles_, angles_);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	// 重力加速
	Gravity();

	// 移動処理
	Move();

	// 攻撃処理
	Attack();

	// 弾更新
	shot_->Update();

	// アニメーションの更新
	anim_->Update();
}

void ActorBase::Draw(void)
{
	if (hp_ > 0)
	{
		// 描画処理
		MV1DrawModel(modelId_);

#ifdef _DEBUG

		// 手の位置
		//DrawSphere3D(handPos_, 30.0f, 16, 0x00ff00, 0xffffff, false);

		// 胴体の当たり判定
		DrawCapsule3D(pos_, GetPosHead(), radius_, 16, 0xff0000, 0xffffff, false);
#endif

		shot_->Draw();
	}
}

void ActorBase::Release(void)
{
	delete shot_;

	anim_->Release();
	delete anim_;
}

void ActorBase::CollisionStage(const VECTOR& pos)
{
	if (!isGround_ && jumpPow_ <= 0.0f)
	{
		pos_ = pos;
		jumpPow_ = 0.0f;
		isGround_ = true;
	}
}

void ActorBase::SetDamage(void)
{
	if (timeInv_ > 0.0f) { return; }

	timeInv_ = INV_TIME;

	// HPが0より大きいときのみ、１減少
	hp_ -= ((hp_ > 0) ? 1 : 0);
}


void ActorBase::InitTransformPost(void)
{
	// 大きさをモデルに反映
	MV1SetScale(modelId_, scales_);
	
	// 角度から方向に変換する
	moveDir_ = { sinf(angles_.y), 0.0f, cosf(angles_.y) };
	VECTOR preInputDir_ = moveDir_;

	// 行列の合成(子, 親と指定すると親⇒子の順に適用される)
	MATRIX mat = MatrixUtility::Multiplication(localAngles_, angles_);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	// 座標をモデルに反映
	MV1SetPosition(modelId_, pos_);
}

void ActorBase::Gravity(void)
{
	if (isGround_) { return; }

	// 重力加速
	jumpPow_ -= GRAVITY_ACC;

	if (jumpPow_ < -GRAVITY_MAX)
	{
		jumpPow_ = -GRAVITY_MAX;
	}

	// 位置に反映
	pos_.y += jumpPow_;
}

void ActorBase::DelayRotate(void)
{
	// 移動方向から角度に変換する
	float goal = atan2f(moveDir_.x, moveDir_.z);

	// 常に最短経路で補間
	angles_.y = AsoUtility::LerpAngle(angles_.y, goal, 0.2f);
}

void ActorBase::SetFrame(void)
{
	handPos_ = MV1GetFramePosition(modelId_, handNum_);
	handLocalMat_ = MV1GetFrameLocalMatrix(modelId_, handNum_);
}
