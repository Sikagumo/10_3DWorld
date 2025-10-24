#pragma once
#include <DxLib.h>
class ActorBase;

class Shot
{
public:

	enum class STATE
	{
		NONE = -1,
		TRACK,
		SHOT,
		HIT,
	};

	static constexpr float SHOT_SPEED = 3.5f;

	static constexpr float HIT_WAIT = 2.75f;


	Shot(int handle, unsigned int color, ActorBase& actor);

	~Shot(void) = default;

	void Init(void);

	void Update(void);

	void Draw(void);

	void SetShot(VECTOR& dir, float shotSpeed, float shotY);

	const VECTOR& GetPos(void) { return pos_; };

	float GetRadius(void) { return radius_; };

	/// @brief ”­ŽË‰Â”\‚©”Û‚©
	/// @param  
	/// @return 
	bool GetIsShot(void) { return (state_ == STATE::TRACK); };

	STATE GetState(void) { return state_; };

	void SetIsHit(void);

private:

	ActorBase& actor_;

	float waitTime_;

	int modelId_;

	STATE state_;

	VECTOR scale_;
	
	float radius_;

	unsigned int difColor_;

	VECTOR pos_;
	VECTOR velocity_;

	VECTOR angle_;

	VECTOR dir_;
};