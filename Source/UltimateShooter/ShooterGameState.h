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

public:

	AShooterGameState();

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Announcer, meta = (AllowPrivateAccess = "true"))
		class AAnnouncer* Announcer;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = Level, meta = (AllowPrivateAccess = "true"))
		bool bDaytime = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = Combat, meta = (AllowPrivateAccess = "true"))
		bool bInCombat;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Kill Streaks", meta = (AllowPrivateAccess = "true"))
		int32 CurrentKills;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Kill Streaks", meta = (AllowPrivateAccess = "true"))
		int32 CurrentKillStreak;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Kill Streaks", meta = (AllowPrivateAccess = "true"))
		FDateTime CurrentKillTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Kill Streaks", meta = (AllowPrivateAccess = "true"))
		FDateTime LastKillTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Kill Streaks", meta = (AllowPrivateAccess = "true"))
		float KillStreakThreshold;

protected:

	UFUNCTION(BlueprintCallable)
		void SetDayNight(bool bDay);

public:
	FORCEINLINE float GetLevelNoiseModifier() const { return LevelNoiseModifier; }
	FORCEINLINE void SetLevelNoiseModifier(float Amount) { LevelNoiseModifier = Amount; }

	FORCEINLINE bool GetSceneFringeEnabled() const { return bSceneFringeEnabled; }
	FORCEINLINE void SetSceneFingeEnabled(bool bEnabled) { bSceneFringeEnabled = bEnabled; }
	FORCEINLINE float GetDefaultSceneFringe() const { return DefaultSceneFringe; }

	FORCEINLINE bool GetVignetteEnabled() const { return bVignetteEnabled; }
	FORCEINLINE void SetVignetteEnabled(bool bEnabled) { bVignetteEnabled = bEnabled; }
	FORCEINLINE float GetDefaultVignette() const { return DefaultVignette; }

	FORCEINLINE bool GetDaytime() { return bDaytime; }
	FORCEINLINE void SetDaytime(bool bDayNight) { bDaytime = bDayNight; }

	FORCEINLINE bool GetIsInCombat() const { return bInCombat; }
	FORCEINLINE void SetIsInCombat(bool bCombatState) { bInCombat = bCombatState; }

	FORCEINLINE int32 GetCurrentKills() const { return CurrentKills; }
	FORCEINLINE void IncrementCurrentKills() { CurrentKills++; }

	FORCEINLINE AAnnouncer* GetAnnouncer() const { return Announcer; }
	FORCEINLINE void SetAnnouncer(AAnnouncer* AnnouncerInst) { Announcer = AnnouncerInst; }

	FORCEINLINE FDateTime GetCurrentKillTime() const { return CurrentKillTime; }
	FORCEINLINE void SetCurrentKillTime(FDateTime KillTime) { CurrentKillTime = KillTime; }
	FORCEINLINE FDateTime GetLastKillTime() const { return LastKillTime; }
	FORCEINLINE void SetLastKillTime(FDateTime KillTime) { LastKillTime = KillTime; }

	FORCEINLINE int32 GetCurrentKillStreak() const { return CurrentKillStreak; }
	FORCEINLINE void IncrementCurrentKillStreak() { CurrentKillStreak++; }

	FORCEINLINE FTimespan GetKillTimeDifference() const { return (CurrentKillTime - LastKillTime); }
};
