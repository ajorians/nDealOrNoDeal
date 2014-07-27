#include "AchieveConfig.h"
#include "Config.h"
#include "Defines.h"

AchieveConfig::AchieveConfig(Config* pConfig)
: m_pConfig(pConfig), m_bNewAchievement(false)
{
}

AchieveConfig::~AchieveConfig()
{
}

bool AchieveConfig::HasWonAMillion() const
{
   return m_pConfig->GetTimesWon1Million() > 0;
}

bool AchieveConfig::HasPlayed10Games() const
{
   return m_pConfig->GetGamesPlayed() >= 10;
}

bool AchieveConfig::HasPlayed50Games() const
{
   return m_pConfig->GetGamesPlayed() >= 50;
}

bool AchieveConfig::HasAccepted10Deals() const
{
   return m_pConfig->GetDealsAccepted() >= 10;
}

bool AchieveConfig::HasHadCloseCall() const//2 cases are 1 million & 1/2 million
{
   return m_pConfig->GetCloseCalls() > 0;
}

bool AchieveConfig::LookForAchievements(DealOrNoDealLib deal)
{
   bool bNewAchievement = false;
   int nUnopenedCase = -1, nPickedCaseIndex = -1;
   if( DEALORNODEAL_CASE_NOT_PICKED == HavePickedCase(deal, &nPickedCaseIndex) )
      goto Exit;
   for(int i=0; i<DEALORNODEAL_NUMBER_OF_CASES; i++) {
      if( nUnopenedCase != -1 )
        goto Exit;
      if( DEALORNODEAL_CASENOTOPENED == IsCaseOpened(deal, i) )
         nUnopenedCase = i;
   }
   if( nUnopenedCase != -1 && nPickedCaseIndex != -1 ) {
      int nAmount = GetPickedCaseAmount(deal);
      int nAmount2 = GetCaseAmount(deal, nUnopenedCase);
      if( (nAmount + nAmount2) == 1500000 ) {
         if( !HasHadCloseCall() ) {
            m_bNewAchievement = true;
            bNewAchievement = true;
         }
         m_pConfig->IncrementCloseCalls();
      }
   }

Exit:
   
   return bNewAchievement;
}

bool AchieveConfig::JustPlayedAGame()
{
   bool bNewAchievement = false;
   bool bPlayed10Games = HasPlayed10Games(), bPlayed50Games = HasPlayed50Games();
   m_pConfig->IncrementGamesPlayed();
   if( !bPlayed10Games && HasPlayed10Games() ) {
      bNewAchievement = true;
      goto Exit;
   }
   if( !bPlayed50Games && HasPlayed50Games() ) {
      bNewAchievement = true;
      goto Exit;
   }

Exit:
   if( bNewAchievement ) {
      printf("New achievement!\n");
      m_bNewAchievement = true;
   }

   return bNewAchievement;
}

bool AchieveConfig::JustAcceptedADeal()
{
   bool bAccepted10Deals = HasAccepted10Deals();
   m_pConfig->IncrementDealsAccepted();
   if( !bAccepted10Deals && HasAccepted10Deals() ) {
      m_bNewAchievement = true;
      return true;
   }

   return false;
}

bool AchieveConfig::GetNewAchievement()
{
   bool bNewAchievement = m_bNewAchievement;
   m_bNewAchievement = false;
   return bNewAchievement;
}
