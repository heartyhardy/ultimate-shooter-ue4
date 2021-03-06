// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "ShooterCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Explosive.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "MarkedExecutionDamageType.h"
#include "ShooterGameState.h"
#include "Announcer.h"
#include "Misc/DateTime.h"

// Sets default values
AEnemy::AEnemy() :
	Health(100.f),
	MaxHealth(100.f),
	HealthBarDisplayTime(4.f),
	bCanHitReact(true),
	HitReactTimeMin(.5f),
	HitReactTimeMax(3.5f),
	HitNumberDestroyTime(1.5f),
	bStunned(false),
	StunChance(0.5f),
	AttackLFast(TEXT("AttackLFast")),
	AttackRFast(TEXT("AttackRFast")),
	AttackL(TEXT("AttackL")),
	AttackR(TEXT("AttackR")),
	BaseDamage(20.f),
	LeftWeaponSocket(TEXT("FX_Trail_L_02")),
	RightWeaponSocket(TEXT("FX_Trail_R_02")),
	bCanAttack(true),
	AttackWaitTime(1.f),
	bDying(false),
	DeathTime(4.f),
	ExplosiveSlowMotionTime(1.25f),
	bInExplosiveSlowMotion(false),
	EmoteBubbleDisplayTime(4.f),
	bScouting(false),
	bRespondToScouts(true),
	bRaging(false),
	EnemyDetectedSoundCooldown(3.f),
	InitiateAmbushSoundCooldown(7.f),
	ScoutMinWalkSpeedBoost(30.f),
	ScoutMaxWalkSpeedBoost(60.f),
	ScoutMinRageDamageBonus(5.f),
	ScoutMaxRageDamageBonus(15.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Agro Sphere
	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());

	// Create Scout Sphere
	ScoutSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ScoutSphere"));
	ScoutSphere->SetupAttachment(GetRootComponent());

	// Create Combat Range Sphere
	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRangeSphere"));
	CombatRangeSphere->SetupAttachment(GetRootComponent());

	LeftWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWeaponBox"));
	LeftWeaponCollision->SetupAttachment(GetMesh(), FName("LeftWeaponBone"));

	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWeaponBox"));
	RightWeaponCollision->SetupAttachment(GetMesh(), FName("RightWeaponBone"));
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOverlap); // Bind the overlap event

	if (bScouting)
	{
		ScoutSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::ScoutSphereOverlap); // Bind the scout sphere overlap event
	}

	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOverlap); //Bind the combat sphere begin overlap
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereEndOverlap); // Bind the combat sphere end overlap

	// Bind Functions to Weapon Overlap Events
	LeftWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnLeftWeaponOverlap);
	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnRightWeaponOverlap);

	// Set Collisions For Weapons
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// Enable Collisions for Impact
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	// Ignore the camera when attacking for mesh and capsule
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	// Get the AI controller
	EnemyController = Cast<AEnemyController>(GetController());

	// Set Attack Flag
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), true);
	}

	// Transform Local Vector: PatrolPoint to World Space Vector
	const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(
		GetActorTransform(), 
		PatrolPoint
	);

	// Transform Local Vector: PatrolPoint to World Space Vector
	const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(
		GetActorTransform(),
		PatrolPoint2
	);

	if (EnemyController)
	{
		// Set 1st Patrol point Vector value to blackboard
		EnemyController->GetBlackboardComponent()->SetValueAsVector(
			TEXT("PatrolPoint"), 
			WorldPatrolPoint
		);

		// Set 2nd Patrol point Vector value to blackboard
		EnemyController->GetBlackboardComponent()->SetValueAsVector(
			TEXT("PatrolPoint2"),
			WorldPatrolPoint2
		);

		EnemyController->RunBehaviorTree(BehaviorTree);
	}

}

void AEnemy::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(
		HealthBarTimer, 
		this, 
		&AEnemy::HideHealthBar, 
		HealthBarDisplayTime
	);
}

void AEnemy::ShowEmoteBubble_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT("SHOW EMOTE BUBBLE IMPL"));
	GetWorldTimerManager().ClearTimer(EmoteBubbleTimer);
	GetWorldTimerManager().SetTimer(
		EmoteBubbleTimer,
		this,
		&AEnemy::HideEmoteBubble,
		EmoteBubbleDisplayTime
	);
}

