// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (!ShooterCharacter)
	{
		/** Get The Pawn this Anim Instance belogs to */
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}

	if (ShooterCharacter)
	{
		/** Get Velocity of the Character*/
		FVector Velocity = ShooterCharacter->GetVelocity();
		/** Get only the Lateral speed of the Velocity. We don't need Z-axis (character falling/flying) */
		Velocity.Z = 0;
		/** Get the magnitude of the Vector */
		Speed = Velocity.Size();

		/** Check if the Character is Airborne */
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		/** 
		* Check if the Character is Accelerating 
		*	NOTE that this is not actual acceleration according to physics since...
		*	Acceleration is rate of change of velocity per unit of time
		*/
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}
	}
}
