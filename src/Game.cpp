#include "Game.h"
#include "BankOffer.h"
#include "GameBackground.h"
#include "BriefcaseGraphic.h"
#include "LabelGraphic.h"
#include "Review.h"
#include "Config.h"
#include "AchieveConfig.h"
#include "MouseHandling.h"
#include "Defines.h"

extern "C"
{
#include "SDL/SDL_gfxPrimitives.h"
}

CaseSprite* CreateCaseSprite(int nNumber, nSDL_Font* pFont)
{
	const int nCaseWidth = ScreenObjectsControl::GetCaseWidth(), nCaseHeight = ScreenObjectsControl::GetCaseHeight();
	CaseSprite* pCase = (CaseSprite*)malloc(sizeof(struct CaseSprite));

        pCase->imgOriginal = nSDL_LoadImage(image_Briefcase);
        pCase->img = SDL_CreateRGBSurface(SDL_SWSURFACE, nCaseWidth, nCaseHeight, 16, 0, 0, 0, 0);

	SDL_SetColorKey(pCase->imgOriginal, SDL_SRCCOLORKEY, SDL_MapRGB(pCase->imgOriginal->format, 0, 0, 0));

	char buffer[5];
	Deal_itoa(nNumber, buffer, 5);
	nSDL_DrawString(pCase->imgOriginal, pFont, nCaseWidth/2-nSDL_GetStringWidth(pFont, buffer)/2, nCaseHeight/2, buffer);
        pCase->replace = SDL_CreateRGBSurface(SDL_SWSURFACE, nCaseWidth, nCaseHeight, 16, 0, 0, 0, 0);

	SDL_BlitSurface(pCase->imgOriginal, NULL, pCase->img, NULL);
	SDL_SetColorKey(pCase->img, SDL_SRCCOLORKEY, SDL_MapRGB(pCase->img->format, 0, 0, 0));

	int nX = ScreenObjectsControl::GetCaseX(nNumber), nY = ScreenObjectsControl::GetCaseY(nNumber);
        pCase->x = nX;
        pCase->y = nY;
        pCase->toX = nX;
        pCase->toY = nY;
        pCase->width = nCaseWidth;
        pCase->height = nCaseHeight;

        pCase->bSelected = false;
        pCase->nSelectedAmount = 0;

	pCase->bToFade = false;
	pCase->nFadeAmount = 0;

        pCase->nNumber = nNumber;
	return pCase;
}

AmountLabelSprite* CreateLabelSprite(int nAmount, nSDL_Font* pFont)
{
	const int nLabelWidth = ScreenObjectsControl::GetLabelWidth(), nLabelHeight = ScreenObjectsControl::GetLabelHeight();
	const int nX = ScreenObjectsControl::GetLabelX(nAmount), nY = ScreenObjectsControl::GetLabelY(nAmount);

	AmountLabelSprite* pLabel = (AmountLabelSprite*)malloc(sizeof(struct AmountLabelSprite));
        pLabel->img = nSDL_LoadImage(image_Label);
	char buffer[10];
        Deal_itoa(nAmount, buffer, 10);
	if( nAmount == 0 )
		strcpy(buffer, "0.01");
	char text[15];
	strcpy(text, "$");
	strcat(text, buffer);
        nSDL_DrawString(pLabel->img, pFont, strlen(text) < 7 ? nLabelWidth/2-nSDL_GetStringWidth(pFont, text)/2 : 0, nLabelHeight/2-nSDL_GetStringHeight(pFont, text)/2, text);
        pLabel->replace = SDL_CreateRGBSurface(SDL_SWSURFACE, nLabelWidth, nLabelHeight, 16, 0, 0, 0, 0);

        pLabel->x = nX;
        pLabel->y = nY;
        pLabel->toX = nX;
        pLabel->toY = nY;
        pLabel->width = ScreenObjectsControl::GetLabelWidth();
        pLabel->height = ScreenObjectsControl::GetLabelHeight();

        pLabel->bToShade = false;
        pLabel->nShadeAmount = 0;

        pLabel->nAmount = nAmount;
        return pLabel;
}

