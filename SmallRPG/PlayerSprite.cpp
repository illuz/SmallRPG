#include "PlayerSprite.h"


PlayerSprite::PlayerSprite(Bitmap* pBitmap, RECT& rcBounds,
	BOUNDSACTION baBoundsAction = BA_STOP) : Sprite(pBitmap, rcBounds,
	baBoundsAction)
{
	isPlayer = false;
	faceDirection = false;
	iBlood = 100;
}


PlayerSprite::~PlayerSprite()
{
}

void PlayerSprite::UpdateFrame() {

}

void PlayerSprite::Action() {
	if (faceDirection) {
		if (fight) {
			m_iCurFrame = m_iCurFrame ? 0 : 4;
		}
		else {
			m_iCurFrame = m_iCurFrame ? 0 : 2;
		}
	}
	else {
		if (fight) {
			m_iCurFrame = m_iCurFrame ? 0 : 3;
		}
		else {
			m_iCurFrame = m_iCurFrame ? 0 : 1;
		}
	}
}

void PlayerSprite::Stop() {
	m_iCurFrame = 0;
}
