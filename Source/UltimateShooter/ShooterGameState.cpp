// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterGameState.h"

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

