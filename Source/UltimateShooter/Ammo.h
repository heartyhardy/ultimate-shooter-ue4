// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Ammo.generated.h"

/**
 * 
 */
UCLASS()
class ULTIMATESHOOTER_API AAmmo : public AItem
{
	GENERATED_BODY()

public:

	AAmmo();
	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;

	/** Override so that we can set Ammo properties */
	virtual void SetItemProperties(EItemState State) override;

	/** Ammo Sphere Overlap */
	UFUNCTION()
	void AmmoSphereOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);


private:

	/** Mesh for the Ammo Pickup */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* AmmoMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;

	/** Texture for the Ammo Icon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoIconTexture;

	/** Ammo pickup on overlap sphere */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AmmoCollisionSphere;

public:

	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return AmmoMesh; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }

	virtual void EnableCustomDepth() override;
	virtual void DisableCustomDepth() override;
};
