// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterGameState.h"
#include "Announcer.h"

AShooterGameState::AShooterGameState():
	// Combat State
	bInCombat(false),
	// Kill Streaks
	CurrentKills(0),
	KillStreakThreshold(4.f)
{
}

void AShooterGameState::SetDayNight(bool bDay)
{
	if (bDay && !bDaytime)
	{
		bDaytime = bDay;
	}
	else if(!bDay && bDaytime)
	{
		bDaytime = bDay;
	}
}

void AShooterGameState::PlayKillStreakAnnouncement()
{
	switch (CurrentKillStreak)
	{

	case 0:
	case 1:
		break;

	case 2:
		GetAnnouncer()->PlayKillStreakAnnouncement(EKillStreakAnnoucementType::EKSAT_DoubleKill);
		break;

	case 3:
		GetAnnouncer()->PlayKillStreakAnnouncement(EKillStreakAnnoucementType::EKSAT_TripleKill);
		break;

	case 4:
		GetAnnouncer()->PlayKillStreakAnnouncement(EKillStreakAnnoucementType::EKSAT_QuadraKill);
		break;

	case 5:
		GetAnnouncer()->PlayKillStreakAnnouncement(EKillStreakAnnoucementType::EKSAT_PentaKill);
		break;

	default:
		GetAnnouncer()->PlayKillStreakAnnouncement(EKillStreakAnnoucementType::EKAST_EpicKill);
		break;

	}
}

