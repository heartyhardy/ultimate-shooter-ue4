// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "ShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Curves/CurveVector.h"

// Sets default values
AItem::AItem() :
	ItemName(FString("Default")),
	ItemCount(0),
	ItemState(EItemState::EIS_Pickup),
	ItemRarity(EItemRarity::EWR_Damaged),
	// Item interping
	ZCurveTime(.7f),
	ItemInterpStartLocation(FVector(0.f)),
	CameraTargetLocation(FVector(0.f)),
	bIsInterping(false),
	ItemInterpX(0.f),
	ItemInterpY(0.f),
	InterpInitialYawOffset(0.f),
	ItemType(EItemType::EIT_MAX),
	InterpLocIndex(0),
	MaterialIndex(0),
	bCanChangeCustomDepth(true),
	//Dynamic material params
	GlowAmount(30.f),
	FresnelExponent(3.f),
	FresnelReflectFraction(4.f),
	PulseCurveTime(5.f),
	//Inventory
	SlotIndex(0),
	bCharacterInventoryFull(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(ItemMesh);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(GetRootComponent());

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	// Hide Widget at start
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}

	// Set Active Stars based on Item Rarity
	SetActiveStars();

	// Setup AreaSphere Overlap Callbacks
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	// Set Default Item State
	SetItemProperties(ItemState);

	// Initialize Custom Depth to false
	InitializeCustomDepth();

	// Start the Curve Pulse Timer for Dynamic materials If in PICKUP state
	StartPulseTimer();
}

/** Callback function for AreaSphere BeginComponentOverlap */
void AItem::OnSphereOverlap(
	UPrimitiveComponent* OverlappedComp, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AShooterCharacter* Shooter = Cast<AShooterCharacter>(OtherActor);
		if (Shooter)
		{
			Shooter->IncrememtOverlappedItemCount(1);
		}
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	if (OtherActor)
	{
		AShooterCharacter* Shooter = Cast<AShooterCharacter>(OtherActor);
		if (Shooter)
		{
			Shooter->IncrememtOverlappedItemCount(-1);
			Shooter->UnHighlightInventorySlot();
		}
	}
}

void AItem::SetActiveStars()
{
	// Element 0 is not used because its easy to correspond to stars (1-5)
	for (int32 i = 0; i <= 5; i++)
	{
		// Initilize all elements to false
		ActiveStars.Add(false);
	}

	switch (ItemRarity)
	{

	case EItemRarity::EWR_Damaged:
		ActiveStars[1] = true;
		break;
	case EItemRarity::EWR_Common:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		break;
	case EItemRarity::EWR_Uncommon:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		break;
	case EItemRarity::EWR_Rare:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		break;
	case EItemRarity::EWR_Legendary:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		ActiveStars[5] = true;
		break;
	default:
		break;
	}
}

