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
	ChainExplosionDelay(0.5f)
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

				FTimerHandle ExplosionTimer;
				FTimerDelegate DelayedExplosionDelegate;

				DelayedExplosionDelegate.BindUFunction(
					ExplosiveActor, 
					FName("DelayedExplosion"), 
					HitResult,
					Shooter,
					ShooterController,
					ExplodingActor,
					ExplosiveActor
				);

				GetWorld()->GetTimerManager().SetTimer(
					ExplosionTimer,
					DelayedExplosionDelegate,
					BlastDelay,
					false
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
			HitResult.Location,
			FRotator(0.f),
			true
		);
	}
	UGameplayStatics::ApplyDamage(
		DamagedActor,
		Damage,
		ShooterController,
		this, // This has changed from Actor to Explosive
		UDamageType::StaticClass()
	);

	ExplosiveActor->Destroy();
}

