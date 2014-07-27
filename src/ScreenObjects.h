#ifndef SCREENOBJECTS_H
#define SCREENOBJECTS_H

extern "C"
{
        #include <os.h>
        #include "SDL/SDL.h"
}

#include "ColorIntensity.h"

class Config;

#define ANIMATE_CASES	(0x1)
#define ANIMATE_LABELS	(0x1 << 1)
#define ANIMATE_TEXT	(0x1 << 2)
#define ANIMATE_CURSOR	(0x1 << 3)
#define ANIMATE_ALL	(ANIMATE_CASES|ANIMATE_LABELS|ANIMATE_TEXT|ANIMATE_CURSOR)

struct BaseSprite
{
   SDL_Surface* img;
   SDL_Surface* replace;
   int x,y;
   int toX, toY;
   int width, height;
   struct BaseSprite* next;
};

struct CaseSprite : public BaseSprite
{
   SDL_Surface* imgOriginal;
   int nNumber;
   bool bSelected;
   int nSelectedAmount;
   bool bToFade;
   int nFadeAmount;
};

struct AmountLabelSprite : public BaseSprite
{
   double nAmount;
   bool bToShade;
   int nShadeAmount;
};

struct LetterSprite : public BaseSprite
{
   char ch;
   static int m_nStayDuration;//Static because only need one for a message; bad if you wanted to have multiple messages
};

struct CursorSprite : BaseSprite
{
   bool bVisible;
};

template<typename T>
class ItemControl
{
public:
   ItemControl():m_pRoot(NULL){}
   ~ItemControl(){ ClearAll(); }

   T* GetFirst() { return m_pRoot; }
   const T* GetFirst() const { return m_pRoot; }
   void ClearAll();
   bool Add(T* pT);
   
protected:
   T* m_pRoot;
};

template<typename T>
void ItemControl<T>::ClearAll()
{
   T* pSprite = m_pRoot;
   while(pSprite != NULL) {
      T* pTemp = pSprite;
      SDL_FreeSurface(pSprite->img);
      SDL_FreeSurface(pSprite->replace);
      pSprite = (T*)pSprite->next;
      free(pTemp);
   }
   m_pRoot = NULL;
}

template<typename T>
bool ItemControl<T>::Add(T* pT)
{
   pT->next = m_pRoot;
   m_pRoot = pT;
   return true;
}

class ScreenObjectsControl
{
public:
   ScreenObjectsControl(SDL_Surface* pScreen, Config* pConfig);
   ~ScreenObjectsControl();

   //Cases
   bool AddCaseSprite(CaseSprite* pCase);
   bool SelectCase(int nNumber);
   bool HideCase(int nNumber);
   static int GetCaseWidth();
   static int GetCaseHeight();
   static int GetCaseRow(int nNumber);
   static int GetCaseCol(int nNumber);
   static int GetCaseX(int nNumber);
   static int GetCaseY(int nNumber);

   //Labels
   bool AddAmountLabelSprite(AmountLabelSprite* pLabel);
   bool ShadeAmountLabel(int nAmount);
   static int GetLabelWidth();
   static int GetLabelHeight();
   static int GetLabelRow(int nAmount);
   static int GetLabelCol(int nAmount);
   static int GetLabelX(int nAmount);
   static int GetLabelY(int nAmount);

   //Text
   bool CreateMessage(const char* pstr);
   bool HasMessage() const;
   void ClearMessage();

   //Cursor
   bool MoveCursor(int nX, int nY);

   bool DisplayMessage(int nRound, int nCases, int nCasesRemaining);
   bool IsLettersAnimating() const;
   int LettersStayDuration() const;
   bool Animate(int nFlags = ANIMATE_ALL);

protected:
   SDL_Surface* m_pScreen;//Does NOT own
   Config* m_pConfig;//Does NOT own
   nSDL_Font       *m_pFont;
   nSDL_Font       *m_pFontLetters;
   ItemControl<CaseSprite> m_Cases;
   ItemControl<AmountLabelSprite> m_Labels;
   ItemControl<LetterSprite> m_Letters;
   int m_nStayDuration;
   ItemControl<CursorSprite> m_Cursor;
   //ColorIntensity m_ColorIntensity;
};

#endif


