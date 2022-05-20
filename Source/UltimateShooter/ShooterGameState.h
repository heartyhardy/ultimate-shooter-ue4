// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ShooterGameState.generated.h"

/**
 * 
 */
UCLASS()
class ULTIMATESHOOTER_API AShooterGameState : public AGameStateBase
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Level, meta = (AllowPrivateAccess = "true"))
		float LevelNoiseModifier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Effects, meta = (AllowPrivateAccess = "true"))
		bool bScreenFringeEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Effects, meta = (AllowPrivateAccess = "true"))
		float DefaultScreenFringe = 0.5f;

public:
	FORCEINLINE float GetLevelNoiseModifier() const { return LevelNoiseModifier; }
	FORCEINLINE void SetLevelNoiseModifier(float Amount) { LevelNoiseModifier = Amount; }

	FORCEINLINE bool GetScreenFringeEnabled() const { return bScreenFringeEnabled; }
	FORCEINLINE void SetScreenFingeEnabled(bool bEnabled) { bScreenFringeEnabled = bEnabled; }
	FORCEINLINE float GetDefaultScreenFringe() const { return DefaultScreenFringe; }
};
