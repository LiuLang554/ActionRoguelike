// Fill out your copyright notice in the Description page of Project Settings.


#include "SSaveGameSubsystem.h"
#include "EngineUtils.h"
#include "SGameplayInterface.h"
#include "SSaveGame.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "SPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "SSaveGameSettings.h"

void USSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

    const USSaveGameSettings* SGSetting=GetDefault<USSaveGameSettings>();

    CurrentSlotName=SGSetting->SaveSlotName;

    UDataTable* DummyTable=SGSetting->DummyTable.LoadSynchronous();
}

void USSaveGameSubsystem::HandlesStartingNewPlayer(APlayerController* NewPlayer)
{
	ASPlayerState* PS=NewPlayer->GetPlayerState<ASPlayerState>();

    if(ensure(PS))
    {
        PS->LoadPlayerState(CurrentSaveGame);
    }

}

bool USSaveGameSubsystem::OverrideSpawnTransform(APlayerController* NewPlayer)
{
	check(NewPlayer);

    APlayerState* PS=NewPlayer->GetPlayerState<APlayerState>();

    check(PS);

    if(APawn* MyPawn=PS->GetPawn())
    {
        FPlayerSaveData* FoundData=CurrentSaveGame->GetPlayerData(PS);
        if(FoundData&&FoundData->bResumeAtTransform)
        {
            MyPawn->SetActorLocation(FoundData->Location);

            MyPawn->SetActorRotation(FoundData->Rotation);

            AController* PC=CastChecked<AController>(PS->GetOwner());

            PC->SetControlRotation(FoundData->Rotation);

            return true;
        }
    }

    return false;
}

void USSaveGameSubsystem::SetSlotName(FString NewSlotName)
{
	if(NewSlotName.Len()==0)
    {
        return;
    }

    CurrentSlotName=NewSlotName;
}

void USSaveGameSubsystem::WriteSaveGame()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(WriteSaveGame);

    CurrentSaveGame->SavedPlayers.Empty();

    CurrentSaveGame->SavedActorMap.Empty();

    AGameStateBase* GS=GetWorld()->GetGameState();

    check(GS);

    for(int32 i=0;i<GS->PlayerArray.Num();i++)
    {
        ASPlayerState* PS=CastChecked<ASPlayerState>(GS->PlayerArray[i]);

        if(PS)
        {
            PS->SavePlayerState(CurrentSaveGame);
            
            break;
        }
    }

    for(AActor* Actor : TActorRange<AActor>(GetWorld()))
    {
        if(!IsValid(Actor)||!Actor->Implements<USGameplayInterface>())
        {
            continue;
        }

        FActorSaveDate ActorDate;

        ActorDate.ActorName=Actor->GetFName();

        ActorDate.Transform=Actor->GetActorTransform();

        FMemoryWriter MemWrite(ActorDate.ByteData);

        FObjectAndNameAsStringProxyArchive Ar(MemWrite,true);

        Ar.ArIsSaveGame=true;

        Actor->Serialize(Ar);

        CurrentSaveGame->SavedActorMap.Add(Actor->GetFName(),ActorDate);

    }
}

void USSaveGameSubsystem::LoadSaveGame(FString InSlotName)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(LoadSaveGame);

    SetSlotName(InSlotName);

    if(UGameplayStatics::DoesSaveGameExist(CurrentSlotName,0))
    {
        CurrentSaveGame=Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSlotName,0));

        if(CurrentSaveGame==nullptr)
        {
            UE_LOG(LogTemp,Warning,TEXT("Failed to Load SaveGame Data."));

            return;
        }

        UE_LOG(LogTemp,Log,TEXT("Load SaveGame Data."));

        for(AActor* Actor : TActorRange<AActor>(GetWorld()))
        {
            if(!Actor->Implements<USGameplayInterface>())
            {
                continue;
            }

            if(FActorSaveDate* FoundData = CurrentSaveGame->SavedActorMap.Find(Actor->GetFName()))
            {
                Actor->SetActorTransform(FoundData->Transform);

                FMemoryReader MemReader(FoundData->ByteData);

                FObjectAndNameAsStringProxyArchive Ar(MemReader,true);

                Ar.ArIsSaveGame=true;

                Actor->Serialize(Ar);

                ISGameplayInterface::Execute_OnActorLoaded(Actor);
            }
        }

        OnSaveGameLoaded.Broadcast(CurrentSaveGame);
    }

    else
    {
        CurrentSaveGame=CastChecked<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));
    }
}