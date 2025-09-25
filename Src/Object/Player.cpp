#include "Player.h"
#include <DxLib.h>
#include <string>
#include "../Utility/AsoUtility.h"
#include "../Utility/MatrixUtility.h"
#include "./AnimationController.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/Resource.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"


Player::Player(void)
{
	modelId_ = -1;
	pos_ = {};
	angles_ = {};
}

void Player::Init(void)
{
	modelId_ = MV1LoadModel((ResourceManager::PATH_MODEL + "Player/Player.mv1").c_str());
	//modelId_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::MODEL_PLAYER);

	pos_ = {};
	SetMatrixPlayer();

	InitAnim();
	InitDice();
}
void Player::InitAnim(void)
{
	int type = static_cast<int>(ANIM_TYPE::IDLE);
	anim_ = new AnimationController(modelId_);

	anim_->Add(static_cast<int>(ANIM_TYPE::IDLE),
		SPEED_ANIM, (ResourceManager::PATH_MODEL + "Player/Idle.mv1").c_str());

	anim_->Add(static_cast<int>(ANIM_TYPE::WALK),
		SPEED_ANIM, (ResourceManager::PATH_MODEL + "Player/Walk.mv1").c_str());

	anim_->Add(static_cast<int>(ANIM_TYPE::WALK),
		SPEED_ANIM, (ResourceManager::PATH_MODEL + "Player/Throw.mv1").c_str());

	animType_ = ANIM_TYPE::IDLE;
	anim_->Play(static_cast<int>(animType_));
}
void Player::InitDice(void)
{
	dice_.modelId = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::MODEL_DICE);
	float scale = 0.1f;
	dice_.scales = { scale, scale, scale };
	dice_.localPos = { 200.0f, 100.0f, 0.0f };
	dice_.pos = VAdd(pos_, dice_.localPos);

	MV1SetScale(dice_.modelId, dice_.scales);
	MV1SetPosition(dice_.modelId, dice_.pos);

	SyncDice();
}


void Player::Update(void)
{
	
	MV1SetPosition(modelId_, pos_);

	SetMatrixPlayer();

	Move();

	anim_->Update();

	SyncDice();
}
void Player::SyncDice(void)
{
	// ローカル回転行列
	dice_.angles.z += (2.0f * (DX_PI_F / 180.0f));

	// 行列の合成(子→親の順で合成することで親→子の順に適応される)
	MATRIX selfMat = MatrixUtility::Multiplication(dice_.localAngles, dice_.angles);

	// 親(プレイヤー)の回転行列
	MATRIX parentMat = MatrixUtility::GetMatrixRotateXYZ(angles_);

	MATRIX mat = MatrixUtility::Multiplication(selfMat, parentMat);

	// 行列を使用してモデルの角度を設定
	MV1SetRotationMatrix(dice_.modelId, mat);
	
	// サイコロのローカル座標を親の回転行列で回転
	VECTOR localRotPos = VTransform(dice_.localPos, parentMat);

	// サイコロのワールド座標
	dice_.pos = VAdd(localRotPos, pos_);

	// サイコロの位置の設定
	MV1SetPosition(dice_.modelId, dice_.pos);
}

void Player::SetMatrixPlayer(void)
{
	MATRIX mat = MGetIdent();
	mat = MatrixUtility::Multiplication(LOCAL_ANGLES, angles_);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	// モデルの位置設定
	MV1SetPosition(modelId_, pos_);
}

void Player::Move(void)
{
	InputManager& input = InputManager::GetInstance();

	float speed = 3.0f;
	VECTOR dir = {};
	VECTOR cameraAngle = SceneManager::GetInstance().GetCamera().GetAngles();

	if (GetJoypadNum() == 0)
	{
		if (input.KeyIsNew(KEY_INPUT_W)) { dir = AsoUtility::DIR_F; }
		if (input.KeyIsNew(KEY_INPUT_S)) { dir = AsoUtility::DIR_B; }
		if (input.KeyIsNew(KEY_INPUT_A)) { dir = AsoUtility::DIR_L; }
		if (input.KeyIsNew(KEY_INPUT_D)) { dir = AsoUtility::DIR_R; }
	}
	else
	{
		dir = input.GetAlgKeyDirXZ(1, InputManager::JOYPAD_ALGKEY::LEFT);
	}

	

	if (!AsoUtility::EqualsVZero(dir))
	{
		MATRIX mat = MGetIdent();

		//mat = MMult(mat, MGetRotX(angles_.x));
		mat = MMult(mat, MGetRotY(angles_.y));
		//mat = MMult(mat, MGetRotZ(angles_.z));

		VECTOR moveDir = VTransform(dir, mat);

		pos_ = VAdd(pos_, VScale(moveDir, speed));
	}


	//float rot = 2.5f;
	//if (input.KeyIsNew(KEY_INPUT_LEFT)) { angles_.y -= (rot * (DX_PI_F / 180.0f)); }
	//if (input.KeyIsNew(KEY_INPUT_RIGHT)) { angles_.y += (rot * (DX_PI_F / 180.0f)); }

	if (input.KeyIsNewAll() && animType_ != ANIM_TYPE::WALK)
	{
		animType_ = ANIM_TYPE::WALK;
		anim_->Play(static_cast<int>(animType_));
	}
	if (!input.KeyIsNewAll() && animType_ != ANIM_TYPE::IDLE)
	{
		animType_ = ANIM_TYPE::IDLE;
		anim_->Play(static_cast<int>(animType_));
	}
}

void Player::Draw(void)
{
	MV1DrawModel(modelId_);

	MV1DrawModel(dice_.modelId);
}

void Player::DrawDebug(void)
{
#ifdef _DEBUG
	DrawFormatString(
		0, 64, 0xffffff,
		"キャラ角度　 ：(% .1f°, % .1f°, % .1f°)",
		AsoUtility::Rad2DegF(angles_.x),
		AsoUtility::Rad2DegF(angles_.y),
		AsoUtility::Rad2DegF(angles_.z)
	);

	DrawFormatString(
		0, 80, 0xffffff,
		"ダイス角度　 ：(% .1f°, % .1f°, % .1f°)",
		AsoUtility::Rad2DegF(dice_.angles.x),
		AsoUtility::Rad2DegF(dice_.angles.y),
		AsoUtility::Rad2DegF(dice_.angles.z)
	);


#endif
}

void Player::Release(void)
{
	anim_->Release();
	delete anim_;
}
