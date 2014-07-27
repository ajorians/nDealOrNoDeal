//Public domain :)

#include <os.h>
#include "include/DealOrNoDealLib.h"
#include "Defines.h"

struct Case
{
   int m_nAmount;
   int m_nOpened;
};

struct DealOrNoDeal
{
   struct Case m_Cases[DEALORNODEAL_NUMBER_OF_CASES];
   int m_nPickedCase;
   int m_nRound;//0=pick case; 1-DEALORNODEAL_NUMBER_OF_ROUNDS is case picking
   int m_nPreviousOffers[DEALORNODEAL_NUMBER_OF_ROUNDS];
   int m_nStatus;
   int m_nLastError;
};

int g_Amounts[] =
{
        0,
        1,
        5,
        10,
        25,
        50,
        75,
        100,
        200,
        300,
        400,
        500,
        750,
        1000,
        5000,
        10000,
        25000,
        50000,
        75000,
        100000,
        200000,
        300000,
        400000,
        500000,
        750000,
        1000000
};

int DealOrNoDealLibCreate(DealOrNoDealLib* api)
{
   DEBUG_FUNC_NAME;

   struct DealOrNoDeal* pD = malloc(sizeof(struct DealOrNoDeal));
   if( pD == NULL ){//Out of memory
      return DEALORNODEALLIB_OUT_OF_MEMORY;
   }

   for(int i=0; i<DEALORNODEAL_NUMBER_OF_CASES; i++)
   {
      pD->m_Cases[i].m_nAmount = g_Amounts[i];
      pD->m_Cases[i].m_nOpened = 0;
   }

   //Randomize it
   for(int i=0; i<DEALORNODEAL_NUMBER_OF_CASES; i++)
   {
      int nRandCase = rand() % DEALORNODEAL_NUMBER_OF_CASES;
      int nTemp = pD->m_Cases[nRandCase].m_nAmount;
      pD->m_Cases[nRandCase].m_nAmount = pD->m_Cases[i].m_nAmount;
      pD->m_Cases[i].m_nAmount = nTemp;
   }

   pD->m_nPickedCase = -1;
   pD->m_nRound = 0;
   for(int i=0; i<DEALORNODEAL_NUMBER_OF_ROUNDS; i++) {
      pD->m_nPreviousOffers[i] = 0;
   }
   pD->m_nStatus = DEALORNODEAL_STATUS_STILLPLAYING;
   pD->m_nLastError = DEALORNODEALLIB_OK;

   *api = pD;

   return DEALORNODEALLIB_OK;
}

int DealOrNoDealLibFree(DealOrNoDealLib* api)
{
   DEBUG_FUNC_NAME;

   struct DealOrNoDeal* pD = *api;

   free(pD);
   pD = NULL;

   *api = NULL;
   return DEALORNODEALLIB_OK;
}

int GetDealOrNoDealLibError(DealOrNoDealLib api)
{
   DEBUG_FUNC_NAME;

   struct DealOrNoDeal* pD = (struct DealOrNoDeal*)api;
   return pD->m_nLastError;
}

void ClearDealOrNoDealLibError(DealOrNoDealLib api)
{
   DEBUG_FUNC_NAME;

   struct DealOrNoDeal* pD = (struct DealOrNoDeal*)api;
   pD->m_nLastError = DEALORNODEALLIB_OK;
}

//DealOrNoDealLib related functions
int GetIndexAmount(int nIndex, int* pnAmount)
{
   DEBUG_FUNC_NAME;

   if( nIndex < 0 || nIndex >= DEALORNODEAL_NUMBER_OF_CASES )
      return DEALORNODEALLIB_BADARGUMENT;

   if( pnAmount ) *pnAmount = g_Amounts[nIndex];

   return DEALORNODEALLIB_OK;
}

int GetRoundNumber(DealOrNoDealLib api)
{
   DEBUG_FUNC_NAME;

   struct DealOrNoDeal* pD = (struct DealOrNoDeal*)api;
   if( DEALORNODEAL_CASE_NOT_PICKED == HavePickedCase(api, NULL) )
      return 0;

   return pD->m_nRound;
}

