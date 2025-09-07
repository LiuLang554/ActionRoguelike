// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileBase.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/FastReferenceCollector.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

// Sets default values
ASProjectileBase::ASProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereComp=CreateDefaultSubobject<USphereComponent>("Mesh");
	SphereComp->OnComponentHit.AddDynamic(this,&ASProjectileBase::OnActorHit);
	RootComponent=SphereComp;

	ParticleComp=CreateDefaultSubobject<UParticleSystemComponent>("Effects");
	ParticleComp->SetupAttachment(SphereComp);

	AudioComp=CreateDefaultSubobject<UAudioComponent>("AudioComp");
	AudioComp->SetupAttachment(RootComponent);

	MoveComp=CreateDefaultSubobject<UProjectileMovementComponent>("Move");
	MoveComp->InitialSpeed=8000.0f;
	MoveComp->bRotationFollowsVelocity=true;
	MoveComp->bInitialVelocityInLocalSpace=true;
	MoveComp->ProjectileGravityScale=0.0f;

	ImpactShakeInnerRadius=0.0f;
	ImpactShakeOuterRadius=1500.0f;

	SetReplicates(true);

}

void ASProjectileBase::OnActorHit(UPrimitiveComponent* HitComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,FVector NormalImpulse,const FHitResult& Hit)
{
	Explode();
}

void ASProjectileBase::Explode_Implementation()
{	
	
	UGameplayStatics::SpawnEmitterAtLocation(this,ImpactVFX,GetActorLocation(),GetActorRotation());

	UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,GetActorLocation());

	UGameplayStatics::PlayWorldCameraShake(this,ImpactShake,GetActorLocation(),ImpactShakeInnerRadius,ImpactShakeOuterRadius);

	Destroy();
}

void ASProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

