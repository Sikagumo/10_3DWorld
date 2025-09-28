#pragma once
#include"./ActorBase.h"

class Enemy : public ActorBase
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
	Enemy(void);

	// デストラクタ
	~Enemy(void) = default;

	void Update(void)override;

	void Draw(void)override;

	void DrawDebug(void);

	void Release(void)override;


private:

	void InitLoad(void)override;

	void InitTransform(void)override;

	void InitAnim(void)override;

	void InitPost(void)override;

	/// <summary>
	/// アニメーション遷移処理
	/// </summary>
	/// <param name="type"></param>
	/// <param name="isLoop">ループするか否か</param>
	void ChangeAnim(ANIM_TYPE type, bool isLoop = true);
};
