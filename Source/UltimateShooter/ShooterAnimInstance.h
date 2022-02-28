// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EOffsetState : uint8
{
	EOS_Aiming UMETA(DisplayName = "Aiming"),
	EOS_Hip UMETA(DisplayName = "Hip"),
	EOS_Reloading UMETA(DisplayName = "Reloading"),
	EOS_InAir UMETA(DisplayName = "InAir"),

	EOS_MAX UMETA(DisplayName = "DefaultMax"),
};

/**
 * 
 */
UCLASS()
class ULTIMATESHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	UShooterAnimInstance();

	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

protected:

	/** Handle Turning in place variables */
	void TurnInPlace();

	/** Handle calculation for leaning while running */
	void Lean(float DeltaTime);

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* ShooterCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	/** Offset used for strafing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw;

	/** Movement Offset value while character is moving */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw;

	/** Is the character aiming the gun */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	/** Turn In Place: Yaw of the character this frame: Only updated when standing still or not in air */
	float TIPCharacterYaw;

	/** Turn In Place: Yaw of the character prvious frame: Only updated when standing still or not in air */
	float TIPCharacterYawLastFrame;

	/** To rotate Root bone back we need this offset */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float RootYawOffset;

	/** Rotation curve value for this frame */
	float RotationCurve;

	/** Rotation curve value for last frame */
	float RotationCurveLastFrame;

	/** The pitch of the aim rotation used for aim offset */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float Pitch;

	/** True when reloading, used to prevent AimOffset while reloading */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	bool bReloading;

	/** Offset state used to determine which aim offset to use */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	EOffsetState OffsetState;

	/** Yaw of the character this frame */
	FRotator CharacterRotation;

	/** Yaw of the character prvious frame */
	FRotator CharacterRotationLastFrame;

	/** Yaw delta used for leaning in the Running blendspace */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lean", meta = (AllowPrivateAccess = "true"))
	float YawDelta;

	/** Used to determine if the Character is crouching */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crouching", meta = (AllowPrivateAccess = "true"))
	bool bCrouching;

	/** Change the recoil weight depending on turning-in-place or aiming */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float RecoilWeight;

	/** True when turning in place */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bTurningInPlace;
};
