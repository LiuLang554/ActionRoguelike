// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectileBase.h"
#include "GameplayTagContainer.h"
#include "SMagicProjectileBase.generated.h"

class USActionEffect;

UCLASS()
class ACTIONROGUELIKE_API ASMagicProjectileBase : public ASProjectileBase
{
	GENERATED_BODY()

public:

	ASMagicProjectileBase();
	
protected:

	UPROPERTY(EditDefaultsOnly,Category="Damage")
	float DamageAmount;

	UPROPERTY(EditDefaultsOnly,Category="Damage")
	FGameplayTag ParryTag;

	UFUNCTION()
	void OnActorOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);

	UPROPERTY(EditDefaultsOnly,Category="Damage")
	TSubclassOf<USActionEffect> BurningActionClass;
};


