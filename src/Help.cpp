#include "Help.h"

DealHelp::DealHelp(SDL_Surface* pScreen)
: m_pScreen(pScreen)
{
	m_pFont = nSDL_LoadFont(NSDL_FONT_VGA, 0, 0, 0);
}

DealHelp::~DealHelp()
{
	nSDL_FreeFont(m_pFont);
}

bool DealHelp::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;
	
	UpdateDisplay();
	
	return true;
}

bool DealHelp::PollEvents()
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
					case SDLK_RETURN:
					case SDLK_SPACE:
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
	return true;
}

void DealHelp::UpdateDisplay()
{
	SDL_FillRect(m_pScreen, NULL, SDL_MapRGB(m_pScreen->format, 153, 153, 255));

	nSDL_DrawString(m_pScreen, m_pFont, 15, 20, 
"Deal or No Deal is a game.\n\
There are 26 cases each containing\n\
money from a lot ($1,000,000) to\n\
very little ($0.01).\n\
\n\
You want the high value case; however\n\
the banker wants you to take a deal\n\
for the lesser value.\n\
\n\
Your object is to get the highest\n\
amount you can!\n\
\n\
Good luck!\n\
\n\
Also have fun!");		
	
	SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}




