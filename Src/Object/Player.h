#pragma once
#include <DxLib.h>
#include "./ActorBase.h"

class Player : public ActorBase
{
public:

	// アニメーションの再生速度
	static constexpr float SPEED_ANIM = 20.0f;

	static constexpr VECTOR LOCAL_ANGLES = { 0.0f, 180.0f * (DX_PI_F / 180.0f), 0.0f };

	// 回転速度
	static constexpr float ROT_SPEED = 0.1f;

	// アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE,
		WALK,
		THROW,
		MAX,
	};

	// コンストラクタ
	Player(void);

	// デストラクタ
	~Player(void) = default;

	void Update(void)override;

	void Draw(void)override;

	void DrawDebug(void);

	void Release(void)override;
	

private:

	// サイコロモデル情報
	struct Dice
	{
		int modelId;
		VECTOR pos;
		VECTOR angles;
		VECTOR scales;
		VECTOR localPos;
		VECTOR localAngles;
	};
	Dice dice_;


	void InitLoad(void)override;

	void InitTransform(void)override;

	void InitAnim(void)override;

	void InitPost(void)override;

	void InitDice(void);

	void SyncDice(void);

	void Move(void);

	// プレイヤーの遅延回転処理
	void DelayRotate(void);

	/// <summary>
	/// アニメーション遷移処理
	/// </summary>
	/// <param name="type"></param>
	/// <param name="isLoop">ループするか否か</param>
	void ChangeAnim(ANIM_TYPE type, bool isLoop = true);
};
