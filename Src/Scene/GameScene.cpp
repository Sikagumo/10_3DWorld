#include "GameScene.h"
#include <cmath>
#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Object/Grid.h"
#include "../Object/Stage.h"
#include "../Object/ActorBase.h"
#include "../Object/Player.h"
#include "../Object/Enemy.h"
#include "../Object/Shot.h"
#include "../Manager/Camera.h"
#include "../Utility/AsoUtility.h"

GameScene::GameScene(void) : SceneBase()
{
	grid_ = nullptr;
	stage_ = nullptr;
	player_ = nullptr;
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	// グリッド初期化
	grid_ = new Grid();
	grid_->Init();

	stage_ = new Stage();
	stage_->Init();

	// プレイヤー初期化
	player_ = new Player();
	player_->Init();

	// 敵初期化
	enemy_ = new Enemy(player_);
	enemy_->Init();

	Camera& camera = SceneManager::GetInstance().GetCamera();
	camera.SetFollow(player_);
	camera.ChangeMode(Camera::MODE::FOLLOW);
}

void GameScene::Update(void)
{
#ifdef _DEBUG
	// グリッド更新
	grid_->Update();
#endif

	// ステージ更新
	stage_->Update();

	// プレイヤー更新
	player_->Update();
	CollisionGround(*player_);
	CollisionWall(*player_);

	// 敵更新
	enemy_->Update();
	CollisionGround(*enemy_);
	CollisionWall(*enemy_);
	CollisionEnemys();

	
	Shot& pShot = player_->GetShot();
	if (AsoUtility::IsHitSphereCapsule(pShot.GetPos(), pShot.GetRadius(),
									   enemy_->GetPos(), enemy_->GetPosHead(), enemy_->GetRadius()))
	{
		if (pShot.GetState() == Shot::STATE::SHOT)
		{
			pShot.SetIsHit();

			enemy_->SetDamage();
		}
	}

	Shot& eShot = enemy_->GetShot();
	if (AsoUtility::IsHitSphereCapsule(eShot.GetPos(), eShot.GetRadius(),
									   player_->GetPos(), enemy_->GetPosHead(), player_->GetRadius()))
	{
		//eShot.SetIsHit();
	}
}

void GameScene::Draw(void)
{
	bool isDebug = SceneManager::GetInstance().GetIsDebugMode();

	// ステージ描画
	stage_->Draw();

#ifdef _DEBUG
	if (isDebug)
	{
		// グリッド描画
		grid_->Draw();
	}
#endif

	player_->Draw();

	enemy_->Draw();

	player_->DrawDebug();
	enemy_->DrawDebug();
}

void GameScene::Release(void)
{
	enemy_->Release();
	delete enemy_;

	player_->Release();
	delete player_;

	// グリッド解放
	grid_->Release();
	delete grid_;

	delete stage_;
}

void GameScene::CollisionGround(ActorBase& actor)
{
	// 座標を取得
	VECTOR actorPos = actor.GetPos();

	// 線分の上座標
	VECTOR startPos = actorPos;
	startPos.y = actorPos.y + 10.0f;

	// 線分の下座標
	VECTOR endPos = actorPos;
	endPos.y = actorPos.y - 10.0f;

	// ステージモデルを取得
	int stageId = stage_->GetModelId();

	// 線分とモデルの衝突判定
	MV1_COLL_RESULT_POLY res =
		MV1CollCheck_Line(stageId, -1, startPos, endPos);

	// ステージに当たっているか判定
	if (!actor.GetIsGround() && res.HitFlag)
	{
		// 衝突位置に戻す
		actor.CollisionStage(res.HitPosition);
	}
	else if (actor.GetIsGround() &&!res.HitFlag)
	{
		actor.SetIsGround(false);
	}
}

void GameScene::CollisionWall(ActorBase& actor)
{
	// 座標を取得
	VECTOR pos = actor.GetPos();
	bool isHit = false;

	// カプセルの座標
	VECTOR capStartPos = actor.GetPosHead();
	VECTOR capEndPos = pos;

	// カプセルとの当たり判定
	auto hits = MV1CollCheck_Capsule
	(
		stage_->GetModelId(),			// ステージのモデルID
		-1,								// ステージ全てのポリゴンを指定
		capStartPos,					// カプセルの上
		capEndPos,						// カプセルの下
		actor.GetRadius()		// カプセルの半径
	);

	// 衝突したポリゴン全ての検索
	for (int i = 0; i < hits.HitNum; i++)
	{
		// ポリゴンを1枚に分割
		auto hit = hits.Dim[i];

		// ポリゴン検索を制限(全てを検索すると重いので)
		for (int tryCnt = 0; tryCnt < 10; tryCnt++)
		{
			// 最初の衝突判定で検出した衝突ポリゴン1枚と衝突判定を取る
			int pHit = HitCheck_Capsule_Triangle
			(
				capStartPos,					// カプセルの上
				capEndPos,						// カプセルの下
				actor.GetRadius(),		// カプセルの半径
				hit.Position[0],				// ポリゴン1
				hit.Position[1],				// ポリゴン2
				hit.Position[2]					// ポリゴン3
			);

			// カプセルとポリゴンが当たっていた
			if (pHit)
			{
				// 当たっていたので座標をポリゴンの法線方向に移動させる
				pos = VAdd(pos, VScale(hit.Normal, 1.0f));

				// 球体の座標も移動させる
				capStartPos = VAdd(capStartPos, VScale(hit.Normal, 1.0f));
				capEndPos = VAdd(capEndPos, VScale(hit.Normal, 1.0f));

				isHit = true;
				// 複数当たっている可能性があるので再検索
				continue;
			}
		}
	}
	// 検出したポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(hits);

	if (isHit)
	{
		// 計算した場所にアクターを戻す
		actor.CollisionStage(pos);
	}
	else if (actor.GetIsGround())
	{
		actor.SetIsGround(false);
	}
}

void GameScene::CollisionEnemys(void)
{
	VECTOR pPos = player_->GetPos();
	float pRad = player_->GetRadius();

	VECTOR ePos = enemy_->GetPos();
	float eRad = enemy_->GetRadius();

	if (AsoUtility::IsHitSpheres(pPos, pRad, ePos, eRad))
	{
		const float reflectPow = 2.0f;
		VECTOR dir = VScale(enemy_->GetDir(), -1);
		dir = VScale(dir, enemy_->GetSpeed() * reflectPow);

		enemy_->SetPos(VAdd(ePos, dir));
	}
}
