// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PlacedInWorldEnemyController.generated.h"

/**
 * 
 */
UCLASS()
class ULTIMATESHOOTER_API APlacedInWorldEnemyController : public AAIController
{
	GENERATED_BODY()

public:

	APlacedInWorldEnemyController();
	virtual void OnPossess(APawn* InPawn) override;

private:

	/** Blackboard for this enemy */
	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true"))
		class UBlackboardComponent* BlackboardComponent;

	/** Behavior tree for this enemy */
	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true"))
		class UBehaviorTreeComponent* BehaviorTreeComponent;

public:

	FORCEINLINE UBlackboardComponent* GetBlackboardComponent() const { return BlackboardComponent; }
};
