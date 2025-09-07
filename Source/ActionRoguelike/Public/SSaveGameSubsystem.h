// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SSaveGameSubsystem.generated.h"

class USSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameSignature,class USSaveGame*,SaveObject);
/**
 * 
 */
UCLASS(meta=(DisplayName="SaveGame System"))
class ACTIONROGUELIKE_API USSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	protected:

	FString CurrentSlotName;

	UPROPERTY()
	TObjectPtr<USSaveGame> CurrentSaveGame;

	public:

	void HandlesStartingNewPlayer(APlayerController* NewPlayer);

	UFUNCTION(BlueprintCallable)
	bool OverrideSpawnTransform(APlayerController* NewPlayer);

	UFUNCTION(BlueprintCallable)
	void SetSlotName(FString NewSlotName);

	UFUNCTION(BlueprintCallable,Category="SaveGame")
	void WriteSaveGame();

	void LoadSaveGame(FString InSlotName = "");

	UPROPERTY(BlueprintAssignable)
	FOnSaveGameSignature OnSaveGameLoaded;

	UPROPERTY(BlueprintAssignable)
	FOnSaveGameSignature OnSaveGameWritten;

	void Initialize(FSubsystemCollectionBase& Collection)override;

};