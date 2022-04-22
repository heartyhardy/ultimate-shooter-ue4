// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterCharacter.h"


// Sets default values
AExplosive::AExplosive() :
	Damage(100.f),
	ExplosionDelay(1.f),
	ChainExplosionDelay(0.1f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ExplosiveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplosiveMesh"));
	SetRootComponent(ExplosiveMesh);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AExplosive::BeginPlay()
{
	Super::BeginPlay();
	
}

void AExplosive::BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController)
{
	// Do when linetrace of Player hits thie enemy
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ImpactSound,
			GetActorLocation()
		);
	}

	if (ExplodeParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplodeParticles,
			HitResult.Location,
			FRotator(0.f),
			true
		);
	}

	// Apply Explosive Damage!!
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());

	for (auto Actor : OverlappingActors)
	{
		UGameplayStatics::ApplyDamage(
			Actor,
			Damage,
			ShooterController,
			Shooter,
			UDamageType::StaticClass()
		);
	}

	TArray<AActor*> OverlappingExplosives;
	GetOverlappingActors(OverlappingExplosives, AExplosive::StaticClass());

	float BlastDelay = ExplosionDelay;

	for (auto Actor : OverlappingExplosives)
	{
		auto ExplosiveActor = Cast<AExplosive>(Actor);
		if (ExplosiveActor)
		{
			TArray<AActor*> OverlappingExplodingActors;
			ExplosiveActor->GetOverlappingActors(OverlappingExplodingActors, ACharacter::StaticClass());


			for (auto ExplodingActor : OverlappingExplodingActors)
			{
				if (!ExplodingActor || !ExplosiveActor) continue;
				FuseExplosive(
					HitResult,
					Shooter,
					ShooterController,
					ExplodingActor,
					ExplosiveActor,
					BlastDelay
				);

				BlastDelay += ChainExplosionDelay;
			}
		}
	}

	Destroy();
}



// Called every frame
void AExplosive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AExplosive::DelayedExplosion(FHitResult HitResult, AActor* Shooter, AController* ShooterController, AActor* DamagedActor, AActor* ExplosiveActor)
{
	// Do when linetrace of Player hits thie enemy
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ImpactSound,
			GetActorLocation()
		);
	}

	if (ExplodeParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplodeParticles,
			GetActorLocation(),//HitResult.Location,
			FRotator(0.f),
			true
		);
	}

	if (DamagedActor && this)
	{
		UGameplayStatics::ApplyDamage(
			DamagedActor,
			Damage,
			ShooterController,
			this, // This has changed from Actor to Explosive
			UDamageType::StaticClass()
		);
	}

	ExplosiveActor->Destroy();
}

void AExplosive::FuseExplosive(FHitResult HitResult, AActor* Shooter, AController* ShooterController, AActor* DamagedActor, AActor* ExplosiveActor, float BlastDelay)
{

	FTimerHandle ExplosionTimer;
	FTimerDelegate DelayedExplosionDelegate;

	DelayedExplosionDelegate.BindUFunction(
		ExplosiveActor,
		FName("DelayedExplosion"),
		HitResult,
		Shooter,
		ShooterController,
		DamagedActor,
		ExplosiveActor
	);

	GetWorld()->GetTimerManager().SetTimer(
		ExplosionTimer,
		DelayedExplosionDelegate,
		FMath::FRandRange(0.2f, 1.f),
		false
	);
}

