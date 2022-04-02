// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AmmoType.h"
#include "ShooterCharacter.generated.h"

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_UnOccupied UMETA(DisplayName = "UnOccupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "FireTimerInProgress"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_Equipping UMETA(DisplayName = "Equipping"),
	
	ECS_MAX UMETA(DisplayName = "DefaultMax")
};

USTRUCT(BlueprintType)
struct FInterpLocation
{
	GENERATED_BODY()

	// Scene component to use for its location
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponent;

	// No of items interping to/at this scene comp location
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighlightIconDelegate, int32, SlotIndex, bool, bStartAnimation);

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
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult);

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
	void EquipWeapon(AWeapon* WeaponToEquip, bool bSwapping = false);

	/** Detach weapon and let it fall to the ground */
	void DropWeapon();

	/** Drops currently Equipped weapon and Equipps TraceHitWeapon */
	void SwapWeapon(AWeapon* WeaponToSwap);

	/** Detach Weapon Action Mappings */
	void SelectButtonPressed();
	void SelectButtonReleased();

	/** Initialize AmmoMap with Starting Ammo values */
	void InitializeAmmoMap();

	/** Chedck to make sure weapon has ammo! */
	bool WeaponHasAmmo();

	/** FireWeapon functions */
	void PlayFireSound();
	void SendBullet();
	void PlayGunfireMontage();

	/** Bound to R key */
	void ReloadButtonPressed();

	/** Reload the weapon if Unoccupied */
	void ReloadWeapon();

	/** Called when Finished Reloading (on Notifier) */
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	/** Called when Finished Equipping (on Notifier) */
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	/** Checks to see if we have ammo for the carrying weapon */
	bool CarryingAmmo();

	/** Called when Character grabs the clip to reload notify (See ReloadSMG montage) */
	UFUNCTION(BlueprintCallable)
	void GrabClip();

	/** Called when Character releases the clip notify (See ReloadSMG montage) */
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();

	/** Pressed when need to crouch */
	void CrouchButtonPressed();

	/** Jump override */
	virtual void Jump() override;

	/** Interps capsule half height when crouching/standing */
	void InterpCapsuleHalfHeight(float DeltaTime);

	void Aim();
	void ExitAiming();

	void PickupAmmo(class AAmmo* Ammo);

	void InitializeInterpLocations();

	/** Inventory Key Binds */
	void FKeyPressed();
	void OneKeyPressed();
	void TwoKeyPressed();
	void ThreeKeyPressed();
	void FourKeyPressed();
	void FiveKeyPressed();

	void ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex);

	int32 GetEmptyInventorySlot();

	void HighlightInventorySlot();

	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetSurfaceType();
	
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float DefaultCameraFOV;

	/** Zoomed Camera FOV when aiming */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
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

	/** Map to keep track of different ammo types */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;
	
	/** Starting amount of 9mm ammo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 Starting9mmAmmo;

	/** Starting amount of Assult Rifle ammo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 StartingARAmmo;

	/** Current combat state: Can only fire or Reload if UnOccupied */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

	/** Montage for Reload Animations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;

	/** Montage for Equip Animations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* EquipMontage;

	/** Transform of the clip when we first grabbed the clip during reloading */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FTransform ClipTransform;

	/** Scene Component to be attached to Actors hand during reloading  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandSceneComponent;

	/** Set to true when crouching */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bCrouching;

	/** Regular movement speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float BaseMovementSpeed;

	/** Crouched movement speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CrouchMovementSpeed;

	/** Current Half Height of the capsule */
	float CurrentCapsuleHalfHeight;

	/** Half height of the capsule when standing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float StandingCapsuleHalfHeight;

	/** Half height of the capsule when crouching */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CrouchingCapsuleHalfHeight;

	/** Base Ground Friction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float BaseGroundFriction;

	/** Ground Friction when Crouching: Set to Higher value to avoid Skid to stop */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CrouchingGroundFriction;

	/** Used for knowing when Aiming button is pressed */
	bool bAimingButtonPressed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	USceneComponent* WeaponInterpComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp6;

	/** Array of interp locations struct */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	TArray<FInterpLocation> InterpLocations;

	FTimerHandle PickupSoundTimer;
	FTimerHandle EquipSoundTimer;

	bool bShouldPlayPickupSound;
	bool bShouldPlayEquipSound;

	// Timer Handles: Private
	void ResetPickupSoundTimer();
	void ResetEquipSoundTimer();

	/** Time to wait before playing another pickup sound */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float PickupSoundResetTime;

	/** Time to wait before playing another equip sound */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float EquipSoundResetTime;

	/** Array of AItems for Inventory */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	TArray<AItem*> Inventory;

	const int32 INVENTORY_CAPACITY{ 6 };

	/** Delegate for sending slot information to Inventory Bar when equipping  */
	UPROPERTY(BlueprintAssignable, Category = Delegate, meta = (AllowPrivateAccess = "true"))
	FEquipItemDelegate EquipItemDelegate;

	/** Delegate for sending slot information to Play Icon Animation  */
	UPROPERTY(BlueprintAssignable, Category = Delegate, meta = (AllowPrivateAccess = "true"))
	FHighlightIconDelegate HighlightIconDelegate;
	
	/** Index for the currently highlited slot */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	int32 HighlightedSlot;

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
	/** DEPRECATED */
	//FVector GetCameraInterpLocation();

	/** Get Pickup item */
	void GetPickupItem(AItem* Item);

	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	FORCEINLINE bool GetCrouching() const { return bCrouching; }
	
	FInterpLocation GetInterpLocation(int32 index);
	
	// Gets a least amount of occupied interp location slot
	int32 GetInterpLocationIndex();

	void IncrementInterpLocItemCount(int32 Index, int32 Amount);

	FORCEINLINE bool ShouldPlayPickupSound() const { return bShouldPlayPickupSound; }
	FORCEINLINE bool ShouldPlayEquipSound() const { return bShouldPlayEquipSound; }

	void StartPickupSoundTimer();
	void StartEquipSoundTimer();

	void UnHighlightInventorySlot();

	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
};
