#include "Resource.h"
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include <string>
#include <cassert>

Resource::Resource(void)
{
	resType_ = LOAD_TYPE::NONE; // 読み込むファイル種類
	path_	 = "";				// 読み込み先

	numX_  = numY_  = -1; // 画像数
	sizeX_ = sizeY_ = -1; // 画像サイズ

	handleId_  = -1;	  // 読み込みハンドルID
	handleIds_ = nullptr; // 読み込み複数画像ハンドルID

#ifdef _DEBUG
	assert("\nリソースの引数が未割当です\n");
#endif
}

Resource::Resource(LOAD_TYPE type, const std::string& path)
{
	/*　コンストラクタ　*/

	resType_ = type; // 読み込みファイル種類
	path_	 = path; // 読み込み先

	numX_  = numY_  = -1; // 画像数
	sizeX_ = sizeY_ = -1; // 画像サイズ

	handleId_  = -1;	  // 読み込みハンドルID
	handleIds_ = nullptr; // 読み込み複数画像ハンドルID

}
Resource::Resource(LOAD_TYPE type, const std::string& path, int numX, int numY, int sizeX, int sizeY)
{
	/* 複数画像時のコンストラクタ */

	resType_ = type; // 読み込みファイル種類
	path_	 = path; // 読み込み先

	numX_  = numX;	// 横画像数
	numY_  = numY;	// 縦画像数
	sizeX_ = sizeX; // 横画像サイズ
	sizeY_ = sizeY; // 縦画像サイズ

	handleId_  = -1;	  // 読み込みハンドルID
	handleIds_ = nullptr; // 読み込み複数画像ハンドルID

	// 読み込み処理
	Load();
}



/// <summary>
/// 読み込み処理
/// </summary>
void Resource::Load(void)
{
	switch (resType_)
	{
		case LOAD_TYPE::IMAGE:
		{
			handleId_ = LoadGraph(path_.c_str());
		}
		break;

		case LOAD_TYPE::IMAGES:
		{
			handleIds_ = new int[numX_ * numY_];
			LoadDivGraph(path_.c_str(),
						 (numX_ * numY_), numX_, numY_,
						 sizeX_, sizeY_,(&handleIds_[0]));
		}
		break;

		case LOAD_TYPE::MODEL:
		{
			handleId_ = MV1LoadModel(path_.c_str());
		}
		break;

		
		case LOAD_TYPE::EFFECT:
		{ 
			handleId_ = LoadEffekseerEffect(path_.c_str());
			if (handleId_ == -1)
			{
#ifdef _DEBUG
				OutputDebugStringA(("Effekseerエフェクトの読み込みに失敗しました: " + path_ + "\n").c_str());
#endif
				// 必要に応じて例外を投げる、またはエラー状態を記録する
				// throw std::runtime_error("Effekseer effect load failed: " + path_);
			}
		}
		break;
	}
}

void Resource::Release(void)
{
	switch (resType_)
	{
		// 単一画像
		case LOAD_TYPE::IMAGE:
		{
			DeleteGraph(handleId_);
		}
		break;

		// 複数画像
		case LOAD_TYPE::IMAGES:
		{
			int max = (numX_ * numY_);
			for (int i = 0; i < max; i++)
			{
				DeleteGraph(handleIds_[i]);
			}
		}
		break;

		// 3Dモデル
		case LOAD_TYPE::MODEL:
		{
			MV1DeleteModel(handleId_);
			auto ids = duplicationModelIds_;
			for (auto id : ids)
			{
#if _DEBUG
				if (id == -1)
				{
					OutputDebugString("\nモデルが未割当です\n");
				}
#endif
				MV1DeleteModel(id);
			}
		}
		break;

		// エフェクト
		
		case LOAD_TYPE::EFFECT:
		{
			DeleteEffekseerEffect(handleId_);
		}
		break;
	}
}

/// <summary>
/// ハンドルコピー処理
/// </summary>
/// <param name="images">画像ハンドル</param>
void Resource::CopyHandle(int* images)
{
	// 複数IDがない時、処理終了
	if (handleIds_ == nullptr) return;

	int max = (numX_ * numY_);
	for (int i = 0; i < max; i++)
	{
		images[i] = handleIds_[i]; // ハンドル割り当て
	}
}

/// <summary>
/// 重複させる3DモデルID 割り当て処理
/// </summary>
/// <param name="id"></param>
void Resource::SetDuplicateModelId(int id)
{
	// IDをリストの末尾に追加
	duplicationModelIds_.push_back(id);
}

/// <summary>
/// ハンドルID取得処理
/// </summary>
/// <returns>ファイルのハンドル</returns>
int Resource::GetHandleId(void)
{
	return handleId_;
}
