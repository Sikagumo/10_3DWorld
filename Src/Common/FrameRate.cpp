#include "FrameRate.h"
#include <DxLib.h>

// シングルトンインスタンス
FrameRate* FrameRate::instance_ = nullptr;


FrameRate::FrameRate(void)
{
	curTime_ = lateTime_ = 0;

	frameCnt_ = updateTime_ = 0;

	viewFramelate_ = 0;
}

void FrameRate::CreateInstance(void)
{
	// インスタンス未生成時に生成する
	if (instance_ == nullptr) instance_ = new FrameRate();
}


void FrameRate::Update(void)
{
	// システムに処理を返す。「世界」ッ！時よ止まれ！
	Sleep(1);

	//現在時刻を取得する
	curTime_ = GetNowCount();
}


void FrameRate::Draw(void)
{
	// 平均フレームレート 描画
	DrawFormatString(0, 0, 0xFFFFFF, "%.1ffps", viewFramelate_);
}


void FrameRate::Destroy(void)
{
	if (instance_ == nullptr)
	{
#ifdef _DEBUG
		OutputDebugString("\nマネージャが生成されていません。\n");
#endif
		return;
	}
	delete instance_;
}


void FrameRate::SetFrameRate(void)
{
	lateTime_ = curTime_; // 前フレームの時間 割り当て

	frameCnt_++;

	// 現在時間との差分
	int nDifTime = (curTime_ - updateTime_);

	if (nDifTime > 1000)
	{
		// フレームレート単位変更(ミリ秒 → 秒)
		float fFrameCount = static_cast<float>(frameCnt_ * 1000);

		// 描画フレーム単位 取得
		viewFramelate_ = (fFrameCount / nDifTime);

		frameCnt_ = 0;

		// フレームレート 更新
		updateTime_ = curTime_;
	}
}