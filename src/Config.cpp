#include "Config.h"
#include "Defines.h"

#define SETTINGS_CONFIG_TAG		"Settings"
#define SETTINGS_ACHIEVEMENT_TAG	"Achievements"

#define SETTINGS_FILENAME		"DealOrNoDealSettings.tns"
#define SETTINGTAG_ALLOWMOUSE		"AllowMouse"
#define SETTINGTAG_POINTS		"Points"
#define SETTINGTAG_BLINKDURATION	"LabelBlinkDuration"
//Achievements
#define SETTINGTAG_GAMESPLAYED		"GamesPlayed"
#define SETTINGTAG_DEALSACCEPTED	"DealsAccepted"
#define SETTINGTAG_TIMESWON1MILLION	"TimesWon1Million"
#define SETTINGTAG_NUMBERCLOSECALLS	"NumberCloseCalls"

Config::Config()
: m_bAllowMouse(true), m_nPlayerStanding(1000), m_nBlinkDuration(5), m_nGamesPlayed(0), m_nDealsAccepted(0), m_nTimesWon1Million(0), m_nNumberOfCloseCalls(0)
{
   ArchiveCreate(&m_Archive);

   OpenArchiveFile(m_Archive, SETTINGS_FILENAME);

   int nSettings = GetNumberArchives(m_Archive, SETTINGS_CONFIG_TAG);
   char strName[MAX_NAME_LENGTH];
   for(int i=0; i<nSettings; i++) {
      strcpy(strName, GetName(m_Archive, SETTINGS_CONFIG_TAG, i));

      if( strcmp(strName, SETTINGTAG_ALLOWMOUSE) == 0 ) {
         if( strcmp("0", GetValue(m_Archive, SETTINGS_CONFIG_TAG, i)) == 0 ) {
            m_bAllowMouse = false;
         } else {
            m_bAllowMouse = true;
         }
      }
   }

   nSettings = GetNumberArchives(m_Archive, SETTINGS_ACHIEVEMENT_TAG);
   for(int i=0; i<nSettings; i++) {
      strcpy(strName, GetName(m_Archive, SETTINGS_ACHIEVEMENT_TAG, i));

      if( strcmp(strName, SETTINGTAG_POINTS) == 0 ) {
         m_nPlayerStanding = atoi(GetValue(m_Archive, SETTINGS_ACHIEVEMENT_TAG, i));
      }

      if( strcmp(strName, SETTINGTAG_BLINKDURATION) == 0 ) {
         m_nBlinkDuration = atoi(GetValue(m_Archive, SETTINGS_ACHIEVEMENT_TAG, i));
      }

      if( strcmp(strName, SETTINGTAG_GAMESPLAYED) == 0 ) {
         m_nGamesPlayed = atoi(GetValue(m_Archive, SETTINGS_ACHIEVEMENT_TAG, i));
      }

      if( strcmp(strName, SETTINGTAG_DEALSACCEPTED) == 0 ) {
         m_nDealsAccepted = atoi(GetValue(m_Archive, SETTINGS_ACHIEVEMENT_TAG, i));
      }

      if( strcmp(strName, SETTINGTAG_TIMESWON1MILLION) == 0 ) {
         m_nTimesWon1Million = atoi(GetValue(m_Archive, SETTINGS_ACHIEVEMENT_TAG, i));
      }

      if( strcmp(strName, SETTINGTAG_NUMBERCLOSECALLS) == 0 ) {
         m_nNumberOfCloseCalls = atoi(GetValue(m_Archive, SETTINGS_ACHIEVEMENT_TAG, i));
      }
   }
}

Config::~Config()
{
   ArchiveSetBatchMode(m_Archive, ARCHIVE_ENABLE_BATCH);
   UpdateArchiveEntry(m_Archive, SETTINGS_CONFIG_TAG, SETTINGTAG_ALLOWMOUSE, m_bAllowMouse ? "1" : "0", NULL);
   char buffer[10];
   Deal_itoa(m_nPlayerStanding, buffer, 10);
   UpdateArchiveEntry(m_Archive, SETTINGS_CONFIG_TAG, SETTINGTAG_POINTS, buffer, NULL);
   Deal_itoa(m_nBlinkDuration, buffer, 10);
   UpdateArchiveEntry(m_Archive, SETTINGS_CONFIG_TAG, SETTINGTAG_BLINKDURATION, buffer, NULL);

   Deal_itoa(m_nGamesPlayed, buffer, 10);
   UpdateArchiveEntry(m_Archive, SETTINGS_ACHIEVEMENT_TAG, SETTINGTAG_GAMESPLAYED, buffer, NULL);

   Deal_itoa(m_nDealsAccepted, buffer, 10);
   UpdateArchiveEntry(m_Archive, SETTINGS_ACHIEVEMENT_TAG, SETTINGTAG_DEALSACCEPTED, buffer, NULL);

   Deal_itoa(m_nTimesWon1Million, buffer, 10);
   UpdateArchiveEntry(m_Archive, SETTINGS_ACHIEVEMENT_TAG, SETTINGTAG_TIMESWON1MILLION, buffer, NULL);

   Deal_itoa(m_nNumberOfCloseCalls, buffer, 10);
   UpdateArchiveEntry(m_Archive, SETTINGS_ACHIEVEMENT_TAG, SETTINGTAG_NUMBERCLOSECALLS, buffer, NULL);

   ArchiveSetBatchMode(m_Archive, ARCHIVE_DISABLE_BATCH);

   ArchiveFree(&m_Archive);
}

bool Config::GetAllowMouse() const
{
   return m_bAllowMouse;
}

void Config::SetAllowMouse(bool bEnable)
{
   m_bAllowMouse = bEnable;
}

int Config::GetBlinkDuration() const
{
   return m_nBlinkDuration;
}

void Config::SetBlinkDuration(int nDuration)
{
   m_nBlinkDuration = nDuration;
}

int Config::GetPlayerStanding() const
{
   return m_nPlayerStanding;
}

void Config::ResetPlayerStanding()
{
   m_nPlayerStanding = 1000;
}

void Config::AdjustPlayerStanding(int nAmount)
{
   m_nPlayerStanding = Deal_Max(0, m_nPlayerStanding + nAmount);
}

//Achievements
int Config::GetGamesPlayed() const
{
   return m_nGamesPlayed;
}

void Config::IncrementGamesPlayed()
{
   m_nGamesPlayed++;
}

int Config::GetDealsAccepted() const
{
   return m_nDealsAccepted;
}

void Config::IncrementDealsAccepted()
{
   m_nDealsAccepted++;
}

int Config::GetTimesWon1Million() const
{
   return m_nTimesWon1Million;
}

void Config::IncrementTimeWon1Million()
{
   m_nTimesWon1Million++;
}

int Config::GetCloseCalls() const
{
   return m_nNumberOfCloseCalls;
}

void Config::IncrementCloseCalls()
{
   m_nNumberOfCloseCalls++;
}

