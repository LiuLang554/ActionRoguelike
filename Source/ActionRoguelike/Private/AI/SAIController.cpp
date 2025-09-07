// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

void ASAIController::BeginPlay()
{
	Super::BeginPlay();

    if(ensureMsgf(MinionRanged,TEXT("BehaviorTree is nullptr! please assign BehaviorTree in your AI Controller.")))
    {
        RunBehaviorTree(MinionRanged);
    }

    //APawn* MyPawn=UGameplayStatics::GetPlayerPawn(this,0);

    //GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"),MyPawn->GetActorLocation());

    //GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"),MyPawn);
}