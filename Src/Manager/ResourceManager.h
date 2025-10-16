#pragma once

#include <map>
#include <string>
#include "./Resource.h"

/// <summary>
/// リソース管理マネージャ
/// </summary>
class ResourceManager
{
public:

	/// <summary>
	/// リソース識別列挙
	/// </summary>
	enum SRC
	{
		NONE = -1,

		/*　3Dモデルリソース　*/

		MODEL_PLAYER,
		MODEL_SHOT,
		MODEL_DICE,
		MODEL_CAMERA,
		MODEL_STAGE,

		ANIM_IDLE,
		ANIM_THROW,
		ANIM_WALK,
		ANIM_RUN,
	};


	/*　各ハンドルパス　*/

	// エフェクトファイルパス
	static const std::string PATH_EFFECT;

	// フォントファイルパス
	static const std::string PATH_FONT;

	// 画像ファイルパス
	static const std::string PATH_IMAGE;

	// 3Dモデルファイルパス
	static const std::string PATH_MODEL;

	// セーブファイルパス
	static const std::string PATH_SAVE_FILE;

	// 効果音ファイルパス
	static const std::string PATH_SE;

	// BGMファイルパス
	static const std::string PATH_BGM;

	// 動画ファイルパス
	static const std::string PATH_MOVIE;


	/// <summary>
	/// インスタンス生成
	/// </summary>
	static void CreateInstance(void);

	/// <summary>
	/// インスタンス取得処理
	/// </summary>
	/// <returns>リソースマネージャ</returns>
	static ResourceManager& GetInstance(void);

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init(void);

	/// <summary>
	/// 解放処理
	/// </summary>
	void Release(void);

	/// <summary>
	/// インスタンス削除
	/// </summary>
	void Destroy(void);

	/// <summary>
	/// リソースのハンドルを取得
	/// </summary>
	/// <param name="src"></param>
	/// <returns></returns>
	int LoadHandleId(SRC src);

	/// <summary>
	/// 3Dモデル重複利用時の読み込み
	/// </summary>
	/// <param name="src">読み込み対象</param>
	/// <returns>重複するモデルのハンドル</returns>
	int LoadModelDuplicate(SRC src);



private:

	//静的インスタンス
	static ResourceManager* instance_;

	// リソース管理対象
	std::map<SRC, Resource> resourcesMap_;

	// 読み込み済みリソース
	std::map<SRC, Resource*> loadedMap_;



	/// <summary>
	/// デフォルトコンストラクタ(private化で外部生成不可化)
	/// </summary>
	ResourceManager(void);

	/// <summary>
	/// デストラクタ(private化で外部で生成不可化)
	/// </summary>
	/// <param name=""></param>
	~ResourceManager(void) = default;

	/// <summary>
	/// コピーコンストラクタ
	/// </summary>
	ResourceManager(const ResourceManager& other) = default;

	/// <summary>
	/// リソースのロード
	/// </summary>
	/// <param name="src">読み込み対象</param>
	/// <returns>リソース</returns>
	Resource Load(SRC src);

	/// <summary>
	/// リソースを取得する処理
	/// </summary>
	void SetResources(void);

	void SetResource(Resource::LOAD_TYPE type, SRC src, const std::string& path);

	/// <summary>
	/// 内部ロード
	/// </summary>
	/// <param name="source">読み込み対象</param>
	/// <returns></returns>
	Resource* _Load(SRC source);
};