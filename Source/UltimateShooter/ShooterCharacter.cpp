// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Item.h"
#include "Weapon.h"
#include "Ammo.h"


// Sets default values
AShooterCharacter::AShooterCharacter() :
	// Turning and LookUp rates
	BaseTurnRate(45.f),
	BaseLookupRate(45.f),
	HipTurnRate(60.f),
	HipLookUpRate(60.f),
	AimTurnRate(20.f),
	AimLookUpRate(20.f),
	// Mouse Turn and LookUp rates
	MouseHipTurnRate(1.f),
	MouseHipLookUpRate(1.f),
	MouseAimTurnRate(0.6f),
	MouseAimLookUpRate(0.6f),
	bAiming(false),
	// FOV values when Aiming ON/OFF
	DefaultCameraFOV(0.f), // Will be changed in constructor
	ZoomedCameraFOV(25.f),
	CurrentCameraFOV(0.f),
	CameraInterpSpeed(20.f),
	// Crosshair Spread
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimingFactor(0.f),
	CrosshairFiringFactor(0.f),
	// Crosshair spread when shooting
	ShootTimeDuration(0.1f),
	bFiring(false),
	// Auto Fire
	bShouldAutoFire(true),
	bAutoFireButtonPressed(false),
	// Item Tracing
	bShouldTraceForItems(false),
	// Camera Interp Distances
	CameraInterpDistance(250.f),
	CameraInterpElevation(65.f),
	// Ammo defaults
	Starting9mmAmmo(85),
	StartingARAmmo(125),
	// Combat State
	CombatState(ECombatState::ECS_UnOccupied),
	// Movement
	bCrouching(false),
	BaseMovementSpeed(650.f),
	CrouchMovementSpeed(300.f),
	CrouchingCapsuleHalfHeight(44.f),
	StandingCapsuleHalfHeight(88.f),
	BaseGroundFriction(2.f),
	CrouchingGroundFriction(100.f),
	//Aiming
	bAimingButtonPressed(false),
	// Pickup and Equip sound properties
	bShouldPlayEquipSound(true),
	bShouldPlayPickupSound(true),
	PickupSoundResetTime(0.2f),
	EquipSoundResetTime(0.2f),
	// Icon animation
	HighlightedSlot(-1)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/** Create a Camera Boom: Pulls in towards the character if there's a collision **/
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 200.f; // Camera follows the pawn at this distance
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 75.f); // Offset the camera to right and up in Z
	CameraBoom->bUsePawnControlRotation = true; // Uses Pawn's orientation 

	/** Create a Follow Camera and setup: Camera Boom controls it **/
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach it to the end of the Spring arm
	FollowCamera->bUsePawnControlRotation = false; // Follow Rotation of the Camera Boom instead

	/** Disable Character rotation when Controller rotates. Let the Controller only affect the Camera */
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true; // Let the character rotate with the controller

	/** Configure Character movement */
	GetCharacterMovement()->bOrientRotationToMovement = false; // Charcter doesn't orient in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator{ 0.f, 540.f, 0.f }; // ... at this Rotation Rate

	/** Configure Character Jump */
	GetCharacterMovement()->JumpZVelocity = 350.f;
	GetCharacterMovement()->AirControl = 0.2f; // How much responsive Character is while Airborne

	/** Create HandSceneComponebnt */
	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComp"));

	/** Create Interpolation components */
	WeaponInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Interpolation Component"));
	WeaponInterpComp->SetupAttachment(GetFollowCamera());

	InterpComp1 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 1"));
	InterpComp1->SetupAttachment(GetFollowCamera());
	
	InterpComp2 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 2"));
	InterpComp2->SetupAttachment(GetFollowCamera());
	
	InterpComp3 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 3"));
	InterpComp3->SetupAttachment(GetFollowCamera());
	
	InterpComp4 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 4"));
	InterpComp4->SetupAttachment(GetFollowCamera());
	
	InterpComp5 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 5"));
	InterpComp5->SetupAttachment(GetFollowCamera());
	
	InterpComp6 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 6"));
	InterpComp6->SetupAttachment(GetFollowCamera());
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultCameraFOV = FollowCamera->FieldOfView;
	CurrentCameraFOV = DefaultCameraFOV;

	// Spawn the default weapon and equip it
	EquipWeapon(SpawnDefaultWeapon());
	// Add the Default Weapon to the Inventory
	Inventory.Add(EquippedWeapon);
	EquippedWeapon->SetSlotIndex(0);

	EquippedWeapon->DisableCustomDepth();
	EquippedWeapon->DisableGlowMaterial();

	// Set Character ref for the default Weapon
	EquippedWeapon->SetCharacter(this);

	// Initialize Ammo map
	InitializeAmmoMap();

	// Set the base movement speed
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;

	// Create Interplocations for each element in array
	InitializeInterpLocations();
}

