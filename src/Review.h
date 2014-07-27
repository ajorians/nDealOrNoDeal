#ifndef REVIEW_H
#define REVIEW_H

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
	#include "DealOrNoDealLib/DealOrNoDealLib.h"
}

class ScreenObjectsControl;
class Config;
class MouseHandling;

class Review
{
public:
	Review(SDL_Surface* pScreen, Config* pConfig, MouseHandling* pMouse, int nOffer, int nCaseContained, bool bAcceptedOffer);
	~Review();

	bool Loop();
	
protected:
	bool PollEvents();
	void UpdateDisplay();

protected:
	SDL_Surface	*m_pScreen;//Does not own
	Config		*m_pConfig;//Does not own
	MouseHandling	*m_pMouse;//Does not own
	nSDL_Font *m_pFont;
	int m_nOffer;
	int m_nCaseContained;
        bool m_bAcceptedOffer;
};

#endif
