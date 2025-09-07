// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged,AActor*,InstigatorActor,USAttributeComponent*,OwningComp,float,NewHealth,float,Delta);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAttributeChanged,AActor*,InstigatorActor,USAttributeComponent*,OwningComp,float,NewValue,float,Delta);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USAttributeComponent();

protected:

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Replicated,Category="Attribute")
	float Health;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Replicated,Category="Attribute")
	float HealthMax;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Replicated,Category="Attribute")
	float Rage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Replicated,Category="Attribute")
	float RageMax;

	UFUNCTION(NetMulticast,Reliable)
	void MuliticastHealthChanged(AActor* InstigatorActor,float NewHealth,float Delta);

	UFUNCTION(NetMulticast,Unreliable)
	void MuliticastRageChanged(AActor* InstigatorActor,float NewRage,float Delta);

public:	

	UFUNCTION(BlueprintCallable,Category="Attribute")
	bool ApplyHealthChanger(AActor* InstigatorActor,float Delta);

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnRageChanged;

	UFUNCTION(BlueprintCallable)
	bool IsAlive()const;

	UFUNCTION(BlueprintCallable)
	bool IsFullHealth()const;

	UFUNCTION(BlueprintCallable)
	float GetHealthMax()const;

	UFUNCTION(BlueprintCallable,Category="Attribute")
	static USAttributeComponent* GetAttribute(AActor* FromActor);

	UFUNCTION(BlueprintCallable,Category="Attribute",meta=(DisplayName="IsAlive"))
	static bool IsActorAlive(AActor* FromActor);
	
	UFUNCTION(BlueprintCallable)
	bool Kill(AActor* InstigatorActor);

	UFUNCTION(BlueprintCallable)
	float GetHealth()const;

	UFUNCTION(BlueprintCallable)
	float GetRage() const;

	UFUNCTION(BlueprintCallable,Category="Attribute")
	bool ApplyRage(AActor* InstigatorActor,float Delta);
};