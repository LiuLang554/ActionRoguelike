// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_HealthPotion.h"
#include "Components/StaticMeshComponent.h"
#include "SAttributeComponent.h"
#include "SPlayerState.h"

#define LOCTEXT_NAMESPACE "InteractableActors"

ASPowerup_HealthPotion::ASPowerup_HealthPotion()
{
	CreditCost=50;
}

void ASPowerup_HealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	if(!ensure(InstigatorPawn))
    {
        return;
    }

    USAttributeComponent* AttributeComp=Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));

    if(ensure(AttributeComp)&&!AttributeComp->IsFullHealth())
    {
        if(ASPlayerState* PS=InstigatorPawn->GetPlayerState<ASPlayerState>())
        {
            if(PS->RemoveCredits(CreditCost)&&AttributeComp->ApplyHealthChanger(this,AttributeComp->GetHealthMax()))
            {
                HideAndCooldownPowerup();
            }
        }
    }
}

FText ASPowerup_HealthPotion::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	USAttributeComponent* AttributeComp=Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));

    if(AttributeComp && AttributeComp->IsFullHealth())
    {
        return LOCTEXT("HealthPotion_FullHealthWarning","Already at full Health.");
    }

    return FText::Format(LOCTEXT("HealthPotion_InteractMessage","Cost {0} Credits. Restores health to maximum."),CreditCost);
}

#undef LOCTEXT_NAMESPACE