#include "BankOffer.h"
#include "ScreenObjects.h"
#include "Config.h"
#include "MouseHandling.h"
#include "Defines.h"

extern "C"
{
#include "SDL/SDL_gfxPrimitives.h"
}

#define TOP_BUTTONS_Y		(180)
#define DEAL_BUTTONS_SPACING	(15)
#define DEAL_BUTTONS_HEIGHT	(50)

BankOffer::BankOffer(SDL_Surface* pScreen, DealOrNoDealLib deal, ScreenObjectsControl  *pScreenObjects, MouseHandling* pMouse, int nOffer, int nMoreCases, Config* pConfig)
: m_pScreen(pScreen), m_Deal(deal), m_pScreenObjects(pScreenObjects), m_pMouse(pMouse), m_nOffer(nOffer), m_nMoreCases(nMoreCases), m_bAcceptedOffer(false), m_eSelection(None)
{
	m_pFont = nSDL_LoadFont(NSDL_FONT_VGA, 0, 0, 0);
	m_pFontButtons = nSDL_LoadFont(NSDL_FONT_VGA, 255, 255, 255);
}

BankOffer::~BankOffer()
{
	nSDL_FreeFont(m_pFont);
	nSDL_FreeFont(m_pFontButtons);
}

bool BankOffer::AcceptedOffer() const
{
	return m_bAcceptedOffer;
}

bool BankOffer::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;
	
	UpdateDisplay();
	
	return true;
}

bool BankOffer::PollEvents()
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
					
					case SDLK_RETURN:
					case SDLK_LCTRL:
					case SDLK_5:
						if( m_eSelection != None ) {
							m_bAcceptedOffer = m_eSelection == Deal;
							return false;
						}
						break;

					case SDLK_CLEAR:
					case SDLK_DELETE:
					case SDLK_BACKSPACE:
						m_bAcceptedOffer = false;
						return false;
						break;

					case SDLK_LEFT:
					case SDLK_4:
						m_eSelection = Deal;
						break;

					case SDLK_RIGHT:
					case SDLK_6:
						m_eSelection = NoDeal;
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
                    if( m_eSelection != None ) {
                       m_bAcceptedOffer = m_eSelection == Deal;
                       return false;
                    }
                }
                else if( eMouseButton == LeftButton ) {
                    m_eSelection = Deal;
                }
                else if( eMouseButton == RightButton ) {
                    m_eSelection = NoDeal;
                }

                if( nMX != -1 && nMY != -1 )
                        Move(nMX, nMY);
        }

	return true;
}

