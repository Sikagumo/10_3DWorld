#include "Camera.h"
#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Object/Player.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"


Camera::Camera(void)
{
	// DxLibの初期設定では、
	// カメラの位置が x = 320.0f, y = 240.0f, z = (画面のサイズによって変化)、
	// 注視点の位置は x = 320.0f, y = 240.0f, z = 1.0f
	// カメラの上方向は x = 0.0f, y = 1.0f, z = 0.0f
	// 右上位置からZ軸のプラス方向を見るようなカメラ
	mode_ = MODE::NONE;
}


void Camera::Init(void)
{
	// カメラの初期位置
	pos_ = DERFAULT_POS;

	// カメラの初期角度
	angles_ = DERFAULT_ANGLES;
}

void Camera::Update(void)
{
	
}

void Camera::SetBeforeDraw(void)
{
	// クリップ距離を設定する(SetDrawScreenでリセットされる)
	SetCameraNearFar(VIEW_NEAR, VIEW_FAR);

	switch (mode_)
	{
		case Camera::MODE::FIXED_POINT:
			SetBeforeDrawFixedPoint();
		break;

		case Camera::MODE::FREE:
			//SetBeforeDrawFree();
			MoveXYZDirection();
		break;

		case Camera::MODE::FOLLOW:
			SetBeforeDrawFollow();
			break;

	}
}

void Camera::SetBeforeDrawFixedPoint(void)
{
	// 何もしない
	// カメラの設定(位置と角度による制御)
	SetCameraPositionAndAngle(
		pos_,
		angles_.x,
		angles_.y,
		angles_.z
	);
}

void Camera::SetBeforeDrawFree(void)
{
	auto& input = InputManager::GetInstance();

	// 矢印キーでカメラの角度を変える
	float rotPow = (1.0f * (DX_PI_F / 180.0f));

	if (input.KeyIsNew(KEY_INPUT_DOWN)) { angles_.x += rotPow; }
	if (input.KeyIsNew(KEY_INPUT_UP))   { angles_.x -= rotPow; }
	if (input.KeyIsNew(KEY_INPUT_RIGHT)){ angles_.y += rotPow; }
	if (input.KeyIsNew(KEY_INPUT_LEFT)) { angles_.y -= rotPow; }


	// WASDでカメラを移動させる
	float movePow = 3.0f;

	if (input.KeyIsNew(KEY_INPUT_W)) { pos_.z += movePow; }
	if (input.KeyIsNew(KEY_INPUT_A)) { pos_.x -= movePow; }
	if (input.KeyIsNew(KEY_INPUT_S)) { pos_.z -= movePow; }
	if (input.KeyIsNew(KEY_INPUT_D)) { pos_.x += movePow; }
	if (input.KeyIsNew(KEY_INPUT_Q)) { pos_.y += movePow; }
	if (input.KeyIsNew(KEY_INPUT_E)) { pos_.y -= movePow; }

	// カメラの設定(位置と角度による制御)
	SetCameraPositionAndAngle(
		pos_,
		angles_.x,
		angles_.y,
		angles_.z
	);

}

void Camera::SetBeforeDrawFollow(void)
{
	auto& input = InputManager::GetInstance();

	// 方向回転によるXYZの移動
	MoveXYZDirection();

	// カメラの移動
	// カメラの回転行列を作成
	MATRIX mat = MGetIdent();
	mat = MMult(mat, MGetRotX(angles_.x));
	mat = MMult(mat, MGetRotY(angles_.y));
	//mat = MMult(mat, MGetRotZ(angles_.z));
	
	// 注視点の移動
	VECTOR followPos = follow_->GetPos();
	VECTOR targetLocalRotPos = VTransform(FOLLOW_TARGET_LOCAL_POS, mat);
	targetPos_ = VAdd(followPos, targetLocalRotPos);

	// カメラの移動
	// 相対座標を回転させて、回転後の相対座標を取得する
	VECTOR cameraLocalRotPos = VTransform(FOLLOW_CAMERA_LOCAL_POS, mat);

	// 相対座標からワールド座標に直して、カメラ座標とする
	pos_ = VAdd(followPos, cameraLocalRotPos);
	
	// カメラの上方向を計算
	VECTOR up = VTransform(AsoUtility::DIR_U, mat);

	// カメラの設定(位置と注視点による制御)
	SetCameraPositionAndTargetAndUpVec(
		pos_,
		targetPos_,
		up
	);
}

