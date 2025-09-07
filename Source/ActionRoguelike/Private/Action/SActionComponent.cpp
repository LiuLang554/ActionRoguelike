// Fill out your copyright notice in the Description page of Project Settings.


#include "Action/SActionComponent.h"
#include "Action/SAction.h"
#include "../ActionRoguelike.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

//统计信息宏
DECLARE_CYCLE_STAT(TEXT("StartActionByName"),STAT_StartActionByName,STATGROUP_STANFORD);

USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}



void USActionComponent::BeginPlay()
{
	Super::BeginPlay();

	if(GetOwner()->HasAuthority())
	{
		for(TSubclassOf<USAction> ActionClass : DefaultActions)
	{
		AddAction(GetOwner(),ActionClass);
	}
	
	}
}



void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//FString DebugMsg=GetNameSafe(GetOwner())+":"+ActiveGameplayTags.ToStringSimple();
	
	//GEngine->AddOnScreenDebugMessage(-1,0.0f,FColor::White,DebugMsg);

	//for(USAction* Action : Actions)
	//{
		//FColor TextColor=Action->IsRunning()?FColor::Blue:FColor::White;

		//FString ActionMsg=FString::Printf(TEXT("[%s] Action: %s"),*GetNameSafe(GetOwner()),*GetNameSafe(Action));

		//LogOnScreen(this,ActionMsg,TextColor,0.0f);
	//}
}

void USActionComponent::AddAction(AActor* Instigator,TSubclassOf<USAction> ActionClass)
{
	if(!ensure(ActionClass))
	{
		return;
	}

	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp,Warning,TEXT("Client attempting to AddAction. [Class: %s]"),*GetNameSafe(ActionClass));

		return;
	}

	USAction* NewAction=NewObject<USAction>(GetOwner(),ActionClass);

	if(ensure(NewAction))
	{
		NewAction->Initialize(this);

		Actions.Add(NewAction);

		if(NewAction->bAutoStart&&ensure(NewAction->CanStart(Instigator)))
		{
			NewAction->StartAction(Instigator);
		}
	}

}

void USActionComponent::RemoveAction(USAction* ActionToRemove)
{
	if(!ensure(ActionToRemove && !ActionToRemove->IsRunning()))
	{
		return;
	}

	Actions.Remove(ActionToRemove);

}

bool USActionComponent::StartActionByName(AActor* Instigator,FName ActionsName)
{
	SCOPE_CYCLE_COUNTER(STAT_StartActionByName);

	for(USAction* Action : Actions)
	{
		if(Action && Action->ActionName == ActionsName)
		{
			if(!Action->CanStart(Instigator))
			{
				FString FailedMsg=FString::Printf(TEXT("Failed to Run : %s"),*ActionsName.ToString());
				GEngine->AddOnScreenDebugMessage(-1,2.0f,FColor::Red,FailedMsg);
				continue;
			}

			//是客户端？
			if(!GetOwner()->HasAuthority())
			{
				ServerStartAction(Instigator,ActionsName);
			}

			//书签
			TRACE_BOOKMARK(TEXT("StartAction::%s"),*GetNameSafe(Action));

			Action->StartAction(Instigator);
			return true;
			
		}
	}

	return false;
}

bool USActionComponent::StopActionByName(AActor* Instigator,FName ActionsName)
{
	for(USAction* Action : Actions)
	{
		if(Action && Action->ActionName == ActionsName)
		{
			if(Action->IsRunning())
			{
				//Is Client?
			if(!GetOwner()->HasAuthority())
			{
				ServerStopAction(Instigator,ActionsName);
			}

			Action->StopAction(Instigator);
			    
			return true;
			}
		}
	}

	return false;
}

void USActionComponent::ServerStartAction_Implementation(AActor* Instigator,FName ActionsName)
{
	StartActionByName(Instigator,ActionsName);
}

void USActionComponent::ServerStopAction_Implementation(AActor* Instigator,FName ActionsName)
{
	StopActionByName(Instigator,ActionsName);
}

USAction* USActionComponent::GetAction(TSubclassOf<USAction> ActionClass) const
{
	for(USAction* Action : Actions)
	{
		if(Action&&Action->IsA(ActionClass))
		{
			return Action;
		}
	}

	return nullptr;
}

void USActionComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USActionComponent,Actions);
}

bool USActionComponent::ReplicateSubobjects(class UActorChannel* Channel,class FOutBunch* bunch,FReplicationFlags* RepFlags)
{
	bool WroteSomeThing=Super::ReplicateSubobjects(Channel,bunch,RepFlags);

	for(USAction* Action : Actions)
	{
		if(Action)
		{
			WroteSomeThing |= Channel->ReplicateSubobject(Action,*bunch,*RepFlags);
		}
	}

	return WroteSomeThing;
}

void USActionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	TArray<USAction*> ActionCopy=Actions;

	for(USAction* Action : ActionCopy)
	{
		if(Action && Action->IsRunning())
		{
			Action->StopAction(GetOwner());
		}
	}

	Super::EndPlay(EndPlayReason);
}
