// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Item.h"


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
	MouseAimTurnRate(0.2f),
	MouseAimLookUpRate(0.2f),
	bAiming(false),
	// FOV values when Aiming ON/OFF
	DefaultCameraFOV(0.f), // Will be changed in constructor
	ZoomedCameraFOV(35.f),
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
	AutomaticFireRate(0.1f),
	// Item Tracing
	bShouldTraceForItems(false)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/** Create a Camera Boom: Pulls in towards the character if there's a collision **/
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 200.f; // Camera follows the pawn at this distance
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f); // Offset the camera to right and up in Z
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
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultCameraFOV = FollowCamera->FieldOfView;
	CurrentCameraFOV = DefaultCameraFOV;
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
	/** Play the sound cue */
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");

	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
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

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && FireFromHipMontage)
	{
		AnimInstance->Montage_Play(FireFromHipMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFireFromHip"));
	}

	/** Start Timer for crosshair spread factor when firing */
	StartCrosshairFireTimer();
}

/** Set Aiming ON/OFF */
void AShooterCharacter::StartAiming()
{
	bAiming = true;
}

void AShooterCharacter::StopAiming()
{
	bAiming = false;
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
	StartAutoFire();
}

void AShooterCharacter::AutoFireReleased()
{
	bAutoFireButtonPressed = false;
}

void AShooterCharacter::StartAutoFire()
{
	if (bShouldAutoFire)
	{
		FireWeapon();
		bShouldAutoFire = false;
		GetWorldTimerManager().SetTimer(
			AutoFireTimerHandle,
			this,
			&AShooterCharacter::ResetAutoFire,
			AutomaticFireRate
		);
	}
}

void AShooterCharacter::ResetAutoFire()
{
	bShouldAutoFire = true;
	/** If Auto Fire button is still pressed, restart the firing sequence */
	if (bAutoFireButtonPressed)
	{
		StartAutoFire();
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

			AItem* HitItem = Cast<AItem>(CrosshairTraceHit.Actor);
			if (HitItem && HitItem->GetPickupWidget())
			{
				HitItem->GetPickupWidget()->SetVisibility(true);
			}

			// If we hit an Item last frame
			if (TraceHitItemLastFrame)
			{
				if (HitItem != TraceHitItemLastFrame)
				{
					// We are hitting a different AItem this frame from last
					// Or AItem is null
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
				}
			}

			// Store reference to HitItem next frame
			TraceHitItemLastFrame = HitItem;
		}
	}
	else if (TraceHitItemLastFrame)
	{
		// No longer overlapping any items
		// Hide the last traced item if its not null
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
	}
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
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ACharacter::StopJumping);

	/** Binds Weapon Primary Fire Input */
	PlayerInputComponent->BindAction("FireButton", EInputEvent::IE_Pressed, this, &AShooterCharacter::AutoFirePressed);
	PlayerInputComponent->BindAction("FireButton", EInputEvent::IE_Released, this, &AShooterCharacter::AutoFireReleased);

	/** Bind Aim ON/OFF to Input */
	PlayerInputComponent->BindAction("AimWeapon", EInputEvent::IE_Pressed, this, &AShooterCharacter::StartAiming);
	PlayerInputComponent->BindAction("AimWeapon", EInputEvent::IE_Released, this, &AShooterCharacter::StopAiming);
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

