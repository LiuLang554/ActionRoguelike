// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SActionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActorStateChanged,USActionComponent*,OwningComp,USAction*,Action);

class USAction;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API USActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	USActionComponent();

	UFUNCTION(BlueprintCallable,Category="Action")
	void AddAction(AActor* Instigator,TSubclassOf<USAction> ActionClass);

	UFUNCTION(BlueprintCallable,Category="Action")
	void RemoveAction(USAction* ActionToRemove);

	UFUNCTION(BlueprintCallable,Category="Action")
	bool StartActionByName(AActor* Instigator,FName ActionsName);

	UFUNCTION(BlueprintCallable,Category="Action")
	bool StopActionByName(AActor* Instigator,FName ActionsName);

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Tags")
	FGameplayTagContainer ActiveGameplayTags;

	UPROPERTY(BlueprintAssignable)
	FOnActorStateChanged OnActorStartChanged;

	UPROPERTY(BlueprintAssignable)
	FOnActorStateChanged OnActorStopChanged;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly,Replicated)
	TArray<USAction*> Actions;

	UPROPERTY(EditAnywhere,Category="Action")
	TArray<TSubclassOf<USAction>> DefaultActions;

	UFUNCTION(Server,Reliable)
	void ServerStartAction(AActor* Instigator,FName ActionsName);

	UFUNCTION(Server,Reliable)
	void ServerStopAction(AActor* Instigator,FName ActionsName);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Actions")
	USAction* GetAction(TSubclassOf<USAction> ActionClass) const;

	bool ReplicateSubobjects(class UActorChannel* Channel,class FOutBunch* bunch,FReplicationFlags* RepFlags)override;		
};