void AShooterCharacter::MoveForward(float Value)
{
	if (Controller && Value != 0.f)
	{
		// Get Controller Rotation
		const FRotator ControllerRotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, ControllerRotation.Yaw, 0 };

		// Get Forward Direction
		const FVector ForwardDirection{ FRotationMatrix{ YawRotation }.GetUnitAxis(EAxis::X) };

		// Scales the Direction using the Input value
		AddMovementInput(ForwardDirection, Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if (Controller && Value != 0.f)
	{
		// Get Controller Rotation
		const FRotator ControllerRotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, ControllerRotation.Yaw, 0 };

		// Get Forward Direction
		const FVector ForwardDirection{ FRotationMatrix{ YawRotation }.GetUnitAxis(EAxis::Y) };
		
		// Scales the Direction using the Input value
		AddMovementInput(ForwardDirection, Value);
	}
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	// Calculate Delta for this frame from the given Rate
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->DeltaTimeSeconds); // Degrees/sec * sec/Frame = Degrees/Frame
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	// Calculate Delta for this frame for the given Rate
	AddControllerPitchInput(Rate * BaseLookupRate * GetWorld()->DeltaTimeSeconds); // Degrees/sec * sec/Frame = Degrees/Frame
}

void AShooterCharacter::MouseTurnAtRate(float Rate)
{
	float TurnRateFactor{};

	if (bAiming)
	{
		TurnRateFactor = MouseAimTurnRate;
	}
	else
	{
		TurnRateFactor = MouseHipTurnRate;
	}
	AddControllerYawInput(Rate * TurnRateFactor);
}

void AShooterCharacter::MouseLookUpAtRate(float Rate)
{
	float LookUpRateFactor{};

	if (bAiming)
	{
		LookUpRateFactor = MouseAimLookUpRate;
	}
	else
	{
		LookUpRateFactor = MouseHipLookUpRate;
	}
	AddControllerPitchInput(Rate * LookUpRateFactor);
}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& BeamEndLocation)
{
	// Check for crosshair trace hit
	FHitResult CrosshairHitResult;
	bool bTraceSuccessful = TraceUnderCrosshairs(CrosshairHitResult, BeamEndLocation);

	if (bTraceSuccessful)
	{
		// This is tenative. Still need to trace from gun barrel
		BeamEndLocation = CrosshairHitResult.Location;
	}
	else // No crosshair hit
	{
		// BeamEndLocation is the end in line trace (because blockinghit failed)
	}

	// Perform Gun barrel trace
	FHitResult WeaponTraceHit;
	FVector WeaponTraceStart{ MuzzleSocketLocation };
	FVector StartToEnd{ BeamEndLocation - MuzzleSocketLocation };
	FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25f }; // Extend the 2nd Trace by 1.25 Times to hit obstacles properly

	GetWorld()->LineTraceSingleByChannel(
		WeaponTraceHit,
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility
	);

	if (WeaponTraceHit.bBlockingHit) // If there was an object between Gun Barrel and the BeamEndPoint?
	{
		BeamEndLocation = WeaponTraceHit.Location;
		return true;
	}
	return false;
}

void AShooterCharacter::FireWeapon()
{
	if (!EquippedWeapon) return;
	
	// If not unoccupied early return
	if (CombatState != ECombatState::ECS_UnOccupied) return;

	if (WeaponHasAmmo())
	{
		PlayFireSound();
		SendBullet();
		PlayGunfireMontage();
		EquippedWeapon->DecrementAmmo();
		/** Start Timer for crosshair spread factor when firing */
		StartCrosshairFireTimer();
		// Start Auto Fire
		StartAutoFire(); // Start Fire Timer

		if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol)
		{
			//Start Moving Slide Timer
			EquippedWeapon->StartSlideTimer();
		}
	}
}

