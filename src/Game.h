#ifndef GAME_H
#define GAME_H

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
	#include "DealOrNoDealLib/DealOrNoDealLib.h"
}

#include "ScreenObjects.h"

class Config;
class AchieveConfig;
class MouseHandling;

enum Direction{Up, Down, Left, Right};

class Game
{
public:
	Game(SDL_Surface* pScreen, DealOrNoDealLib Deal, Config* pConfig, AchieveConfig* pAchieve, MouseHandling* pMouse);
	~Game();

	bool Loop();
	
protected:
	bool CheckGameState();
	bool PollEvents();
	void UpdateDisplay();
	void Move(Direction eDirection);
	void Move(int nX, int nY);
	bool SelectCase(bool bMouse);
	void PickNewCurrentCase();

protected:
	SDL_Surface	*m_pScreen;//Does not own
	Config* m_pConfig;
	AchieveConfig* m_pAchieve;
	SDL_Surface *m_pBackground;
	ScreenObjectsControl    m_Objs;
	nSDL_Font       *m_pFont;
	nSDL_Font       *m_pFontSmall;
	MouseHandling* m_pMouse;
	int m_nCurrentCase;
        DealOrNoDealLib m_Deal;
};

#endif

