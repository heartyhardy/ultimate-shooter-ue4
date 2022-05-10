// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ControlPointType.h"
#include "ControlPoint.generated.h"

UCLASS()
class ULTIMATESHOOTER_API AControlPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AControlPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Callback for RangeSphere BeginComponentOverlap
	UFUNCTION()
	void OnRangeSphereOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	// Callback for RangeSphere EndComponentOverlap
	UFUNCTION()
	void OnRangeSphereEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int OtherBodyIndex
	);

	void ApplyControlPointPerSecondBonus();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Control Point", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StructureMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Control Point", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* RangeSphere;

	/** Type of the control point: FOR NOW: Armor/Damage/Health */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Control Point", meta = (AllowPrivateAccess = "true"))
	EControlPointType ControlPointType;

	/** If bUseConstantBonus is FALSE, this control point grants per second bonus */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Control Point", meta = (AllowPrivateAccess = "true"))
	float PerSecondBonus;

	/** If bUseConstantBonus is TRUE this control point grants a constant bonus within its radius */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Control Point", meta = (AllowPrivateAccess = "true"))
	float ConstantBonus;

	/** Switch between Constant bonus and Per Second bonus within control point's range */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Control Point", meta = (AllowPrivateAccess = "true"))
	bool bUseConstantBonus;

	/** Faction of the control point. Bonus works on Allies / Against Allies (Enemies) or / Neutral */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Control Point", meta = (AllowPrivateAccess = "true"))
	EControlPointFaction ControlPointFaction;

	/** If bUseConstantBonus is FALSE cooldown time between each bonus grant */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Control Point", meta = (AllowPrivateAccess = "true"))
	float CooldownTime;

	/** Particle system to play when bonus is applied */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Control Point", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ApplyBonusParticles;

	FTimerHandle CooldownTimer;

	class AShooterCharacter* ShooterCharacter;

private:

	void StartPersecondBonusTimer();
	void PlayApplyBonusEffect();
	void CleanUpBonusEffects(AShooterCharacter* TargetCharacter);
};
