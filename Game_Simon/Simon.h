#pragma once
#include "GameObject.h"

#define SIMON_WALKING_SPEED		2.1f 
//0.1f
#define SIMON_JUMP_SPEED_Y		5.f
#define SIMON_JUMP_DEFLECT_SPEED 1.2f
#define SIMON_GRAVITY			 0.2f
#define SIMON_DIE_DEFLECT_SPEED	 0.5f

#define SIMON_STATE_IDLE			0
#define SIMON_STATE_WALKING_RIGHT	100
#define SIMON_STATE_WALKING_LEFT	200
#define SIMON_STATE_JUMP			300
#define SIMON_STATE_WALKING_JUMP    400
#define SIMON_STATE_DIE				500
#define SIMON_STATE_ATTACK			600
#define SIMON_STATE_SIT				700


#define SIMON_ANI_IDLE_RIGHT			0
#define SIMON_ANI_IDLE_LEFT				1
#define SIMON_ANI_WALKING_RIGHT			2
#define SIMON_ANI_WALKING_LEFT			3
#define SIMON_ANI_JUMP_RIGHT			4
#define SIMON_ANI_JUMP_LEFT				5
#define SIMON_ANI_DIE_RIGHT				6
#define SIMON_ANI_DIE_LEFT		        7
#define SIMON_ANI_ATTACK_RIGHT			8
#define SIMON_ANI_ATTACK_LEFT			9



#define SIMON_BBOX_WIDTH  13//28
#define SIMON_BBOX_HEIGHT 15//59

//#define SIMON_SMALL_BBOX_WIDTH  20
//#define SIMON_SMALL_BBOX_HEIGHT 25

#define SIMON_UNTOUCHABLE_TIME 5000


class CSIMON : public CGameObject
{
	//int level;
	int untouchable;
	DWORD untouchable_start;
public:
	CSIMON() : CGameObject()
	{
		//level = SIMON_LEVEL_BIG;
		untouchable = 0;
	}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *colliable_objects = NULL);
	virtual void Render();
	void SetState(int state);
	//void SetLevel(int l) { level = l; }
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount(); }
	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
	int GetX();
	int GetY();
	void SetY(int vt);
	void SetX(int vt);
	void SetAni(int Ani);

};
