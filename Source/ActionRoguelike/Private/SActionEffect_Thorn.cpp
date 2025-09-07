// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionEffect_Thorn.h"
#include "SGameplayFunctionLibrary.h"
#include "SAttributeComponent.h"

USActionEffect_Thorn::USActionEffect_Thorn()
{
	ReflectFraction=0.2f;

    Duration=0.0f;

    Period=0.0f;
}

void USActionEffect_Thorn::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

    USAttributeComponent* Attribute=USAttributeComponent::GetAttribute(GetOwningComponent()->GetOwner());

    if(Attribute)
    {
        Attribute->OnHealthChanged.AddDynamic(this,&USActionEffect_Thorn::OnHealthChanged);
    }
}

void USActionEffect_Thorn::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);

    USAttributeComponent* Attribute=USAttributeComponent::GetAttribute(GetOwningComponent()->GetOwner());

    if(Attribute)
    {
        Attribute->OnHealthChanged.RemoveDynamic(this,&USActionEffect_Thorn::OnHealthChanged);
    }
}

void USActionEffect_Thorn::OnHealthChanged(AActor* InstigatorActor,USAttributeComponent* OwningComp,float NewHealth,float Delta)
{
	AActor* OwningActor=GetOwningComponent()->GetOwner();

    if(Delta<0.0f && OwningActor!=InstigatorActor)
    {
        int32 ReflectedAmount=FMath::RoundToInt(Delta * ReflectFraction);

        if(ReflectedAmount==0)
        {
            return;
        }

        ReflectedAmount=FMath::Abs(ReflectedAmount);

        USGameplayFunctionLibrary::ApplyDamage(OwningActor,InstigatorActor,ReflectedAmount);
    }
}