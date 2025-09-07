// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "AI/AICharacter.h"
#include "SAttributeComponent.h"
#include "EngineUtils.h"
#include "SPlayerState.h"
#include "SSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"
#include "SGameplayInterface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "SMonsterData.h"
#include "../ActionRoguelike.h"
#include "Action/SActionComponent.h"
#include "Action/SAction.h"
#include "Engine/AssetManager.h"
#include "SSaveGameSubsystem.h"

static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("AC.SpawnBots"),true,TEXT("Enable Spawning of bots via timer."),ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval=2.0f;

    CreditsPerKill=20;

    DesiredPowerupCount=10;

    RequiredPowerupDistance=2000;

    PlayerStateClass=ASPlayerState::StaticClass();

    SlotName="SaveGame01";
}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName,Options,ErrorMessage);

    USSaveGameSubsystem* SG=GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();

    FString SelectedSaveSlot=UGameplayStatics::ParseOption(Options,"SaveGame");

    SG->LoadSaveGame(SelectedSaveSlot);
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

    GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots,this,&ASGameModeBase::SpawnBotTimerElapsed,SpawnTimerInterval,true);

    if(ensure(PowerupClasses.Num()>0))
    {
        UEnvQueryInstanceBlueprintWrapper* QueryInstance=UEnvQueryManager::RunEQSQuery(this,PowerupSpawnQuery,this,EEnvQueryRunMode::AllMatching,nullptr);

        if(ensure(QueryInstance))
        {
            QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this,&ASGameModeBase::OnPowerupSpawnQueryCompleted);
        }
    }
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
    if(!CVarSpawnBots.GetValueOnGameThread())
    {
        UE_LOG(LogTemp,Warning,TEXT("Bot Spawning disabled via cvar 'CVarSpawnbots'."));

        return;
    }

    int32 NrOfAliveBots=0;

    for(TActorIterator<AAICharacter>It(GetWorld());It;++It)
    {
        AAICharacter* Bot= *It;

        USAttributeComponent* AttributeComp=USAttributeComponent::GetAttribute(Bot);
        
        if(ensure(AttributeComp)&&AttributeComp->IsAlive())
        {
            NrOfAliveBots++;
        }
    }

    UE_LOG(LogTemp,Log,TEXT("Found %i Alive Bots"),NrOfAliveBots);

    float MaxBotCount=10.0f;

    if(DifficultyCurve)
    {
        MaxBotCount=DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
    }

    if(NrOfAliveBots>=MaxBotCount)
    {
        UE_LOG(LogTemp,Log,TEXT("At maximum bot Capacity. Skipping bot Spawn."))
        return;
    }

	UEnvQueryInstanceBlueprintWrapper* QueryInstance=UEnvQueryManager::RunEQSQuery(this,SpawnBotQuery,this,EEnvQueryRunMode::RandomBest5Pct,nullptr);

    if(ensure(QueryInstance))
    {
        QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this,&ASGameModeBase::OnQueryCompleted);
    }
}

void ASGameModeBase::OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance,EEnvQueryStatus::Type QueryStatus)
{
	if(QueryStatus!=EEnvQueryStatus::Success)
    {
        UE_LOG(LogTemp,Warning,TEXT("Spawn bot EQS Query Failed!"));

        return;
    }

    TArray<FVector> Locations=QueryInstance->GetResultsAsLocations();

    if(Locations.IsValidIndex(0))
    {
        if(MonsterTable)
        {
            TArray<FMonsterInfoRow*> Rows;

            MonsterTable->GetAllRows("",Rows);

            int32 RandIndex=FMath::RandRange(0,Rows.Num()-1);

            FMonsterInfoRow* SelectRows=Rows[RandIndex];

            UAssetManager* Manager=UAssetManager::GetIfInitialized();

            if(Manager)
            {
                LogOnScreen(this,"Loading Monster...",FColor::Green);

                TArray<FName> Bundles;

                FStreamableDelegate Delegate=FStreamableDelegate::CreateUObject(this,&ASGameModeBase::OnMonsterLoaded,SelectRows->MonsterID,Locations[0]);

                Manager->LoadPrimaryAsset(SelectRows->MonsterID,Bundles,Delegate);
            }
        }
    }
}

