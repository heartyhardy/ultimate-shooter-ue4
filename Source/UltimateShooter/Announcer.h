// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KillStreakAnnoucementType.h"
#include "Announcer.generated.h"

UCLASS()
class ULTIMATESHOOTER_API AAnnouncer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAnnouncer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Buffs", meta = (AllowPrivateAccess = "true"))
		class USoundCue* DamageBuffAnnounce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Buffs", meta = (AllowPrivateAccess = "true"))
		class USoundCue* ArmorBuffAnnounce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Buffs", meta = (AllowPrivateAccess = "true"))
		class USoundCue* SpeedBuffAnnounce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Kill Streaks", meta = (AllowPrivateAccess = "true"))
		class USoundCue* FirstBloodAnnounce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Kill Streaks", meta = (AllowPrivateAccess = "true"))
		class USoundCue* DoubleKillAnnounce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Kill Streaks", meta = (AllowPrivateAccess = "true"))
		class USoundCue* TripleKillAnnounce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Kill Streaks", meta = (AllowPrivateAccess = "true"))
		class USoundCue* QuadraKillAnnounce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Kill Streaks", meta = (AllowPrivateAccess = "true"))
		class USoundCue* PentaKillAnnounce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Kill Streaks", meta = (AllowPrivateAccess = "true"))
		class USoundCue* EpicKillingAnnounce;


public:

	UFUNCTION(BlueprintCallable)
		void PlayDamageBuffAnnouncement();

	UFUNCTION(BlueprintCallable)
		void PlayArmorBuffAnnouncement();

	UFUNCTION(BlueprintCallable)
		void PlaySpeedBuffAnnouncement();

	UFUNCTION(BlueprintCallable)
		void PlayFirstBloodAnnouncement();

	UFUNCTION(BlueprintCallable)
		void PlayKillStreakAnnouncement(EKillStreakAnnoucementType AnnouncemetType);
};
