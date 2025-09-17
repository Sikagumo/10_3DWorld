#include "Camera.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"


Camera::Camera(void)
{
	// DxLibの初期設定では、
	// カメラの位置が x = 320.0f, y = 240.0f, z = (画面のサイズによって変化)、
	// 注視点の位置は x = 320.0f, y = 240.0f, z = 1.0f
	// カメラの上方向は x = 0.0f, y = 1.0f, z = 0.0f
	// 右上位置からZ軸のプラス方向を見るようなカメラ
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
			SetBeforeDrawFree();
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
	float rotPow = 1.0f * DX_PI_F / 180.0f;

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

void Camera::DrawDebug(void)
{
#ifdef _DEBUG
#endif
}

void Camera::Release(void)
{
}

const VECTOR& Camera::GetPos(void) const
{
	return pos_;
}

const VECTOR& Camera::GetAngles(void) const
{
	return angles_;
}
