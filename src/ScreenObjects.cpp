#include "ScreenObjects.h"
#include "CursorGraphic.h"
#include "Config.h"
#include "Defines.h"
extern "C"
{
        #include "DealOrNoDealLib/DealOrNoDealLib.h"
}

#define PIECE_FADE_LIMIT		(4)
#define PIECE_BRIGHTEN_STEP_AMOUNT	(25)
#define PIECE_STEP_AMOUNT		(5)

#define MESSAGE_SPRITE_WIDTH    (20)
#define MESSAGE_SPRITE_HEIGHT   (20)
#define MESSAGE_LINE_SPACING    (10)
#define MESSAGE_SPRITE_SPACING  (10)
#define MESSAGE_STAY_DURATION   (40)

#define CASE_WIDTH		(35)
#define CASE_HEIGHT		(30)

#define LABEL_WIDTH		(44)
#define LABEL_HEIGHT		(10)

#define NUMBER_OF_ROWS		(5)

#define HALF_NUMBER_OF_CASES		((DEALORNODEAL_NUMBER_OF_CASES+1)/2)

ScreenObjectsControl::ScreenObjectsControl(SDL_Surface* pScreen, Config* pConfig)
: m_pScreen(pScreen), m_pConfig(pConfig)
{
   m_pFont = has_colors ? nSDL_LoadFont(NSDL_FONT_VGA, 255, 255, 255) : nSDL_LoadFont(NSDL_FONT_VGA, 0, 0, 0);
   m_pFontLetters = nSDL_LoadFont(NSDL_FONT_VGA, 0, 0, 0);

   CursorSprite* pCursor = (CursorSprite*)malloc(sizeof(struct CursorSprite));
   pCursor->img = nSDL_LoadImage(image_cursor);
   SDL_SetColorKey(pCursor->img, SDL_SRCCOLORKEY, SDL_MapRGB(pCursor->img->format, 0, 0, 0));
   const int nX = 0, nY = 0, nWidth = 16, nHeight = 16;
   pCursor->x = nX;
   pCursor->y = nY;
   pCursor->toX = nX;
   pCursor->toY = nY;
   pCursor->width = nWidth;
   pCursor->height = nHeight;
   pCursor->next = NULL;
   pCursor->replace = SDL_CreateRGBSurface(SDL_SWSURFACE, 16, nWidth, nHeight, 0, 0, 0, 0);
   pCursor->bVisible = false;
   m_Cursor.Add(pCursor);
}

ScreenObjectsControl::~ScreenObjectsControl()
{
   nSDL_FreeFont(m_pFont);
   nSDL_FreeFont(m_pFontLetters);
}

bool ScreenObjectsControl::AddCaseSprite(CaseSprite* pCase)
{
   m_Cases.Add(pCase);

   return true;
}

bool ScreenObjectsControl::SelectCase(int nNumber)
{
   CaseSprite* pCurrent = NULL;
   for(pCurrent = m_Cases.GetFirst(); pCurrent != NULL; pCurrent = (CaseSprite*)pCurrent->next) {
      if( pCurrent->nNumber == nNumber ) {
         pCurrent->bSelected = true;
         pCurrent->nSelectedAmount = 0;
         SDL_BlitSurface(pCurrent->imgOriginal, NULL, pCurrent->img, NULL);
         SDL_SetColorKey(pCurrent->img, SDL_SRCCOLORKEY, SDL_MapRGB(pCurrent->img->format, 0, 0, 0));
      }
      else {
         if( pCurrent->bSelected ) {
            SDL_BlitSurface(pCurrent->imgOriginal, NULL, pCurrent->img, NULL);
            SDL_SetColorKey(pCurrent->img, SDL_SRCCOLORKEY, SDL_MapRGB(pCurrent->img->format, 0, 0, 0));
         }
         pCurrent->bSelected = false;
      }
   }
   return true;
}

bool ScreenObjectsControl::HideCase(int nNumber)
{
   CaseSprite* pCurrent = NULL;
   for(pCurrent = m_Cases.GetFirst(); pCurrent != NULL; pCurrent = (CaseSprite*)pCurrent->next) {
      if( pCurrent->nNumber == nNumber ) {
         pCurrent->bToFade = true;
         break;
      }
   }

   return true;
}

