#include <algorithm>
#include "debug.h"

#include "SIMON.h"
#include "Game.h"

//#include "Goomba.h"

void CSIMON::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	CGameObject::Update(dt);

	// simple fall down
	vy += SIMON_GRAVITY;
	if (y > 100) 
	{
		vy = 0; y = 100.0f;
	}

	// simple screen edge collision!!!
	if (vx > 0 && x > 290) x = 290;
	if (vx < 0 && x < 0) x = 0;
}

void CSIMON::Render()
{
	int ani;

	if (state == SIMON_STATE_DIE) {
		if (nx == 1)
			ani = SIMON_ANI_DIE_RIGHT;
		else
			ani = SIMON_ANI_DIE_LEFT;
	}
			
	else if (state == SIMON_STATE_JUMP) {
			if (nx == 1) {
				ani = SIMON_ANI_JUMP_RIGHT;
			}
			else 
				ani = SIMON_ANI_JUMP_LEFT;
		}
	else if (state == SIMON_STATE_ATTACK) {
		if (nx == 1) {
			ani = SIMON_ANI_ATTACK_RIGHT;
		}
		else
			ani = SIMON_ANI_ATTACK_LEFT;
	}
	else if (vx == 0)
			{
				if (nx>0) ani = SIMON_ANI_IDLE_RIGHT;
				else ani = SIMON_ANI_IDLE_LEFT;
			}
			else if (vx > 0)
				ani = SIMON_ANI_WALKING_RIGHT;
			else ani = SIMON_ANI_WALKING_LEFT;
	
	animations[ani]->Render(x, y);
}


void CSIMON::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case SIMON_STATE_WALKING_RIGHT:
		vx = SIMON_WALKING_SPEED;
		nx = 1;
		break;
	case SIMON_STATE_WALKING_LEFT:
		vx = -SIMON_WALKING_SPEED;
		nx = -1;
		break;
	case SIMON_STATE_JUMP:
		vy = -SIMON_JUMP_SPEED_Y;
	case SIMON_STATE_IDLE:
	case SIMON_STATE_ATTACK :
		vx = 0;
		break;
	case SIMON_STATE_DIE:
		vy = -SIMON_DIE_DEFLECT_SPEED;
		break;
	}
}

void CSIMON::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	left = x;
	top = y;
	right = x + SIMON_BBOX_WIDTH;
	bottom = y + SIMON_BBOX_HEIGHT;	
}

