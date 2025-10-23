#include "Player.h"
#include <DxLib.h>
#include <string>
#include "../Utility/AsoUtility.h"
#include "../Utility/MatrixUtility.h"
#include "./AnimationController.h"
#include "../Manager/Camera.h"
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
	autoAngles_ = {};
	isDash_ = false;
}

void Player::InitLoad(void)
{
	ResourceManager& res = ResourceManager::GetInstance();

	modelId_ = res.LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER);

	dice_.modelId = res.LoadHandleId(ResourceManager::SRC::MODEL_DICE);
}

void Player::InitTransform(void)
{
	angles_ = {};
	localAngles_ = LOCAL_ANGLES;

	scales_ = { 1.0f, 1.0f, 1.0f };

	// 角度から方向に変換する
	moveDir_ = { sinf(angles_.y), 0.0f, cosf(angles_.y) };


	MATRIX mat = MatrixUtility::Multiplication(LOCAL_ANGLES, angles_);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	pos_ = START_POS;

	speed_ = MOVE_SPEED;
	isDash_ = false;

	// モデルの位置設定
	MV1SetPosition(modelId_, pos_);
}
void Player::InitAnim(void)
{
	int type = static_cast<int>(ANIM_TYPE::IDLE);
	anim_ = new AnimationController(modelId_);

	anim_->AddExternal(static_cast<int>(ANIM_TYPE::IDLE),
		               SPEED_ANIM, (ResourceManager::PATH_MODEL + "Player/Idle.mv1").c_str());

	anim_->AddExternal(static_cast<int>(ANIM_TYPE::WALK),
		               SPEED_ANIM, (ResourceManager::PATH_MODEL + "Player/Walk.mv1").c_str());

	anim_->AddExternal(static_cast<int>(ANIM_TYPE::THROW),
		               SPEED_ANIM, (ResourceManager::PATH_MODEL + "Player/Throw.mv1").c_str());

	anim_->AddExternal(static_cast<int>(ANIM_TYPE::RUN),
		               SPEED_ANIM, (ResourceManager::PATH_MODEL + "Player/Run.mv1").c_str());

	anim_->AddExternal(static_cast<int>(ANIM_TYPE::JUMP),
		               SPEED_ANIM_JUMP, (ResourceManager::PATH_MODEL + "Player/Jump.mv1").c_str());

	// 歩行アニメーション有効
	ChangeAnim(ANIM_TYPE::IDLE);
}
void Player::InitPost(void)
{
	// ダイス初期化
	InitDice();

	autoAngles_ = {};
}
void Player::InitDice(void)
{
	const float scale = 0.1f;
	dice_.scales = { scale, scale, scale };
	//dice.localPos = { 200.0f, 100.0f, 0.0f };
	dice_.localPos = { 0.0f, 100.0f, 200.0f };
	dice_.pos = VAdd(pos_, dice_.localPos);

	MV1SetScale(dice_.modelId, dice_.scales);
	MV1SetPosition(dice_.modelId, dice_.pos);


	SyncDice();
}


