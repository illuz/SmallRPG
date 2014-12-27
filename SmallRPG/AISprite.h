#pragma once
#include "PlayerSprite.h"
class AISprite :
	public PlayerSprite
{
public:
	AISprite(Bitmap* pBitmap, RECT& rcBounds,
		BOUNDSACTION baBoundsAction);
	~AISprite();
};

