// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter() :
	BaseTurnRate(45.f),
	BaseLookupRate(45.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/** Create a Camera Boom: Pulls in towards the character if there's a collision **/
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f; // Camera follows the pawn at this distance
	CameraBoom->bUsePawnControlRotation = true; // Uses Pawn's orientation 

	/** Create a Follow Camera and setup: Camera Boom controls it **/
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach it to the end of the Spring arm
	FollowCamera->bUsePawnControlRotation = false; // Follow Rotation of the Camera Boom instead
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
}

