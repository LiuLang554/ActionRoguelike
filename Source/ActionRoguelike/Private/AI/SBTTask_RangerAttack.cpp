// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_RangerAttack.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SAttributeComponent.h"

USBTTask_RangerAttack::USBTTask_RangerAttack()
{
	MaxBulletSpread=2.0f;
}

EBTNodeResult::Type USBTTask_RangerAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory)
{
	AAIController* AIController=OwnerComp.GetAIOwner();
    if(ensure(AIController))
    {
        ACharacter* MyCharacter=Cast<ACharacter>(AIController->GetPawn());

        if(MyCharacter==nullptr)
        {
            return EBTNodeResult::Failed;
        }

        FVector MuzzleLocation=MyCharacter->GetMesh()->GetSocketLocation("Muzzle_Front");

        AActor* TargetActor=Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")));

        if(TargetActor==nullptr)
        {
            return EBTNodeResult::Failed;
        }

        if(!USAttributeComponent::IsActorAlive(TargetActor))
        {
            return EBTNodeResult::Failed;
        }

        FVector Direction=TargetActor->GetActorLocation()-MuzzleLocation;

        FRotator MuzzleRotation=Direction.Rotation();

        //MuzzleRotation.Pitch+=FMath::RandRange(0.0f,MaxBulletSpread);

        //MuzzleRotation.Yaw+=FMath::RandRange(-MaxBulletSpread,MaxBulletSpread);

        FActorSpawnParameters Params;

        Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        Params.Instigator=MyCharacter;

        AActor* NewProj=GetWorld()->SpawnActor<AActor>(ProjectileClass,MuzzleLocation,MuzzleRotation,Params);

        return NewProj?EBTNodeResult::Succeeded:EBTNodeResult::Failed;
    }

    return EBTNodeResult::Failed;
    
}
