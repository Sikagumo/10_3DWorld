#include <cmath>
#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Object/Grid.h"
#include "../Object/Stage.h"
#include "../Manager/Camera.h"
#include "TitleScene.h"

TitleScene::TitleScene(void) : SceneBase()
{
	grid_ = nullptr;
	stage_ = nullptr;
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
}

void TitleScene::Update(void)
{
	// グリッド更新
	grid_->Update();

	// ステージ更新
	stage_->Update();
}

void TitleScene::Draw(void)
{
	// ステージ描画
	stage_->Draw();

	// グリッド描画
	grid_->Draw();
}

void TitleScene::Release(void)
{
	// グリッド解放
	grid_->Release();
	delete grid_;

	delete stage_;
}
