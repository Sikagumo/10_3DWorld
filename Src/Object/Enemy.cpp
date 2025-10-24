#include "Enemy.h"
#include <DxLib.h>
#include "./ActorBase.h"
#include "../Utility/AsoUtility.h"
#include "../Utility/MatrixUtility.h"
#include "./AnimationController.h"
#include "./Player.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "./Shot.h"

Enemy::Enemy(Player* player)
{
	player_ = player;

	modelId_ = -1;
	pos_ = {};
	angles_ = {};
}

void Enemy::InitLoad(void)
{
	modelId_ = ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER);
}

void Enemy::InitTransform(void)
{
	angles_ = { 0.0f, AsoUtility::Deg2Rad(45.0f), 0.0f };
	localAngles_ = LOCAL_ANGLES;

	scales_ = { SCALE, SCALE, SCALE };
	shotScale_ = SCALE_SHOT;

	hp_ = 3;

	// 角度から方向に変換する
	moveDir_ = { sinf(angles_.y), 0.0f, cosf(angles_.y) };


	MATRIX mat = MatrixUtility::Multiplication(LOCAL_ANGLES, angles_);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	pos_ = {200.0f , 0.0f, 150.0f};

	radiusShot_ = RADIUS_SHOT;

	speed_ = 3.0f;

	timeAttack_ = ATTACK_TIME;

	// モデルの位置設定
	MV1SetPosition(modelId_, pos_);

	SetFrame();

	int handle = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::MODEL_SHOT);
	shot_ = new Shot(handle, 0xff0000, *this);
}

void Enemy::InitAnim(void)
{
	int type = -1;
	anim_ = new AnimationController(modelId_);

	type = static_cast<int>(ANIM_TYPE::IDLE);
	anim_->AddExternal(type, SPEED_ANIM, (ResourceManager::PATH_MODEL + "Player/Idle.mv1").c_str());

	type = static_cast<int>(ANIM_TYPE::WALK);
	anim_->AddExternal(type, SPEED_ANIM, (ResourceManager::PATH_MODEL + "Player/Walk.mv1").c_str());

	type = static_cast<int>(ANIM_TYPE::THROW);
	anim_->AddExternal(type, SPEED_ANIM_ATK, (ResourceManager::PATH_MODEL + "Player/Throw.mv1").c_str());

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
	if (hp_ <= 0) { return; }

	ActorBase::Update();

	// 索敵
	Search();

	// 索敵
	SearchField();

	SetFrame();

	MV1SetPosition(modelId_, pos_);

	if (!isAttack_)
	{
		if (isNoticeHearing_)
		{
			VECTOR dir = player_->GetPos();
			dir.y = 0.0f;
			dir = VNorm(dir);
			float pDir = atan2(dir.x, dir.z);

			angles_.y = pDir;

			LookPlayer();

			// 移動アニメーション
			ChangeAnim(ANIM_TYPE::WALK);
		}
		else
		{
			// 待機アニメーション
			ChangeAnim(ANIM_TYPE::IDLE);
		}
	}

	// 角度から方向に変換する
	moveDir_ = { sinf(angles_.y), 0.0f, cosf(angles_.y) };
}

void Enemy::Draw(void)
{
	if (hp_ <= 0) { return; }

	COLOR_F defaultColor = { 1.0f, 1.0f,1.0f, 1.0f };
	COLOR_F damageColor = { 1.0f, 0.0f,0.0f, 1.0f };
	COLOR_F color = ((timeInv_ > 0.0f) ? damageColor : defaultColor);

	MV1SetDifColorScale(modelId_, color);
	ActorBase::Draw();

	if (SceneManager::GetInstance().GetIsDebugMode())
	{
		DrawSearchRange();

		DrawViewRange();
	}
}
void Enemy::DrawViewRange(void)
{
	const float offsetY = 25.0f;
	VECTOR viewPos = pos_;
	viewPos.y += offsetY;

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
	VECTOR forwardPos = VAdd(viewPos, VScale(forward, VIEW_RANGE));

	// 左の位置
	VECTOR leftPos = VAdd(viewPos, VScale(left, VIEW_RANGE));

	// 右の位置
	VECTOR rightPos = VAdd(viewPos, VScale(right, VIEW_RANGE));

	DrawTriangle3D(viewPos, leftPos, forwardPos, color, true);
	DrawTriangle3D(viewPos, forwardPos, rightPos, color, true);

	DrawLine3D(viewPos, forwardPos, 0x0000ff);
	DrawLine3D(viewPos, leftPos, 0x0000ff);
	DrawLine3D(viewPos, rightPos, 0x0000ff);

	VECTOR pPos = player_->GetPos();
	pPos.y += offsetY;

	VECTOR vec = VSub(pPos, viewPos);
	float distance = ((vec.x * vec.x) + (vec.z * vec.z));
	VECTOR _forward = VScale(forward, (VIEW_ANGLE * VIEW_ANGLE));

	DrawLine3D(viewPos, _forward, 0xffffff);
}

