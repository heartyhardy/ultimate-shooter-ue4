// Fill out your copyright notice in the Description page of Project Settings.


#include "Announcer.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "ShooterGameState.h"

// Sets default values
AAnnouncer::AAnnouncer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAnnouncer::BeginPlay()
{
	Super::BeginPlay();
	
	auto GameState = Cast<AShooterGameState>(GetWorld()->GetGameState());
	if (GameState)
	{
		GameState->SetAnnouncer(this);
	}
}

// Called every frame
void AAnnouncer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAnnouncer::PlayDamageBuffAnnouncement()
{
	if (DamageBuffAnnounce)
	{
		UGameplayStatics::PlaySound2D(
			GetWorld(),
			DamageBuffAnnounce
		);
	}
}

void AAnnouncer::PlayArmorBuffAnnouncement()
{
	if (ArmorBuffAnnounce)
	{
		UGameplayStatics::PlaySound2D(
			GetWorld(),
			ArmorBuffAnnounce
		);
	}
}

void AAnnouncer::PlaySpeedBuffAnnouncement()
{
	if (SpeedBuffAnnounce)
	{
		UGameplayStatics::PlaySound2D(
			GetWorld(),
			SpeedBuffAnnounce
		);
	}
}

void AAnnouncer::PlayFirstBloodAnnouncement()
{
	if (FirstBloodAnnounce)
	{
		UGameplayStatics::PlaySound2D(
			GetWorld(),
			FirstBloodAnnounce
		);
	}
}

void AAnnouncer::PlayKillStreakAnnouncement(EKillStreakAnnoucementType AnnouncemetType)
{
	switch (AnnouncemetType)
	{
	case EKillStreakAnnoucementType::EKSAT_FirstKill:
		PlayFirstBloodAnnouncement();
		break;

	case EKillStreakAnnoucementType::EKSAT_DoubleKill:
		if (DoubleKillAnnounce)
		{
			UGameplayStatics::PlaySound2D(
				GetWorld(),
				DoubleKillAnnounce
			);
		}
		break;

	case EKillStreakAnnoucementType::EKSAT_TripleKill:
		if (TripleKillAnnounce)
		{
			UGameplayStatics::PlaySound2D(
				GetWorld(),
				TripleKillAnnounce
			);
		}
		break;

	case EKillStreakAnnoucementType::EKSAT_QuadraKill:
		if (QuadraKillAnnounce)
		{
			UGameplayStatics::PlaySound2D(
				GetWorld(),
				QuadraKillAnnounce
			);
		}
		break;

	case EKillStreakAnnoucementType::EKSAT_PentaKill:
		if (PentaKillAnnounce)
		{
			UGameplayStatics::PlaySound2D(
				GetWorld(),
				PentaKillAnnounce
			);
		}
		break;

	case EKillStreakAnnoucementType::EKAST_EpicKill:
		if (EpicKillingAnnounce)
		{
			UGameplayStatics::PlaySound2D(
				GetWorld(),
				EpicKillingAnnounce
			);
		}
		break;
	}
}

