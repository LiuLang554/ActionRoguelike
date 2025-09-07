// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SMonsterData.generated.h"

class USAction;
class UTexture2D;

UCLASS()
class ACTIONROGUELIKE_API USMonsterData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
	public:

	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Monsters",GetFName());
	}

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="MonsterInfo")
	TSubclassOf<AActor> MonsterClass;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="MonsterInfo")
	TArray<TSubclassOf<USAction>> Actions;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="MonsterInfo")
	TObjectPtr<UTexture2D> Icon;

};
