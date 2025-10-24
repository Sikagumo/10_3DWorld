#pragma once
#include <DxLib.h>
#include "./ActorBase.h"

class Player : public ActorBase
{
public:

	// アニメーションの再生速度
	static constexpr float SPEED_ANIM = 20.0f;

	// ジャンプアニメーションの再生速度
	static constexpr float SPEED_ANIM_JUMP = 40.0f;

	
	static constexpr VECTOR LOCAL_ANGLES = { 0.0f, 180.0f * (DX_PI_F / 180.0f), 0.0f };

	static constexpr VECTOR START_POS = { 0.0f, 0.0f, -250.0f };

	// 回転速度
	static constexpr float ROT_SPEED = (5.0f * (DX_PI_F / 180.0f));

	// 移動速度
	static constexpr float MOVE_SPEED = 5.0f;

	// ダッシュ倍率
	static constexpr float DASH_MULT = 1.5f;


	// 胴体のスケール
	static constexpr float SCALE = 1.0f;

	// 弾のスケール
	static constexpr float SCALE_SHOT = 0.1f;

	// 弾の半径
	static constexpr float RADIUS_SHOT = 30.0f;


	// アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE, // 待機
		WALK, // 歩行
		THROW, // 投げる
		RUN, // ダッシュ
		JUMP,// ジャンプ

		MAX,
	};

	// コンストラクタ
	Player(void);

	// デストラクタ
	~Player(void) = default;

	void Update(void)override;

	void Draw(void)override;

	void DrawDebug(void)override;

	void Release(void)override;
	

protected:

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

	VECTOR autoAngles_;

	bool isDash_;

	float waitTime_;


	void InitLoad(void)override;

	void InitTransform(void)override;

	void InitAnim(void)override;

	void InitPost(void)override;

	void InitDice(void);

	void SyncDice(void);

	void Move(void)override;

	void Jump(void);
	
	void Attack(void)override;

	// プレイヤーの遅延回転処理
	void DelayRotate(void);

	void DrawDicePractice(void);

	/// <summary>
	/// アニメーション遷移処理
	/// </summary>
	/// <param name="type"></param>
	/// <param name="isLoop">ループするか否か</param>
	void ChangeAnim(ANIM_TYPE type, bool isLoop = true);

private:

};
