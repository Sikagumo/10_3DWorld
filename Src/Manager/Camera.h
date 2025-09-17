#pragma once
#include <DxLib.h>

class Camera
{

public:

	// カメラの初期座標
	static constexpr VECTOR DERFAULT_POS = { 0.0f, 200.0f, -500.0f };

	// カメラの初期角度
	static constexpr VECTOR DERFAULT_ANGLES = {(30.0f * (DX_PI_F / 180.0f)), 0.0f, 0.0f};

	// カメラのクリップ範囲
	static constexpr float VIEW_NEAR = 1.0f;
	static constexpr float VIEW_FAR = 30000.0f;

	// カメラモード
	enum class MODE
	{
		NONE = -1,
		FIXED_POINT, // 定点カメラ
		FREE, // フリーモード
	};
	

	// コンストラクタ
	Camera(void);

	// デストラクタ
	~Camera(void) = default;

	// 初期化
	void Init(void);

	// 更新
	void Update(void);

	// 描画前のカメラ設定
	void SetBeforeDraw(void);

	// デバッグ用描画
	void DrawDebug(void);

	// 解放
	void Release(void);


	// 座標の取得
	const VECTOR& GetPos(void) const;

	// 角度の取得
	const VECTOR& GetAngles(void) const;

	// 描画前のカメラ設定
	void SetBeforeDraw(void);
	void SetBeforeDrawFixedPoint(void);
	void SetBeforeDrawFree(void);

	// カメラモードの変更
	void ChangeMode(MODE mode);


private:

	// カメラモード
	MODE mode_;

	// カメラの位置
	VECTOR pos_;

	// カメラの角度
	VECTOR angles_;

};
