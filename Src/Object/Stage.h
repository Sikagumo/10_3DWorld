#pragma once
#include <DxLib.h>

class Stage
{
public:

	static constexpr VECTOR LOCAL_POS = { 0.0f, 62.5f, 0.0f };

	Stage(void);

	~Stage(void) = default;

	void Init(void);

	void Update(void);

	void Draw(void);

	void Release(void);


private:

	// ステージモデルのハンドルID
	int modelId_;

	// ステージモデルの位置
	VECTOR pos_;
};