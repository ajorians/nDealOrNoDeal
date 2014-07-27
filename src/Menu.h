#ifndef MENU_H
#define MENU_H

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
}
#include "ColorIntensity.h"

class Config;
class AchieveConfig;
class MouseHandling;

class MainMenu
{
public:
	MainMenu(SDL_Surface* pScreen, Config* pConfig, AchieveConfig* pAchieveConfig, MouseHandling* pMouse);
	~MainMenu();

	bool Loop();

	bool ShouldQuit() const;
	bool ShouldShowOptions() const;
	bool ShouldShowHelp() const;
	bool ShouldShowAchievements() const;
	
protected:
	bool PollEvents();
	void UpdateDisplay();

protected:
	SDL_Surface	*m_pScreen;//Does not own
	Config		*m_pConfig;//Does not own
	AchieveConfig	*m_pAchieve;
	MouseHandling	*m_pMouse;
	SDL_Surface 	*m_pBackground;
	nSDL_Font 	*m_pFont;
	nSDL_Font 	*m_pFontPoints;
	SDL_Surface	*m_pStar;
	enum MenuChoice{Play, Options, Help, Achieve, Quit};
	MenuChoice 	m_eChoice;
	ColorIntensity m_Color;
	bool		m_bNewAchievement;
	int		m_nFlashAchievement;
};

#endif