Game::Game(SDL_Surface* pScreen, DealOrNoDealLib Deal, Config* pConfig, AchieveConfig* pAchieve, MouseHandling* pMouse)
: m_pScreen(pScreen), m_pConfig(pConfig), m_pAchieve(pAchieve), m_pBackground(NULL), m_Objs(pScreen, pConfig), m_pMouse(pMouse), m_nCurrentCase(-1), m_Deal(Deal)
{
	m_pFont = nSDL_LoadFont(NSDL_FONT_VGA, 5, 5, 5);//Just so it isn't black & SetColorKey don't transparent it
	m_pFontSmall = nSDL_LoadFont(NSDL_FONT_TINYTYPE, 0, 0, 0);

	m_pBackground = nSDL_LoadImage(image_gamebackground);

	for(unsigned i=0; i<DEALORNODEAL_NUMBER_OF_CASES; i++) {
		m_Objs.AddCaseSprite(CreateCaseSprite(i+1, m_pFont));
		int nAmount;
		GetIndexAmount(i, &nAmount);
		m_Objs.AddAmountLabelSprite(CreateLabelSprite(nAmount, m_pFontSmall));
        }
	int nPickedCase = 0;
        if( DEALORNODEAL_CASE_PICKED == HavePickedCase(m_Deal, &nPickedCase) ) {
                m_Objs.HideCase(nPickedCase+1);
		for(unsigned i=0; i<DEALORNODEAL_NUMBER_OF_CASES; i++) {
			if( DEALORNODEAL_CASEOPENED == IsCaseOpened(m_Deal, i) ) {
                        	m_Objs.HideCase(i+1);
	                        m_Objs.ShadeAmountLabel(GetCaseAmount(m_Deal, i));
                	}
		}
	}
}

Game::~Game()
{
	SDL_FreeSurface(m_pBackground);
	nSDL_FreeFont(m_pFont);
	nSDL_FreeFont(m_pFontSmall);
}

bool Game::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;

	//Check game state conditions
	if( CheckGameState() == false )
		return false;

	//Update screen
	UpdateDisplay();
	
	SDL_Delay(30);
	
	return true;
}

bool Game::CheckGameState()
{
	if( m_Objs.HasMessage() && !m_Objs.IsLettersAnimating() && m_Objs.LettersStayDuration() <= 0 ) {
		int nCaseAmount = GetPickedCaseAmount(m_Deal);
		int nOffer = 0;
		GetPriorBankerAmount(m_Deal, DEALORNODEAL_NUMBER_OF_ROUNDS-1, &nOffer);
		m_pAchieve->JustPlayedAGame();
		Review r(m_pScreen, m_pConfig, m_pMouse, nOffer, nCaseAmount, false);
		while(r.Loop()){}
		return false;
        }
	return true;
}

bool Game::PollEvents()
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
						fprintf(stderr, "Hit Escape!n");
					return false;
					break;

					case SDLK_RETURN:
                                        case SDLK_LCTRL:
                                        case SDLK_RCTRL:
                                        case SDLK_5:
						if( !SelectCase(false) ) {
							return false;
						}
                                        break;

					case SDLK_UP:
					case SDLK_8:
						Move(Up);
						break;

					case SDLK_DOWN:
					case SDLK_2:
						Move(Down);
						break;
	
					case SDLK_RIGHT:
					case SDLK_6:
						Move(Right);
						break;
					
					case SDLK_LEFT:
					case SDLK_4:
						Move(Left);
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
		if( eMouseButton == CenterButton ) {
                    if( !SelectCase(true) )
			return false;
                }
                else if( eMouseButton == LeftButton ) {
                    Move(Left);
                }
                else if( eMouseButton == RightButton ) {
                    Move(Right);
                }
                else if( eMouseButton == UpButton ) {
                    Move(Up);
                }
                else if( eMouseButton == DownButton ) {
                    Move(Down);
                }

		if( nMX != -1 && nMY != -1 )
			Move(nMX, nMY);
	}

	return true;
}

void Game::UpdateDisplay()
{
	//Draw background
	if( !is_classic ) {
		SDL_BlitSurface(m_pBackground, NULL, m_pScreen, NULL);
	} else {
		SDL_FillRect(m_pScreen, NULL, SDL_MapRGB(m_pScreen->format, 255, 255, 255));
	}

	int nCases = 0, nCasesRemaining = 0;
	HowManyCasesToChoose(m_Deal, &nCases, &nCasesRemaining);
	m_Objs.DisplayMessage(GetRoundNumber(m_Deal), nCases, nCasesRemaining);

	if( !m_Objs.Animate() )
	{
		SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}
}