int ScreenObjectsControl::GetCaseWidth() { return CASE_WIDTH; }
int ScreenObjectsControl::GetCaseHeight() { return CASE_HEIGHT; }
int ScreenObjectsControl::GetCaseRow(int nNumber)
{
   if( nNumber >=1 && nNumber < 7 )
   {
      return 4;
   }
   else if( nNumber >= 7 && nNumber < 12 )
   {
      return 3;
   }
   else if( nNumber >= 12 && nNumber < 17 )
   {
      return 2;
   }
   else if( nNumber >= 17 && nNumber < 22 )
   {
      return 1;
   }
   return 0;//For 22-26
}
int ScreenObjectsControl::GetCaseCol(int nNumber)
{
   if( nNumber < 7 )
      return nNumber - 1;

   nNumber -= 7;
   return nNumber%5;
}

int ScreenObjectsControl::GetCaseX(int nNumber)
{
   int nSpacing = 3;
   int nLeft = (SCREEN_WIDTH-6*GetCaseWidth()-(6-1)*nSpacing)/2;
   if( nNumber >=1 && nNumber < 7 )
   {
      return nLeft + GetCaseCol(nNumber)*GetCaseWidth() + GetCaseCol(nNumber)*nSpacing;
   }
   return nLeft + GetCaseWidth()/2 + GetCaseCol(nNumber)*GetCaseWidth() + GetCaseCol(nNumber)*nSpacing;
}

int ScreenObjectsControl::GetCaseY(int nNumber)
{
   int nSpacing = 13;
   int nTop = (SCREEN_HEIGHT-NUMBER_OF_ROWS * GetCaseHeight() - (NUMBER_OF_ROWS-1)*nSpacing)/2;
   return nTop + GetCaseRow(nNumber)*GetCaseHeight() + GetCaseRow(nNumber)*nSpacing;
}

bool ScreenObjectsControl::AddAmountLabelSprite(AmountLabelSprite* pLabel)
{
   m_Labels.Add(pLabel);

   return true;
}

bool ScreenObjectsControl::ShadeAmountLabel(int nAmount)
{
   AmountLabelSprite* pCurrent = NULL;
   for(pCurrent = m_Labels.GetFirst(); pCurrent != NULL; pCurrent = (AmountLabelSprite*)pCurrent->next) {
      if( pCurrent->nAmount == nAmount ) {
         pCurrent->bToShade = true;
         break;
      }
   }

   return true;
}

int IndexOfAmount(int nAmount)
{
   for(int i=0; i<DEALORNODEAL_NUMBER_OF_CASES; i++) {
      int nAmountSpot;
      GetIndexAmount(i, &nAmountSpot);
      if( nAmountSpot == nAmount )
         return i;
   }
   return -1;
}

int ScreenObjectsControl::GetLabelWidth() { return LABEL_WIDTH; }
int ScreenObjectsControl::GetLabelHeight() { return LABEL_HEIGHT; }
int ScreenObjectsControl::GetLabelRow(int nAmount)
{
   int nIndex = IndexOfAmount(nAmount);
   return nIndex < HALF_NUMBER_OF_CASES ? nIndex : nIndex - HALF_NUMBER_OF_CASES;
}

int ScreenObjectsControl::GetLabelCol(int nAmount)
{
   int nIndex = IndexOfAmount(nAmount);
   return nIndex < HALF_NUMBER_OF_CASES ? 0 : 1;
}

int ScreenObjectsControl::GetLabelX(int nAmount)
{
   return GetLabelCol(nAmount) == 0 ? 1 : SCREEN_WIDTH - GetLabelWidth()-1;
}

int ScreenObjectsControl::GetLabelY(int nAmount)
{
   int nSpaceRemaining = SCREEN_HEIGHT - HALF_NUMBER_OF_CASES*GetLabelHeight();
   int nSpacing = nSpaceRemaining/(HALF_NUMBER_OF_CASES+2);
   int nTop = SCREEN_HEIGHT - HALF_NUMBER_OF_CASES*GetLabelHeight() - (HALF_NUMBER_OF_CASES+1)*nSpacing;
   return nTop + GetLabelRow(nAmount)*GetLabelHeight() + GetLabelRow(nAmount)*nSpacing;
}