/** Set Aiming ON/OFF */
void AShooterCharacter::StartAiming()
{
	bAimingButtonPressed = true;
	if (CombatState != ECombatState::ECS_Reloading)
	{
		Aim();
	}
}

void AShooterCharacter::Aim()
{
	bAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed; //Walking slowly while aiming
}

void AShooterCharacter::ExitAiming()
{
	bAiming = false;
	if (!bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed; //Walking slowly while aiming
	}
}

void AShooterCharacter::PickupAmmo(AAmmo* Ammo)
{
	// Check to see if AmmoMap contains above ammo type
	if (AmmoMap.Find(Ammo->GetAmmoType()))
	{
		int32 AmmoCount{ AmmoMap[Ammo->GetAmmoType()] };
		AmmoCount += Ammo->GetItemCount();

		// Set amount of ammo for this type
		AmmoMap[Ammo->GetAmmoType()] = AmmoCount;
	}

	if (EquippedWeapon && EquippedWeapon->GetAmmoType() == Ammo->GetAmmoType())
	{
		// Check if Gun is empty
		if (EquippedWeapon->GetAmmo() == 0)
		{
			ReloadWeapon();
		}
	}

	Ammo->Destroy();
}

void AShooterCharacter::InitializeInterpLocations()
{
	FInterpLocation WeaponLocation{WeaponInterpComp,0};
	InterpLocations.Add(WeaponLocation);

	FInterpLocation InterpLoc1{ InterpComp1, 0 };
	InterpLocations.Add(InterpLoc1);

	FInterpLocation InterpLoc2{ InterpComp2, 0 };
	InterpLocations.Add(InterpLoc2);

	FInterpLocation InterpLoc3{ InterpComp3, 0 };
	InterpLocations.Add(InterpLoc3);

	FInterpLocation InterpLoc4{ InterpComp4, 0 };
	InterpLocations.Add(InterpLoc4);

	FInterpLocation InterpLoc5{ InterpComp5, 0 };
	InterpLocations.Add(InterpLoc5);

	FInterpLocation InterpLoc6{ InterpComp6, 0 };
	InterpLocations.Add(InterpLoc6);
}

void AShooterCharacter::FKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 0) return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 0);
}

void AShooterCharacter::OneKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 1) return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 1);
}

void AShooterCharacter::TwoKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 2) return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 2);
}

void AShooterCharacter::ThreeKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 3) return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 3);
}

void AShooterCharacter::FourKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 4) return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 4);
}

void AShooterCharacter::FiveKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 5) return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 5);
}

void AShooterCharacter::ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex)
{
	const bool bCanExchangeItems = (CurrentItemIndex != NewItemIndex)
		&& (NewItemIndex < Inventory.Num())
		&& (CombatState == ECombatState::ECS_UnOccupied || CombatState == ECombatState::ECS_Equipping);

	if (bCanExchangeItems)
	{
		auto OldEquippedWeapon = EquippedWeapon;
		auto NewWeapon = Cast<AWeapon>(Inventory[NewItemIndex]);
		EquipWeapon(NewWeapon);

		OldEquippedWeapon->SetItemState(EItemState::EIS_PickedUp);
		NewWeapon->SetItemState(EItemState::EIS_Equipped);

		CombatState = ECombatState::ECS_Equipping;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && EquipMontage)
		{
			AnimInstance->Montage_Play(EquipMontage, 1.0f);
			AnimInstance->Montage_JumpToSection(FName(TEXT("Equip")));
		}

		NewWeapon->PlayEquipSound(true); // Force sound to play
	}
}

int32 AShooterCharacter::GetEmptyInventorySlot()
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] == nullptr)
		{
			return i;
		}
	}
	if (Inventory.Num() < INVENTORY_CAPACITY)
	{
		return Inventory.Num();
	}
	return -1; //Inventory is full
}