void Enemy::DrawSearchRange(void)
{
	// 描画位置調整
	const float offsetY = 25.0f;
	VECTOR viewPos = pos_;
	viewPos.y += offsetY;

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

		leftPos = VAdd(viewPos, VScale(left, VIEW_RANGE));
		rightPos = VAdd(viewPos, VScale(right, VIEW_RANGE));

		DrawTriangle3D(viewPos, leftPos, rightPos, color, true);

		DrawLine3D(viewPos, leftPos, 0xff0000);
		DrawLine3D(viewPos, rightPos, 0xff0000);
	}
}

void Enemy::DrawDebug(void)
{
#ifdef _DEBUG
	if (SceneManager::GetInstance().GetIsDebugMode())
	{
		DrawFormatString(0, 148, 0xffffff, "Enemy anim(%d)", animType_);
	}
#endif
}

void Enemy::Release(void)
{
	ActorBase::Release();
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

	// プレイヤーとの距離
	VECTOR vec = VSub(player_->GetPos(), pos_);
	float distance = ((vec.x * vec.x) + (vec.z * vec.z));

	// 視野左右内に入っている
	isNoticeVision_ = (distance <= VIEW_RANGE * VIEW_RANGE && (angle <= viewRad));
}

void Enemy::SearchField(void)
{
	// 360°範囲索敵処理
	VECTOR vec = VSub(player_->GetPos(), pos_);

	float diff = ((vec.x * vec.x) + (vec.z * vec.z));

	// 視野左右内に入っている
	isNoticeHearing_ = (diff <= (VIEW_RANGE * VIEW_RANGE));
}

void Enemy::LookPlayer(void)
{
	/*　プレイヤー向き追尾処理　*/
	VECTOR pPos = player_->GetPos();

	VECTOR diff = VSub(pPos, pos_);

	VECTOR dir = VNorm({ diff.x, 0.0f, diff.z });

	// Y軸の向きに反映
	float rotY = atan2f(dir.x, dir.z);
	angles_.y = rotY;

	// ベクトルの長さを正規化
	float len = sqrtf((dir.x * dir.x) + (dir.z * dir.z));
	float length = sqrtf(len);
	if (length > 1.0f)
	{
		float scale = (1.0f / length);
		dir.x *= scale;
		dir.z *= scale;
	}


	MATRIX mat = MatrixUtility::GetMatrixRotateXYZ(angles_);

	// 移動方向を取得
	moveDir_ = VTransform(dir, mat);
}

void Enemy::Move(void)
{
	// 移動処理
	if (!isNoticeHearing_ || isAttack_) { return; }

	pos_ = VAdd(pos_, VScale(moveDir_, speed_));

	SetFrame();
}

void Enemy::Attack(void)
{
	return;
	float delta = SceneManager::GetInstance().GetDeltaTime();


	if (isAttack_)
	{
		if (!shot_->GetIsShot()) { return; }

		timeAttack_ -= ((isNoticeVision_) ? delta : 0.0f);

		if (timeAttack_ <= 0.0f)
		{
			isAttack_ = true;
			ChangeAnim(ANIM_TYPE::THROW, false);
		}
	}
	else
	{
		SetFrame();

		if (anim_->IsEndPoint(0.35f) && shot_->GetIsShot())
		{
			shot_->SetShot(moveDir_, 1.5f, 7.5f);
		}

		if (anim_->IsEnd())
		{
			isAttack_ = false;
			timeAttack_ = ATTACK_TIME;
			//ChangeAnim(ANIM_TYPE::IDLE);
		}
	}
}