int GetLettersInLine(const char* pstrMessage)
{
   int nCount = 0;
   const char* pstr = pstrMessage;
   while(*pstr != '\0') {
      if( *pstr == '\n' )
         return nCount;
      pstr = pstr+1;
      nCount++;
   }
   return nCount;
}

int GetStartXForBonusMessage(int nLettersInLine)
{
   int nLengthForLine = (MESSAGE_SPRITE_WIDTH + MESSAGE_SPRITE_SPACING)*nLettersInLine;

   nLengthForLine -= MESSAGE_SPRITE_SPACING;//So flush with last letter!

   int nX = (SCREEN_WIDTH - nLengthForLine)/2;
   return nX;
}

bool ScreenObjectsControl::CreateMessage(const char* pstrMessage)
{
   ClearMessage();

   int nNumLines = 1;
   for(int n=0; n<strlen(pstrMessage); n++){
      if( *(pstrMessage+n) == '\n' )
         nNumLines++;
   }

   int nLettersInLine = GetLettersInLine(pstrMessage);
   int nX = GetStartXForBonusMessage(nLettersInLine);
   int nY = (SCREEN_HEIGHT - (nNumLines*(MESSAGE_SPRITE_HEIGHT+MESSAGE_LINE_SPACING)))/2;
   int nLettersIntoLine = 0;

   for(int n=0; n<strlen(pstrMessage); n++){
      LetterSprite* pSprite = (LetterSprite*)malloc(sizeof(LetterSprite));

      pSprite->img = SDL_CreateRGBSurface(SDL_SWSURFACE, MESSAGE_SPRITE_WIDTH, MESSAGE_SPRITE_HEIGHT, 16, 0, 0, 0, 0);

      SDL_FillRect(pSprite->img, NULL, SDL_MapRGB(m_pScreen->format, MESSAGE_BACKGROUND_R, MESSAGE_BACKGROUND_G, MESSAGE_BACKGROUND_B));

      char ch = *(pstrMessage+n);
      if( ch == '\n' ) {
         nLettersInLine = GetLettersInLine(pstrMessage+n+1/*Get past newline*/);
         nX = GetStartXForBonusMessage(nLettersInLine);
         nLettersIntoLine = 0;
         nY += (MESSAGE_SPRITE_HEIGHT+MESSAGE_LINE_SPACING);
         continue;
      }
      if( ch == ' ' ) {
         nLettersIntoLine++;
         continue;
      }

      nSDL_DrawString(pSprite->img, m_pFontLetters, MESSAGE_SPRITE_WIDTH/2-5, MESSAGE_SPRITE_HEIGHT/2-5, "%c", ch);
      pSprite->replace = SDL_CreateRGBSurface(SDL_SWSURFACE, MESSAGE_SPRITE_WIDTH, MESSAGE_SPRITE_HEIGHT, 16, 0, 0, 0, 0);

      pSprite->ch = ch;

      pSprite->x = rand() % 320;//I do random start position; I suppose it is alright :)
      pSprite->y = rand() % 240;

      int nDesiredX = nX + nLettersIntoLine*(MESSAGE_SPRITE_WIDTH + MESSAGE_SPRITE_SPACING);
      pSprite->toX = nDesiredX;
      pSprite->toY = nY;

      nLettersIntoLine++;

      m_Letters.Add(pSprite);
   }

   m_nStayDuration = MESSAGE_STAY_DURATION;

   return true;
}

bool ScreenObjectsControl::HasMessage() const
{
   return m_Letters.GetFirst() != NULL;
}

void ScreenObjectsControl::ClearMessage()
{
   m_Letters.ClearAll();
}

bool ScreenObjectsControl::MoveCursor(int nX, int nY)
{
   CursorSprite* pCursor = (CursorSprite*)m_Cursor.GetFirst();
   pCursor->x = nX;
   pCursor->y = nY;
   pCursor->bVisible = true;

   return true;
}

bool IsMoving(const BaseSprite* pSprite)
{
   return (pSprite->y != pSprite->toY) || (pSprite->x != pSprite->toX);
}

