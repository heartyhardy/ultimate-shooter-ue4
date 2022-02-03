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
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"


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
	CameraInterpSpeed(20.f)
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
	GetCharacterMovement()->JumpZVelocity = 600.f;
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
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport) // GEngine is the Global Game Engine pointer
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize); // Get Game Viewport Size
	}

	/** Get Screen Location of the Crosshair */
	FVector2D CrosshairLocation{ ViewportSize.X / 2.f, ViewportSize.Y / 2.f };
	CrosshairLocation.Y -= 50.f;

	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	/** Get World Location and Direction of the Crosshair */
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorld) // Was de-projection successful?
	{
		// Prepare a hit result
		FHitResult ScreenTraceHit;
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ CrosshairWorldPosition + CrosshairWorldDirection * 50'000.f };

		BeamEndLocation = End; // If hit then we can use the Hit location. Otherwise will travel max amount specified

		GetWorld()->LineTraceSingleByChannel(
			ScreenTraceHit,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);

		if (ScreenTraceHit.bBlockingHit)
		{
			BeamEndLocation = ScreenTraceHit.Location;

			FHitResult WeaponTraceHit;
			FVector WeaponTraceStart{ MuzzleSocketLocation };
			FVector WeaponTraceEnd{ BeamEndLocation };

			GetWorld()->LineTraceSingleByChannel(
				WeaponTraceHit,
				WeaponTraceStart,
				WeaponTraceEnd,
				ECollisionChannel::ECC_Visibility
			);

			if (WeaponTraceHit.bBlockingHit) // If there was an object between Gun Barrel and the BeamEndPoint?
			{
				BeamEndLocation = WeaponTraceHit.Location;
			}

			return true;
		}
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

	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor;
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
	PlayerInputComponent->BindAction("FireButton", EInputEvent::IE_Pressed, this, &AShooterCharacter::FireWeapon);

	/** Bind Aim ON/OFF to Input */
	PlayerInputComponent->BindAction("AimWeapon", EInputEvent::IE_Pressed, this, &AShooterCharacter::StartAiming);
	PlayerInputComponent->BindAction("AimWeapon", EInputEvent::IE_Released, this, &AShooterCharacter::StopAiming);
}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

