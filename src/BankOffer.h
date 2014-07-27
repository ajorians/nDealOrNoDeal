#ifndef BANKOFFER_H
#define BANKOFFER_H

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
	#include "DealOrNoDealLib/DealOrNoDealLib.h"
}
#include "ColorIntensity.h"

class ScreenObjectsControl;
class Config;
class MouseHandling;

class BankOffer
{
public:
	enum DealSelection
	{
		None,
		Deal,
		NoDeal
	};
public:
	BankOffer(SDL_Surface* pScreen, DealOrNoDealLib deal, ScreenObjectsControl* pScreenObjects, MouseHandling* pMouse, int nOffer, int nMoreCases, Config* pConfig);
	~BankOffer();

	bool AcceptedOffer() const;

	bool Loop();
	
protected:
	bool PollEvents();
	void UpdateDisplay();
	void Move(int nX, int nY);

protected:
	SDL_Surface	*m_pScreen;//Does not own
	DealOrNoDealLib m_Deal;
	ScreenObjectsControl	*m_pScreenObjects;//Does not own
	MouseHandling	*m_pMouse;
	nSDL_Font *m_pFont;
	nSDL_Font *m_pFontButtons;
	int m_nOffer;
	int m_nMoreCases;
        bool m_bAcceptedOffer;
	DealSelection m_eSelection;
	ColorIntensity m_Color;
};

#endif
