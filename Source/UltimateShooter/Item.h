// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UENUM()
enum class EItemRarity : uint8
{
	EWR_Damaged UMETA(DisplayName = "Damaged"),
	EWR_Common UMETA(DisplayName = "Common"),
	EWR_Uncommon UMETA(DisplayNAme = "Uncommon"),
	EWR_Rare UMETA(DisplayName = "Rare"),
	EWR_Legendary UMETA(DisplayName = "Legendary"),

	EWR_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM()
enum class EItemState : uint8
{
	EIS_Pickup UMETA(DisplayName = "Pickup"),
	EIS_EquipInterping UMETA(DisplayName = "EquipInterping"),
	EIS_PickedUp UMETA(DisplayName = "PickedUp"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Falling UMETA(DisplayName = "Falling"),

	EIS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM()
enum class EItemType: uint8
{
	EIT_Ammo UMETA(DisplayName = "Ammo"),
	EIT_Weapon UMETA(DisplayName = "Weapon"),


	EIT_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class ULTIMATESHOOTER_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Callback for AreaSphere BeginComponentOverlap
	UFUNCTION()
	void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	// Callback for AreaSpehre EndComponentOverlap
	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int OtherBodyIndex
	);

	/** Sets stars based on Item Rarity */
	void SetActiveStars();

	/** Sets item properties based on item state */
	virtual void SetItemProperties(EItemState State);

	/** Called when Item Interping is finished */
	void FinishItemInterping();

	/** Perform item interp curve: Handles item interping */
	void ItemInterp(float DeltaTime);

	/** Get interplocation based on Type */
	FVector GetInterpLocation();

	void PlayPickupSound();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called in ShooterCharacter->GetPickupItem
	void PlayEquipSound();

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Display", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Display", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AreaSphere;

	/** Name displayed in the Item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FString ItemName;

	/** Ammo Count */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 ItemCount;

	/** No of stars displayed in the item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemRarity ItemRarity;

	/** No of Active Stars in item. Visible anywhere because only C++ can change this */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	TArray<bool> ActiveStars;

	/** State of the item */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemState ItemState;

	/** The curve asset to use for Item's Z location when interping */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* ItemZCurve;

	/** Starting Location when the interping begins */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FVector ItemInterpStartLocation;

	/** Target interp location in front of the camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FVector CameraTargetLocation;

	/** True when item interping */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	bool bIsInterping;

	/** Plays when starting Item interping */
	FTimerHandle ItemInterpTimer;

	/** Pointer to the character to access it from this class */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* ShooterCharacter;

	/** This time is same as the specified curve duration (0.7 -> see the curve asset) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float ZCurveTime;

	/** X and Y for the item while interping in the EquippedInterp state */
	float ItemInterpX;
	float ItemInterpY;

	/** Initial Yaw Offset between Item and Camera */
	float InterpInitialYawOffset;

	/** Curve for Item Scale Interp */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* ItemScaleCurve;

	/** Sound Cue to play when picking up an item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class USoundCue* PickupSound;

	/** Sound Cue to play when an item is equipped */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class USoundCue* EquipSound;

	/** Enum for the type of item this item is */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemType ItemType;

	/** Index of the interp location this item interping to */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 InterpLocIndex;

public:
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; };
	FORCEINLINE USoundCue* GetPickupSound() const { return PickupSound; }
	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }
	FORCEINLINE int32 GetItemCount() const { return ItemCount; }

	void SetItemState(EItemState State);

	/** Called From AShooterCharacter class */
	void StartItemCurve(AShooterCharacter* Char);
};
