#include "Menu.h"
#include "MainMenuGraphic.h"
#include "Config.h"
#include "AchieveConfig.h"
#include "MouseHandling.h"
#include "StarGraphic.h"
#include "Defines.h"

MainMenu::MainMenu(SDL_Surface* pScreen, Config* pConfig, AchieveConfig* pAchieve, MouseHandling* pMouse)
: m_pScreen(pScreen), m_pConfig(pConfig), m_pAchieve(pAchieve), m_pMouse(pMouse), m_eChoice(Play), m_nFlashAchievement(0)
{
	m_pBackground = nSDL_LoadImage(image_DealMainMenu);
	m_pStar = nSDL_LoadImage(image_Star);
	SDL_SetColorKey(m_pStar, SDL_SRCCOLORKEY, SDL_MapRGB(m_pStar->format, 0, 0, 0));

	m_pFont = nSDL_LoadFont(NSDL_FONT_THIN, 0/*R*/, 0/*G*/, 0/*B*/);
	m_pFontPoints = nSDL_LoadFont(NSDL_FONT_THIN, 0/*R*/, 0/*G*/, 255/*B*/);

	m_bNewAchievement = pAchieve->GetNewAchievement();
}

MainMenu::~MainMenu()
{
	SDL_FreeSurface(m_pBackground);
	SDL_FreeSurface(m_pStar);
	nSDL_FreeFont(m_pFont);
}

bool MainMenu::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;
	
	UpdateDisplay();

	rand();//Instead of srand
	sleep(5);
	
	return true;
}

bool MainMenu::ShouldQuit() const
{
	return m_eChoice == Quit;
}

bool MainMenu::ShouldShowOptions() const
{
        return m_eChoice == Options;
}

bool MainMenu::ShouldShowHelp() const
{
        return m_eChoice == Help;
}

bool MainMenu::ShouldShowAchievements() const
{
	return m_eChoice == Achieve;
}

bool MainMenu::PollEvents()
{
	SDL_Event event;
	
	/* Poll for events. SDL_PollEvent() returns 0 when there are no  */
	/* more events on the event queue, our while loop will exit when */
	/* that occurs.                                                  */
	while( SDL_PollEvent( &event ) )
	{
		/* We are only worried about SDL_KEYDOWN and SDL_KEYUP events */
		switch( event.type )
		{
			case SDL_KEYDOWN:
				printf( "Key press detected\n" );
				switch (event.key.keysym.sym) 
				{
					case SDLK_ESCAPE:
						fprintf(stderr, "Hit Escape!n");
						m_eChoice = Quit;
					return false;
					break;
					
					case SDLK_UP:
					case SDLK_8:
						m_eChoice = Achieve;
						break;

					case SDLK_LEFT:
					case SDLK_4:
						if( m_eChoice != Achieve ) {
							if( m_eChoice == Help )
								m_eChoice = Play;
							else if( m_eChoice == Play )
								m_eChoice = Options;
							else if( m_eChoice == Options )
								m_eChoice = Help;
						}
						break;
					
					case SDLK_DOWN:
					case SDLK_2:
						if( m_eChoice == Achieve )
							m_eChoice = Play;
						break;

					case SDLK_RIGHT:
					case SDLK_6:
						if( m_eChoice != Achieve ) {
							if( m_eChoice == Play )
								m_eChoice = Help;
							else if( m_eChoice == Options )
								m_eChoice = Play;
							else if( m_eChoice == Help )
								m_eChoice = Options;
						}
						break;

					case SDLK_SPACE:
					case SDLK_RETURN:
					case SDLK_LCTRL:
					case SDLK_5:
						return false;
						break;
					
					default:
						break;
				}
				break;
			
			//Called when the mouse moves
			case SDL_MOUSEMOTION:
				break;
			
			case SDL_KEYUP:
				printf( "Key release detected\n" );
				break;
			
			default:
				break;
		}
	}

	int nMX = -1, nMY = -1;
        if( m_pMouse->PollMouse(nMX, nMY) ) {
                MouseButton eMouseButton = m_pMouse->GetMouseButton();
                if( eMouseButton == CenterButton ) {
                    return false;
                }
                /*else if( eMouseButton == LeftButton ) {
                    m_eSelection = Deal;
                }
                else if( eMouseButton == RightButton ) {
                    m_eSelection = NoDeal;
                }*/

                /*if( nMX != -1 && nMY != -1 )
                        Move(nMX, nMY);*/
        }

	return true;
}

void MainMenu::UpdateDisplay()
{
	//Draw background
	if( is_classic ) {
		SDL_FillRect(m_pScreen, NULL, SDL_MapRGB(m_pScreen->format, 255, 255, 255));
		nSDL_DrawString(m_pScreen, m_pFont, SCREEN_WIDTH/2-nSDL_GetStringWidth(m_pFont, "nDealOrNoDeal")/2, SCREEN_HEIGHT/2-15, "nDealOrNoDeal");
		nSDL_DrawString(m_pScreen, m_pFont, 141, 212, "Play");
		nSDL_DrawString(m_pScreen, m_pFont, 12, 212, "Options");
		nSDL_DrawString(m_pScreen, m_pFont, 262, 212, "Help");
		nSDL_DrawString(m_pScreen, m_pFont, 192, 7, "Achievements");
	} else {
		SDL_BlitSurface(m_pBackground, NULL, m_pScreen, NULL);
	}

	//Show player standing
	char buffer[80];
	char buf[10];
	Deal_itoa(m_pConfig->GetPlayerStanding(), buf, 10);
	strcpy(buffer, "Current player standing: ");
	strcat(buffer, buf);
	nSDL_DrawString(m_pScreen, m_pFontPoints, SCREEN_WIDTH/2-nSDL_GetStringWidth(m_pFontPoints, buffer)/2, 198, buffer);

	//Draw selection rectanges
	int r=255, g=0, b=0, a=0;
	m_Color.AdjustColor(r,g,b,a);
	m_Color.Animate();
	if( m_eChoice == Play )
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, r, g, b), 135, 209, 45, 21, 5);
	else if( m_eChoice == Options )
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, r, g, b), 8, 209, 78, 21, 5);
	else if( m_eChoice == Help )
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, r, g, b), 255, 209, 53, 21, 5);
	else if( m_eChoice == Achieve )
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, r, g, b), 190, 5, 128, 21, 5);

	m_nFlashAchievement = (m_nFlashAchievement+1)%100;
	if( m_bNewAchievement && (m_nFlashAchievement<50) ) {
		SDL_Rect rectStar;
		rectStar.x = 176;
		rectStar.y = 7;
		rectStar.w = 16;
		rectStar.h = 16;
		SDL_BlitSurface(m_pStar, NULL, m_pScreen, &rectStar);
	}
	
	SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}


