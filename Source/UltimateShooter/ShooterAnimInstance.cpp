// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UShooterAnimInstance::UShooterAnimInstance() :
	Speed(0.f),
	bIsInAir(false),
	bIsAccelerating(false),
	MovementOffsetYaw(0.f),
	LastMovementOffsetYaw(0.f),
	bAiming(false),
	TIPCharacterYaw(0.f),
	TIPCharacterYawLastFrame(0.f),
	RootYawOffset(0.f),
	Pitch(0.f),
	bReloading(false),
	OffsetState(EOffsetState::EOS_Hip),
	CharacterRotation(FRotator::ZeroRotator),
	CharacterRotationLastFrame(FRotator::ZeroRotator),
	RecoilWeight(1.f),
	bTurningInPlace(false)
{

}

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
		
		bCrouching = ShooterCharacter->GetCrouching();
		bReloading = ShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading;
		bEquipping = ShooterCharacter->GetCombatState() == ECombatState::ECS_Equipping;

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

		/** Is the Character aiming their weapon? Needs this in Animation Blueprint */
		bAiming = ShooterCharacter->IsAiming();

		if (bReloading)
		{
			OffsetState = EOffsetState::EOS_Reloading;
		}
		else if (bIsInAir)
		{
			OffsetState = EOffsetState::EOS_InAir;
		}
		else if (ShooterCharacter->IsAiming())
		{
			OffsetState = EOffsetState::EOS_Aiming;
		}
		else
		{
			OffsetState = EOffsetState::EOS_Hip;
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
	/** Handle Turn in Place */
	TurnInPlace();

	/** Lean */
	Lean(DeltaTime);
}

void UShooterAnimInstance::TurnInPlace()
{
	if (!ShooterCharacter) return;

	Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;

	// No need to turn in place if character is moving
	if (Speed > 0 || bIsInAir)
	{
		RootYawOffset = 0.f;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		TIPCharacterYawLastFrame = TIPCharacterYaw;

		RotationCurve = 0.f;
		RotationCurveLastFrame = 0.f;
	}

	TIPCharacterYawLastFrame = TIPCharacterYaw;
	TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;

	float DeltaYaw{ TIPCharacterYaw - TIPCharacterYawLastFrame };

	// RootYawOffset updated and clamped to -180 to 180
	RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - DeltaYaw);

	// Turning metadata value will be 1 if animation playing or 0 if not
	const float Turning = GetCurveValue(TEXT("Turning"));

	if (Turning > 0.f)
	{
		bTurningInPlace = true;
		RotationCurveLastFrame = RotationCurve;
		RotationCurve = GetCurveValue(TEXT("CurveRotation"));
		const float DeltaRotaion{ RotationCurve - RotationCurveLastFrame };

		// If RootYawOffset is positive = Character is Turning Left
		// ELSE Turning Right
		RootYawOffset > 0.f ? RootYawOffset -= DeltaRotaion : RootYawOffset += DeltaRotaion;

		const float ABSRootYawOffset{ FMath::Abs(RootYawOffset) };

		if (ABSRootYawOffset > 90.f)
		{
			const float YawExcess{ ABSRootYawOffset - 90.f };
			RootYawOffset > 0.f ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
		}

	}
	else
	{
		bTurningInPlace = false;
	}

	// Turning in Place dependant recoil weights
	if (bTurningInPlace)
	{
		if (bReloading || bEquipping)
		{
			RecoilWeight = 1.f;
		}
		else
		{
			RecoilWeight = 0.f;
		}
	}
	else // Not turning in place
	{
		if (bCrouching)
		{
			if (bReloading || bEquipping)
			{
				RecoilWeight = 1.f;
			}
			else
			{
				RecoilWeight = 0.1f;
			}
		}
		else
		{
			if (bAiming || bReloading || bEquipping)
			{
				RecoilWeight = 1.f;
			}
			else
			{
				RecoilWeight = 0.5f;
			}
		}
	}
}

void UShooterAnimInstance::Lean(float DeltaTime)
{
	if (!ShooterCharacter) return;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = ShooterCharacter->GetActorRotation();

	FRotator Delta{ UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame) };

	const float Target{ Delta.Yaw / DeltaTime }; // Dividing from DT instead of multiplying to increase the lean
	const float Interp{ FMath::FInterpTo(YawDelta, Target, DeltaTime, 6.f) };

	YawDelta = FMath::Clamp(Interp, -90.f, 90.f);

	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(3, -1, FColor::Blue, FString::Printf(TEXT("Yaw Delta: %f"), YawDelta));
		//GEngine->AddOnScreenDebugMessage(4, -1, FColor::Blue, FString::Printf(TEXT("Delta.Yaw: %f"), Delta.Yaw));
	}
}
