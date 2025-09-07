// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SProjectileBase.generated.h"

class USphereComponent;
class UParticleSystemComponent;
class UProjectileMovementComponent;
class UAudioComponent;
class UCameraShakeBase;
class USoundBase;

UCLASS()
class ACTIONROGUELIKE_API ASProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASProjectileBase();

protected:
	UPROPERTY(EditAnywhere,Category="Effects")
	TObjectPtr<UParticleSystem> ImpactVFX;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components");
	TObjectPtr<USphereComponent> SphereComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components");
	TObjectPtr<UProjectileMovementComponent> MoveComp;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<UAudioComponent> AudioComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components");
	TObjectPtr<UParticleSystemComponent> ParticleComp;

	UPROPERTY(EditAnywhere,Category="Effects")
	TSubclassOf<UCameraShakeBase> ImpactShake;

	UPROPERTY(EditAnywhere,Category="Effects")
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditAnywhere)
	float ImpactShakeInnerRadius;

	UPROPERTY(EditAnywhere)
	float ImpactShakeOuterRadius;

public:

	UFUNCTION()
	virtual void OnActorHit(UPrimitiveComponent* HitComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,FVector NormalImpulse,const FHitResult& Hit);

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void Explode();

	virtual void PostInitializeComponents()override;

};
