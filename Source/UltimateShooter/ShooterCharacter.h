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

	/** Called when Fire Button is pressed */
	void FireWeapon();

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

public:

	/** Returns Camera boom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns Camera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
