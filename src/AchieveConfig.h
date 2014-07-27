#ifndef ACHIEVECONFIG_H
#define ACHIEVECONFIG_H

extern "C"
{
	#include <os.h>
	#include "DealOrNoDealLib/DealOrNoDealLib.h"
}

class Config;

class AchieveConfig
{
public:
   AchieveConfig(Config* pConfig);
   ~AchieveConfig();

   bool HasWonAMillion() const;
   bool HasPlayed10Games() const;
   bool HasPlayed50Games() const;
   bool HasAccepted10Deals() const;
   bool HasHadCloseCall() const;//2 cases are 1 million & 1/2 million

   bool LookForAchievements(DealOrNoDealLib deal);
   bool JustPlayedAGame();
   bool JustAcceptedADeal();

   bool GetNewAchievement();

   Config* GetConfig() { return m_pConfig; }

protected:
   Config*	m_pConfig;
   bool		m_bNewAchievement;
};

#endif

