#ifndef CONFIG_H
#define CONFIG_H

extern "C"
{
	#include <os.h>
	#include "ArchiveLib/ArchiveLib.h"
}

class Config
{
public:
   Config();
   ~Config();

   bool GetAllowMouse() const;
   void SetAllowMouse(bool bEnable);

   int GetBlinkDuration() const;
   void SetBlinkDuration(int nDuration);

   int GetPlayerStanding() const;
   void ResetPlayerStanding();
   void AdjustPlayerStanding(int nAmount);

//Achievements
   int GetGamesPlayed() const;
   void IncrementGamesPlayed();

   int GetDealsAccepted() const;
   void IncrementDealsAccepted();

   int GetTimesWon1Million() const;
   void IncrementTimeWon1Million();

   int GetCloseCalls() const;
   void IncrementCloseCalls();

protected:
   ArchiveLib	m_Archive;
   bool         m_bAllowMouse;
   int          m_nPlayerStanding;
   int		m_nBlinkDuration;

//Achievements
   int		m_nGamesPlayed;
   int		m_nDealsAccepted;
   int		m_nTimesWon1Million;
   int		m_nNumberOfCloseCalls;
};

#endif

