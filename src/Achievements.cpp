#include "Achievements.h"
#include "AchieveConfig.h"
#include "Config.h"
#include "MouseHandling.h"
#include "DealGraphic.h"
#include "PlayGraphic.h"
#include "PlayGraphic2.h"
#include "MoneyGraphic.h"
#include "CloseCallGraphic.h"
#include "Defines.h"

#define ACHIEVEMENT_ADJUST_AMOUNT      (75)

void AdjustAchievementImage(SDL_Surface* pSurface, bool bAchieved)
{
   //If the surface must be locked 
   if( SDL_MUSTLOCK( pSurface ) )
   {
      //Lock the surface 
      SDL_LockSurface( pSurface );
   }

   //Go through columns 
   for( int x = 0; x < pSurface->w; x++ )
   {
      //Go through rows 
      for( int y = 0; y < pSurface->h; y++ )
      {
         Uint32 pixel = nSDL_GetPixel(pSurface, x, y);
         Uint8 r = 0, g = 0, b = 0;
         SDL_GetRGB(pixel, pSurface->format, &r, &g, &b);
         if( r == 0 && g == 0 && b == 0 ) continue;
         if( bAchieved ) {
                 r = Deal_Min(255, r+ACHIEVEMENT_ADJUST_AMOUNT);
                 g = Deal_Min(255, g+ACHIEVEMENT_ADJUST_AMOUNT);
                 b = Deal_Min(255, b+ACHIEVEMENT_ADJUST_AMOUNT);
        }
        else {
                r = Deal_Max(0, r-ACHIEVEMENT_ADJUST_AMOUNT);
                g = Deal_Max(0, g-ACHIEVEMENT_ADJUST_AMOUNT);
                b = Deal_Max(0, b-ACHIEVEMENT_ADJUST_AMOUNT);
        }

         nSDL_SetPixel(pSurface, x, y, SDL_MapRGB(pSurface->format, r, g, b));
      }
   }

   //Unlock surface 
   if( SDL_MUSTLOCK( pSurface ) )
   {
      SDL_UnlockSurface( pSurface );
   }
}

SDL_Surface* LoadImage(Uint16 *data)
{
   SDL_Surface* pSurface = nSDL_LoadImage(data);
   SDL_SetColorKey(pSurface, SDL_SRCCOLORKEY, SDL_MapRGB(pSurface->format, 0, 0, 0));
   return pSurface;
}

Achievements::Achievements(SDL_Surface* pScreen, AchieveConfig* pAchieveConfig, MouseHandling* pMouse)
: m_pScreen(pScreen), m_eChoice(Won1Million), m_pAchieveConfig(pAchieveConfig), m_pMouse(pMouse), m_Progress(pScreen, 40, 190, SCREEN_WIDTH-40-40, 15)
{
	m_pFont = nSDL_LoadFont(NSDL_FONT_THIN, 0, 0, 0);
	nSDL_SetFontSpacing(m_pFont, 0, 3);

	m_pFontRed = nSDL_LoadFont(NSDL_FONT_THIN, 255, 0, 0);

	m_imgDeal = LoadImage(image_Deal);
	m_imgPlay = LoadImage(image_Play);
	m_imgPlay2 = LoadImage(image_Play2);
	m_imgMoney = LoadImage(image_Money);
	m_imgCloseCall = LoadImage(image_CloseCall);

	m_imgDkDeal = LoadImage(image_Deal);
        m_imgDkPlay = LoadImage(image_Play);
        m_imgDkPlay2 = LoadImage(image_Play2);
        m_imgDkMoney = LoadImage(image_Money);
	m_imgDkCloseCall = LoadImage(image_CloseCall);
	AdjustAchievementImage(m_imgDkDeal, false);
	AdjustAchievementImage(m_imgDkPlay, false);
	AdjustAchievementImage(m_imgDkPlay2, false);
	AdjustAchievementImage(m_imgDkMoney, false);
	AdjustAchievementImage(m_imgDkCloseCall, false);
}