void AEnemy::Die(bool bForce)
{
	if (!bForce && bDying) return;
	
	bDying = true;

	// Check Kill Streaks
	// THIS IS DUMMY CODE: NEEDS TO GO INTO GAMESTATE AS AN EVENT
	auto GameState = Cast<AShooterGameState>(GetWorld()->GetGameState());
	if (GameState)
	{
		// Increment Kills
		GameState->IncrementCurrentKills();

		GameState->SetLastKillTime(GameState->GetCurrentKillTime());
		GameState->SetCurrentKillTime(FDateTime::Now());
		float KillTimeDiff = GameState->GetKillTimeDifference().GetSeconds();

		if (KillTimeDiff <= GameState->GetKillStreakThreshold())
		{
			GameState->IncrementCurrentKillStreak();
		}
		else
		{
			GameState->ResetCurrentKillStreak();
			GameState->IncrementCurrentKillStreak();
		}


		if (GameState->GetCurrentKills() == 1)
		{
			if (GameState->GetAnnouncer())
			{
				// Play First Blood
				// 1s Delay in the CUE
				GameState->GetAnnouncer()->PlayKillStreakAnnouncement(EKillStreakAnnoucementType::EKSAT_FirstKill);
			}
		}
		else if (GameState->GetCurrentKillStreak() >= 0)
		{
			GameState->PlayKillStreakAnnouncement();
		}
	}

	// Stops shots from hitting the corpse
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);

	//Unbind Overlap Events
	AgroSphere->OnComponentBeginOverlap.RemoveAll(this);
	CombatRangeSphere->OnComponentBeginOverlap.RemoveAll(this);
	ScoutSphere->OnComponentBeginOverlap.RemoveAll(this);
	LeftWeaponCollision->OnComponentBeginOverlap.RemoveAll(this);
	RightWeaponCollision->OnComponentBeginOverlap.RemoveAll(this);

	// TODO: IMPROVE THIS: SEE Take Damage
	//HideHealthBar();
	HideEmoteBubble();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		
		int32 RandAnimation = FMath::RandRange(1, 10);
		FName RandSection = RandAnimation > 5 ? FName(TEXT("DeathA")) : FName(TEXT("DeathB"));

		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(RandSection);
		
		if (DeathSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
		}
	}

	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
		// Also stop movement after Death
		EnemyController->StopMovement();
	}
}

void AEnemy::PlayHitMontage(FName Section, float PlayRate)
{
	if (bCanHitReact)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(HitMontage, PlayRate);
			AnimInstance->Montage_JumpToSection(Section, HitMontage);
		}

		bCanHitReact = false;
		const float HitReactTime{ FMath::FRandRange(HitReactTimeMin, HitReactTimeMax) };
		GetWorldTimerManager().SetTimer(HitReactTimer, this, &AEnemy::ResetHitReactTimer, HitReactTime);
	}

}

void AEnemy::ResetHitReactTimer()
{
	bCanHitReact = true;
}

void AEnemy::StoreHitNumber(UUserWidget* HitNumber, FVector Location)
{
	HitNumbers.Add(HitNumber, Location);

	// Create a local timer and bind it to a function(DestroyHitNumber) which has input params
	// We dont use a member variable for this timer because for each hit we need a seperate timer!
	FTimerHandle HitNumberTimer;
	FTimerDelegate HitNumberDelegate;

	HitNumberDelegate.BindUFunction(this, FName("DestroyHitNumber"), HitNumber);
	GetWorld()->GetTimerManager().SetTimer(
		HitNumberTimer, 
		HitNumberDelegate, 
		HitNumberDestroyTime, 
		false
	);
}

void AEnemy::DestroyHitNumber(UUserWidget* HitNumber)
{
	HitNumbers.Remove(HitNumber);
	HitNumber->RemoveFromParent();
}

