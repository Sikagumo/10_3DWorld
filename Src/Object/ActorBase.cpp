#include "ActorBase.h"
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Utility/MatrixUtility.h"
#include "./AnimationController.h"


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
}

void ActorBase::Update(void)
{
	// モデルのY軸回転
	//angles_.y += 0.01f;
	
	// プレイヤーの遅延回転処理
	DelayRotate();

	// 行列の合成(子, 親と指定すると親⇒子の順に適用される)
	MATRIX mat = MatrixUtility::Multiplication(localAngles_, angles_);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	// プレイヤーの移動処理
	Move();

	MV1SetPosition(modelId_, pos_);

	// アニメーションの更新
	anim_->Update();
}

void ActorBase::Draw(void)
{
	MV1DrawModel(modelId_);
}

void ActorBase::Release(void)
{
	delete anim_;
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
void ActorBase::Move(void)
{
}
void ActorBase::DelayRotate(void)
{
	// 移動方向から角度に変換する
	float goal = atan2f(moveDir_.x, moveDir_.z);

	// 常に最短経路で補間
	angles_.y = AsoUtility::LerpAngle(angles_.y, goal, 0.2f);
}