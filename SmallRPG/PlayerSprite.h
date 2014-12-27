#pragma once
#include "Sprite.h"


class PlayerSprite :
	public Sprite
{
public:
	bool faceDirection;		// false:right, true:left
	bool fight;
	bool isPlayer;
	int iBlood;

	PlayerSprite(Bitmap* pBitmap, RECT& rcBounds,
		BOUNDSACTION baBoundsAction);
	virtual ~PlayerSprite();

	virtual void  UpdateFrame();
	void Action();
	void Stop();
	int getX() { return m_rcPosition.top; }
	int getY() { return m_rcPosition.left; }
};

