#ifndef ACHIEVEMENTS_H
#define ACHIEVEMENTS_H

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
}

#include "ProgressMeter.h"

class AchieveConfig;
class MouseHandling;

enum Achievement_Direction{A_Up, A_Down, A_Left, A_Right};

class Achievements
{
public:
	Achievements(SDL_Surface* pScreen, AchieveConfig* pAchieveConfig, MouseHandling* pMouse);
	~Achievements();

	bool Loop();
	
protected:
	bool PollEvents();
	void UpdateDisplay();
	void Move(Achievement_Direction eDirection);

protected:
	SDL_Surface	*m_pScreen;//Does not own
	enum AchievementChoice{None, Won1Million, Played10Games, Played50Games, Accepted10Deals, HadCloseCall};
	AchievementChoice m_eChoice;
	nSDL_Font	*m_pFont;
	nSDL_Font	*m_pFontRed;
	AchieveConfig	*m_pAchieveConfig;
	MouseHandling   *m_pMouse;
	SDL_Surface	*m_imgDeal;
	SDL_Surface	*m_imgCloseCall;
	SDL_Surface	*m_imgPlay;
	SDL_Surface	*m_imgPlay2;
	SDL_Surface	*m_imgMoney;

	SDL_Surface     *m_imgDkDeal;
	SDL_Surface     *m_imgDkCloseCall;
        SDL_Surface     *m_imgDkPlay;
        SDL_Surface     *m_imgDkPlay2;
        SDL_Surface     *m_imgDkMoney;

	ProgressMeter	m_Progress;
};

#endif
