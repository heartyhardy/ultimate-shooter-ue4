// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"


UCLASS()
class ULTIMATESHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called for Forward/Backward Movement */
	void MoveForward(float Value);

	/** Called for Side to Side Movement */
	void MoveRight(float Value);

	/**
	* Called via Input to turn at a given rate
	* @param Rate This is normalized rate. i.e. 1.0 means 100% of the desired turn rate.
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via Input to Look up/down at a given rate
	* @param Rate This is a normalized rate. i.e. 1.0 means 100% of the desired look up/down rate.
	*/
	void LookUpAtRate(float Rate);

	/**
	* Called via Mouse movement in X at a rate
	* @param Rate Movement in Mouse X direction.
	* @param Rate This is a normalized rate. i.e. 1.0 means 100% of the desired Turn rate.
	*/
	void MouseTurnAtRate(float Rate);

	/**
	* Called via Mouse movement in Y at a rate
	* @param Rate Movement in Mouse Y direction.
	* @param Rate This is a normalized rate. i.e. 1.0 means 100% of the desired look up/down rate.
	*/
	void MouseLookUpAtRate(float Rate);

	/** Beam End Location depending on the Gun socket or Crosshairs */
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& BeamEndLocation);

	/** Called when Fire Button is pressed */
	void FireWeapon();

	/** Start/Stop Aiming depending on mouse/gamepad input */
	void StartAiming();
	void StopAiming();

	/** Interpolate Camera Zoom when aiming is ON/OFF */
	void InterpCameraZoom(float DeltaTime);

	/** Setup Base Turn/LookUp Rates when Aiming ON/OFF */
	void SetupTurnRate();

	/** Calculate Crosshair size */
	void CalculateCrosshairSpread(float DeltaTime);

	/** Start Stop Crosshair spread when firing the weapon */
	UFUNCTION()
	void StartCrosshairFireTimer();
	UFUNCTION()
	void StopCrosshairFireTimer();

	/** Registers Auto fire input with PlayerInputController */
	void AutoFirePressed();
	void AutoFireReleased();

	/** Starts firing if bShould fire is true, also starts the auto-fire cooldown timer */
	void StartAutoFire();

	UFUNCTION()
	void ResetAutoFire();

	/** Line trace for Item under crosshairs  */
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);

	/** Trace for items if overlappedItemCount > 0 */
	void TraceForItems();

	/** Spawn the default weapon and attaches it to the mesh */
	class AWeapon* SpawnDefaultWeapon();

	/** Equippes any given weapon by attaching it to mesh */
	void EquipWeapon(AWeapon* WeaponToEquip);

	/** Detach weapon and let it fall to the ground */
	void DropWeapon();

	/** Drops currently Equipped weapon and Equipps TraceHitWeapon */
	void SwapWeapon(AWeapon* WeaponToSwap);

	/** Detach Weapon Action Mappings */
	void SelectButtonPressed();
	void SelectButtonReleased();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	/** Camera boom positioning behind the Character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera , meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Camera that follows the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Base turn rate in degrees per second. Other scaling may affect the final turn rate */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	/** Base Look up/down Rate in degrees per second. Other scaling may affect the final rate */
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookupRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipTurnRate;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimTurnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipTurnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimTurnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	/** Default Camera FOV just after creation */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float DefaultCameraFOV;

	/** Zoomed Camera FOV when aiming */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ZoomedCameraFOV;

	/** Current value of Camera's FOV during aiming */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float CurrentCameraFOV;

	/** Determine the speed of camera zoom in and out */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float CameraInterpSpeed;
	
	/** Dynamic Crosshair Size Factor */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;

	/** Velocity component for the crosshair spread */
	float CrosshairVelocityFactor;

	/** In air component for the crosshair spread */
	float CrosshairInAirFactor;

	/** Aiming factor for the crosshair spread */
	float CrosshairAimingFactor;

	/** Firing factor for the crosshair spread */
	float CrosshairFiringFactor;

	/** Randomized Sound cue reference for weapon fire. Using editanywhere and blueprintreadwrite to set it in the blueprint */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	/** Gun Muzzle Flash particle system. Attached to: BarrelSocket */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	/** Animation montage for Primary Weapon Fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* FireFromHipMontage;

	/** Bullet impact particle system */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	/** Bullet Smoke Trail Beam particle system */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* SmokeBeam;

	/** Determine crosshair spread when firing */
	bool bFiring;
	float ShootTimeDuration;
	FTimerHandle CrosshairShootTimerHandle;

	/** LMB or Right Console Trigger Pressed*/
	bool bAutoFireButtonPressed;
	
	/** Fire cooldown, waiting for timer tick */
	bool bShouldAutoFire;

	/** Rate of Auto-Fire */
	float AutomaticFireRate;

	/** Timer handle for automatic fire cooldown */
	FTimerHandle AutoFireTimerHandle;

	/** True if Item Line Tracing to be run every frame */
	bool bShouldTraceForItems;

	/** No of overlapped items when tracing for items */
	int8 OverlappedItemCount;

	/** Item that was traced last frame. When trace doesnt hit, we can hide it using this */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item, meta =  (AllowPrivateAccess = "true"))
	class AItem* TraceHitItemLastFrame;

	/** Currently Equipped Weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

	/** Set this in blueprint as the default weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	/** Item Currently Hit by in TraceForItems() */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combt, meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItem;

	/** Distance outward from camera for the interp destination */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpDistance;
	
	/** Distance upward from camera for the interp destination */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpElevation;

public:

	/** Returns Camera boom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns Camera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Exposes Aiming state to AimInstance */
	FORCEINLINE bool IsAiming() const { return bAiming; };

	/** Expose no of items overlapped in AreaSphere item tracing */
	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

	/** Incrememt/Decrement no of overlapped Items. When 0, can stop tracing */
	void IncrememtOverlappedItemCount(int8 Amount);

	/** Exposes CrosshairSpreadMultiplier to Blueprints */
	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	/** Camera Interp Location */
	FVector GetCameraInterpLocation();

	/** Get Pickup item */
	void GetPickupItem(AItem* Item);
};
