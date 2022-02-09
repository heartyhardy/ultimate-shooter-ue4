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

	EWR_MAX UMETA(DisplayName = "DefaultMax")
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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

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

public:
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }

};
