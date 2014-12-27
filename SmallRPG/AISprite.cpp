#include "AISprite.h"


AISprite::AISprite(Bitmap* pBitmap, RECT& rcBounds,
	BOUNDSACTION baBoundsAction = BA_STOP) : PlayerSprite(pBitmap, rcBounds,
	baBoundsAction)
{
}


AISprite::~AISprite()
{
}
