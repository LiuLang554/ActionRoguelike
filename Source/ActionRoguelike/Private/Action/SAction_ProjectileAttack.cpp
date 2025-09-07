// Fill out your copyright notice in the Description page of Project Settings.


#include "Action/SAction_ProjectileAttack.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"


USAction_ProjectileAttack::USAction_ProjectileAttack()
{
	HandSocketName="Muzzle_01";

    AttackAnimDelay=0.2f;
}

void USAction_ProjectileAttack::StartAction_Implementation(AActor* InstiagtorActor)
{
	Super::StartAction_Implementation(InstiagtorActor);

    ACharacter* MyCharacter=Cast<ACharacter>(InstiagtorActor);

    if(MyCharacter)
    {
        MyCharacter->PlayAnimMontage(AttackAnim);

        UGameplayStatics::SpawnEmitterAttached(CastingEffect,MyCharacter->GetMesh(),HandSocketName,FVector::ZeroVector,FRotator::ZeroRotator,EAttachLocation::SnapToTarget);

		if(MyCharacter->HasAuthority())
		{
		FTimerHandle TimerHandle_AttackDelay;

        FTimerDelegate Delegate;

        Delegate.BindUFunction(this,"AttackDelay_Elapsed",MyCharacter);

        GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay,Delegate,AttackAnimDelay,false);
		}
    }

}

void USAction_ProjectileAttack::AttackDelay_Elapsed(ACharacter* InstigatorCharacter)
{
	if(ensure(ProjectileClass))
	{
	FVector HandLocation=InstigatorCharacter->GetMesh()->GetSocketLocation(HandSocketName);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator=InstigatorCharacter;

	FCollisionShape Shape;
	Shape.SetSphere(20.0f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(InstigatorCharacter);

	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjParams.AddObjectTypesToQuery(ECC_Pawn);

	FVector TraceStart=InstigatorCharacter->GetPawnViewLocation();

	FVector TraceEnd=TraceStart+(InstigatorCharacter->GetControlRotation().Vector()*5000);

	FHitResult Hit;

	if(GetWorld()->SweepSingleByObjectType(Hit,TraceStart,TraceEnd,FQuat::Identity,ObjParams,Shape,Params))
	{
		TraceEnd=Hit.ImpactPoint;
	}

	FRotator ProjRotation=FRotationMatrix::MakeFromX(TraceEnd-HandLocation).Rotator();

	FTransform SpawnTM=FTransform(ProjRotation,HandLocation);
	GetWorld()->SpawnActor<AActor>(ProjectileClass,SpawnTM,SpawnParams);
	}

    StopAction(InstigatorCharacter);
}