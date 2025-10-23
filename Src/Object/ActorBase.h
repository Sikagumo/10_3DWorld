#pragma once
#include <string>
#include <DxLib.h>
class AnimationController;

class ActorBase
{
public:
	
	// アニメーションの再生速度
	static constexpr float SPEED_ANIM = 20.0f;

	static constexpr float GRAVITY_MAX = 9.8f;

	static constexpr float GRAVITY_ACC = 0.2f;

	static constexpr float RADIUS = 35.0f;

	static constexpr float HEAD_POS = 100.0f;


	// コンストラクタ
	ActorBase(void);

	// デストラクタ
	virtual ~ActorBase(void) = default;

	void Init(void);

	virtual void Update(void);

	virtual void Draw(void);

	virtual void DrawDebug(void) {};

	virtual void Release(void);


	// 座標を取得
	const VECTOR& GetPos(void) const { return pos_; };

	const VECTOR& GetAngles(void) const { return angles_; }

	bool GetIsGround(void) { return isGround_; }

	const VECTOR GetDir(void) { return moveDir_; };

	float GetSpeed(void) { return speed_; };


	void CollisionStage(const VECTOR& pos);

	void SetIsGround(bool flag) { isGround_ = flag; };

	VECTOR GetPosHead(void) { return { pos_.x, pos_.y + HEAD_POS, pos_.z }; };

	float GetRadius(void) { return radius_; };

	void SetPos(const VECTOR& pos) { pos_ = pos; };

protected:

	// アニメーション制御
	AnimationController* anim_;

	// アニメーション種別
	int animType_;

	// モデル情報
	int modelId_;

	bool isGround_;

	float jumpPow_;

	VECTOR pos_;
	VECTOR angles_;
	VECTOR scales_;
	VECTOR localAngles_;

	// 移動方向
	VECTOR moveDir_;

	// 移動速度
	float speed_;

	// 半径
	float radius_;


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

	void Gravity(void);

	// 遅延回転処理
	void DelayRotate(void);
};