void BankOffer::UpdateDisplay()
{
	m_pScreenObjects->Animate(ANIMATE_LABELS|ANIMATE_CURSOR);

	int nLeft = ScreenObjectsControl::GetLabelWidth();
	int nWidth = SCREEN_WIDTH - 2*ScreenObjectsControl::GetLabelWidth();
        boxRGBA(m_pScreen, nLeft, 0/*top*/, nLeft + nWidth/*right*/, SCREEN_HEIGHT/*Bottom*/, 100, 149, 237, 70);
	
	nSDL_DrawString(m_pScreen, m_pFont, nLeft + 15, 10, "The banker has an offer\n\
if you don't take this offer\n\
you will need to choose %d\n\
more cases", m_nMoreCases);

	int nTopBankOffer = 55;
        nSDL_DrawString(m_pScreen, m_pFont, nLeft + 15, nTopBankOffer, "The banks offer: %d", m_nOffer);

	//Prior offers
	int nRoundNumber = GetRoundNumber(m_Deal);
	if( nRoundNumber > 2/*round just increased since last case; plus need to be in round 2*/ ) {
           int nTopOffers = nTopBankOffer + 15;
	   nSDL_DrawString(m_pScreen, m_pFont, nLeft + 15, nTopOffers, "Prior bank offers");
           for(int i=1; i<(nRoundNumber-1/*Round just updated*/); i++) {
              int nPriorOffer = 0;
              GetPriorBankerAmount(m_Deal, i, &nPriorOffer);
              nSDL_DrawString(m_pScreen, m_pFont, nLeft + 15, nTopOffers + i*10, "Round %d: $%d", i, nPriorOffer);
           }
           if( nRoundNumber >= DEALORNODEAL_NUMBER_OF_ROUNDS ) {
              char buf[16];
              strcpy(buf, "!!!LAST CHANCE -- FINAL OFFER!!!");
              nSDL_DrawString(m_pScreen, m_pFont, nLeft + nWidth/2 - nSDL_GetStringWidth(m_pFont, buf)/2, nTopOffers + DEALORNODEAL_NUMBER_OF_ROUNDS*10, buf);
           }
	}

	//Deal/no deal buttons
	int nTopButtons = TOP_BUTTONS_Y;
	int nDealBtnSpacing = DEAL_BUTTONS_SPACING;
	int nDealBtnWidth = nWidth/2 - 2*nDealBtnSpacing;
	int nDealBtnHeight = DEAL_BUTTONS_HEIGHT;
	int nDealBtnLeft = nLeft + nDealBtnSpacing;
	int nDealBtnRight = nDealBtnLeft + nDealBtnWidth;
	int rdeal = 0, gdeal = 255, bdeal = 0, adeal = 120;
        if( !has_colors ) {
		rdeal = 139, gdeal = 136, bdeal = 120, adeal = 255;
	}
	if( m_eSelection == Deal )
		m_Color.AdjustColor(rdeal, gdeal, bdeal, adeal);
	boxRGBA(m_pScreen, nDealBtnLeft, nTopButtons, nDealBtnRight, nTopButtons+nDealBtnHeight, rdeal, gdeal, bdeal, adeal/*Alpha*/);

	int r=255, g=0, b=0, a=0;
	m_Color.AdjustColor(r,g,b,a);
        m_Color.Animate();

	if( m_eSelection == Deal )
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, r, g, b), nDealBtnLeft, nTopButtons, nDealBtnWidth, nDealBtnHeight, 5);

	int nNoDealBtnLeft = nLeft + nWidth/2 + nDealBtnSpacing;
	int nNoDealBtnRight = nNoDealBtnLeft + nDealBtnWidth;
	int rnodeal = 178, gnodeal = 34, bnodeal = 34, anodeal = 120;
	if( !has_colors ) {
		rnodeal = 139, gnodeal = 136, bnodeal = 120, anodeal = 255;
	}
	if( m_eSelection == NoDeal )
		m_Color.AdjustColor(rnodeal, gnodeal, bnodeal, anodeal);
	boxRGBA(m_pScreen, nNoDealBtnLeft, nTopButtons, nNoDealBtnRight, nTopButtons+nDealBtnHeight, rnodeal, gnodeal, bnodeal, anodeal/*Alpha*/);

	if( m_eSelection == NoDeal )
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, r, g, b), nNoDealBtnLeft, nTopButtons, nDealBtnWidth, nDealBtnHeight, 5);

	char buffer[80];
	strcpy(buffer, "Deal!");
	nSDL_DrawString(m_pScreen, m_pFontButtons, nDealBtnLeft + nDealBtnWidth/2 - nSDL_GetStringWidth(m_pFontButtons, buffer)/2, nTopButtons + nDealBtnHeight/2 - nSDL_GetStringHeight(m_pFontButtons, buffer)/2, buffer);

	strcpy(buffer, "No Deal!");
	nSDL_DrawString(m_pScreen, m_pFontButtons, nNoDealBtnLeft + nDealBtnWidth/2 - nSDL_GetStringWidth(m_pFontButtons, buffer)/2, nTopButtons + nDealBtnHeight/2 - nSDL_GetStringHeight(m_pFontButtons, buffer)/2, buffer);

	SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void BankOffer::Move(int nX, int nY)
{
   m_pScreenObjects->MoveCursor(nX, nY);

   int nLeft = ScreenObjectsControl::GetLabelWidth();
   int nWidth = SCREEN_WIDTH - 2*ScreenObjectsControl::GetLabelWidth();

   int nTopButtons = TOP_BUTTONS_Y;
   int nDealBtnSpacing = DEAL_BUTTONS_SPACING;
   int nDealBtnWidth = nWidth/2 - 2*nDealBtnSpacing;
   int nDealBtnHeight = DEAL_BUTTONS_HEIGHT;
   int nDealBtnLeft = nLeft + nDealBtnSpacing;
   int nDealBtnRight = nDealBtnLeft + nDealBtnWidth;

   int nNoDealBtnLeft = nLeft + nWidth/2 + nDealBtnSpacing;
   int nNoDealBtnRight = nNoDealBtnLeft + nDealBtnWidth;

   if( nX >= nDealBtnLeft && nY >= nTopButtons && nX <= nDealBtnRight && nY <= (nTopButtons+nDealBtnHeight) ) {
      m_eSelection = Deal;
   }
   else if( nX >= nNoDealBtnLeft && nY >= nTopButtons && nX <= nNoDealBtnRight && nY <= (nTopButtons+nDealBtnHeight) ) {
      m_eSelection = NoDeal;
   }
   else {
      m_eSelection = None;
   }
}


