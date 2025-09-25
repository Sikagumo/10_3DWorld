#include <cmath>
#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Object/Grid.h"
#include "../Object/Stage.h"
#include "../Object/Player.h"
#include "../Manager/Camera.h"
#include "TitleScene.h"

TitleScene::TitleScene(void) : SceneBase()
{
	grid_ = nullptr;
	stage_ = nullptr;
	player_ = nullptr;
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	Camera& camera = SceneManager::GetInstance().GetCamera();
	camera.ChangeMode(Camera::MODE::FREE);

	// グリッド初期化
	grid_ = new Grid();
	grid_->Init();

	stage_ = new Stage();
	stage_->Init();

	player_ = new Player();
	player_->Init();
}

void TitleScene::Update(void)
{
#ifdef _DEBUG
	// グリッド更新
	grid_->Update();
#endif

	// ステージ更新
	stage_->Update();

	// プレイヤー更新
	player_->Update();
}

void TitleScene::Draw(void)
{
	// ステージ描画
	stage_->Draw();

#ifdef _DEBUG
	// グリッド描画
	grid_->Draw();
#endif

	player_->Draw();

	player_->DrawDebug();
}

void TitleScene::Release(void)
{
	player_->Release();
	delete player_;

	// グリッド解放
	grid_->Release();
	delete grid_;

	delete stage_;
}