Achievements::~Achievements()
{
	nSDL_FreeFont(m_pFont);

	SDL_FreeSurface(m_imgDeal);
	SDL_FreeSurface(m_imgPlay);
	SDL_FreeSurface(m_imgPlay2);
	SDL_FreeSurface(m_imgMoney);
	SDL_FreeSurface(m_imgCloseCall);
}

bool Achievements::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;
	
	UpdateDisplay();
	
	return true;
}

bool Achievements::PollEvents()
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
				switch (event.key.keysym.sym) 
				{
					case SDLK_ESCAPE:
						return false;
					break;

					case SDLK_SPACE:
                                        case SDLK_RETURN:
                                        case SDLK_LCTRL:
                                                break;

					case SDLK_UP:
                                        case SDLK_8:
						Move(A_Up);
                                                break;

                                        case SDLK_DOWN:
                                        case SDLK_2:
						Move(A_Down);
                                                break;

					case SDLK_LEFT:
					case SDLK_4:
						Move(A_Left);
						break;

					case SDLK_RIGHT:
					case SDLK_6:
						Move(A_Right);
						break;
					
					default:
						break;
				}

				break;
			
			//Called when the mouse moves
			case SDL_MOUSEMOTION:
				break;
			
			case SDL_KEYUP:
				break;
			
			default:
				break;
		}
	}

	int nMX = -1, nMY = -1;
        if( m_pMouse->PollMouse(nMX, nMY) ) {
                MouseButton eMouseButton = m_pMouse->GetMouseButton();
                if( eMouseButton == LeftButton ) {
                    Move(A_Left);
                }
                else if( eMouseButton == RightButton ) {
                    Move(A_Right);
                }

//                if( nMX != -1 && nMY != -1 )
//                        Move(nMX, nMY);
        }

	return true;
}

void Achievements::Move(Achievement_Direction eDirection)
{
	if( m_eChoice == None ) {
		m_eChoice = Won1Million;
		goto Exit;
	}
	if( eDirection == A_Up ) {
		if( m_eChoice == HadCloseCall ) {
			m_eChoice = Played10Games;
		}
		else if( m_eChoice == Accepted10Deals ) {
			m_eChoice = Won1Million;
		}
	}
	else if( eDirection == A_Down ) {
		if( m_eChoice == Won1Million ) {
			m_eChoice = Accepted10Deals;
		}
		else {
			m_eChoice = HadCloseCall;
		}
	}
	else if( eDirection == A_Left ) {
		if( m_eChoice == Played10Games ) {
			m_eChoice = Won1Million;
		}
		else if( m_eChoice == Played50Games ) {
			m_eChoice = Played10Games;
		}
		else if( m_eChoice == HadCloseCall ) {
			m_eChoice = Accepted10Deals;
		}
	}
	else if( eDirection == A_Right ) {
		if( m_eChoice == Won1Million ) {
			m_eChoice = Played10Games;
		}
		else if( m_eChoice == Played10Games ) {
			m_eChoice = Played50Games;
		}
		else if( m_eChoice == Accepted10Deals ) {
			m_eChoice = HadCloseCall;
		}
	}

Exit:
	Config* pConfig = m_pAchieveConfig->GetConfig();
	switch(m_eChoice) {
	default:
	case None:
		m_Progress.SetDoneAmount(0,0);
		break;
	case Won1Million:
		m_Progress.SetDoneAmount(Deal_Min(1, pConfig->GetTimesWon1Million()), 1);
		break;
	case Played10Games:
		m_Progress.SetDoneAmount(Deal_Min(10, pConfig->GetGamesPlayed()), 10);
		break;
	case Played50Games:
		m_Progress.SetDoneAmount(Deal_Min(50, pConfig->GetGamesPlayed()), 50);
		break;
	case Accepted10Deals:
		m_Progress.SetDoneAmount(Deal_Min(10, pConfig->GetDealsAccepted()), 10);
		break;
	case HadCloseCall:
		m_Progress.SetDoneAmount(Deal_Min(1, pConfig->GetCloseCalls()), 1);
		break;
	}
}