void AShooterCharacter::HighlightInventorySlot()
{
	const int32 EmptySlot = GetEmptyInventorySlot();

	HighlightIconDelegate.Broadcast(EmptySlot, true);
	HighlightedSlot = EmptySlot;
}

void AShooterCharacter::UnHighlightInventorySlot()
{
	HighlightIconDelegate.Broadcast(HighlightedSlot, false);
	HighlightedSlot = -1;
}

int32 AShooterCharacter::GetInterpLocationIndex()
{
	int32 LowestIndex{ 1 };
	int32 LowestCount{ INT_MAX };

	// Find min
	for (int32 i = 1; i < InterpLocations.Num(); i++)
	{
		if (InterpLocations[i].ItemCount < LowestCount)
		{
			LowestIndex = i;
			LowestCount = InterpLocations[i].ItemCount;
		}
	}

	return LowestIndex;
}

void AShooterCharacter::IncrementInterpLocItemCount(int32 Index, int32 Amount)
{
	if (Amount < -1 || Amount > 1) return;

	if (InterpLocations.Num() >= Index)
	{
		InterpLocations[Index].ItemCount += Amount;
	}
}

void AShooterCharacter::StartPickupSoundTimer()
{
	bShouldPlayPickupSound = false;
	GetWorldTimerManager().SetTimer(PickupSoundTimer, this, &AShooterCharacter::ResetPickupSoundTimer, PickupSoundResetTime);
}

void AShooterCharacter::StartEquipSoundTimer()
{
	bShouldPlayEquipSound = false;
	GetWorldTimerManager().SetTimer(EquipSoundTimer, this, &AShooterCharacter::ResetEquipSoundTimer, EquipSoundResetTime);
}

void AShooterCharacter::StopAiming()
{
	bAimingButtonPressed = false;
	ExitAiming();
}

void AShooterCharacter::InterpCameraZoom(float DeltaTime)
{
	/** Set Camera FOV */
	if (bAiming)
	{
		// Interp to Zoomed when Aiming
		CurrentCameraFOV = FMath::FInterpTo(
			CurrentCameraFOV,
			ZoomedCameraFOV,
			DeltaTime,
			CameraInterpSpeed
		);
	}
	else
	{
		// Interp to Default when not Aiming
		CurrentCameraFOV = FMath::FInterpTo(
			CurrentCameraFOV,
			DefaultCameraFOV,
			DeltaTime,
			CameraInterpSpeed
		);
	}

	FollowCamera->SetFieldOfView(CurrentCameraFOV);
}

void AShooterCharacter::SetupTurnRate()
{
	if (bAiming)
	{
		BaseTurnRate = AimTurnRate;
		BaseLookupRate = AimLookUpRate;
	}
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookupRate = HipLookUpRate;
	}
}

void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{

	FVector2D WalkSpeedRange { 0.f, 600.f };
	FVector2D VelocityMultiplierRange{ 0.f, 1.f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f; // We just need the lateral component of the velocity

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
		WalkSpeedRange,
		VelocityMultiplierRange,
		GetVelocity().Size()
	);

	/** Spread the crosshair while in air */
	if (GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(
			CrosshairInAirFactor,
			2.25f,
			DeltaTime,
			2.25f
		);
	}
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(
			CrosshairInAirFactor,
			0.f,
			DeltaTime,
			30.f
		);
	}

	/** Decrease crosshair spread while aiming */

	if (bAiming)
	{
		CrosshairAimingFactor = FMath::FInterpTo(
			CrosshairAimingFactor,
			0.5f,
			DeltaTime,
			2.25f
		);
	}
	else
	{
		CrosshairAimingFactor = FMath::FInterpTo(
			CrosshairAimingFactor,
			0.f,
			DeltaTime,
			30.f
		);
	}

	/** Spread Crosshair while shooting */

	if (bFiring)
	{
		CrosshairFiringFactor = FMath::FInterpTo(
			CrosshairFiringFactor,
			0.5f,
			DeltaTime,
			20.f
		);
	}
	else
	{
		CrosshairFiringFactor = FMath::FInterpTo(
			CrosshairFiringFactor,
			0.f,
			DeltaTime,
			20.f
		);
	}

	CrosshairSpreadMultiplier = 
		0.5f + 
		CrosshairVelocityFactor +
		CrosshairInAirFactor - // Substract when aiming to reduce the crosshair spread 
		CrosshairAimingFactor +
		CrosshairFiringFactor;
}

