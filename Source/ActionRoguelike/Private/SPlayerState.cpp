// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include "SSaveGame.h"
#include "Net/UnrealNetwork.h"

int32 ASPlayerState::GetCredits() const
{
	return Credits;
}

void ASPlayerState::AddCredits(int32 Delta)
{
    if(!ensure(Delta>0.0f))
    {
        return;
    }

	Credits+=Delta;

    OnCreditsChanged.Broadcast(this,Credits,Delta);
}

bool ASPlayerState::RemoveCredits(int32 Delta)
{
	if(!ensure(Delta>0.0f))
    {
        return false;
    }

    if(Credits<Delta)
    {
        return false;
    }

    Credits-=Delta;

    OnCreditsChanged.Broadcast(this,Credits,-Delta);

    return true;
}

void ASPlayerState::SavePlayerState_Implementation(USSaveGame* SaveObjects)
{
	if(SaveObjects)
    {
        FPlayerSaveData SaveData;

        SaveData.Credits=Credits;

        SaveData.PersonalRecordTime=PersonalRecordTime;

        SaveData.PlayerID=GetUniqueId().ToString();

        if(APawn* MyPawn=GetPawn())
        {
            SaveData.Location=MyPawn->GetActorLocation();

            SaveData.Rotation=MyPawn->GetActorRotation();

            SaveData.bResumeAtTransform=true;
        }

        SaveObjects->SavedPlayers.Add(SaveData);
    }
}

void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* SaveObjects)
{
	if(SaveObjects)
    {
        FPlayerSaveData* FoundData=SaveObjects->GetPlayerData(this);

        if(FoundData)
        {
            AddCredits(FoundData->Credits);

            PersonalRecordTime=FoundData->PersonalRecordTime;
        }
        else
        {
           
        }
    }
}

void ASPlayerState::OnRep_Credits(int32 OldCredits)
{
	OnCreditsChanged.Broadcast(this,Credits,Credits-OldCredits);
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ASPlayerState,Credits);
}