void AEnemy::UpdateHitNumbers()
{
	for (auto& HitPair : HitNumbers) // Range based for loop
	{
		UUserWidget* HitNumber{ HitPair.Key };
		const FVector Location{ HitPair.Value };
		FVector2D ScreenPosition;

		UGameplayStatics::ProjectWorldToScreen(
			GetWorld()->GetFirstPlayerController(),
			Location,
			ScreenPosition
		);

		HitNumber->SetPositionInViewport(ScreenPosition); // Loop through and set screen pos for each
	}
}

void AEnemy::AgroSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bDying || !OtherActor) return;

	auto Character = Cast<AShooterCharacter>(OtherActor);
	if (Character)
	{
		if (EnemyController)
		{
			if (EnemyController->GetBlackboardComponent())
			{
				// Set the value of the "Target" key
				EnemyController->GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), Character);
				GetCharacterMovement()->RotationRate = FRotator(0.f, 120.f, 0.f);
			}
		}
	}
}

void AEnemy::ScoutSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bDying) return;
	if (!bScouting) return;
	if (!OtherActor) return;

	auto Character = Cast<AShooterCharacter>(OtherActor);
	if (Character)
	{
		// Show Emote Bubble of the Scout
		// TODO: Decide if this is acceptable in gameplay as this shows the location of the Scout!
		ShowEmoteBubble();
		
		// Start Enemy Detected Timer
		if (!GetWorldTimerManager().IsTimerActive(EnemyDetectedSoundTimer))
		{
			GetWorldTimerManager().SetTimer(
				EnemyDetectedSoundTimer,
				this,
				&ThisClass::PlayEnemyDetectedSound,
				EnemyDetectedSoundCooldown
			);
		}

		TArray<AActor*> OverlappedAllies;
		GetOverlappingActors(OverlappedAllies, AEnemy::StaticClass());

		for (auto AllyActor : OverlappedAllies)
		{
			auto Ally = Cast<AEnemy>(AllyActor);
			if (Ally && !Ally->bScouting && Ally->bRespondToScouts)
			{
				if (Ally->EnemyController)
				{
					// Speed boost all allies!
					if (bRaging)
					{
						Ally->BaseDamage += FMath::FRandRange(ScoutMinRageDamageBonus, ScoutMaxRageDamageBonus);
					}
					Ally->GetCharacterMovement()->MaxWalkSpeed += FMath::FRandRange(ScoutMinWalkSpeedBoost, ScoutMaxWalkSpeedBoost);
					
					if (Ally->InitiateAmbushSound)
					{
						if (!GetWorldTimerManager().IsTimerActive(Ally->InitiateAmbushSoundTimer))
						{
							GetWorldTimerManager().SetTimer(
								Ally->InitiateAmbushSoundTimer,
								Ally,
								&ThisClass::PlayInitiateAmbushSound,
								FMath::RandRange(EnemyDetectedSoundCooldown + 1.0f, Ally->InitiateAmbushSoundCooldown)
							);
						}
					}
					
					Ally->EnemyController->GetBlackboardComponent()->SetValueAsObject(FName("TargetActor"), Character);
					GetCharacterMovement()->RotationRate = FRotator(0.f, 120.f, 0.f);
				}
			}
		}
	}
}

void AEnemy::SetStunned(bool Stunned)
{
	bStunned = Stunned;
	
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("Stunned"), Stunned);
	}
}

void AEnemy::CombatSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bDying || !OtherActor) return;
	auto Character = Cast<AShooterCharacter>(OtherActor);

	if (Character)
	{
		bInAttackRange = true;
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), true);
		}
	}
}

void AEnemy::CombatSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{

	if (!OtherActor) return;
	auto Character = Cast<AShooterCharacter>(OtherActor);

	if (Character)
	{
		bInAttackRange = false;
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), false);
		}
	}
}

void AEnemy::PlayAttackMontage(FName Section, float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage, PlayRate);
		AnimInstance->Montage_JumpToSection(Section, AttackMontage);
	}

	bCanAttack = false;
	GetWorldTimerManager().SetTimer(
		AttackWaitTimer, 
		this, 
		&AEnemy::ResetCanAttack, 
		AttackWaitTime);
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), false);
	}
}

FName AEnemy::GetAttackSectionName()
{
	const int32 Section{ FMath::RandRange(1, 4) };
	FName SectionName;

	switch (Section)
	{
	case 1:
		SectionName = AttackLFast;
		break;
	case 2:
		SectionName = AttackRFast;
		break;
	case 3:
		SectionName = AttackL;
		break;
	case 4:
		SectionName = AttackR;
		break;
	}

	return SectionName;
}

