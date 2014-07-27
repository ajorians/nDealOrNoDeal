extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
	#include "ArchiveLib/ArchiveLib.h"
	#include "DealOrNoDealLib/DealOrNoDealLib.h"
}

#include "Menu.h"
#include "Game.h"
#include "Config.h"
#include "Options.h"
#include "AchieveConfig.h"
#include "Achievements.h"
#include "MouseHandling.h"
#include "Help.h"

#define SCREEN_BIT_DEPTH        (16)
#define SCREEN_VIDEO_MODE (SDL_SWSURFACE|SDL_FULLSCREEN|SDL_HWPALETTE)

class GameObject
{
public:
   GameObject()
	: m_deal(NULL)
   {
   }
   ~GameObject()
   {
      EndCurrentGame();
   }

   void EndCurrentGame() {
      if( m_deal ) {
        DealOrNoDealLibFree(&m_deal);
        m_deal = NULL;
      }
   }

   bool GameInProgress() {
      if( m_deal == NULL )
         return false;
      return DEALORNODEAL_STATUS_STILLPLAYING == GetGameStatus(m_deal) && DEALORNODEAL_CASE_PICKED == HavePickedCase(m_deal, NULL);
   }

   DealOrNoDealLib StartGame() {
      if( m_deal )
         return m_deal;

      return RestartGame();
   }

   DealOrNoDealLib RestartGame() {
      EndCurrentGame();

      DealOrNoDealLibCreate(&m_deal);
      return m_deal;
   }

protected:
   DealOrNoDealLib m_deal;
};

int main(int argc, char *argv[]) 
{
	printf("Initializing SDL.\n");
	
	/* Initialize the SDL library (starts the event loop) */
    if ( SDL_Init(SDL_INIT_VIDEO ) < 0 )
	{
        fprintf(stderr,
                "Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
	
	printf("SDL initialized.\n");
	
	SDL_Surface* pScreen = NULL;
	pScreen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BIT_DEPTH, SCREEN_VIDEO_MODE);

	ArchiveSetCurrentDirectory(argv[0]);
	Config config;
	MouseHandling mouse(&config);
	AchieveConfig ac(&config);
	GameObject gameobj;

	if( pScreen == NULL )
	{
		fprintf(stderr, "Couldn't set %dx%dx%d video mode: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BIT_DEPTH, SDL_GetError());
	}
	else
	{
		/* Hides the cursor */
		SDL_ShowCursor(SDL_DISABLE);

		while(true)
		{
			bool bShowHelp = false, bShowOptions = false, bShowAchievements = false;
			SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

			MainMenu menu(pScreen, &config, &ac, &mouse);
			while(menu.Loop()){}
			if( menu.ShouldQuit() )
				break;
			bShowHelp = menu.ShouldShowHelp();
			bShowOptions = menu.ShouldShowOptions();
			bShowAchievements = menu.ShouldShowAchievements();

			if( bShowOptions ) {
				Options ops(pScreen, &config);
				while(ops.Loop()){}
				mouse.UpdateConfig(&config);
				continue;
			}
			else if( bShowHelp ) {
				DealHelp help(pScreen);
				while(help.Loop()){}
				continue;
			}
			else if( bShowAchievements ) {
				Achievements ach(pScreen, &ac, &mouse);
				while(ach.Loop()){}
				continue;
			}
			else {
				if( gameobj.GameInProgress() ) {
					while(isKeyPressed(KEY_NSPIRE_ENTER)){}
					if( 2 == show_msgbox_2b("Continue game?", "There is an unfinished game; would you like to continue that one or start a new one?", "Continue", "New game") ) {
					gameobj.EndCurrentGame();
					}
				}
				else {
					gameobj.EndCurrentGame();
				}
				bool bPlay = true;
				while( bPlay ) {
					Game game(pScreen, gameobj.StartGame(), &config, &ac, &mouse);
	
					/* Game loop */
					while(game.Loop()){}
	
					bPlay = false;
				}
			} 
		}
	}
	
	printf("Quitting SDL.\n");
    
    /* Shutdown all subsystems */
    SDL_Quit();

    printf("Quitting...\n");

	return 0;
}
