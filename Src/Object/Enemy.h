#pragma once
#include"./ActorBase.h"
class Player;

class Enemy : public ActorBase
{
public:

	// アニメーションの再生速度
	static constexpr float SPEED_ANIM = 20.0f;

	static constexpr VECTOR LOCAL_ANGLES = { 0.0f, 180.0f * (DX_PI_F / 180.0f), 0.0f };

	// 視野の広さ
	static constexpr float VIEW_RANGE = 600.0f;

	// 視野角
	static constexpr float VIEW_ANGLE = 30.0f;


	// アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE,
		WALK,
		THROW,
		MAX,
	};

	// コンストラクタ
	Enemy(Player* player);

	// デストラクタ
	~Enemy(void)override = default;

	void Update(void)override;

	void Draw(void)override;

	void DrawDebug(void);

	void Release(void)override;


protected:

	void InitLoad(void)override;

	void InitTransform(void)override;

	void InitAnim(void)override;

	void InitPost(void)override;

	void DrawViewRange(void);

	void DrawSearchRange(void);


	/// <summary>
	/// アニメーション遷移処理
	/// </summary>
	/// <param name="type"></param>
	/// <param name="isLoop">ループするか否か</param>
	void ChangeAnim(ANIM_TYPE type, bool isLoop = true);

private:

	// プレイヤー
	Player* player_;

	// 検知フラグ
	bool isNoticeVision_;

	// 検知フラグ
	bool isNoticeHearing_;

	void Search(void);

	void SearchField(void);
};
