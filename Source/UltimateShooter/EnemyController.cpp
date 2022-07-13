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
			//PathFollowComp->SetCrowdCollisionQueryRange(1000.f);
			PathFollowComp->SetCrowdSeparationWeight(500.f, true);
			PathFollowComp->SetCrowdPathOptimizationRange(2000.f, true);
			PathFollowComp->SetCrowdObstacleAvoidance(true, true);
			PathFollowComp->SetCrowdPathOffset(true, true);
			PathFollowComp->SetCrowdSeparation(true, true);
			UE_LOG(LogTemp, Warning, TEXT("CROWD AI SET"));
		}
	}

}