void Camera::MoveXYZDirection(void)
{
	auto& input = InputManager::GetInstance();
	
	// 矢印キーでカメラの角度を変える
	float rotPow = (3.0f * (DX_PI_F / 180.0f));
	VECTOR dir = {};

	if (GetJoypadNum() == 0)
	{
		if (input.KeyIsNew(KEY_INPUT_DOWN)) { angles_.x += rotPow; }
		if (input.KeyIsNew(KEY_INPUT_UP)) { angles_.x -= rotPow; }
		if (input.KeyIsNew(KEY_INPUT_RIGHT)) { angles_.y += rotPow; }
		if (input.KeyIsNew(KEY_INPUT_LEFT)) { angles_.y -= rotPow; }
	}
	else
	{
		dir = input.GetAlgKeyDirXZ(1, InputManager::JOYPAD_ALGKEY::RIGHT);

		angles_.x += (dir.z * rotPow);
		angles_.y += (dir.x * rotPow);
	}

	
	// X軸の回転の絶対値が一定のとき。制限する
	float angX = ((angles_.x > 0.0f) ? angles_.x : -angles_.x);
	if (angX > ROT_LIMIT_X)
	{
		// 絶対値を戻して値を制限する
		angX = ROT_LIMIT_X;
		angles_.x = ((angles_.x > 0.0f) ? angX : -angX);
	}


	// プレイヤーの後方を追従
	//angles_.y = follow_->GetAngles().y;
	
	// WASDでカメラを移動させる
	const float movePow = 3.0f;

	if (GetJoypadNum() == 0)
	{
		//if (input.KeyIsNew(KEY_INPUT_W)) { dir = { 0.0f, 0.0f, 1.0f }; }
		//if (input.KeyIsNew(KEY_INPUT_A)) { dir = { -1.0f, 0.0f, 0.0f }; }
		//if (input.KeyIsNew(KEY_INPUT_S)) { dir = { 0.0f, 0.0f, -1.0f }; }
		//if (input.KeyIsNew(KEY_INPUT_D)) { dir = { 1.0f, 0.0f, 0.0f }; }
	}

	if (!AsoUtility::EqualsVZero(dir))
	{
		// XYZの回転行列
		// XZ平面移動にする場合は、XZの回転を考慮しないようにする
		MATRIX mat = MGetIdent();
		//mat = MMult(mat, MGetRotX(angles_.x));
		mat = MMult(mat, MGetRotY(angles_.y));
		//mat = MMult(mat, MGetRotZ(angles_.z));
		
		// 回転行列を使用して、ベクトルを回転させる
		VECTOR moveDir = VTransform(dir, mat);

		// 方向×スピードで移動量を作って、座標に足して移動
		//pos_ = VAdd(pos_, VScale(moveDir, movePow));
	}
}


void Camera::DrawDebug(void)
{
	if (!SceneManager::GetInstance().GetIsDebugMode())return;

#ifdef _DEBUG
	DrawFormatString(
		0, 10, 0xffffff,
		"カメラ座標　 ：(% .1f, % .1f, % .1f)",
		pos_.x, pos_.y, pos_.z
	);
	DrawFormatString(
		0, 30, 0xffffff,
		"カメラ角度　 ：(% .1f°, % .1f°, % .1f°)",
		AsoUtility::Rad2Deg(angles_.x),
		AsoUtility::Rad2Deg(angles_.y),
		AsoUtility::Rad2Deg(angles_.z)
	);
	DrawFormatString(
		0, 46, 0xffffff,
		"追従位置：(%.1f, %.1f, %.1f)",
		targetPos_.x,
		targetPos_.y,
		targetPos_.z);
#endif

	if (mode_ == MODE::FOLLOW)
	{
		// 注視点
		DrawSphere3D(targetPos_, 20.0f, 10, 0xff0000, 0xff0000, true);
	}
}

void Camera::ChangeMode(MODE mode)
{
	// カメラモードの変更
	mode_ = mode;

	// 変更時の初期化処理
	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		break;

	case Camera::MODE::FREE:
		break;

	case Camera::MODE::FOLLOW:
		break;
	}
}



void Camera::Release(void)
{
}