void Game::Move(Direction eDirection)
{
	int nPickedCaseIndex = -1;
	int nPickedCase = HavePickedCase(m_Deal, &nPickedCaseIndex);

        if( m_nCurrentCase <= 0 ) {
           for(m_nCurrentCase=1; m_nCurrentCase <= DEALORNODEAL_NUMBER_OF_CASES; m_nCurrentCase++) {
              if( DEALORNODEAL_CASE_PICKED == nPickedCase ) {
                 if( m_nCurrentCase == (nPickedCaseIndex+1) )
                    continue;
              }
              if( DEALORNODEAL_CASENOTOPENED == IsCaseOpened(m_Deal, m_nCurrentCase-1/*index*/) )
                 break; 
           }
        }
        else {
		if( eDirection == Up ) {
			if( ScreenObjectsControl::GetCaseRow(m_nCurrentCase) > 0 ) {
				int nCase;
				for(nCase = m_nCurrentCase+(ScreenObjectsControl::GetCaseRow(m_nCurrentCase) == 4 ? (ScreenObjectsControl::GetCaseCol(m_nCurrentCase) == 5 ? 5 : 6) : 5); nCase <= DEALORNODEAL_NUMBER_OF_CASES; nCase+=5) {
					if( DEALORNODEAL_CASE_PICKED == nPickedCase && nCase == (nPickedCaseIndex+1) )
						continue;

					if( DEALORNODEAL_CASENOTOPENED == IsCaseOpened(m_Deal, nCase-1/*index*/) )
						break;
				}
				if( nCase <= DEALORNODEAL_NUMBER_OF_CASES ) {
					m_nCurrentCase = nCase;
				}
			}
		}
		else if( eDirection == Down ) {
			if( ScreenObjectsControl::GetCaseRow(m_nCurrentCase) < 4 ) {
				int nCase;
                                for(nCase = m_nCurrentCase-5; nCase >= 1; nCase-=5) {
                                        if( DEALORNODEAL_CASE_PICKED == nPickedCase && nCase == (nPickedCaseIndex+1) ) {
						if( ScreenObjectsControl::GetCaseRow(nCase) == 4 && 
							(DEALORNODEAL_CASEOPENED == IsCaseOpened(m_Deal, nCase-1-1/*Index and row 3 special case*/))) {
	                                                continue;
						}
						else {
							nCase--;
							break;
						}
					}

                                        if( DEALORNODEAL_CASENOTOPENED == IsCaseOpened(m_Deal, nCase-1/*index*/) ) {
                                                break;
					}
					else if( ScreenObjectsControl::GetCaseRow(nCase) == 4 &&
						!((DEALORNODEAL_CASE_PICKED == nPickedCase && (nCase-1) == (nPickedCaseIndex)) ||
                                                        (DEALORNODEAL_CASENOTOPENED == IsCaseOpened(m_Deal, nCase-1-1/*Index and row 3 special case*/))) ) {
						nCase -=6;
						break;
					}
                                }
                                if( nCase >= 1 ) {
                                        m_nCurrentCase = nCase;
                                }
			}
		}
		else if( eDirection == Left ) {
			int nCase;
			for(nCase = m_nCurrentCase-1; nCase >= 1; nCase--) {
				if( DEALORNODEAL_CASE_PICKED == nPickedCase && nCase == (nPickedCaseIndex+1) )
					continue;

				if( DEALORNODEAL_CASENOTOPENED == IsCaseOpened(m_Deal, nCase-1/*index*/) )
					break;
			}
			if( nCase >= 1 ) {
				m_nCurrentCase = nCase;
			}
		}
		else {//Right
			int nCase;
                        for(nCase = m_nCurrentCase+1; nCase <= DEALORNODEAL_NUMBER_OF_CASES; nCase++) {
                               if( DEALORNODEAL_CASE_PICKED == nPickedCase && nCase == (nPickedCaseIndex+1) )
                                     continue;

                               if( DEALORNODEAL_CASENOTOPENED == IsCaseOpened(m_Deal, nCase-1/*index*/) )
                                     break;
                        }
                        if( nCase <= DEALORNODEAL_NUMBER_OF_CASES ) {
                                m_nCurrentCase = nCase;
			}
		}
	}

	m_Objs.SelectCase(m_nCurrentCase);
}

void Game::Move(int nX, int nY)
{
   m_Objs.MoveCursor(nX, nY);
   int nPickedCaseIndex = -1;
   int nPickedCase = HavePickedCase(m_Deal, &nPickedCaseIndex);
   int nChooseCase = -1;
   for(int nCase = 1; nCase <= DEALORNODEAL_NUMBER_OF_CASES; nCase++) {
      if( ScreenObjectsControl::GetCaseX(nCase) > nX || ScreenObjectsControl::GetCaseY(nCase) > nY || 
          (ScreenObjectsControl::GetCaseX(nCase)+ScreenObjectsControl::GetCaseWidth()) < nX ||
          (ScreenObjectsControl::GetCaseY(nCase)+ScreenObjectsControl::GetCaseHeight()) < nY )
           continue; 

      if( DEALORNODEAL_CASE_PICKED == nPickedCase && nCase == (nPickedCaseIndex+1) )
           continue;

      if( DEALORNODEAL_CASENOTOPENED == IsCaseOpened(m_Deal, nCase-1/*index*/) ) {
           nChooseCase = nCase;
           break;
      }
   }
   m_nCurrentCase = nChooseCase;
   m_Objs.SelectCase(nChooseCase);
}

