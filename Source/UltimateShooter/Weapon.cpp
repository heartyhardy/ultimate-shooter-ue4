// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

AWeapon::AWeapon() :
	ThrowWeaponTime(0.7f),
	bFalling(false),
	Ammo(30),
	MagazineCapacity(30),
	WeaponType(EWeaponType::EWT_SubmachineGun),
	AmmoType(EAmmoType::EAT_9mm),
	ReloadMontageSection(FName(TEXT("ReloadSMG"))),
	ClipBoneName(TEXT("smg_clip"))
{
	// This is a must for tick to work!
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Keep the weapon Upright while falling
	if (GetItemState() == EItemState::EIS_Falling && bFalling)
	{
		const FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AWeapon::ThrowWeapon()
{
	FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
	// Set World Rotation of the weapon to Yaw only
	GetItemMesh()->SetWorldRotation(
		MeshRotation,
		false,
		nullptr,
		ETeleportType::TeleportPhysics
	);

	const FVector MeshForward{ GetItemMesh()->GetForwardVector() };
	const FVector MeshRight{ GetItemMesh()->GetRightVector() };

	// Direction the weapon will be thrown
	FVector ImpulseVector = MeshRight.RotateAngleAxis(-20.f, MeshForward);

	float RandomRotation{ 30.f };
	// Further Apply a random rotation around Z
	ImpulseVector = ImpulseVector.RotateAngleAxis(RandomRotation, FVector{ 0.f, 0.f, 1.f }); // Z axis
	ImpulseVector *= 20'000.f;

	// Apply Impulse
	GetItemMesh()->AddImpulse(ImpulseVector);

	// Item is now falling
	bFalling = true;
	// Start the timer
	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);
}

void AWeapon::DecrementAmmo()
{
	if (Ammo - 1 <= 0)
	{
		Ammo = 0;
	}
	else
	{
		--Ammo;
	}
}

void AWeapon::ReloadAmmo(int32 Amount)
{
	checkf(Ammo + Amount <= MagazineCapacity, TEXT("Attempted to overreload the Mag Capacity!!"));
	Ammo += Amount;
}

bool AWeapon::ClipIsFull()
{
	return Ammo >= MagazineCapacity;
}

void AWeapon::StopFalling()
{
	bFalling = false;
	// Set the weapon state to Pickup (Ready to pickup)
	SetItemState(EItemState::EIS_Pickup);
	UE_LOG(LogTemp, Warning, TEXT("Item State: %d") , GetItemState());
}
