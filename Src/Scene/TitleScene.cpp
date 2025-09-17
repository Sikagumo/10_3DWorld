#include <cmath>
#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Object/Grid.h"
#include "TitleScene.h"

TitleScene::TitleScene(void) : SceneBase()
{
	grid_ = nullptr;
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{

	// グリッド初期化
	grid_ = new Grid();
	grid_->Init();
}

void TitleScene::Update(void)
{
	// グリッド更新
	grid_->Update();
}

void TitleScene::Draw(void)
{
	// グリッド描画
	grid_->Draw();
}

void TitleScene::Release(void)
{
	// グリッド解放
	grid_->Release();
	delete grid_;
}
