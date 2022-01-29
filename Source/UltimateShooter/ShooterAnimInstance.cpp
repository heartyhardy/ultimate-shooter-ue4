// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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

		/** Base Aim Rotation is 0.f at World X Rotation */
		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		/** Get a Rotator from a given Direction Vector */
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());

		/**
		* Get the difference between Aim Rotation and Movement Rotation 
		* Note that this is 0 when moving forward but different when strafing
		* We just need the Yaw only
		*/
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		/** Save the MovmentOffsetYaw while Character's velocity > 0.f
		* This is required for Ground Locomotion - JogStop Blendspace...
		* as it can't determine MovementOffsetYaw when Character stops(Velocity = 0 therefore can't get the movment offset)
		*/
		if (ShooterCharacter->GetVelocity().Size() > 0.f)
		{
			LastMovementOffsetYaw = MovementOffsetYaw;
		}

		/** FOR DEBUG PURPOSES ONLY
		
		FString RotationMessage = FString::Printf(TEXT("Base Aim Rotation: %f"), AimRotation.Yaw);
		FString MovementMessage = FString::Printf(TEXT("Movement Rotation: %f"), MovementRotation.Yaw);
		FString MovementOffsetMessage = FString::Printf(TEXT("Movement Offset: %f"), MovementOffsetYaw);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, RotationMessage);
			GEngine->AddOnScreenDebugMessage(2, 0.f, FColor::White, MovementMessage);
			GEngine->AddOnScreenDebugMessage(3, 0.f, FColor::White, MovementOffsetMessage);
		}
		*/
	}
}