void AEnemy::OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Character = Cast<AShooterCharacter>(OtherActor);
	if (Character)
	{
		float AbsoluteDamage = DoDamage(Character);		
		//Spawn Armor Negate Effect VFX
		if (AbsoluteDamage <= 0.f)
		{
			ShowArmorNegation(Character, LeftWeaponSocket);
			return;
		}

		SpawnBlood(Character, LeftWeaponSocket);
		StunCharacter(Character);
	}
}

void AEnemy::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Character = Cast<AShooterCharacter>(OtherActor);
	if (Character)
	{
		float AbsoluteDamage = DoDamage(Character);

		//Spawn Armor Negate Effect VFX
		if (AbsoluteDamage <= 0.f)
		{
			ShowArmorNegation(Character, RightWeaponSocket);
			return;
		}

		SpawnBlood(Character, RightWeaponSocket);
		StunCharacter(Character);
	}
}

void AEnemy::ActivateLeftWeapon()
{
	if (bDying) return;
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeActivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::ActivateRightWeapon()
{
	if (bDying) return;
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeActivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

float AEnemy::DoDamage(AShooterCharacter* Victim)
{
	if (bDying) return 0.f;
	if (!Victim) return 0.f;

	const float AbsoluteDamage = UGameplayStatics::ApplyDamage(
		Victim,
		BaseDamage,
		EnemyController,
		this,
		UDamageType::StaticClass()
	);

	if (Victim->GetMeleeImpactSound())
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			Victim->GetMeleeImpactSound(),
			GetActorLocation()
		);
	}

	return AbsoluteDamage;
}

void AEnemy::SpawnBlood(AShooterCharacter* Victim, FName SocketName)
{
	// Get Socket at the tip of the weapon(Left)
	const USkeletalMeshSocket* TipSocket{ GetMesh()->GetSocketByName(SocketName) };
	if (TipSocket)
	{
		const FTransform SocketTransform{ TipSocket->GetSocketTransform(GetMesh()) };
		if (Victim->GetBloodParticles()) // Spawn Blood Particles
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				Victim->GetBloodParticles(),
				SocketTransform
			);
		}
	}
}

void AEnemy::ShowArmorNegation(AShooterCharacter* Victim, FName SocketName)
{
	const USkeletalMeshSocket* TipSocket{ GetMesh()->GetSocketByName(SocketName) };
	if (TipSocket)
	{
		const FTransform SocketTransform{ TipSocket->GetSocketTransform(GetMesh()) };
		if (Victim->GetArmorNegationParticles())
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				Victim->GetArmorNegationParticles(),
				SocketTransform
			);
		}
	}
}

void AEnemy::StunCharacter(AShooterCharacter* Victim)
{
	if (Victim)
	{
		const float Stun{ FMath::FRandRange(0.f, 1.f) };
		if (Stun <= Victim->GetStunChance())
		{
			Victim->Stun();
		}
	}
}

void AEnemy::ResetCanAttack()
{
	bCanAttack = true;
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), true);
	}
}

void AEnemy::FinishDeath()
{
	GetMesh()->bPauseAnims = true; // Pause all animations
	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::DestroyEnemy, DeathTime);
}

void AEnemy::DestroyEnemy()
{
	// If Slow Motion for Explosives are active, Reset TimeDilation
	if (bInExplosiveSlowMotion)
	{
		ResetExplosiveSlowMotion();
	}
	Destroy();
	// You can perform other tasks here that comes after enemy death
}

void AEnemy::ApplyExplosiveSlowMotion(AActor* DamageCauser)
{
	if (bInExplosiveSlowMotion) return;

	auto ExplosiveActor = Cast<AExplosive>(DamageCauser);
	if (ExplosiveActor)
	{
		bInExplosiveSlowMotion = true;

		auto Shooter = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (Shooter)
		{
			// TODO: Slow Motion Emote
			Shooter->SetSceneFringe(3.0f);
			Shooter->SetSceneVignette(1.f);
			Shooter->StartExplosionSlowMoEmote();
		}

		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.3f);
		GetWorldTimerManager().SetTimer(
			ExplosiveSlowMotionTimer,
			this,
			&ThisClass::ResetExplosiveSlowMotion,
			ExplosiveSlowMotionTime
		);
	}
}

