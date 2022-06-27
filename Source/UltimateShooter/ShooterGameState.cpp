// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterGameState.h"

AShooterGameState::AShooterGameState():
	bInCombat(false),
	CurrentKills(0)
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

