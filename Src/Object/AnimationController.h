 #pragma once
#include <map>
#include <string>

class AnimationController
{
public:

	// アニメーションデータ
	struct Animation
	{
		int model		= -1;
		int attachNo	= -1;
		int animIndex	= 0;
		float speed		= 0.0f; // アニメーション速度
		float totalTime = 0.0f; // アニメーション最大再生時間
		float step		= 0.0f; // アニメーション現在再生時間
	};


	// コンストラクタ
	AnimationController(int modelId);

	// デストラクタ
	~AnimationController(void) = default;

	// 同じFBX内のアニメーションを準備
	void AddInFbx(int type, float speed, int animIndex);

	// 別のFBXからアニメーションを準備
	void Add(int type, float speed, const std::string path);


	/// <summary>
	/// アニメーション再生
	/// </summary>
	/// <param name="type">アニメーションの種類</param>
	/// <param name="isLoop">ループするか否か(default=true)</param>
	void Play(int type, bool isLoop = true);


	void Update(void);

	void DrawDebug(void);

	void Release(void);

	bool IsEnd(void) const;


private:

	// アニメーションするモデルのハンドルID
	int modelId_;

	// 種類別のアニメーションデータ
	std::map<int, Animation> animations_;

	// 再生中のアニメーション
	int playType_;

	// 再生中のアニメーションデータ
	Animation playAnim_;

	// ループするか否かの判定
	bool isLoop_;

	// アニメーション追加の共通処理
	void Add(int type, float speed, Animation& animIndex);
};