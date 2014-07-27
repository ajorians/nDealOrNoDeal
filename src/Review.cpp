#include "Review.h"
#include "ScreenObjects.h"
#include "Config.h"
#include "MouseHandling.h"
#include "Defines.h"

extern "C"
{
#include "SDL/SDL_gfxPrimitives.h"
}

Review::Review(SDL_Surface* pScreen, Config* pConfig, MouseHandling* pMouse, int nOffer, int nCaseContained, bool bAcceptedOffer)
: m_pScreen(pScreen), m_pConfig(pConfig), m_pMouse(pMouse), m_nOffer(nOffer), m_nCaseContained(nCaseContained), m_bAcceptedOffer(bAcceptedOffer)
{
	m_pFont = nSDL_LoadFont(NSDL_FONT_VGA, 0, 0, 0);
}

Review::~Review()
{
	nSDL_FreeFont(m_pFont);
}

bool Review::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;
	
	UpdateDisplay();
	
	return true;
}

bool Review::PollEvents()
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
						return false;
						break;

					case SDLK_CLEAR:
					case SDLK_DELETE:
					case SDLK_BACKSPACE:
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
        }

	return true;
}

void Review::UpdateDisplay()
{
	//m_pScreenObjects->Animate(ANIMATE_LABELS);

	int nLeft = ScreenObjectsControl::GetLabelWidth();
        int nWidth = SCREEN_WIDTH - 2*ScreenObjectsControl::GetLabelWidth();

	boxRGBA(m_pScreen, nLeft, 0/*top*/, nLeft + nWidth/*right*/, SCREEN_HEIGHT/*Bottom*/, 100, 149, 237, 70);

	char bufContained[10];
        if( m_nCaseContained == 0 ) {
           strcpy(bufContained, "0.01");
        }
        else {
            Deal_itoa(m_nCaseContained, bufContained, 10);
        }

	if( m_bAcceptedOffer ) {
		nSDL_DrawString(m_pScreen, m_pFont, nLeft + 15, 10,
"You accepted the offer of %d;\n\
the case you picked contained:\n\
%s", m_nOffer, bufContained);

		if( m_nOffer >= m_nCaseContained ) {
			nSDL_DrawString(m_pScreen, m_pFont, nLeft + 15, 55,
"Because you accepted you got\n\
more money that if you kept\n\
going your points increased!");
		}
		else {
			nSDL_DrawString(m_pScreen, m_pFont, nLeft + 15, 55,
"Because you kept going you got\n\
less money that if you accepted\n\
the offer so your points\n\
decreased!");
		}
		
	}
	else {
		nSDL_DrawString(m_pScreen, m_pFont, nLeft + 15, 10,
"The case you picked\n\
contained %s; the last offer\n\
was %d", bufContained, m_nOffer);

		if( m_nCaseContained >= m_nOffer ) {
			nSDL_DrawString(m_pScreen, m_pFont, nLeft + 15, 55,
"Your case was worth more than\n\
the last offer so your points\n\
increased!");
		}
		else {
			nSDL_DrawString(m_pScreen, m_pFont, nLeft + 15, 55,
"Your case had less than the\n\
offer so your points decreased.");
		}
	}

	//Show player standing
        char buffer[80];
        char buf[10];
        Deal_itoa(m_pConfig->GetPlayerStanding(), buf, 10);
        strcpy(buffer, "Current player standing: ");
        strcat(buffer, buf);
        nSDL_DrawString(m_pScreen, m_pFont, SCREEN_WIDTH/2-nSDL_GetStringWidth(m_pFont, buffer)/2, 95, buffer);


	SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}




