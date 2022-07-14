// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Enemy.h"


AEnemyController::AEnemyController(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	check(BlackboardComponent);

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	check(BehaviorTreeComponent);
}

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!InPawn) return;

	AEnemy* Enemy = Cast<AEnemy>(InPawn);

	if (Enemy)
	{
		if (Enemy->GetBehaviorTree())
		{
			BlackboardComponent->InitializeBlackboard(*(Enemy->GetBehaviorTree()->BlackboardAsset));
		}

		UCrowdFollowingComponent* PathFollowComp = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent());
		if (PathFollowComp)
		{
			PathFollowComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High);
			PathFollowComp->SetCrowdCollisionQueryRange(500.f);
			PathFollowComp->SetCrowdSeparationWeight(500.f, true);
			PathFollowComp->SetAcceptanceRadius(1000.f);
			PathFollowComp->SetBlockDetection(300.f, 3.f, 12);
			PathFollowComp->SetCrowdAnticipateTurns(true, true);
			PathFollowComp->SetCrowdSeparation(true, true);
			PathFollowComp->SetBlockDetectionState(true);
			UE_LOG(LogTemp, Warning, TEXT("CROWD AI SET"));
		}
	}

}