void AShooterCharacter::StartCrosshairFireTimer()
{
	bFiring = true;
	GetWorldTimerManager().SetTimer(
		CrosshairShootTimerHandle,
		this,
		&AShooterCharacter::StopCrosshairFireTimer,
		ShootTimeDuration
	);
}

void AShooterCharacter::StopCrosshairFireTimer()
{
	bFiring = false;
}

void AShooterCharacter::AutoFirePressed()
{
	bAutoFireButtonPressed = true;	
	FireWeapon();
}

void AShooterCharacter::AutoFireReleased()
{
	bAutoFireButtonPressed = false;
}

void AShooterCharacter::StartAutoFire()
{
	if (!EquippedWeapon) return;

	CombatState = ECombatState::ECS_FireTimerInProgress;

	GetWorldTimerManager().SetTimer(
		AutoFireTimerHandle,
		this,
		&AShooterCharacter::ResetAutoFire,
		EquippedWeapon->GetAutoFireRate()
	);
}

void AShooterCharacter::ResetAutoFire()
{
	CombatState = ECombatState::ECS_UnOccupied;

	if (WeaponHasAmmo())
	{
		if (bAutoFireButtonPressed)
		{
			FireWeapon();
		}
	}
	else
	{
		// Reload Weapon Happens here
		ReloadWeapon();
	}
}

bool AShooterCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	FVector2D ViewportSize;

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation{ ViewportSize.X / 2.f, ViewportSize.Y / 2.f };
	FVector CrosshairWorldLocation;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldLocation,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)
	{
		FVector Start{ CrosshairWorldLocation };
		FVector End{ CrosshairWorldLocation + CrosshairWorldDirection * 50'000 };
		OutHitLocation = End;

		GetWorld()->LineTraceSingleByChannel(
			OutHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);

		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}
	return false;
}

// Trace for items in Tick() if bShouldTraceForItems is true (OverlappedItem count > 0)
void AShooterCharacter::TraceForItems()
{
	if (bShouldTraceForItems)
	{
		FHitResult CrosshairTraceHit;
		FVector HitLocation;
		bool bTraceSuccessful = TraceUnderCrosshairs(CrosshairTraceHit, HitLocation);

		if (CrosshairTraceHit.bBlockingHit)
		{

			TraceHitItem = Cast<AItem>(CrosshairTraceHit.Actor);
			auto TraceHitWeapon = Cast<AWeapon>(TraceHitItem);
			
			if (TraceHitWeapon)
			{
				if (HighlightedSlot == -1)
				{
					// Not currently highlighting  a slot: Highlight one
					HighlightInventorySlot();
				}
			}
			else
			{
				// Is a slot being highlighted
				if (HighlightedSlot != -1)
				{
					// Unhighlight inventory slot
					UnHighlightInventorySlot();
				}
			}

			// Wont be able to Spam the SELECT key now
			if (TraceHitItem && TraceHitItem->GetItemState() == EItemState::EIS_EquipInterping)
			{
				TraceHitItem = nullptr;
			}

			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				TraceHitItem->GetPickupWidget()->SetVisibility(true);
				TraceHitItem->EnableCustomDepth();

				if (Inventory.Num() >= INVENTORY_CAPACITY)
				{
					// Inventory is full
					TraceHitItem->SetCharacterInventoryFull(true);
				}
				else
				{
					// Inventory has room
					TraceHitItem->SetCharacterInventoryFull(false);
				}

			}

			// If we hit an Item last frame
			if (TraceHitItemLastFrame)
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					// We are hitting a different AItem this frame from last
					// Or AItem is null
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
					TraceHitItemLastFrame->DisableCustomDepth();
				}
			}

			// Store reference to HitItem next frame
			TraceHitItemLastFrame = TraceHitItem;
		}
	}
	else if (TraceHitItemLastFrame)
	{
		// No longer overlapping any items
		// Hide the last traced item if its not null
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
		TraceHitItemLastFrame->DisableCustomDepth();
	}
}

AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass)
	{
		// Spawn the weapon
		AWeapon* DefaultWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
		return DefaultWeapon;
	}
	return nullptr;
}

