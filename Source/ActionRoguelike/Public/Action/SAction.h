// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "SAction.generated.h"

class UWorld;
class USActionComponent;
class UTexture2D;

USTRUCT()
struct FActionRepData
{
	GENERATED_BODY()

public:

	UPROPERTY()
	bool bIsRunning;

	UPROPERTY()
	AActor* Instigator;
};

UCLASS(Blueprintable)
class ACTIONROGUELIKE_API USAction : public UObject
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Tags")
	FGameplayTagContainer GrantsTags;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Tags")
	FGameplayTagContainer BlockedTags;

	UFUNCTION(BlueprintCallable,Category="Action")
	USActionComponent* GetOwningComponent() const;

	UPROPERTY(ReplicatedUsing="OnRep_RepData")
	FActionRepData RepData;

	UFUNCTION()
	void OnRep_RepData();

	UPROPERTY(Replicated)
	TObjectPtr<USActionComponent> ActionComp;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="UI")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(Replicated)
	float StartTime;

public:

	UFUNCTION(BlueprintNativeEvent,Category="Action")
	void StartAction(AActor* Instigator);

	void Initialize(USActionComponent* NewActionComp);

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Category="Action")
	void StopAction(AActor* Instigator);

	UPROPERTY(EditDefaultsOnly,Category="Action")
	FName ActionName;

	UWorld* GetWorld() const override;

	UFUNCTION(BlueprintNativeEvent,Category="Action")
	bool CanStart(AActor* Instigator);

	UFUNCTION(BlueprintCallable,Category="Action")
	bool IsRunning() const;

	UPROPERTY(EditDefaultsOnly,Category="Action")
	bool bAutoStart;
	
	bool IsSupportedForNetworking() const override
	{
		return true;
	}

};



