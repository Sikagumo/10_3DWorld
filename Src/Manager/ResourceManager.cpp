
#include "./Resource.h"
#include "ResourceManager.h"
#include "../Application.h"
#include <DxLib.h>
#include <string>

ResourceManager* ResourceManager::instance_ = nullptr;

// ファイルパスの割り当て
const std::string ResourceManager::PATH_EFFECT	  = "Data/Effect/";
const std::string ResourceManager::PATH_FONT	  = "Data/Font/";
const std::string ResourceManager::PATH_IMAGE	  = "Data/Image/";
const std::string ResourceManager::PATH_MODEL	  = "Data/Model/";
const std::string ResourceManager::PATH_SAVE_FILE = "Data/SaveFile/";
const std::string ResourceManager::PATH_SE		  = "Data/Sound/SE/";
const std::string ResourceManager::PATH_BGM		  = "Data/Sound/BGM/";
const std::string ResourceManager::PATH_MOVIE	  = "Data/Movie/";


void ResourceManager::CreateInstance(void)
{
	/*　インスタンス生成処理　*/
	if (instance_ == nullptr)
	{
		instance_ = new ResourceManager();
	}

	instance_->Init(); // マネージャ初期化
}

ResourceManager& ResourceManager::GetInstance(void)
{
	/*　インスタンス取得処理　*/
	return *instance_;
}

ResourceManager::ResourceManager(void)
{
	
}


void ResourceManager::Init(void)
{
	SetResources(); // リソース取得
}


void ResourceManager::SetResources(void)
{
	/*　リソースを取得する処理　*/

	SetResource(Resource::LOAD_TYPE::MODEL, SRC::MODEL_PLAYER, PATH_MODEL + "Player/Player.mv1");
	SetResource(Resource::LOAD_TYPE::MODEL, SRC::MODEL_IDLE, PATH_MODEL + "Player/Idle.mv1");
	SetResource(Resource::LOAD_TYPE::MODEL, SRC::MODEL_THROW, PATH_MODEL + "Player/Throw.mv1");
	SetResource(Resource::LOAD_TYPE::MODEL, SRC::MODEL_WALK, PATH_MODEL + "Player/Walk.mv1");

	SetResource(Resource::LOAD_TYPE::MODEL, SRC::MODEL_SHOT, PATH_MODEL + "Shot/Bomb.mv1");

	SetResource(Resource::LOAD_TYPE::MODEL, SRC::MODEL_DICE, PATH_MODEL + "Dice.mv1");
	SetResource(Resource::LOAD_TYPE::MODEL, SRC::MODEL_CAMERA, PATH_MODEL + "MiniCamera.mv1");
	SetResource(Resource::LOAD_TYPE::MODEL, SRC::MODEL_STAGE, PATH_MODEL + "Stage.mv1");
}
void ResourceManager::SetResource(Resource::LOAD_TYPE type, SRC src, const std::string& path)
{
	/*　リソースを登録する処理　*/
	Resource res(type, path);
	resourcesMap_.emplace(src, res);
}

void ResourceManager::Release(void)
{
	/*　メモリ解放処理　*/

	if (loadedMap_.empty()) return;

	for (auto& p : loadedMap_) 
	{
		p.second->Release(); // 読み込み済みリソース解放
		delete p.second;	 // リソース要素削除
	}

	loadedMap_.clear(); // 読み込み済みリソースリストをクリア
}

void ResourceManager::Destroy(void)
{
	/*　インスタンス削除処理　*/

	Release(); // リソースマネージャ解放

	if (!resourcesMap_.empty())
	{
		resourcesMap_.clear(); // リソースリストをクリア(空の時は行わない)
	}

	delete instance_; // インスタンス削除
}

int ResourceManager::LoadHandleId(SRC src)
{
	/*　ハンドルIDの取得処理　*/
	return Load(src).GetHandleId();
}


Resource ResourceManager::Load(SRC src)
{
	/*　リソースのロード　*/
	Resource* res = _Load(src); // 読み込み処理

	if (res == nullptr) return Resource();

	Resource ret = *res;

	return *res;
}

/// <summary>
/// 読み込み処理
/// </summary>
/// <param name="src">読み込み対象</param>
/// <returns>リソース</returns>
Resource* ResourceManager::_Load(SRC src)
{
	// 読み込み済みリストを検索
	const auto& loaded = loadedMap_.find(src);

	//読み込み済みリストに対象がある時、要素を返す
	if (loaded != loadedMap_.end()) return loaded->second;


	// リソースリスト内を検索
	const auto& resource = resourcesMap_.find(src);

	// リソースリストに登録されてない時、NULLを返す
	if (resource == resourcesMap_.end()) return nullptr;


	// リソースリスト登録済み時、読み込み処理
	resource->second.Load();

	// 念のためにコピーコンストラクタ
	Resource* ret = new Resource(resource->second);

	// 読み込み済みリストに格納
	loadedMap_.emplace(src, ret);

	return ret;
}

/// <summary>
/// 3Dモデル重複利用時の読み込み
/// </summary>
/// <param name="src">読み込み対象</param>
/// <returns>重複するモデルのハンドル</returns>
int ResourceManager::LoadModelDuplicate(SRC src)
{
	Resource* resource = _Load(src); // 読み込み処理

	// 読み込み失敗
	if (resource == nullptr) return -1;

	// 重複するモデルのハンドルを取得
	int id = MV1DuplicateModel(resource->GetHandleId());

	// 重複モデルリストにハンドル追加
	resource->SetDuplicateModelId(id);

	return id;
}