void Achievements::UpdateDisplay()
{
	//Draw background
	SDL_FillRect(m_pScreen, NULL, SDL_MapRGB(m_pScreen->format, 153, 153, 255/*I don't know a better color :( */));
	nSDL_DrawString(m_pScreen, m_pFont, (SCREEN_WIDTH-nSDL_GetStringWidth(m_pFont, "Achievements:"))/2, 15, "Achievements:");

	SDL_Rect rect;
	rect.w = 32;
	rect.h = 32;

	rect.x = 30;
        rect.y = 30;
	SDL_BlitSurface(m_pAchieveConfig->HasWonAMillion() ? m_imgMoney : m_imgDkMoney, NULL, m_pScreen, &rect);

	rect.x = 80;
        rect.y = 30;
	SDL_BlitSurface(m_pAchieveConfig->HasPlayed10Games() ? m_imgPlay : m_imgDkPlay, NULL, m_pScreen, &rect);

	rect.x = 130;
        rect.y = 30;
        SDL_BlitSurface(m_pAchieveConfig->HasPlayed50Games() ? m_imgPlay2 : m_imgDkPlay2, NULL, m_pScreen, &rect);

	rect.x = 30;
        rect.y = 80;
        SDL_BlitSurface(m_pAchieveConfig->HasAccepted10Deals() ? m_imgDeal : m_imgDkDeal, NULL, m_pScreen, &rect);

        rect.x = 80;
        rect.y = 80;
        SDL_BlitSurface(m_pAchieveConfig->HasHadCloseCall() ? m_imgCloseCall : m_imgDkCloseCall, NULL, m_pScreen, &rect);

	char buffer[256];

	int nTop = 120;
	int nLeft = 12;
	nSDL_DrawString(m_pScreen, m_pFont, nLeft, nTop, "Description:");

	bool bAchieved = false;
	if( m_eChoice == Won1Million ) {
		strcpy(buffer, "Won a million:\n\
Win 1 millions dollars!");
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), 30, 30, 32, 32, 1);
		bAchieved = m_pAchieveConfig->HasWonAMillion();
	}
	else if( m_eChoice == Played10Games ) {
		strcpy(buffer, "Played 10 games:\n\
Play at least 10 games whether you win\n\
or lose!");
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), 80, 30, 32, 32, 1);
		bAchieved = m_pAchieveConfig->HasPlayed10Games();
	}
	else if( m_eChoice == Played50Games ) {
                strcpy(buffer, "Played 50 games:\n\
Play at least 50 games whether you win\n\
or lose!");
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), 130, 30, 32, 32, 1);
		bAchieved = m_pAchieveConfig->HasPlayed50Games();
	}
	else if( m_eChoice == Accepted10Deals ) {
                strcpy(buffer, "Accepted 10 deals:\n\
Accept at least 10 deals while playing.");
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), 30, 80, 32, 32, 1);
		bAchieved = m_pAchieveConfig->HasAccepted10Deals();
	}
	else if( m_eChoice == HadCloseCall ) {
                strcpy(buffer, "Close call:\n\
The 2 cases (the one you picked and the\n\
one remaining) are 1 million and 1/2 million");
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), 80, 80, 32, 32, 1);
		bAchieved = m_pAchieveConfig->HasHadCloseCall();
	}

        nSDL_DrawString(m_pScreen, m_pFont, nLeft, nTop + 20, buffer);

	if( bAchieved ) {
		strcpy(buffer, "Achievement completed!");
		nSDL_DrawString(m_pScreen, m_pFontRed, SCREEN_WIDTH/2-nSDL_GetStringWidth(m_pFontRed, buffer)/2, nTop + 54, buffer);
	}
	else {
		strcpy(buffer, "Not yet achieved!");
		nSDL_DrawString(m_pScreen, m_pFontRed, SCREEN_WIDTH/2-nSDL_GetStringWidth(m_pFontRed, buffer)/2, nTop + 54, buffer);
	}

	m_Progress.UpdateDisplay();

	SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

}