bool IsSelecting(BaseSprite* pSprite)
{
   return false;
//   return (pSprite->toFade == true && pSprite->nFadeAmount < PIECE_FADE_LIMIT);
}

bool ScreenObjectsControl::DisplayMessage(int nRound, int nCases, int nCasesRemaining)
{
   char strMessage[80];
   if( nRound == 0 ) {
      strcpy(strMessage, "Pick your case");
   }
   else if( nRound >= 1 && nRound <= 9 ) {
      strcpy(strMessage, "Round ");
      char buffer[5];

      Deal_itoa(nRound, buffer, 5);
      strcat(strMessage, buffer);

      strcat(strMessage, ": Pick ");
      Deal_itoa(nCases, buffer, 5);
      strcat(strMessage, buffer);

      strcat(strMessage, " cases");
      if( true ) {
         strcat(strMessage, " (");
         Deal_itoa(nCasesRemaining, buffer, 5);
         strcat(strMessage, buffer);
         strcat(strMessage, " remaining)");
      }
   }
   nSDL_DrawString(m_pScreen, m_pFont, SCREEN_WIDTH/2-nSDL_GetStringWidth(m_pFont, strMessage)/2, SCREEN_HEIGHT-10, strMessage);

   return true;
}

bool ScreenObjectsControl::IsLettersAnimating() const
{
   const BaseSprite* pCurrent = NULL;
   for(pCurrent = m_Letters.GetFirst(); pCurrent != NULL; pCurrent = pCurrent->next) {
      if( IsMoving(pCurrent) )
         return true;
   }

   return false;
}

int ScreenObjectsControl::LettersStayDuration() const
{
   return m_nStayDuration;
}

void SetBackground(SDL_Surface* pScreen, BaseSprite* pSprite)
{
   SDL_Rect rect;
   rect.x = pSprite->x;
   rect.y = pSprite->y;
   rect.w = pSprite->width;
   rect.h = pSprite->height;

   SDL_BlitSurface(pScreen, &rect, pSprite->replace, NULL);
}

void MoveSprite(SDL_Surface* pScreen, BaseSprite* pSprite)
{
   if( (pSprite->x != pSprite->toX) || (pSprite->y != pSprite->toY) ) {
      //int nDistX = pSprite->toX - pSprite->x;
      //int nDistY = pSprite->toY - pSprite->y;

      for(int n=0; n<PIECE_STEP_AMOUNT; n++) {
         if( pSprite->x < pSprite->toX ) {
            pSprite->x++;
         }
         if( pSprite->x > pSprite->toX ) {
            pSprite->x--;
         }
      }

      for(int n=0; n<5; n++) {
         if( pSprite->y < pSprite->toY ) {
            pSprite->y++;
         }
         if( pSprite->y > pSprite->toY ) {
            pSprite->y--;
         }
      }
   }

   SetBackground(pScreen, pSprite);
}