void AItem::SetItemProperties(EItemState State)
{
	switch (State)
	{
	case EItemState::EIS_Pickup:
		// Set Mesh Properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Set Area Sphere Properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		// Set CollisionBox Properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_Visibility, 
			ECollisionResponse::ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // Need this for physics and collisions to work
		break;

	case EItemState::EIS_Equipped:
		// Hide PickupWidget
		PickupWidget->SetVisibility(false);
		// Set Mesh Properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Set Area Sphere Properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Set Collision Box Properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EIS_Falling:
		// Set Mesh Properties
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_WorldStatic, 
			ECollisionResponse::ECR_Block);// Walls and Floors are ususally set to World Static channel

		// Set Area Sphere Properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Set Collision Box Properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EIS_EquipInterping:
		
		// Hide PickupWidget
		PickupWidget->SetVisibility(false);

		// Set Mesh Properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Set Area Sphere Properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Set Collision Box Properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Set the Yaw to 0
		SetActorRotation(FRotator(0.f, 0.f, 0.f), ETeleportType::None);
		break;

	case EItemState::EIS_PickedUp:

		// Hide PickupWidget
		PickupWidget->SetVisibility(false);

		// Set Mesh Properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Set Area Sphere Properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Set Collision Box Properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

void AItem::FinishItemInterping()
{
	bIsInterping = false;

	if (ShooterCharacter)
	{
		ShooterCharacter->IncrementInterpLocItemCount(InterpLocIndex, -1); // Substract from the interplocaions for this index
		ShooterCharacter->GetPickupItem(this);

		ShooterCharacter->UnHighlightInventorySlot();
	}
	// Set Item Scale to Normal After Picking up
	SetActorScale3D(FVector(1.f, 1.f, 1.f));

	// After Equipping, Glow and Custom depth disabled
	DisableGlowMaterial();
	bCanChangeCustomDepth = true; // This needs to be done before DisableCustomDepth();
	DisableCustomDepth();

}

void AItem::ItemInterp(float DeltaTime)
{
	if (!bIsInterping) return;

	if (ShooterCharacter && ItemZCurve)
	{
		// Get currently elapsed time since starting the timer
		const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
		const float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);

		// Get the item's initial location when curve started
		 FVector ItemLocation = ItemInterpStartLocation;
		// Get the location in front of the camera
		 const FVector CameraInterpLocation{ GetInterpLocation() };

		// Get the Z difference between Item's location and Camera's location in front of it. X,Y Zeroed out!
		 const FVector ItemToCamera{ FVector(0.f, 0.f, (CameraInterpLocation - ItemLocation).Z) };

		 // Scale Factor to Multiply the Curve value!
		 const float DeltaZ = ItemToCamera.Size();

		 FVector CurrentLocation{ GetActorLocation() };
		 // Interped X Value
		 const float InterpXValue = FMath::FInterpTo(CurrentLocation.X, CameraInterpLocation.X, DeltaTime, 30.f);
		 // Interped Y Value
		 const float InterpYValue = FMath::FInterpTo(CurrentLocation.Y, CameraInterpLocation.Y, DeltaTime, 30.f);
		 // Set ItemLocation X and Y to Interped values
		 ItemLocation.X = InterpXValue;
		 ItemLocation.Y = InterpYValue;

		 // Adding Curve Value to the Initial Item location's Z and (Scaling with DeltaZ)
		 ItemLocation.Z += CurveValue * DeltaZ;

		 // Set the Items Z location according to the curve value
		 SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);

		 // Camera Rotation for this Frame
		 const FRotator CameraRotation{ ShooterCharacter->GetFollowCamera()->GetComponentRotation() };
		 // Rotation Yaw Offset + Camera Yaw Offset
		 FRotator ItemRotation{ 0.f, CameraRotation.Yaw + InterpInitialYawOffset, 0.f };
		 // Set Rotation for the Item
		 SetActorRotation(ItemRotation, ETeleportType::TeleportPhysics);

		 // Item Scale interp according to the ItemScaleCurve asset
		 if (ItemScaleCurve)
		 {
			const float ScaleCurveValue = ItemScaleCurve->GetFloatValue(ElapsedTime);
			SetActorScale3D(FVector(ScaleCurveValue, ScaleCurveValue, ScaleCurveValue));
		 }
	}
}

FVector AItem::GetInterpLocation()
{
	if (!ShooterCharacter) return FVector(0.f, 0.f, 0.f);

	switch (ItemType)
	{
	case EItemType::EIT_Ammo:
		return ShooterCharacter->GetInterpLocation(InterpLocIndex).SceneComponent->GetComponentLocation();
		break;
	case EItemType::EIT_Weapon:
		return ShooterCharacter->GetInterpLocation(0).SceneComponent->GetComponentLocation(); // Weapon is always at 0 index
		break;
	}

	return FVector();
}

void AItem::PlayPickupSound(bool bForcePlaySound)
{
	if (ShooterCharacter)
	{
		if (bForcePlaySound)
		{
			if (PickupSound)
			{
				UGameplayStatics::PlaySound2D(this, PickupSound);
			}
		}
		else if (ShooterCharacter->ShouldPlayPickupSound())
		{
			ShooterCharacter->StartPickupSoundTimer();
			if (PickupSound)
			{
				UGameplayStatics::PlaySound2D(this, PickupSound);
			}
		}
	}
}

void AItem::EnableCustomDepth()
{
	if (bCanChangeCustomDepth)
	{
		ItemMesh->SetRenderCustomDepth(true);
	}
}

void AItem::DisableCustomDepth()
{
	if (bCanChangeCustomDepth)
	{
		ItemMesh->SetRenderCustomDepth(false);
	}
}

void AItem::InitializeCustomDepth()
{
	DisableCustomDepth();
}

// Called when Item is changed or moved in the world
void AItem::OnConstruction(const FTransform& Transform)
{
	// Load the data in the Item Rarity Data Table
	
	// Path to the Item Rarity Data Table
	FString RarityTablePath(TEXT("DataTable'/Game/_Game/DataTables/DT_ItemRarity.DT_ItemRarity'"));
	UDataTable* RarityTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *RarityTablePath)); // * needed in front of RarityTable Path since its C Style String

	if (RarityTableObject)
	{
		FItemRarityTable* RarityRow = nullptr;
		switch (ItemRarity)
		{
		case EItemRarity::EWR_Damaged:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Damaged"), TEXT(""));
			break;

		case EItemRarity::EWR_Common:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Common"), TEXT(""));
			break;

		case EItemRarity::EWR_Uncommon:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Uncommon"), TEXT(""));
			break;

		case EItemRarity::EWR_Rare:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Rare"), TEXT(""));
			break;

		case EItemRarity::EWR_Legendary:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Legendary"), TEXT(""));
			break;
		}

		if (RarityRow)
		{
			GlowColor = RarityRow->GlowColor;
			LightColor = RarityRow->LightColor;
			DarkColor = RarityRow->DarkColor;
			NumberOfStars = RarityRow->NumberOfStars;
			IconBackground = RarityRow->IconBackground;

			if (GetItemMesh())
			{
				GetItemMesh()->SetCustomDepthStencilValue(RarityRow->CustomDepthStencil);
			}
		}
	}

	if (MaterialInstance)
	{
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialInstance, this);
		DynamicMaterialInstance->SetVectorParameterValue(TEXT("Fresnel Color"), GlowColor);
		ItemMesh->SetMaterial(MaterialIndex, DynamicMaterialInstance);
		EnableGlowMaterial();
	}

}

