// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlPoint.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
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
			if (!bUseConstantBonus)
			{
				StartPersecondBonusTimer();
			}
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
			GetWorldTimerManager().ClearTimer(CooldownTimer);
			CleanUpBonusEffects(Character);
			ShooterCharacter = nullptr;
		}
	}
}

void AControlPoint::ApplyControlPointPerSecondBonus()
{
	switch (ControlPointType)
	{
	case EControlPointType::ECPT_Armor:
		if (ControlPointFaction == EControlPointFaction::ECPF_Friendly)
		{
			if (!ShooterCharacter) return;
			ShooterCharacter->SetArmor(PerSecondBonus);
			PlayApplyBonusEffect();
		}
		break;

	case EControlPointType::ECPT_Damage:
		if (ControlPointFaction == EControlPointFaction::ECPF_Friendly)
		{
			if (!ShooterCharacter) return;
			ShooterCharacter->SetDamageModifier(PerSecondBonus);
			PlayApplyBonusEffect();
		}
		break;

	case EControlPointType::ECPT_Health:
		if (ControlPointFaction == EControlPointFaction::ECPF_Friendly)
		{
			if (!ShooterCharacter) return;
			ShooterCharacter->SetHealth(PerSecondBonus);
			PlayApplyBonusEffect();
		}
		break;
	}

}

// Called every frame
void AControlPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AControlPoint::StartPersecondBonusTimer()
{
	GetWorldTimerManager().ClearTimer(CooldownTimer);
	GetWorldTimerManager().SetTimer
	(
		CooldownTimer,
		this,
		&ThisClass::ApplyControlPointPerSecondBonus,
		CooldownTime,
		true
	);
}

void AControlPoint::PlayApplyBonusEffect()
{
	if (ApplyBonusParticles)
	{
		UGameplayStatics::SpawnEmitterAttached(
			ApplyBonusParticles,
			Cast<USceneComponent>(ShooterCharacter->GetCapsuleComponent()),
			NAME_None,
			((FVector)(ForceInit)),
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTargetIncludingScale
		);
	}
}

void AControlPoint::CleanUpBonusEffects(AShooterCharacter* TargetCharacter)
{
	switch (ControlPointType)
	{
	case EControlPointType::ECPT_Damage:
		if (!TargetCharacter) return;
		TargetCharacter->ResetBaseDamageModifier();
		break;
	}
}

