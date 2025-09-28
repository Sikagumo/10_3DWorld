#pragma once
#include <string>
#include <DxLib.h>
class AnimationController;

class ActorBase
{
public:

	// アニメーションの再生速度
	static constexpr float SPEED_ANIM = 20.0f;

	// コンストラクタ
	ActorBase(void);

	// デストラクタ
	virtual ~ActorBase(void) = default;

	void Init(void);

	virtual void Update(void);

	virtual void Draw(void);

	virtual void Release(void);


	// 座標を取得
	const VECTOR& GetPos(void) const { return pos_; };

	const VECTOR& GetAngles(void) const { return angles_; }


protected:

	// アニメーション制御
	AnimationController* anim_;

	// アニメーション種別
	int animType_;

	// モデル情報
	int modelId_;

	VECTOR pos_;
	VECTOR angles_;
	VECTOR scales_;
	VECTOR localAngles_;

	// 移動方向
	VECTOR moveDir_;


	// リソースロード
	virtual void InitLoad(void) = 0;

	// 大きさ、回転、座標の初期化
	virtual void InitTransform(void) = 0;

	// 大きさ、回転、座標のモデル設定
	void InitTransformPost(void);

	// アニメーションの初期化
	virtual void InitAnim(void) = 0;

	// 初期化後の個別処理
	virtual void InitPost(void) = 0;

	// 移動処理
	virtual void Move(void);

	// 遅延回転処理
	void DelayRotate(void);
};