void AdjustImage(SDL_Surface* pSurface, bool bBrighten)
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
         if( bBrighten ) {
	         r = Deal_Min(255, r+PIECE_BRIGHTEN_STEP_AMOUNT);
	         g = Deal_Min(255, g+PIECE_BRIGHTEN_STEP_AMOUNT);
	         b = Deal_Min(255, b+PIECE_BRIGHTEN_STEP_AMOUNT);
	}
	else {
		r = Deal_Max(0, r-PIECE_BRIGHTEN_STEP_AMOUNT);
		g = Deal_Max(0, g-PIECE_BRIGHTEN_STEP_AMOUNT);
		b = Deal_Max(0, b-PIECE_BRIGHTEN_STEP_AMOUNT);
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

void Brighten(SDL_Surface* pSurface)
{
   AdjustImage(pSurface, true);
}

void Darken(SDL_Surface* pSurface)
{
   AdjustImage(pSurface, false);
}

void SelectCaseSprite(SDL_Surface* pScreen, CaseSprite* pSprite)
{
   if( pSprite->bSelected == false )
      return;

   if( pSprite->nSelectedAmount < PIECE_FADE_LIMIT ) {
      pSprite->nSelectedAmount++;
      AdjustImage(pSprite->img, has_colors ? true : false);
   }

   if(!has_colors)
      draw_rectangle(pScreen, 0, pSprite->x, pSprite->y, pSprite->width, pSprite->height, 1);
}

void FadeCaseSprite(SDL_Surface* pScreen, CaseSprite* pSprite)
{
   if( pSprite->bToFade == false )
      return;

   if( pSprite->nFadeAmount < PIECE_FADE_LIMIT )
      pSprite->nFadeAmount++;

   Brighten(pSprite->img);
}

void ShadeLabelSprite(SDL_Surface* pScreen, AmountLabelSprite* pSprite, int nBlinkDuration)
{
   if( pSprite->bToShade == false )
      return;

   if( pSprite->nShadeAmount < (nBlinkDuration*20) ) {
      pSprite->nShadeAmount++;
      int nRemain = pSprite->nShadeAmount % 20;
      bool bBrighten = nRemain < 5 || nRemain >= 15;
      AdjustImage(pSprite->img, bBrighten);
   }
   else
   {
      AdjustImage(pSprite->img, has_colors ? true : false);
   }
}

template<typename T>
void ShowSprite(SDL_Surface* pScreen, T* pSprite)
{
   SDL_Rect rect;
   rect.x = pSprite->x;
   rect.y = pSprite->y;
   rect.w = pSprite->width;
   rect.h = pSprite->height;

   SDL_BlitSurface(pSprite->img, NULL, pScreen, &rect);
}

void ShowSprite(SDL_Surface* pScreen, CaseSprite* pSprite)
{
   if( pSprite->bToFade )
      return;

   ShowSprite(pScreen, (BaseSprite*)pSprite);
}

void ResetBackground(SDL_Surface* pScreen, BaseSprite* pSprite)
{
   SDL_Rect rect;
   rect.x = pSprite->x;
   rect.y = pSprite->y;
   rect.w = pSprite->width;
   rect.h = pSprite->height;

   SDL_BlitSurface(pSprite->replace, NULL, pScreen, &rect);
}

bool ScreenObjectsControl::Animate(int nFlags /*= ANIMATE_ALL*/)
{
   /*if( !IsAnimating() ) {
      return false;
   }*/

   if( (nFlags & ANIMATE_CASES) == ANIMATE_CASES )
   {
      CaseSprite* pCurrent = m_Cases.GetFirst();

      while(pCurrent != NULL) {
         MoveSprite(m_pScreen, pCurrent);
         SelectCaseSprite(m_pScreen, pCurrent);
         FadeCaseSprite(m_pScreen, pCurrent);
         ShowSprite(m_pScreen, pCurrent);
         pCurrent = (CaseSprite*)pCurrent->next;
      }
   }

   if( (nFlags & ANIMATE_LABELS) == ANIMATE_LABELS )
   {
      AmountLabelSprite* pCurrent = m_Labels.GetFirst();

      while(pCurrent != NULL) {
         MoveSprite(m_pScreen, pCurrent);
         ShadeLabelSprite(m_pScreen, pCurrent, m_pConfig->GetBlinkDuration());
         ShowSprite(m_pScreen, pCurrent);
         pCurrent = (AmountLabelSprite*)pCurrent->next;
      }
   }

   if( (nFlags & ANIMATE_TEXT) == ANIMATE_TEXT )
   {
      LetterSprite* pCurrent = m_Letters.GetFirst();

      if( pCurrent != NULL ) {
         if( !IsLettersAnimating() ) {
            m_nStayDuration--;
         }
      }

      while(pCurrent != NULL) {
         MoveSprite(m_pScreen, pCurrent);
         ShowSprite(m_pScreen, pCurrent);
         if( m_nStayDuration <= 0 && pCurrent->toY < SCREEN_HEIGHT ) {
            pCurrent->toX = pCurrent->x;
            pCurrent->toY = SCREEN_HEIGHT;
         }
         pCurrent = (LetterSprite*)pCurrent->next;
      }
   }

   if( (nFlags & ANIMATE_CURSOR) == ANIMATE_CURSOR )
   {
      CursorSprite* pCurrent = m_Cursor.GetFirst();

      if( pCurrent != NULL && pCurrent->bVisible )
         ShowSprite(m_pScreen, pCurrent);
   }

   SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

   return true;
}



