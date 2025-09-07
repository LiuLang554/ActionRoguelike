// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AICharacter.generated.h"

class UPawnSensingComponent;
class USAttributeComponent;
class UUserWidget;
class USWorldUserWidget;
class USActionComponent;

UCLASS()
class ACTIONROGUELIKE_API AAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAICharacter();

protected:

	UPROPERTY(VisibleAnywhere,Category="Component")
	TObjectPtr<UPawnSensingComponent> PawnSensingComp;

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	virtual void PostInitializeComponents()override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<USAttributeComponent> AttributeComp;

	void SetTargetActor(AActor* NewTargetActor);

	UPROPERTY(VisibleAnywhere,Category="Effects")
	FName TimeToHitParamName;

	UPROPERTY(EditAnywhere,Category="UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	UPROPERTY(EditAnywhere,Category="UI")
	TSubclassOf<UUserWidget> SpottedwidgetClass;

	UPROPERTY()
	TObjectPtr<USWorldUserWidget> ActiveWidget;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<USActionComponent> ActionComp;

	UFUNCTION(NetMulticast,Unreliable)
	void MuliticastPawnSeen();

public:

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor,USAttributeComponent* OtherComp,float NewHealth,float Delta);

};