void AItem::EnableGlowMaterial()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("Glow Blend Alpha"), 0); // Check Graph: Setting 0 picks Option A (Glow)
	}
}

void AItem::UpdatePulse()
{
	float ElapsedTime{};
	FVector CurveValue{};

	switch (ItemState)
	{
	case EItemState::EIS_Pickup:
		if (PulseCurve)
		{
			ElapsedTime = GetWorldTimerManager().GetTimerElapsed(PulseTimer);
			CurveValue = PulseCurve->GetVectorValue(ElapsedTime);
		}
		break;

	case EItemState::EIS_EquipInterping:
		if (InterpPulseCurve)
		{
			ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
			CurveValue = InterpPulseCurve->GetVectorValue(ElapsedTime);
		}
		break;
	}

	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("Glow Amount"), CurveValue.X * GlowAmount); //GlowAmount var is used to scale CurveValue
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("Fresnel Exponent"), CurveValue.Y * FresnelExponent);
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("Fresnel Reflect Fraction"), CurveValue.Z * FresnelReflectFraction);
	}
}

EItemRarity AItem::GetItemRarity()
{
	return ItemRarity;
}

void AItem::DisableGlowMaterial()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("Glow Blend Alpha"), 1); // Check Graph: Setting 1 picks Option B
	}
}

void AItem::PlayEquipSound(bool bForcePlaySound)
{
	if (ShooterCharacter)
	{
		if (bForcePlaySound)
		{
			if (EquipSound)
			{
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}
		else if (ShooterCharacter->ShouldPlayEquipSound())
		{
			ShooterCharacter->StartEquipSoundTimer();
			if (EquipSound)
			{
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}
	}
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle Item Interping when in EquipInterping State
	ItemInterp(DeltaTime);

	// Get Values from pulse curve and set Dynamic material properties for Glow
	UpdatePulse();
}

void AItem::ResetPulseTimer()
{
	StartPulseTimer();
}

void AItem::StartPulseTimer()
{
	/** Note that this function is calling iteself
	* If in PICKUP State, it will keep running the timer
	*/
	if (ItemState == EItemState::EIS_Pickup)
	{
		GetWorldTimerManager().SetTimer(PulseTimer, this, &AItem::ResetPulseTimer, PulseCurveTime);
	}
}

void AItem::SetItemState(EItemState State)
{
	// Update Current State
	ItemState = State;
	// Update Item properties depending on Current State
	SetItemProperties(State);
}

void AItem::StartItemCurve(AShooterCharacter* Char, bool bForcePlaySound)
{
	// Store a handle to the ShooterCharacter;
	ShooterCharacter = Char;

	// Get item array index in interplocations with min item count
	InterpLocIndex = ShooterCharacter->GetInterpLocationIndex();
	ShooterCharacter->IncrementInterpLocItemCount(InterpLocIndex, 1); // Add 1 to this interplocaion index

	// Play Pickup Sound
	PlayPickupSound(bForcePlaySound);

	// Store initial location of the item
	ItemInterpStartLocation = GetActorLocation();
	bIsInterping = true;

	SetItemState(EItemState::EIS_EquipInterping); // Note: Dont forget to update collision properties
	GetWorldTimerManager().ClearTimer(PulseTimer); // Clear Pulse Timer as soon as Interp Starts

	GetWorldTimerManager().SetTimer(
		ItemInterpTimer,
		this,
		&AItem::FinishItemInterping,
		ZCurveTime
	);

	// Initial Yaw of the Camera
	const float CameraRotationYaw{ ShooterCharacter->GetFollowCamera()->GetComponentRotation().Yaw };
	// Initial Yaw of the Item
	const float ItemRotationYaw{ GetActorRotation().Yaw };

	// Initial Yaw Offset between Camera and the Item
	InterpInitialYawOffset = ItemRotationYaw - CameraRotationYaw;

	bCanChangeCustomDepth = false; // When Interping hide outline and custom depth effects on material
}

