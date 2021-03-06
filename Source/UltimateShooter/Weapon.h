// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Engine/DataTable.h"
#include "WeaponType.h"
#include "Weapon.generated.h"

/**
 * 
 */


USTRUCT(BlueprintType)
struct FWeaponDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MagazineCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundCue* PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMesh* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* InventoryIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* AmmoIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaterialIndex;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ClipBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ReloadMontageSection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimBP;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsMiddle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsBottom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AutoFireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneToHide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutomatic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeadshotDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NoiseRange;
};

USTRUCT(BlueprintType)
struct FRarityBasedPropsTable : public FTableRowBase
{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BonusDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BonusHeadshotDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "30"))
	int32 CriticalChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "5"))
	int32 CriticalDamageMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "5"))
	float BulletTimeModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.3", ClampMax = ".7"))
	float BulletTimeDilation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "125"))
	float BulletTimeResetMoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "10"))
	int32 MaxChainedExecutions;
};

UCLASS()
class ULTIMATESHOOTER_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();

	virtual void Tick(float DeltaTime) override;

protected:
	void StopFalling();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	void FinishMovingSlide();

	void UpdateSlideDisplacement();

	void RotateWhenOnPickup();

private:
	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bFalling;

	/** Ammo count for this weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		int32 Ammo;

	/** Max Magazine capacity for this weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		int32 MagazineCapacity;

	/** Type of the Weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		EWeaponType WeaponType;

	/** Type of Ammo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		EAmmoType AmmoType;

	/** Montage Section Name of the Weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		FName ReloadMontageSection;

	/** This will be set to true when we are moving the clip to reload (See ReloadSMG Montage) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		bool bMovingClip;

	/** Name of the bone that the Mag clip is attached to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		FName ClipBoneName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
		UDataTable* WeaponDataTable;

	int32 PreviousMaterialIndex;

	/** Textures for the weapon Crosshairs */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
		UTexture2D* CrosshairsMiddle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
		UTexture2D* CrosshairsLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
		UTexture2D* CrosshairsRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
		UTexture2D* CrosshairsBottom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
		UTexture2D* CrosshairsTop;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
		float AutoFireRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* MuzzleFlash;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
		USoundCue* FireSound;

	/** Name of the Bone to Hide in Weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
		FName BoneToHide;

	/** Amount of Pushback value when firing */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
		float SlideDisplacement;

	/** Curve for Slide Displacement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
		UCurveFloat* SlideDisplacementCurve;

	/** Timer Handle For Updating Slide Displacement */
	FTimerHandle SlideTimer;

	/** Time for displacing the slide during pistol fire */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
		float SlideDisplacementTime;

	/** True when moving the Pistol Slide */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
		bool bMovingSlide;

	/** Maximum Slide Displacement length */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
		float MaxSlideDisplacement;

	/** Max Rotation for Pistol Recoil */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
		float MaxRecoilRotation;

	/** Amount of Pistol Roll rotation when firing */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
		float RecoilRotation;

	/** True for Auto gun fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		bool bAutomatic;

	/** Weapon Default Damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		float Damage;

	/** Weapon Damage on Headshot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		float HeadshotDamage;

	/** Weapon Default Damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		float RarityBonusDamage;

	/** Weapon Damage on Headshot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		float RarityBonusHeadshotDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "30"))
		int32 RarityCriticalChance;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true", ClampMin = "1", ClampMax = "5"))
		int32 RarityCriticalMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true", ClampMin = "1", ClampMax = "5"))
		float RarityBulletTimeModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true", ClampMin = "1", ClampMax = "5"))
		float RarityBulletTimeDilation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "125"))
		float RarityBulletTimeResetMoveSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "10"))
	int32 RarityMaxChainedExecutions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		float NoiseRange;

public:
	// Add impulse to the weapon
	void ThrowWeapon();

	// Get Ammo Count
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	// Get Max Magazine Capacity
	FORCEINLINE int32 GetMagazineCapacity() const { return MagazineCapacity; }

	// Called from Character Class when Firing the weapon
	void DecrementAmmo();

	// Get Weapon Type, Ammo Type and Montage section
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	void SetWeaponType(EWeaponType NewWeaponType) { WeaponType = NewWeaponType; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; }
	FORCEINLINE void SetReloadMontageSection(FName Name)  { ReloadMontageSection = Name; }

	void ReloadAmmo(int32 Amount);

	FORCEINLINE FName GetClipBoneName() const { return ClipBoneName; }
	FORCEINLINE void SetClipBoneName(FName Name) { ClipBoneName = Name; }
	
	FORCEINLINE void SetMovingClip(bool bMoving) { bMovingClip = bMoving; }

	FORCEINLINE float GetAutoFireRate() const { return AutoFireRate; }
	FORCEINLINE UParticleSystem* GetMuzzleFlash() const { return MuzzleFlash; }
	FORCEINLINE USoundCue* GetFireSound() const { return FireSound; }

	FORCEINLINE bool GetAutomatic() const { return bAutomatic; }

	FORCEINLINE float GetDamage() const { return Damage; }
	FORCEINLINE float GetHeadshotDamage() const { return HeadshotDamage; }
	FORCEINLINE float GetRarityBonusDamage() const { return RarityBonusDamage; }
	FORCEINLINE float GetRarityBonusHeadshotDamage() const { return RarityBonusHeadshotDamage; }

	FORCEINLINE float GetNoiseRange() const { return NoiseRange; }

	void StartSlideTimer();

	bool ClipIsFull();

	bool CanCriticalHit();
	float GetCriticalHit(bool bCanCriticalHit, float NoCritDamage);

	FORCEINLINE float GetRarityBulletTimeModifier() const { return RarityBulletTimeModifier; }
	FORCEINLINE float GetRarityBulletTimeDilation() const { return RarityBulletTimeDilation; }
	FORCEINLINE float GetRarityBulletTimeResetMoveSpeed() const { return RarityBulletTimeResetMoveSpeed; }
	
	FORCEINLINE int32 GetRarityMaxChainedExecutions() const { return RarityMaxChainedExecutions; }
};
