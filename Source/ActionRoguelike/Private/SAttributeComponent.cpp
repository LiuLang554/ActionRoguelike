// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<float> CVerDamageMultiplier(TEXT("AC.DamageMultiplier"),1.0f,TEXT("Global Damage Modifier for Attribute Component."),ECVF_Cheat);

USAttributeComponent::USAttributeComponent()
{
	Health=100.0f;
	HealthMax=Health;

	Rage=0;
	RageMax=100;

	SetIsReplicatedByDefault(true);
}

bool USAttributeComponent::IsAlive() const
{
	return Health>0.0f;
}

bool USAttributeComponent::IsFullHealth() const
{
	return Health==HealthMax;
}


float USAttributeComponent::GetHealthMax() const
{
	return HealthMax;
}

bool USAttributeComponent::ApplyHealthChanger(AActor* InstigatorActor,float Delta)
{
	if(!GetOwner()->CanBeDamaged()&&Delta<0.0f)
	{
		return false;
	}
	
	if(Delta<0.0f)
	{
		float DamageMultiplier=CVerDamageMultiplier.GetValueOnGameThread();

		Delta *= DamageMultiplier;
	}
	
	float OldHealth=Health;

	float NewHealth=FMath::Clamp(Health+Delta,0.0f,HealthMax);

	float ActualDelta=NewHealth-OldHealth;

	if(GetOwner()->HasAuthority())
	{
		Health=NewHealth;

		if(ActualDelta!=0.0f)
	{
		MuliticastHealthChanged(InstigatorActor,Health,ActualDelta);
	}

	//Died
	if(ActualDelta<0.0f&&Health==0.0f)
	{
		ASGameModeBase* GM=GetWorld()->GetAuthGameMode<ASGameModeBase>();

		if(GM)
		{
			GM->OnActorKilled(GetOwner(),InstigatorActor);
		}
	}

	}

	return ActualDelta != 0;
}

USAttributeComponent* USAttributeComponent::GetAttribute(AActor* FromActor)
{
	if(FromActor)
	{
		return Cast<USAttributeComponent>(FromActor->GetComponentByClass(USAttributeComponent::StaticClass()));
	}

	return nullptr;
}

bool USAttributeComponent::IsActorAlive(AActor* FromActor)
{
	USAttributeComponent* AttributeComp=GetAttribute(FromActor);

	if(AttributeComp)
	{
		return AttributeComp->IsAlive();
	}

	return false;
}

bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChanger(InstigatorActor,-GetHealthMax());
}

float USAttributeComponent::GetHealth() const
{
	return Health;
}

void USAttributeComponent::MuliticastHealthChanged_Implementation(AActor* InstigatorActor,float NewHealth,float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor,this,NewHealth,Delta);
}

void USAttributeComponent::MuliticastRageChanged_Implementation(AActor* InstigatorActor,float NewRage,float Delta)
{
	OnRageChanged.Broadcast(InstigatorActor,this,NewRage,Delta);
}

float USAttributeComponent::GetRage() const
{
	return Rage;
}

bool USAttributeComponent::ApplyRage(AActor* InstigatorActor,float Delta)
{
	const float OldRage=Rage;

	Rage=FMath::Clamp(Rage+Delta,0.0f,RageMax);

	float ActualDelta=Rage-OldRage;

	if(!FMath::IsNearlyZero(ActualDelta))
	{
		MuliticastRageChanged(InstigatorActor,Rage,ActualDelta);

		return true;
	}

	return false;
}

void USAttributeComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAttributeComponent,Health);

	DOREPLIFETIME(USAttributeComponent,HealthMax);

	DOREPLIFETIME(USAttributeComponent,Rage);

	DOREPLIFETIME(USAttributeComponent,RageMax);

	//DOREPLIFETIME_CONDITION(USAttributeComponent,HealthMax,COND_InitialOnly);
}