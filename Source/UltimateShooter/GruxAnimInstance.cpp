// Fill out your copyright notice in the Description page of Project Settings.


#include "GruxAnimInstance.h"
#include "Enemy.h"

void UGruxAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (!Enemy)
	{
		Enemy = Cast<AEnemy>(TryGetPawnOwner());
	}

	if (Enemy)
	{
		// Get Enemy Speed Every Frame
		FVector Velocity{ Enemy->GetVelocity() };
		Velocity.Z = 0.f;
		Speed = Velocity.Size();
	}
}