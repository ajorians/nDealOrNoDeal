#ifndef DEALORNODEALLIB_H_INCLUDED
#define DEALORNODEALLIB_H_INCLUDED

typedef void* DealOrNoDealLib;

#define DEALORNODEAL_NUMBER_OF_CASES		26
#define DEALORNODEAL_NUMBER_OF_ROUNDS		10

#define DEALORNODEALLIB_OK			(0)
#define DEALORNODEALLIB_BADARGUMENT		(-1)
#define DEALORNODEALLIB_OUT_OF_MEMORY		(-2)
#define DEALORNODEALLIB_CASE_ALREADY_PICKED	(-3)

#define DEALORNODEAL_CASE_PICKED		(1)
#define DEALORNODEAL_CASE_NOT_PICKED		(0)

#define DEALORNODEAL_ROUNDCHANGE		(1)
#define DEALORNODEAL_SAMEROUND			(0)

#define DEALORNODEAL_CASEOPENED			(1)
#define DEALORNODEAL_CASENOTOPENED		(0)

#define DEALORNODEAL_STATUS_STILLPLAYING	(0)
#define DEALORNODEAL_STATUS_ACCEPTEDOFFER	(1)
#define DEALORNODEAL_STATUS_OPENEDALLCASES	(2)

//////////////////////////////////////////////
//Initalization/Error checking/Mode functions
//////////////////////////////////////////////
int DealOrNoDealLibCreate(DealOrNoDealLib* api );
int DealOrNoDealLibFree(DealOrNoDealLib* api);

int GetDealOrNoDealLibError(DealOrNoDealLib api);
void ClearDealOrNoDealLibError(DealOrNoDealLib api);

//////////////////////////////////////////////
//DealOrNoDealLib related functions
//////////////////////////////////////////////
int GetIndexAmount(int nCase, int* pnAmount);
int GetRoundNumber(DealOrNoDealLib api);
int PickYourCase(DealOrNoDealLib api, int nCaseIndex);
int HavePickedCase(DealOrNoDealLib api, int* pnCaseIndex);
int GetCaseAmount(DealOrNoDealLib api, int nCaseIndex);
int GetPickedCaseAmount(DealOrNoDealLib api);
int HowManyCasesToChoose(DealOrNoDealLib api, int* pnCases, int* pnCasesRemaining);
int ChooseCase(DealOrNoDealLib api, int nCaseIndex, int* pnAmount, int* pnRoundChange);
int IsCaseOpened(DealOrNoDealLib api, int nCaseIndex);
int GetBankerAmount(DealOrNoDealLib api, int* pnAmount);
int GetPriorBankerAmount(DealOrNoDealLib api, int nRound, int* pnAmount);
int SetAcceptedOffer(DealOrNoDealLib api);
int GetGameStatus(DealOrNoDealLib api);

#endif //DEALORNODEALLIB_H_INCLUDED
