#pragma once
#include "SceneBase.h"
#include <vector>
class Grid;
class Stage;
class ActorBase;
class Player;
class Enemy;

class GameScene : public SceneBase
{

public:

	// コンストラクタ
	GameScene(void);

	// デストラクタ
	~GameScene(void) override;

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:

	// グリッド線
	Grid* grid_;

	Stage* stage_;

	Player* player_;

	Enemy* enemy_;

	void CollisionGround(ActorBase& actor);
	void CollisionWall(ActorBase& actor);
	void CollisionEnemys(void);
};
