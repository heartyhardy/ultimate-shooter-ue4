// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterGameState.h"

void AShooterGameState::SetDayNight(bool bDay)
{
	if (bDay && !bDaytime)
	{
		DayArrived();
		bDaytime = bDay;
	}
	else if(!bDay && bDaytime)
	{
		NightArrived();
		bDaytime = bDay;
	}
}

void AShooterGameState::DayArrived_Implementation()
{
}

void AShooterGameState::NightArrived_Implementation()
{
}