void AShooterCharacter::EquipWeapon(AWeapon* WeaponToEquip, bool bSwapping)
{
	if (WeaponToEquip)
	{
		// Get the socket attached to the right hand bone (hand_r)
		const USkeletalMeshSocket* RightHandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));

		if (RightHandSocket)
		{
			// Attach the given weapon to the socket
			RightHandSocket->AttachActor(WeaponToEquip, GetMesh());
		}

		// Broadcast
		if (!EquippedWeapon)
		{
			// -1 if no equipped weapon yet. No need to reverse the icon animation
			EquipItemDelegate.Broadcast(-1, WeaponToEquip->GetSlotIndex());
		}
		else if (!bSwapping)
		{
			EquipItemDelegate.Broadcast(EquippedWeapon->GetSlotIndex(), WeaponToEquip->GetSlotIndex());
		}

		// Set the Given weapon as the Equipped weapon
		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}

void AShooterCharacter::DropWeapon()
{
	if (EquippedWeapon) // ADD && EquippedWeapon->GetItemState() == EItemState::EIS_Equipped ==> To stop Impulsing over and over
	{
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);

		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		// Throw the weapon
		EquippedWeapon->ThrowWeapon();
	}
}

void AShooterCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	// Check inventory is large enough to accomodate that index
	if (Inventory.Num() - 1 >= EquippedWeapon->GetSlotIndex())
	{
		Inventory[EquippedWeapon->GetSlotIndex()] = WeaponToSwap;
		WeaponToSwap->SetSlotIndex(EquippedWeapon->GetSlotIndex());
	}

	DropWeapon();
	EquipWeapon(WeaponToSwap, true);
	// Clear the Tracehitweapon and LastTracehit because we just equipped the item

	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}

// Select Button E is pressed
void AShooterCharacter::SelectButtonPressed()
{
	if (CombatState != ECombatState::ECS_UnOccupied) return;
	if (TraceHitItem)
	{
		TraceHitItem->StartItemCurve(this, true);
		TraceHitItem = nullptr; // Prevent Spamming SELECT
	}
}

// Select Button E is released
void AShooterCharacter::SelectButtonReleased()
{
}

void AShooterCharacter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_9mm, Starting9mmAmmo);
	AmmoMap.Add(EAmmoType::EAT_AR, StartingARAmmo);
}

bool AShooterCharacter::WeaponHasAmmo()
{
	if (!EquippedWeapon) return false;

	return EquippedWeapon->GetAmmo() > 0;
}

void AShooterCharacter::PlayFireSound()
{
	/** Play the sound cue */
	if (EquippedWeapon->GetFireSound())
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->GetFireSound());
	}
}

void AShooterCharacter::SendBullet()
{
	const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");

	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());

		if (EquippedWeapon->GetMuzzleFlash())
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EquippedWeapon->GetMuzzleFlash(), SocketTransform);
		}

		FVector BeamEndLocation;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEndLocation);

		if (bBeamEnd)
		{
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					BeamEndLocation
				);
			}

			if (SmokeBeam)
			{
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					SmokeBeam,
					SocketTransform
				);

				if (Beam)
				{
					Beam->SetVectorParameter(FName("Target"), BeamEndLocation);
				}
			}
		}
	}
}

void AShooterCharacter::PlayGunfireMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && FireFromHipMontage)
	{
		AnimInstance->Montage_Play(FireFromHipMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFireFromHip"));
	}
}

void AShooterCharacter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void AShooterCharacter::ReloadWeapon()
{
	if (CombatState != ECombatState::ECS_UnOccupied) return;

	if (!EquippedWeapon) return;

	// Create a function if we have correct ammo type

	if (CarryingAmmo() && !EquippedWeapon->ClipIsFull())
	{
		if (bAiming) // If Aiming while reloading, stop aiming
		{
			ExitAiming();
		}

		CombatState = ECombatState::ECS_Reloading;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && ReloadMontage)
		{
			AnimInstance->Montage_Play(ReloadMontage);
			AnimInstance->Montage_JumpToSection(EquippedWeapon->GetReloadMontageSection());
		}

	}
}

