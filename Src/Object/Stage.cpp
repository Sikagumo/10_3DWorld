#include "Stage.h"
#include <DxLib.h>
#include "../Manager/ResourceManager.h"

Stage::Stage(void)
{
	pos_ = {};
}

void Stage::Init(void)
{
	modelId_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::MODEL_STAGE);

	MV1SetPosition(modelId_, pos_);
}

void Stage::Update(void)
{
	
}

void Stage::Draw(void)
{
	MV1DrawModel(modelId_);
}