void ASGameModeBase::OnPowerupSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance,EEnvQueryStatus::Type QueryStatus)
{
	if(QueryStatus!=EEnvQueryStatus::Success)
    {
        UE_LOG(LogTemp,Warning,TEXT("Spawn bot EQS query failed"));

        return;
    }

    TArray<FVector> Locations=QueryInstance->GetResultsAsLocations();

    TArray<FVector> UsedLocations;

    int32 SpawnCounter=0;

    while(SpawnCounter<DesiredPowerupCount&&Locations.Num()>0)
    {
        int32 RandomLocationIndex=FMath::RandRange(0,Locations.Num()-1);

        FVector PickedLocation=Locations[RandomLocationIndex];

        Locations.RemoveAt(RandomLocationIndex);

        bool bVaildLocation=true;

        for(FVector OtherLocations : UsedLocations)
        {
            float DistanceTo=(PickedLocation-OtherLocations).Size();

            if(DistanceTo<RequiredPowerupDistance)
            {
                bVaildLocation=false;

                break;
            }
        }

        if(!bVaildLocation)
        {
            continue;
        }

        int32 RandomClassIndex=FMath::RandRange(0,PowerupClasses.Num()-1);

        TSubclassOf<AActor> RandomPowerupClass=PowerupClasses[RandomClassIndex];

        GetWorld()->SpawnActor<AActor>(RandomPowerupClass,PickedLocation,FRotator::ZeroRotator);

        UsedLocations.Add(PickedLocation);

        SpawnCounter++;
    }


}

void ASGameModeBase::KillAll()
{
    for(TActorIterator<AAICharacter>It(GetWorld());It;++It)
    {
        AAICharacter* Bot= *It;

        USAttributeComponent* AttributeComp=USAttributeComponent::GetAttribute(Bot);
        
        if(ensure(AttributeComp)&&AttributeComp->IsAlive())
        {
            AttributeComp->Kill(this);
        }
    }
}

void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if(ensure(Controller))
    {
        Controller->UnPossess();

        RestartPlayer(Controller);
    }
}

void ASGameModeBase::OnActorKilled(AActor* VictimActor,AActor* Killer)
{
    UE_LOG(LogTemp,Log,TEXT("OnActorKilled: Victim: %s,Killer: %s"),*GetNameSafe(VictimActor),*GetNameSafe(Killer));

	ASCharacter* Player=Cast<ASCharacter>(VictimActor);

    if(Player)
    {
        FTimerHandle TimerHandle_RespawnDelay;

        FTimerDelegate Delegate;
        Delegate.BindUFunction(this,"RespawnPlayerElapsed",Player->GetController());

        float RespawnDelay=2.0f;
        GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay,Delegate,RespawnDelay,false);
    }

   APawn* KillerPawn=Cast<APawn>(Killer);

   if(KillerPawn)
   {

    ASPlayerState* PS=KillerPawn->GetPlayerState<ASPlayerState>();

    if(PS)
    {
        PS->AddCredits(CreditsPerKill);
    }

   }
}

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{

    ASPlayerState* PS=NewPlayer->GetPlayerState<ASPlayerState>();

    if(PS)
    {
        PS->LoadPlayerState(CurrentSaveGame);
    }

    Super::HandleStartingNewPlayer_Implementation(NewPlayer);
    
}

void ASGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadID,FVector SpawnLocation)
{
    LogOnScreen(this,"Finished Loading.",FColor::Green);

    UAssetManager* Manager=UAssetManager::GetIfInitialized();

    if(Manager)
    {
        USMonsterData* MonsterData=Cast<USMonsterData>(Manager->GetPrimaryAssetObject(LoadID));
        
        if(MonsterData)
        {
            AActor* NewBot=GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass,SpawnLocation,FRotator::ZeroRotator);

            if(NewBot)
            {
                LogOnScreen(this,FString::Printf(TEXT("Spawn Enemy : %s (%s)"),*GetNameSafe(NewBot),*GetNameSafe(MonsterData)));

                USActionComponent* ActionComp=Cast<USActionComponent>(NewBot->GetComponentByClass(USActionComponent::StaticClass()));

                if(ActionComp)
                {
                    for(TSubclassOf<USAction> ActionClass : MonsterData->Actions)
                    {
                        ActionComp->AddAction(NewBot,ActionClass);
                    }
                }
            }

        }
    }
	
}
