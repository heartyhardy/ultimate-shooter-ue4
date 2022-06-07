// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"

AWeapon::AWeapon() :
	ThrowWeaponTime(0.7f),
	bFalling(false),
	Ammo(15),
	MagazineCapacity(15),
	WeaponType(EWeaponType::EWT_Pistol),
	AmmoType(EAmmoType::EAT_9mm),
	ReloadMontageSection(FName(TEXT("ReloadSMG"))),
	ClipBoneName(TEXT("smg_clip")),
	SlideDisplacement(0.f),
	SlideDisplacementTime(0.3f),
	bMovingSlide(false),
	MaxSlideDisplacement(6.0f),
	MaxRecoilRotation(20.f),
	bAutomatic(true)
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

	// Update Slide on Pistol
	UpdateSlideDisplacement();

	RotateWhenOnPickup();
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

	EnableGlowMaterial(); // Enable Glow after throwing down
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

void AWeapon::StartSlideTimer()
{
	bMovingSlide = true;
	GetWorldTimerManager().SetTimer(SlideTimer, this, &AWeapon::FinishMovingSlide, SlideDisplacementTime);
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
	// Enable Glow Pulsing after throwing down the weapon
	StartPulseTimer();
}

void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const FString WeaponTablePath{ TEXT("DataTable'/Game/_Game/DataTables/DT_Weapon.DT_Weapon'") };
	UDataTable* WeaponTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *WeaponTablePath));

	const FString RarityBonusTablePath{ TEXT("DataTable'/Game/_Game/DataTables/DT_RarityBonusProps.DT_RarityBonusProps'") };
	UDataTable* RarityBonusTableObject = Cast < UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *RarityBonusTablePath));

	if (WeaponTableObject)
	{
		FWeaponDataTable* WeaponDataRow = nullptr;

		switch (WeaponType)
		{
		case EWeaponType::EWT_SubmachineGun:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("SubmachineGun"), TEXT(""));
			break;

		case EWeaponType::EWT_AssaultRifle:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("AssaultRifle"), TEXT(""));
			break;

		case EWeaponType::EWT_Pistol:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("Pistol"), TEXT(""));
			break;
		}

		if (WeaponDataRow)
		{
			AmmoType = WeaponDataRow->AmmoType;
			Ammo = WeaponDataRow->WeaponAmmo;
			MagazineCapacity = WeaponDataRow->MagazineCapacity;
			SetPickupSound(WeaponDataRow->PickupSound);
			SetEquipSound(WeaponDataRow->EquipSound);
			GetItemMesh()->SetSkeletalMesh(WeaponDataRow->ItemMesh);
			SetItemName(WeaponDataRow->ItemName);
			SetIconItem(WeaponDataRow->InventoryIcon);
			SetAmmoIcon(WeaponDataRow->AmmoIcon);

			SetMaterialInstance(WeaponDataRow->MaterialInstance);
			PreviousMaterialIndex = GetMaterialIndex(); // Store prev Material Index
			GetItemMesh()->SetMaterial(PreviousMaterialIndex, nullptr); // Clear Material
			SetMaterialIndex(WeaponDataRow->MaterialIndex);
			SetClipBoneName(WeaponDataRow->ClipBoneName);
			SetReloadMontageSection(WeaponDataRow->ReloadMontageSection);
			GetItemMesh()->SetAnimInstanceClass(WeaponDataRow->AnimBP);
			CrosshairsMiddle = WeaponDataRow->CrosshairsMiddle;
			CrosshairsTop = WeaponDataRow->CrosshairsTop;
			CrosshairsBottom = WeaponDataRow->CrosshairsBottom;
			CrosshairsLeft = WeaponDataRow->CrosshairsLeft;
			CrosshairsRight = WeaponDataRow->CrosshairsRight;
			AutoFireRate = WeaponDataRow->AutoFireRate;
			MuzzleFlash = WeaponDataRow->MuzzleFlash;
			FireSound = WeaponDataRow->FireSound;
			BoneToHide = WeaponDataRow->BoneToHide;
			bAutomatic = WeaponDataRow->bAutomatic;
			Damage = WeaponDataRow->Damage;
			HeadshotDamage = WeaponDataRow->HeadshotDamage;
			NoiseRange = WeaponDataRow->NoiseRange;
		}

		if (GetMaterialInstance())
		{
			SetDynamicMaterialInstance(UMaterialInstanceDynamic::Create(GetMaterialInstance(), this));
			GetDynamicMaterialInstance()->SetVectorParameterValue(TEXT("Fresnel Color"), GetGlowColor());
			GetItemMesh()->SetMaterial(GetMaterialIndex(), GetDynamicMaterialInstance());
			EnableGlowMaterial();
		}
	}

	if (RarityBonusTableObject)
	{
		FRarityBasedPropsTable* RarityBonusPropsRow = nullptr;

		switch (GetItemRarity())
		{
			case EItemRarity::EWR_Damaged:
				RarityBonusPropsRow = RarityBonusTableObject->FindRow<FRarityBasedPropsTable>(FName("Damaged"), TEXT(""));
			break;

			case EItemRarity::EWR_Common:
				RarityBonusPropsRow = RarityBonusTableObject->FindRow<FRarityBasedPropsTable>(FName("Common"), TEXT(""));
				break;

			case EItemRarity::EWR_Uncommon:
				RarityBonusPropsRow = RarityBonusTableObject->FindRow<FRarityBasedPropsTable>(FName("Uncommon"), TEXT(""));
				break;

			case EItemRarity::EWR_Rare:
				RarityBonusPropsRow = RarityBonusTableObject->FindRow<FRarityBasedPropsTable>(FName("Rare"), TEXT(""));
				break;
				
			case EItemRarity::EWR_Legendary:
				RarityBonusPropsRow = RarityBonusTableObject->FindRow<FRarityBasedPropsTable>(FName("Legendary"), TEXT(""));
				break;
		}

		if (RarityBonusPropsRow)
		{
			RarityBonusDamage = RarityBonusPropsRow->BonusDamage;
			RarityBonusHeadshotDamage = RarityBonusPropsRow->BonusHeadshotDamage;
			RarityCriticalChance = RarityBonusPropsRow->CriticalChance;
			RarityCriticalMultiplier = RarityBonusPropsRow->CriticalDamageMultiplier;
			RarityBulletTimeModifier = RarityBonusPropsRow->BulletTimeModifier;
			RarityBulletTimeDilation = RarityBonusPropsRow->BulletTimeDilation;
			RarityBulletTimeResetMoveSpeed = RarityBonusPropsRow->BulletTimeResetMoveSpeed;
		}
	}

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (BoneToHide != FName(""))
	{
		GetItemMesh()->HideBoneByName(BoneToHide, EPhysBodyOp::PBO_None);
	}
}