void Player::Update(void)
{
	ActorBase::Update();

	// ジャンプ
	Jump();

	// モデルの位置設定
	MV1SetPosition(modelId_, pos_);

	autoAngles_.y += 0.01f;
	SyncDice();
}
void Player::SyncDice(void)
{
	// ローカル回転行列
	dice_.angles.z += (2.0f * (DX_PI_F / 180.0f));

	// 行列の合成(子→親の順で合成することで親→子の順に適応される)
	MATRIX selfMat = MatrixUtility::Multiplication(dice_.localAngles, dice_.angles);

	// 親(プレイヤー)の回転行列
	//MATRIX parentMat = MatrixUtility::GetMatrixRotateXYZ(angles_);
	// 自動回転
	MATRIX parentMat = MatrixUtility::GetMatrixRotateXYZ(autoAngles_);

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

void Player::Move(void)
{
	InputManager& input = InputManager::GetInstance();

	VECTOR dir = {};

	if (GetJoypadNum() == 0)
	{
		if (input.KeyIsNew(KEY_INPUT_W)) { dir = VAdd(dir, AsoUtility::DIR_F); }
		if (input.KeyIsNew(KEY_INPUT_S)) { dir = VAdd(dir, AsoUtility::DIR_B); }
		if (input.KeyIsNew(KEY_INPUT_A)) { dir = VAdd(dir, AsoUtility::DIR_L); }
		if (input.KeyIsNew(KEY_INPUT_D)) { dir = VAdd(dir, AsoUtility::DIR_R); }
	}
	else
	{
		dir = input.GetAlgKeyDirXZ(1, InputManager::JOYPAD_ALGKEY::LEFT);
	}
	
	if (!AsoUtility::EqualsVZero(dir))
	{
		// 移動ベクトルを正規化
		float len = sqrtf((dir.x * dir.x) + (dir.z * dir.z));
		float length = sqrtf(len);

		if (length > 1.0f)
		{
			float scale = (1.0f / length);

			// 移動ベクトルの長さを正規化
			dir.x *= scale;
			dir.z *= scale;
		}

		// カメラの方向で進行
		VECTOR cameraAngle = SceneManager::GetInstance().GetCamera().GetAngles();
		cameraAngle.x = 0.0f;
		MATRIX cameraMat = MatrixUtility::GetMatrixRotateXYZ(cameraAngle);

		// 移動方向を取得
		moveDir_ = VTransform(dir, cameraMat);

#pragma region プレイヤーの向きで進行する処理
		/*  
		MATRIX mat = MGetIdent();
		//mat = MMult(mat, MGetRotX(angles_.x));
		mat = MMult(mat, MGetRotY(angles_.y));
		//mat = MMult(mat, MGetRotZ(angles_.z));

		// 回転行列を使用して、ベクトルを回転させる
		moveDir_ = VTransform(dir, mat);
		
		// 移動方向から角度に変換する
		//angles_.y = atan2f(moveDir_.x, moveDir_.z);
		*/
#pragma endregion

		float speed = ((isDash_) ? (speed_ * DASH_MULT) : speed_);

		// 方向×スピードで移動量を作って、座標に足して移動
		pos_ = VAdd(pos_, VScale(moveDir_, speed));


		if (isGround_)
		{
			if (input.KeyIsNew(KEY_INPUT_LSHIFT) ||
				input.PadIsBtnNew(1, InputManager::PAD_BTN::DOWN))
			{
				isDash_ = true;
				ChangeAnim(ANIM_TYPE::RUN);
			}
			else
			{
				isDash_ = false;
				// 歩行アニメーション有効
				ChangeAnim(ANIM_TYPE::WALK);
			}
		}
	}
	else if (isGround_)
	{
		// 待機アニメーション有効
		ChangeAnim(ANIM_TYPE::IDLE);
	}


	//float rot = 2.5f;
	//if (input.KeyIsNew(KEY_INPUT_LEFT)) { angles_.y -= (rot * (DX_PI_F / 180.0f)); }
	//if (input.KeyIsNew(KEY_INPUT_RIGHT)) { angles_.y += (rot * (DX_PI_F / 180.0f)); }
}

void Player::Jump(void)
{
	InputManager& input = InputManager::GetInstance();
	const float jumpPow = 10.0f;

	if (!isGround_) { return; }

	if (input.KeyIsTrgDown(KEY_INPUT_SPACE) ||
		input.PadIsBtnTrgDown(1, InputManager::PAD_BTN::RIGHT))
	{
		// ジャンプアニメーション化
		ChangeAnim(ANIM_TYPE::JUMP, false);

		// ジャンプ処理
		jumpPow_ = jumpPow;
		isGround_ = false;
	}

	// 着地時、待機アニメーションに遷移
	if (anim_->GetPlayType() == static_cast<int>(ANIM_TYPE::JUMP)&&
		isGround_)
	{
		ChangeAnim(ANIM_TYPE::WALK);
	}
}


void Player::Draw(void)
{
	ActorBase::Draw();

	MV1DrawModel(dice_.modelId);

	if (SceneManager::GetInstance().GetIsDebugMode())
	{
		DrawDicePractice();
	}
}

void Player::DrawDebug(void)
{
#ifdef _DEBUG
	if (!SceneManager::GetInstance().GetIsDebugMode()) return;

	DrawFormatString(385, 80, 0xffffff, "player anim(%d)", animType_);

	DrawFormatString(
		0, 64, 0xffffff,
		"角度　 ：(% .1f°, % .1f°, % .1f°)",
		AsoUtility::Rad2Deg(angles_.x),
		AsoUtility::Rad2Deg(angles_.y),
		AsoUtility::Rad2Deg(angles_.z)
	);

	DrawFormatString(
		0, 80, 0xffffff,
		"ダイス角度　 ：(% .1f°, % .1f°, % .1f°)",
		AsoUtility::Rad2Deg(dice_.angles.x),
		AsoUtility::Rad2Deg(dice_.angles.y),
		AsoUtility::Rad2Deg(dice_.angles.z)
	);


#endif
}

void Player::Release(void)
{
	anim_->Release();
	delete anim_;
}


void Player::DelayRotate(void)
{
	// 移動方向から角度に変換する
	float goal = atan2f(moveDir_.x, moveDir_.z);

	// 常に最短経路で補間
	angles_.y = AsoUtility::LerpAngle(angles_.y, goal, ROT_SPEED);

	// 等間隔補間
	// 1frame. a = Lerp(0, 10, 0.1late) = 1
	// 2frame. a = Lerp(0, 10, 0.2late) = 2
	// 3frame. a = Lerp(0, 10, 0.3late) = 3

	// ゴールに近づくほどゆっくり補間
	// a = 0
	// 1frame. a = Lerp(a, 10, 0.1) = 1
	// 2frame. a = Lerp(a, 10, 0.1) = (10 - 1 = 9)
	// 3frame. a = Lerp(a, 10, 0.1) = 3
}

void Player::DrawDicePractice(void)
{
	// プレイヤー座標
	VECTOR playerPos = pos_;
	playerPos.y = 10.0f;

	// サイコロ座標
	VECTOR dicePos = dice_.pos;
	dicePos.y = 10.0f;

	// 角度と方向の演習
	// サイコロの下線描画
	DrawLine3D(dice_.pos, dicePos, 0x00ff00);

	// プレイヤーとサイコロ間の線描画
	DrawLine3D(playerPos, dicePos, 0xffffff);

	// プレイヤーの前方方向線描画
	DrawLine3D(playerPos, VAdd(playerPos, VScale(moveDir_, 200.0f)), 0xcc44cc);

#ifdef _DEBUG
	DrawFormatString(0, 96, 0xffffff, "■サイコロ");
	DrawFormatString(0, 96 + 16, 0xffffff, "ワールド座標(%.1f°,%.1f°,%.1f°)"
		, AsoUtility::Rad2Deg(autoAngles_.x)
		, AsoUtility::Rad2Deg(autoAngles_.y)
		, AsoUtility::Rad2Deg(autoAngles_.z));

	float diff = AsoUtility::RadIn2PI(angles_.y) - AsoUtility::RadIn2PI(autoAngles_.y);

	DrawFormatString(0, 96 + 32, 0xffffff, "ワールド座標(%.1f°,%.1f°,%.1f°)"
		, 0.0f
		, AsoUtility::Rad2Deg(diff)
		, 0.0f);
#endif
}

void Player::ChangeAnim(ANIM_TYPE type, bool isLoop)
{
	// アニメーション遷移処理
	animType_ = static_cast<int>(type);
	anim_->Play(animType_, isLoop);
}