int PickYourCase(DealOrNoDealLib api, int nCaseIndex)
{
   DEBUG_FUNC_NAME;

   struct DealOrNoDeal* pD = (struct DealOrNoDeal*)api;
   if( nCaseIndex < 0 || nCaseIndex >= DEALORNODEAL_NUMBER_OF_CASES )
      return DEALORNODEALLIB_BADARGUMENT;
   if( pD->m_nPickedCase >= 0 )
      return DEALORNODEALLIB_CASE_ALREADY_PICKED;

   pD->m_nPickedCase = nCaseIndex;
   pD->m_nRound = 1;

   return DEALORNODEALLIB_OK;
}

int HowManyCasesOpened(DealOrNoDealLib api)
{
   DEBUG_FUNC_NAME;

   struct DealOrNoDeal* pD = (struct DealOrNoDeal*)api;

   int nCount = 0;
   for(int i=0; i<DEALORNODEAL_NUMBER_OF_CASES; i++) {
      struct Case* pCase = &pD->m_Cases[i];
      if( pCase->m_nOpened )
         nCount++;
   }

   return nCount;
}

int HowManyCasesToChoose(DealOrNoDealLib api, int* pnCases, int* pnCasesRemaining)
{
   DEBUG_FUNC_NAME;

   struct DealOrNoDeal* pD = (struct DealOrNoDeal*)api;

   int nCases = 0, nMaxCasesOpened = 0;
   switch(pD->m_nRound) {
      default:
         //ASSERT(false);
         return DEALORNODEALLIB_BADARGUMENT;
      case 1:
         nCases = 6;
         nMaxCasesOpened = 6;
      break;

      case 2:
         nCases = 5;
         nMaxCasesOpened = 11;
      break;

      case 3:
         nCases = 4;
         nMaxCasesOpened = 15;
      break;

      case 4:
         nCases = 3;
         nMaxCasesOpened = 18;
      break;

      case 5:
         nCases = 2;
         nMaxCasesOpened = 20;
      break;

      case 6:
         nCases = 1;
         nMaxCasesOpened = 21;
      break;

      case 7:
         nCases = 1;
         nMaxCasesOpened = 22;
      break;

      case 8:
         nCases = 1;
         nMaxCasesOpened = 23;
      break;

      case 9:
         nCases = 1;
         nMaxCasesOpened = 24;
      break;

      case 10:
         nCases = 1;
         nMaxCasesOpened = 25;
   }

   if( pnCases ) *pnCases = nCases;
   if( pnCasesRemaining ) *pnCasesRemaining = nMaxCasesOpened - HowManyCasesOpened(api);
   
   return DEALORNODEALLIB_OK;
}

int ChooseCase(DealOrNoDealLib api, int nCaseIndex, int* pnAmount, int* pnRoundChange)
{
   DEBUG_FUNC_NAME;

   struct DealOrNoDeal* pD = (struct DealOrNoDeal*)api;
   if( nCaseIndex < 0 || nCaseIndex >= DEALORNODEAL_NUMBER_OF_CASES )
      return DEALORNODEALLIB_BADARGUMENT;

   if( pD->m_nPickedCase < 0 )
      return DEALORNODEALLIB_BADARGUMENT;

   if( nCaseIndex == pD->m_nPickedCase )
      return DEALORNODEALLIB_CASE_ALREADY_PICKED;

   struct Case* pCase = &pD->m_Cases[nCaseIndex];
   if( pCase->m_nOpened )
      return DEALORNODEALLIB_CASE_ALREADY_PICKED;

   pCase->m_nOpened = 1;
   if( pnAmount ) *pnAmount = pCase->m_nAmount;

   int nCurrentRound = pD->m_nRound;
   int nOpenedCases = HowManyCasesOpened(api);
   if( nOpenedCases >= 25 ) {
       pD->m_nRound = 11;//Pseudo-round
       pD->m_nStatus = DEALORNODEAL_STATUS_OPENEDALLCASES;
   }
   else if( nOpenedCases >= 24 )
       pD->m_nRound = 10;
   else if( nOpenedCases >= 23 )
       pD->m_nRound = 9;
   else if( nOpenedCases >= 22 )
       pD->m_nRound = 8;
   else if( nOpenedCases >= 21 )
       pD->m_nRound = 7;
   else if( nOpenedCases >= 20 )
       pD->m_nRound = 6;
   else if( nOpenedCases >= 18 )
       pD->m_nRound = 5;
   else if( nOpenedCases >= 15 )
       pD->m_nRound = 4;
   else if( nOpenedCases >= 11 )
       pD->m_nRound = 3;
   else if( nOpenedCases >= 6 )
       pD->m_nRound = 2;

   if( pD->m_nRound != nCurrentRound ) {
      if( pnRoundChange )
         *pnRoundChange = DEALORNODEAL_ROUNDCHANGE;
      int nAmount = 0;
      GetBankerAmount(api, &nAmount);
      pD->m_nPreviousOffers[nCurrentRound-1] = nAmount;
   }

   return DEALORNODEALLIB_OK;
}

