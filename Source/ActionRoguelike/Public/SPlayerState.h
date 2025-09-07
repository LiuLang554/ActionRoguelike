// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged,ASPlayerState*,PlayerState,int32,NewCredits,int32,Delta);

class USSaveGame;

UCLASS()
class ACTIONROGUELIKE_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(Transient,EditDefaultsOnly,ReplicatedUsing="OnRep_Credits",Category="Credits")
	int32 Credits;

	UFUNCTION()
	void OnRep_Credits(int32 OldCredits);

	UPROPERTY(Transient,BlueprintReadOnly)
	float PersonalRecordTime;

public:

	UFUNCTION(BlueprintCallable,Category="Credits")
	int32 GetCredits() const;

	UFUNCTION(BlueprintCallable,Category="Credits")
	void AddCredits(int32 Delta);

	UFUNCTION(BlueprintCallable,Category="Credits")
	bool RemoveCredits(int32 Delta);

	UPROPERTY(BlueprintAssignable,Category="Events")
	FOnCreditsChanged OnCreditsChanged;

	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(USSaveGame* SaveObjects);

	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(USSaveGame* SaveObjects);
	
};