void AWeapon::FinishMovingSlide()
{
	bMovingSlide = false;
}

void AWeapon::UpdateSlideDisplacement()
{
	if (SlideDisplacementCurve && bMovingSlide)
	{
		const float ElapsedTime{ GetWorldTimerManager().GetTimerElapsed(SlideTimer) };
		const float CurveValue{ SlideDisplacementCurve->GetFloatValue(ElapsedTime) };
		SlideDisplacement = CurveValue * MaxSlideDisplacement;
		RecoilRotation = CurveValue * MaxRecoilRotation;
	}
}

void AWeapon::RotateWhenOnPickup()
{
	if (GetItemState() == EItemState::EIS_Pickup)
	{
		const FRotator CurrentRotation{ GetActorRotation() };
		const float NewYaw = CurrentRotation.Yaw + 0.75f;
		const FRotator NewRotation{ 0.f, NewYaw , 0.f };

		SetActorRotation(NewRotation, ETeleportType::None);
	}
}

bool AWeapon::CanCriticalHit()
{
	float ChanceRange = FMath::RandRange(0.f, 100.f);
	bool CanCrit = (ChanceRange + RarityCriticalChance > 100.f) ? true : false;

	return CanCrit;
}

float AWeapon::GetCriticalHit(bool bCanCriticalHit, float NoCritDamage)
{
	if (bCanCriticalHit)
	{
		float CritMultiplier = FMath::RandRange(1.f, RarityCriticalMultiplier * 1.0f);
		float CritDamage = NoCritDamage * FMath::RandRange(1.f, RarityCriticalMultiplier * 1.0f);
		
		return CritDamage;
	}
	else return NoCritDamage;
}
