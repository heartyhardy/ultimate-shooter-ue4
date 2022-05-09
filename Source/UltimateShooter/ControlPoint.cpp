// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlPoint.h"
#include "Components/SphereComponent.h"
#include "ShooterCharacter.h"

// Sets default values
AControlPoint::AControlPoint() :
	ControlPointType(EControlPointType::ECPT_Armor),
	ControlPointFaction(EControlPointFaction::ECPF_Friendly),
	PerSecondBonus(1.0f),
	CooldownTime(1.0f),
	bUseConstantBonus(false)

{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StructureMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StructureMesh"));
	SetRootComponent(StructureMesh);

	RangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RangeSpehre"));
	RangeSphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AControlPoint::BeginPlay()
{
	Super::BeginPlay();
	
	RangeSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnRangeSphereOverlap);
	RangeSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnRangeSphereEndOverlap);
}

void AControlPoint::OnRangeSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (OtherActor)
	{
		auto Character = Cast<AShooterCharacter>(OtherActor);
		if (Character)
		{
			ShooterCharacter = Character;
		}
		else
		{
			return;
		}

		if (ShooterCharacter)
		{
			UE_LOG(LogTemp, Warning, TEXT("ENTERING CONTROL POINT RANGE..."));

			GetWorldTimerManager().ClearTimer(CooldownTimer);
			GetWorldTimerManager().SetTimer
			(
				CooldownTimer,
				this,
				&ThisClass::ApplyControlPointBonus,
				CooldownTime,
				true
			);
		}
	}
}

void AControlPoint::OnRangeSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	if (OtherActor)
	{
		auto Character = Cast<AShooterCharacter>(OtherActor);
		if (Character)
		{
			UE_LOG(LogTemp, Warning, TEXT("EXITING CONTROL POINT RANGE..."));

			GetWorldTimerManager().ClearTimer(CooldownTimer);
			ShooterCharacter = nullptr;
		}
	}
}

void AControlPoint::ApplyControlPointBonus()
{
	if (!ShooterCharacter) return;
	UE_LOG(LogTemp, Warning, TEXT("GRANTING BONUS ARMOR..."));

	ShooterCharacter->SetArmor(PerSecondBonus);
}

// Called every frame
void AControlPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

