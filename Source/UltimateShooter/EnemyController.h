// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Runtime/AIModule/Classes/DetourCrowdAIController.h"
#include "DetourCrowdAIController.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class ULTIMATESHOOTER_API AEnemyController : public AAIController
{
	GENERATED_BODY()
	
public:

	AEnemyController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void OnPossess(APawn* InPawn) override;	 

protected:

	UFUNCTION(BlueprintCallable)
		void SetCrowdAttributes();

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
