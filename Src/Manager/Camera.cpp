#include "Camera.h"
#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"


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
	}

	// カメラの設定(位置と角度による制御)
	SetCameraPositionAndAngle(
		pos_,
		angles_.x,
		angles_.y,
		angles_.z
	);

}

void Camera::SetBeforeDrawFixedPoint(void)
{
	// 何もしない
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
}

void Camera::MoveXYZDirection(void)
{
	auto& input = InputManager::GetInstance();
	
	// 矢印キーでカメラの角度を変える
	float rotPow = 1.0f * DX_PI_F / 180.0f;
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
		dir = input.GetAlgKeyDirXZ(0, InputManager::JOYPAD_ALGKEY::RIGHT);
	}

	
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
		mat = MMult(mat, MGetRotX(angles_.x));
		mat = MMult(mat, MGetRotY(angles_.y));
		//mat = MMult(mat, MGetRotZ(angles_.z));
		// 
		// 回転行列を使用して、ベクトルを回転させる
		VECTOR moveDir = VTransform(dir, mat);

		// 方向×スピードで移動量を作って、座標に足して移動
		pos_ = VAdd(pos_, VScale(moveDir, movePow));
	}
}


void Camera::DrawDebug(void)
{
#ifdef _DEBUG
	DrawFormatString(
		0, 10, 0xffffff,
		"カメラ座標　 ：(% .1f, % .1f, % .1f)",
		pos_.x, pos_.y, pos_.z
	);
	DrawFormatString(
		0, 30, 0xffffff,
		"カメラ角度　 ：(% .1f, % .1f, % .1f)",
		AsoUtility::Rad2DegF(angles_.x),
		AsoUtility::Rad2DegF(angles_.y),
		AsoUtility::Rad2DegF(angles_.z)
	);

#endif
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
	}
}



void Camera::Release(void)
{
}