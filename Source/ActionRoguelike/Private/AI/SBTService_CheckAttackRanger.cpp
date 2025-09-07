// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckAttackRanger.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

void USBTService_CheckAttackRanger::TickNode(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory,float DeltaSeconds)
{
	Super::TickNode(OwnerComp,NodeMemory,DeltaSeconds);

    UBlackboardComponent* BlackboardComp=OwnerComp.GetBlackboardComponent();
    if(ensure(BlackboardComp))
    {
       AActor* MyActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("TargetActor")));
       if(MyActor)
       {
        AAIController* AIController=OwnerComp.GetAIOwner();
        if(ensure(AIController))
        {
            APawn* AIPawn=AIController->GetPawn();
            if(ensure(AIPawn))
            {
                float Distance=FVector::Distance(MyActor->GetActorLocation(),AIPawn->GetActorLocation());

                bool bWithinRange=Distance<2000.0f;

                bool bHasLOS=false;

                if(bWithinRange)
                {
                    bHasLOS=AIController->LineOfSightTo(MyActor);
                }

                BlackboardComp->SetValueAsBool(AttackRangeKey.SelectedKeyName,(bWithinRange&&bHasLOS));
            }
        }
       }
    }
}