void AEnemy::ResetExplosiveSlowMotion()
{
	bInExplosiveSlowMotion = false;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);

	auto Shooter = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (Shooter)
	{
		Shooter->SetSceneFringe(3.0f, false); //Value not used
		Shooter->SetSceneVignette(1.f, false);
	}
}

void AEnemy::PlayEnemyDetectedSound()
{
	// Play Enemy Detected Sound
	if (EnemyDetectedSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			EnemyDetectedSound,
			GetActorLocation()
		);
	}
}

void AEnemy::PlayInitiateAmbushSound()
{
	// Play Enemy Detected Sound
	if (InitiateAmbushSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			InitiateAmbushSound,
			GetActorLocation()
		);
	}
}

void AEnemy::PlayMarkedExecutionDamageVFX()
{
	if (MarkedExecutionEffectParticles)
	{
		auto HeadboneLocation = GetMesh()->GetBoneLocation(*HeadBone);

		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			MarkedExecutionEffectParticles,
			HeadboneLocation
		);
	}
}

void AEnemy::SetPatrolPointOne(FVector WorldPoint)
{
	PatrolPoint = WorldPoint;

	if (EnemyController)
	{
		UE_LOG(LogTemp, Warning, TEXT("PatrolPoint Set"));
		// Set 1st Patrol point Vector value to blackboard
		EnemyController->GetBlackboardComponent()->SetValueAsVector(
			TEXT("PatrolPoint"),
			PatrolPoint
		);
	}
}

void AEnemy::SetPatrolPointTwo(FVector WorldPoint)
{
	PatrolPoint2 = WorldPoint;

	if (EnemyController)
	{
		// Set 2nd Patrol point Vector value to blackboard
		EnemyController->GetBlackboardComponent()->SetValueAsVector(
			TEXT("PatrolPoint2"),
			PatrolPoint2
		);
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateHitNumbers();
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController)
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

	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ImpactParticles,
			HitResult.Location,
			FRotator(0.f),
			true
		);
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// Agro Enemy when hit
	if (EnemyController)
	{
		// Setting this Key in blackboard so the enemy can chase!
		auto Character = Cast<AShooterCharacter>(EnemyController->GetBlackboardComponent()->GetValueAsObject(FName(TEXT("TargetActor"))));
		if (!Character)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsObject(FName("TargetActor"), DamageCauser);
			//TODO: THIS IS A TEMP SOLUTION
			GetCharacterMovement()->RotationRate = FRotator(0.f, 120.f, 0.f);
		}
	}

	// TODO:
	// OH YEAH NOW YOU KNOW!
	//CustomTimeDilation = 0.f;

	UMarkedExecutionDamageType* MarkedExecutionDamage = Cast<UMarkedExecutionDamageType>(DamageEvent.DamageTypeClass.Get()->GetDefaultObject());
	if (MarkedExecutionDamage)
	{
		PlayMarkedExecutionDamageVFX();
	}

	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		GetCharacterMovement()->MaxWalkSpeed = 0.f;
		
		// If Damaged by Explosions
		ApplyExplosiveSlowMotion(DamageCauser);
		Die();
	}
	else
	{
		Health -= DamageAmount;
	}

	if (bDying) return DamageAmount; // Early return if enemy is dying

	// TODO: IMPROVE THIS: Only show if its a BOSS OR MINI-BOSS
	//ShowHealthBar();

	// Determine if bullet hit stuns the enemy

	const float Stunned = FMath::FRandRange(0.f, 1.f);
	if (Stunned <= StunChance)
	{
		// TODO: Implement all directions
		PlayHitMontage(FName("HitReactFront"));
		SetStunned(true);
	}

	return DamageAmount;
}

void AEnemy::AlertEnemy()
{
	// For Lurkers this has to be TRUE
	// TODO: Maybe set this TRUE to all enemies at night?
	if (!bSilent)
	{
		ShowEmoteBubble();
	}
}

