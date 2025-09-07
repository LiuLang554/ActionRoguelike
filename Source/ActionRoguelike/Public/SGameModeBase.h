// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "Engine/DataTable.h"
#include "SGameModeBase.generated.h"

class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;
class USSaveGame;
class UDataTable;
class USMonsterData;

USTRUCT(BlueprintType)
struct FMonsterInfoRow : public FTableRowBase
{
	GENERATED_BODY()

	public:

	FMonsterInfoRow()
	{
		Weight=1.0f;

		SpawnCost=5.0f;

		KillReward=20.0f;
	}

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	//TSubclassOf<AActor> MonsterClass;
	FPrimaryAssetId MonsterID;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float Weight;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float SpawnCost;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float KillReward;

};

UCLASS()
class ACTIONROGUELIKE_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:

	FTimerHandle TimerHandle_SpawnBots;

	FString SlotName;

	UPROPERTY()
	TObjectPtr<USSaveGame> CurrentSaveGame;

	UPROPERTY(EditDefaultsOnly,Category="AI")
	float SpawnTimerInterval;

	UFUNCTION()
	void SpawnBotTimerElapsed();

	UPROPERTY(EditDefaultsOnly,Category="AI")
	TObjectPtr<UEnvQuery> SpawnBotQuery;

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	TObjectPtr<UEnvQuery> PowerupSpawnQuery;

	UFUNCTION()
	void OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance,EEnvQueryStatus::Type QueryStatus);

	//UPROPERTY(EditDefaultsOnly,Category="AI")
	//TSubclassOf<AActor> MinionClass;

	UPROPERTY(EditDefaultsOnly,Category="AI")
	TObjectPtr<UDataTable> MonsterTable;
	
	UPROPERTY(EditDefaultsOnly,Category="AI")
	TObjectPtr<UCurveFloat> DifficultyCurve;
	
	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	int32 CreditsPerKill;

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float RequiredPowerupDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 DesiredPowerupCount;

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	TArray<TSubclassOf<AActor>> PowerupClasses;
	
	UFUNCTION()
	void OnPowerupSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance,EEnvQueryStatus::Type QueryStatus);

	void OnMonsterLoaded(FPrimaryAssetId LoadID,FVector SpawnLocation);

public:

	ASGameModeBase();

	virtual void StartPlay()override;

	UFUNCTION(Exec)
	void KillAll();

	virtual void OnActorKilled(AActor* VictimActor,AActor* Killer);

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)override;

	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)override;

};