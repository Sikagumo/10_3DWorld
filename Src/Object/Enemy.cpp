#include "Enemy.h"
#include <DxLib.h>
#include "./ActorBase.h"
#include "../Utility/AsoUtility.h"
#include "../Utility/MatrixUtility.h"
#include "./AnimationController.h"
#include "./Player.h"
#include "../Manager/ResourceManager.h"

Enemy::Enemy(Player* player)
{
	player_ = player;

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
	angles_ = { 0.0f, AsoUtility::Deg2Rad(45.0f), 0.0f };
	localAngles_ = LOCAL_ANGLES;

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

	anim_->Add(static_cast<int>(ANIM_TYPE::THROW),
		SPEED_ANIM, (ResourceManager::PATH_MODEL + "Player/Throw.mv1").c_str());

	// 歩行アニメーション有効
	ChangeAnim(ANIM_TYPE::WALK);
}

void Enemy::InitPost(void)
{
	isNoticeVision_ = false;
	isNoticeHearing_ = false;
}

void Enemy::Update(void)
{
	ActorBase::Update();

	// 索敵
	Search();

	// 索敵
	SearchField();
}

void Enemy::Draw(void)
{
	ActorBase::Draw();

	DrawSearchRange();

	DrawViewRange();
}
void Enemy::DrawViewRange(void)
{
	unsigned int color = ((isNoticeVision_) ? 0xff0000 : 0x00aaff);

	MATRIX mat = MatrixUtility::GetMatrixRotateXYZ(angles_);

	// 前方方向
	VECTOR forward = VTransform(AsoUtility::DIR_F, mat);

	// 左側方向
	MATRIX leftMat = MMult(mat, MGetRotY(AsoUtility::Deg2Rad(-VIEW_ANGLE)));
	VECTOR left = VTransform(AsoUtility::DIR_F, leftMat);

	// 右側方向
	MATRIX rightMat = MMult(mat, MGetRotY(AsoUtility::Deg2Rad(VIEW_ANGLE)));
	VECTOR right = VTransform(AsoUtility::DIR_F, rightMat);

	// 正面の位置
	VECTOR forwardPos = VAdd(pos_, VScale(forward, VIEW_RANGE));

	// 左の位置
	VECTOR leftPos = VAdd(pos_, VScale(left, VIEW_RANGE));

	// 右の位置
	VECTOR rightPos = VAdd(pos_, VScale(right, VIEW_RANGE));

	DrawTriangle3D(pos_, leftPos, forwardPos, color, true);
	DrawTriangle3D(pos_, forwardPos, rightPos, color, true);

	DrawLine3D(pos_, forwardPos, 0x0000ff);
	DrawLine3D(pos_, leftPos, 0x0000ff);
	DrawLine3D(pos_, rightPos, 0x0000ff);
}

void Enemy::DrawSearchRange(void)
{
	unsigned int color = ((isNoticeHearing_) ? 0xffff00 : 0x00aaaa);
	MATRIX mat = MatrixUtility::GetMatrixRotateXYZ(angles_);

	int split = 36;
	float angle = (360.0f / split);

	// 前方方向
	VECTOR forward = VTransform(AsoUtility::DIR_F, mat);

	MATRIX leftMat, rightMat = MGetIdent();
	VECTOR left, right = {};

	VECTOR leftPos, rightPos = {};

	for (int i = 0; i < split; i++)
	{
		leftMat = MMult(mat, MGetRotY(AsoUtility::Deg2Rad(angle * i)));
		left = VTransform(AsoUtility::DIR_F, leftMat);

		rightMat = MMult(mat, MGetRotY(AsoUtility::Deg2Rad(angle * (i + 1))));
		right = VTransform(AsoUtility::DIR_F, rightMat);

		leftPos = VAdd(pos_, VScale(left, VIEW_RANGE));
		rightPos = VAdd(pos_, VScale(right, VIEW_RANGE));

		DrawTriangle3D(pos_, leftPos, rightPos, color, true);

		DrawLine3D(pos_, leftPos, 0xff0000);
		DrawLine3D(pos_, rightPos, 0xff0000);
	}
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

void Enemy::Search(void)
{
	VECTOR pPos = player_->GetPos();

	// 敵の向いている方向
	VECTOR eDir = VNorm(moveDir_);

	// 敵とプレイヤーとの間のベクトル
	VECTOR diff = VSub(pPos, pos_);

	// 敵から見たプレイヤーの方向
	VECTOR dirPlayerFromEnemy = VNorm(diff);


	// 内積を使ってベクトルの比較を行う
	// (+1.0)：２つのベクトルは同じ方向
	// (0.0)：２つのベクトルは直交
	// (-1.0)：２つのベクトルは逆方向
	float dot = VDot(eDir, dirPlayerFromEnemy);
	float angle = acosf(dot);

	const float viewRad = AsoUtility::Deg2Rad(VIEW_ANGLE);

	// 視野左右内に入っている
	isNoticeVision_ = (angle <= viewRad);
}

void Enemy::SearchField(void)
{
	VECTOR vec = VSub(player_->GetPos(), pos_);

	float diff = ((vec.x * vec.x) + (vec.z * vec.z));

	// 視野左右内に入っている
	isNoticeHearing_ = (diff <= (VIEW_RANGE * VIEW_RANGE));
}