void AShooterCharacter::FinishReloading()
{
	// Update Combat State
	CombatState = ECombatState::ECS_UnOccupied;

	// If reloading finished but Aiming button is still pressed
	if (bAimingButtonPressed)
	{
		Aim();
	}

	if (!EquippedWeapon) return;

	const auto AmmoType{ EquippedWeapon->GetAmmoType() };

	// Update ammo map
	if (AmmoMap.Contains(AmmoType))
	{
		// Currently carried ammo for this weapon
		int32 CarriedAmmo = AmmoMap[AmmoType];

		// Mag empty space of this weapon
		const int32 MagaEmptySpace = EquippedWeapon->GetMagazineCapacity() - EquippedWeapon->GetAmmo();

		if (MagaEmptySpace > CarriedAmmo)
		{
			// Reload the mag with all the ammo we carrying
			EquippedWeapon->ReloadAmmo(CarriedAmmo);
			CarriedAmmo = 0;
			AmmoMap.Add(AmmoType, CarriedAmmo); // TMAP will replace the value for the key
		}
		else
		{
			// Fill the Magazine
			EquippedWeapon->ReloadAmmo(MagaEmptySpace);
			CarriedAmmo -= MagaEmptySpace;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}

	}
}

void AShooterCharacter::FinishEquipping()
{
	CombatState = ECombatState::ECS_UnOccupied;
}

bool AShooterCharacter::CarryingAmmo()
{
	if (!EquippedWeapon) return false;

	auto AmmoType = EquippedWeapon->GetAmmoType();

	if (AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType] > 0;
	}

	return false;
}

void AShooterCharacter::GrabClip()
{
	if (!EquippedWeapon) return;
	if (!HandSceneComponent) return;

	int32 ClipBoneIndex{ EquippedWeapon->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetClipBoneName()) };
	ClipTransform = EquippedWeapon->GetItemMesh()->GetBoneTransform(ClipBoneIndex);

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
	// Attach it to the Hand mesh using above rules
	HandSceneComponent->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("hand_l")));
	// Set World Transform of the HandSceneComponent to the clip's world transform
	HandSceneComponent->SetWorldTransform(ClipTransform);

	//Set bool bMovingClip in weapon to true
	EquippedWeapon->SetMovingClip(true);
}

void AShooterCharacter::ReleaseClip()
{
	//Set bool bMovingClip in weapon to false
	EquippedWeapon->SetMovingClip(false);
}

void AShooterCharacter::CrouchButtonPressed()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		bCrouching = !bCrouching;
	}

	if (bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
		GetCharacterMovement()->GroundFriction = CrouchingGroundFriction;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
		GetCharacterMovement()->GroundFriction = BaseGroundFriction;
	}
}

