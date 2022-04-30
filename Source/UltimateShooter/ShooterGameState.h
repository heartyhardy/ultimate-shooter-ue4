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

public:
	FORCEINLINE float GetLevelNoiseModifier() const { return LevelNoiseModifier; }
	FORCEINLINE void SetLevelNoiseModifier(float Amount) { LevelNoiseModifier = Amount; }
};
