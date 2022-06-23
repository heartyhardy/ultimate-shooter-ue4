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

	/** Noise modifier affects how monsters react to the bullet sounds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Level, meta = (AllowPrivateAccess = "true"))
		float LevelNoiseModifier = 1.0f;

	/** Global SceneFringe State */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Effects, meta = (AllowPrivateAccess = "true"))
		bool bSceneFringeEnabled = true;

	/** Global Vignette State */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Effects, meta = (AllowPrivateAccess = "true"))
		bool bVignetteEnabled = true;

	/** Default Scene Fringe Intensity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Effects, meta = (AllowPrivateAccess = "true"))
		float DefaultSceneFringe = 0.5f;

	/** Default Vignette Intensity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Effects, meta = (AllowPrivateAccess = "true"))
		float DefaultVignette = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = Level, meta = (AllowPrivateAccess = true))
		bool bDaytime = true;

protected:

	UFUNCTION(BlueprintCallable)
		void SetDayNight(bool bDay);

	UFUNCTION(BlueprintNativeEvent)
		void DayArrived();
		void DayArrived_Implementation();

	UFUNCTION(BlueprintNativeEvent)
		void NightArrived();
		void NightArrived_Implementation();

public:
	FORCEINLINE float GetLevelNoiseModifier() const { return LevelNoiseModifier; }
	FORCEINLINE void SetLevelNoiseModifier(float Amount) { LevelNoiseModifier = Amount; }

	FORCEINLINE bool GetSceneFringeEnabled() const { return bSceneFringeEnabled; }
	FORCEINLINE void SetSceneFingeEnabled(bool bEnabled) { bSceneFringeEnabled = bEnabled; }
	FORCEINLINE float GetDefaultSceneFringe() const { return DefaultSceneFringe; }

	FORCEINLINE bool GetVignetteEnabled() const { return bVignetteEnabled; }
	FORCEINLINE void SetVignetteEnabled(bool bEnabled) { bVignetteEnabled = bEnabled; }
	FORCEINLINE float GetDefaultVignette() const { return DefaultVignette; }

	FORCEINLINE bool GetDaytime() const { return bDaytime; }
	FORCEINLINE void SetDaytime(bool bDayNight) { bDaytime = bDayNight; }
};