void AShooterCharacter::Jump()
{
	if (bCrouching)
	{
		bCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
	else
	{
		ACharacter::Jump();
	}
}

void AShooterCharacter::InterpCapsuleHalfHeight(float DeltaTime)
{
	float TargetCapsuleHalfHeight{};

	if (bCrouching)
	{
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
	}
	else
	{
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
	}

	const float InterpHalfHeight{ FMath::FInterpTo(
		GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),
		TargetCapsuleHalfHeight,
		DeltaTime,
		20.f
	) };

	// Elevate/Lower mesh
	// Negative value if Crouching
	// Positive Value when Standing
	const float DeltaCapsuleHalfHeight{ InterpHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() };

	const FVector MeshOffset{ 0.f, 0.f, -DeltaCapsuleHalfHeight };

	GetMesh()->AddLocalOffset(MeshOffset);

	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight, true);
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/** Handle Camera Zoom Interpolation when Aiming */
	InterpCameraZoom(DeltaTime);

	/** Setup Turn Rates */
	SetupTurnRate();

	/** Calculate Crosshair Spread per frame */
	CalculateCrosshairSpread(DeltaTime);

	/** Trace for Items */
	TraceForItems();

	/** Interp the capsule half height based on the crouching/standing */
	InterpCapsuleHalfHeight(DeltaTime);
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent); // Assert: If PlayerInputComponent isn't valid, hold execution!

	/** Binds Movement Input */
	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);

	/** Binds Lookup and Turn Input */
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);

	/**
	*	Binds Mouse Turn and Look Up/Down
	*	Dont need to create our function since we arn't scaling the Turn/Lookup values
	*/
	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::MouseTurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::MouseLookUpAtRate);

	/** Binds Jump Action Input */
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ACharacter::StopJumping);

	/** Binds Weapon Primary Fire Input */
	PlayerInputComponent->BindAction("FireButton", EInputEvent::IE_Pressed, this, &AShooterCharacter::AutoFirePressed);
	PlayerInputComponent->BindAction("FireButton", EInputEvent::IE_Released, this, &AShooterCharacter::AutoFireReleased);

	/** Bind Aim ON/OFF to Input */
	PlayerInputComponent->BindAction("AimWeapon", EInputEvent::IE_Pressed, this, &AShooterCharacter::StartAiming);
	PlayerInputComponent->BindAction("AimWeapon", EInputEvent::IE_Released, this, &AShooterCharacter::StopAiming);

	/** Bind Select E key to Input */
	PlayerInputComponent->BindAction("Select", EInputEvent::IE_Pressed, this, &AShooterCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction("Select", EInputEvent::IE_Released, this, &AShooterCharacter::SelectButtonReleased);

	/** Bind Reload R key to Input */
	PlayerInputComponent->BindAction("ReloadButton", EInputEvent::IE_Pressed, this, &AShooterCharacter::ReloadButtonPressed);

	/** Bind Crouch Ctrl key to Input */
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AShooterCharacter::CrouchButtonPressed);

	/** Bind Inventory Keys */
	PlayerInputComponent->BindAction("FKey", EInputEvent::IE_Pressed, this, &AShooterCharacter::FKeyPressed);
	PlayerInputComponent->BindAction("1Key", EInputEvent::IE_Pressed, this, &AShooterCharacter::OneKeyPressed);
	PlayerInputComponent->BindAction("2Key", EInputEvent::IE_Pressed, this, &AShooterCharacter::TwoKeyPressed);
	PlayerInputComponent->BindAction("3Key", EInputEvent::IE_Pressed, this, &AShooterCharacter::ThreeKeyPressed);
	PlayerInputComponent->BindAction("4Key", EInputEvent::IE_Pressed, this, &AShooterCharacter::FourKeyPressed);
	PlayerInputComponent->BindAction("5Key", EInputEvent::IE_Pressed, this, &AShooterCharacter::FiveKeyPressed);

}

void AShooterCharacter::ResetPickupSoundTimer()
{
	bShouldPlayPickupSound = true;
}

void AShooterCharacter::ResetEquipSoundTimer()
{
	bShouldPlayEquipSound = true;
}

void AShooterCharacter::IncrememtOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

/*
* DEPRECATED
* Replaced by GetInterpLocation in AItem
* 
FVector AShooterCharacter::GetCameraInterpLocation()
{
	const FVector CameraWorldLocation{ FollowCamera->GetComponentLocation() };
	const FVector CameraForwardVector{ FollowCamera->GetForwardVector() };

	return CameraWorldLocation + CameraForwardVector * CameraInterpDistance +
		FVector(0.f, 0.f, CameraInterpElevation);

}
*/

/** This will be called from AItem Class */
void AShooterCharacter::GetPickupItem(AItem* Item)
{
	if (Item)
	{
		Item->PlayEquipSound();
	}

	auto PickedWeapon = Cast<AWeapon>(Item);

	if (PickedWeapon)
	{
		if (Inventory.Num() < INVENTORY_CAPACITY) // Got space in Inventory
		{
			PickedWeapon->SetSlotIndex(Inventory.Num());
			Inventory.Add(PickedWeapon);
			PickedWeapon->SetItemState(EItemState::EIS_PickedUp);
		}
		else // Inventory is full so swapping
		{
			SwapWeapon(PickedWeapon);
		}
	}

	auto Ammo = Cast<AAmmo>(Item);
	
	if (Ammo)
	{
		PickupAmmo(Ammo);
	}
}

FInterpLocation AShooterCharacter::GetInterpLocation(int32 index)
{
	if (index <= InterpLocations.Num())
	{
		return InterpLocations[index];
	}
	return FInterpLocation(); //Return empty interp location
}

