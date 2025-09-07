// Fill out your copyright notice in the Description page of Project Settings.


#include "SSaveGame.h"
#include "GameFramework/PlayerState.h"

FPlayerSaveData* USSaveGame::GetPlayerData(APlayerState* PlayerState)
{
	check(PlayerState);

    if(PlayerState->GetWorld()->IsPlayInEditor())
    {
        if(SavedPlayers.IsValidIndex(0))
        {
            return &SavedPlayers[0];
        }

        return nullptr;
    }

    FString PlayerID=PlayerState->GetUniqueId().ToString();

    return SavedPlayers.FindByPredicate([&](const FPlayerSaveData& Data){return Data.PlayerID==PlayerID;});

}
