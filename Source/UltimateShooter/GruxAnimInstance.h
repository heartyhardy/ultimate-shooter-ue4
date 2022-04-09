// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GruxAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ULTIMATESHOOTER_API UGruxAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

private:

	/** Lateral Movment Speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;

	/** Reference to Enemy */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AEnemy* Enemy;
};