bool Game::SelectCase(bool bMouse)
{
   if( m_nCurrentCase <= 0 )
      return true;//False means get out of game

   if( DEALORNODEAL_CASE_NOT_PICKED == HavePickedCase(m_Deal, NULL) ) {
      if( DEALORNODEALLIB_OK == PickYourCase(m_Deal, m_nCurrentCase-1) ) {
         m_Objs.HideCase(m_nCurrentCase);
         PickNewCurrentCase();
      }
   }
   else {
      int nAmount = 0, nRoundChange = DEALORNODEAL_SAMEROUND;
      if( DEALORNODEALLIB_OK == ChooseCase(m_Deal, m_nCurrentCase-1, &nAmount, &nRoundChange) ) {
         m_Objs.HideCase(m_nCurrentCase);
         m_Objs.ShadeAmountLabel(nAmount);
         PickNewCurrentCase();
         if( nRoundChange == DEALORNODEAL_ROUNDCHANGE ) {
            int nRound = GetRoundNumber(m_Deal);

            int nCaseContained = GetPickedCaseAmount(m_Deal);

            if( nRound > DEALORNODEAL_NUMBER_OF_ROUNDS ) {
		

                int nOffer = 0;
                GetPriorBankerAmount(m_Deal, DEALORNODEAL_NUMBER_OF_ROUNDS-1, &nOffer);

		printf("Last offer: %d; Case contained: %d\n", nOffer, nCaseContained);
                if( nCaseContained >= nOffer ) {
                        m_pConfig->AdjustPlayerStanding(10);
                }
                else {
                        m_pConfig->AdjustPlayerStanding(-10);
                }

               //Game over; show what was in their case
               char buffer[80];
               strcpy(buffer, "Your case\ncontained\n$");
               char buf[10];
               Deal_itoa(GetPickedCaseAmount(m_Deal), buf, 10);
               strcat(buffer, buf);
               m_Objs.CreateMessage(buffer);
            }
            else if( nRound > 1 ) {
               //Show banker offer
               int nOffer = 0;
               GetBankerAmount(m_Deal, &nOffer);
               int nCases = 0, nCasesRemaining = 0;
               HowManyCasesToChoose(m_Deal, &nCases, &nCasesRemaining);
               BankOffer offer(m_pScreen, m_Deal, &m_Objs, m_pMouse, nOffer, nCasesRemaining, m_pConfig);
               while(offer.Loop()){}

               if( offer.AcceptedOffer() ) {
                  SetAcceptedOffer(m_Deal);
                  m_pAchieve->JustAcceptedADeal();

                  if( nCaseContained >= nOffer ) {
                     m_pConfig->AdjustPlayerStanding(10);
                  }
                  else {
                     m_pConfig->AdjustPlayerStanding(-10);
                  }

                  m_pAchieve->JustPlayedAGame();
                  Review r(m_pScreen, m_pConfig, m_pMouse, nOffer, nCaseContained, true);
                  while(r.Loop()){}
                  return false;
               }
            }
         }
      }
   }
   m_pAchieve->LookForAchievements(m_Deal);
   return true;
}

void Game::PickNewCurrentCase()
{
   int nCase;
   int nPickedCaseIndex = -1;
   int nPickedCase = HavePickedCase(m_Deal, &nPickedCaseIndex);
   for(nCase = m_nCurrentCase+1; nCase <= DEALORNODEAL_NUMBER_OF_CASES; nCase++) {
      if( DEALORNODEAL_CASE_PICKED == nPickedCase && nCase == (nPickedCaseIndex+1) )
         continue;

      if( DEALORNODEAL_CASENOTOPENED == IsCaseOpened(m_Deal, nCase-1/*index*/) )
         break;
   }

   if( nCase <= DEALORNODEAL_NUMBER_OF_CASES ) {
      m_nCurrentCase = nCase;
      m_Objs.SelectCase(m_nCurrentCase);
   }
   else {
      for(nCase = m_nCurrentCase-1; nCase > 0; nCase--) {
         if( DEALORNODEAL_CASE_PICKED == nPickedCase && nCase == (nPickedCaseIndex+1) )
            continue;

         if( DEALORNODEAL_CASENOTOPENED == IsCaseOpened(m_Deal, nCase-1/*index*/) )
            if( nCase > 0 ) {
               m_nCurrentCase = nCase;
               m_Objs.SelectCase(m_nCurrentCase);
               break;
             }
      }
   }
}