int IsCaseOpened(DealOrNoDealLib api, int nCaseIndex)
{
   DEBUG_FUNC_NAME;

   struct DealOrNoDeal* pD = (struct DealOrNoDeal*)api;
   struct Case* pCase = &pD->m_Cases[nCaseIndex];

   return pCase->m_nOpened ? DEALORNODEAL_CASEOPENED : DEALORNODEAL_CASENOTOPENED;
}

int HavePickedCase(DealOrNoDealLib api, int* pnCaseIndex)
{
   DEBUG_FUNC_NAME;

   struct DealOrNoDeal* pD = (struct DealOrNoDeal*)api;
   if( pD->m_nPickedCase < 0 )
      return DEALORNODEAL_CASE_NOT_PICKED;

   if( pnCaseIndex ) *pnCaseIndex = pD->m_nPickedCase;

   return DEALORNODEAL_CASE_PICKED;
}

int GetCaseAmount(DealOrNoDealLib api, int nCaseIndex)
{
   DEBUG_FUNC_NAME;

   struct DealOrNoDeal* pD = (struct DealOrNoDeal*)api;

   struct Case* pCase = &pD->m_Cases[nCaseIndex];
   return pCase->m_nAmount;
}

int GetPickedCaseAmount(DealOrNoDealLib api)
{
   DEBUG_FUNC_NAME;

   struct DealOrNoDeal* pD = (struct DealOrNoDeal*)api;
   if( pD->m_nPickedCase < 0 )//Eh
      return 0;

   return GetCaseAmount(api, pD->m_nPickedCase);
}

int GetBankerAmount(DealOrNoDealLib api, int* pnAmount)
{
   DEBUG_FUNC_NAME;

   struct DealOrNoDeal* pD = (struct DealOrNoDeal*)api;
   int nSum = 0, nCasesUnOpened = 0;
   for(int i=0; i<DEALORNODEAL_NUMBER_OF_CASES; i++) {
      struct Case* pCase = &pD->m_Cases[i];
      if( !pCase->m_nOpened ) {
         nSum += pCase->m_nAmount;
         nCasesUnOpened++;
      }
   }

   //TODO Handle nCasesUnOpened when it equals 0
   double dAverage = nSum/nCasesUnOpened;
   double dBanker = dAverage * pD->m_nRound / 10.;
   if( pnAmount ) *pnAmount = dBanker;

   return DEALORNODEALLIB_OK;
}

int GetPriorBankerAmount(DealOrNoDealLib api, int nRound, int* pnAmount)
{
   DEBUG_FUNC_NAME;

   struct DealOrNoDeal* pD = (struct DealOrNoDeal*)api;

   if( nRound < 1 || nRound >= pD->m_nRound )
      return DEALORNODEALLIB_BADARGUMENT;

   if( pnAmount ) *pnAmount = pD->m_nPreviousOffers[nRound-1];//nRound isn't an index

   return DEALORNODEALLIB_OK;
}

int SetAcceptedOffer(DealOrNoDealLib api)
{
   DEBUG_FUNC_NAME;

   struct DealOrNoDeal* pD = (struct DealOrNoDeal*)api;

   pD->m_nStatus = DEALORNODEAL_STATUS_ACCEPTEDOFFER;

   return DEALORNODEALLIB_OK;
}

int GetGameStatus(DealOrNoDealLib api)
{
   DEBUG_FUNC_NAME;

   struct DealOrNoDeal* pD = (struct DealOrNoDeal*)api;

   return pD->m_nStatus;
}

