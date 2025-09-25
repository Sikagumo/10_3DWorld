#pragma once
#include <DxLib.h>
class AnimationController;

class Player
{
public:

	// アニメーションの再生速度
	static constexpr float SPEED_ANIM = 20.0f;

	static constexpr VECTOR LOCAL_ANGLES = { 0.0f, 180.0f * (DX_PI_F / 180.0f), 0.0f };

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

	void Init(void);

	void Update(void);

	void Draw(void);

	void DrawDebug(void);

	void Release(void);


private:

	// アニメーション制御
	AnimationController* anim_;

	// アニメーション種別
	ANIM_TYPE animType_;

	// モデル情報
	int modelId_;

	VECTOR pos_;

	VECTOR angles_;

	VECTOR scales_;

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


	void InitAnim(void);

	void InitDice(void);

	void SyncDice(void);

	void SetMatrixPlayer(void);

	void Move(void);
};
