// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
AExplosiveBarrel::AExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComp=CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	RootComponent=StaticMeshComp;

	RadialForceComp=CreateDefaultSubobject<URadialForceComponent>("RadialForce");
	RadialForceComp->SetupAttachment(StaticMeshComp);

	RadialForceComp->SetAutoActivate(false);

	RadialForceComp->Radius=750.0f;
	RadialForceComp->ImpulseStrength=2500.f;

	RadialForceComp->bImpulseVelChange=true;

	RadialForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);

}

void AExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	StaticMeshComp->OnComponentHit.AddDynamic(this,&AExplosiveBarrel::OnActorHit);
}

void AExplosiveBarrel::OnActorHit(UPrimitiveComponent* HitComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,FVector NormalImpulse,const FHitResult& Hit)
{
	RadialForceComp->FireImpulse();

	UE_LOG(LogTemp,Log,TEXT("OnActorHit in Explosive Barrel"));

	UE_LOG(LogTemp,Warning,TEXT("OtherActor : %s,at game time : %f"),*GetNameSafe(OtherActor),GetWorld()->TimeSeconds);

	FString CombinedString=FString::Printf(TEXT("Hit at Location : %s"),*Hit.ImpactPoint.ToString());
	DrawDebugString(GetWorld(),Hit.ImpactPoint,CombinedString,nullptr,FColor::Green,2.0f,true);
}
