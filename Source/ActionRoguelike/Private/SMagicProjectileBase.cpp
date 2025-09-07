// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectileBase.h"
#include "Components/SphereComponent.h"
#include "SAttributeComponent.h"
#include "SGameplayFunctionLibrary.h"
#include "Action/SActionComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SActionEffect.h"

ASMagicProjectileBase::ASMagicProjectileBase()
{

	SphereComp->SetSphereRadius(20.0f);
	SphereComp->OnComponentBeginOverlap.AddDynamic(this,&ASMagicProjectileBase::OnActorOverlap);

	DamageAmount=20.0f;

	InitialLifeSpan=10.0f;

}

void ASMagicProjectileBase::OnActorOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
	if(OtherActor&&OtherActor!=GetInstigator())
	{
		//USAttributeComponent* AttributeComp=Cast<USAttributeComponent>(OtherActor->GetComponentByClass(USAttributeComponent::StaticClass()));
		//if(AttributeComp)
		//{
			//AttributeComp->ApplyHealthChanger(GetInstigator(),-DamageAmount);

			//Destroy();
		//}

		USActionComponent* ActionComp=Cast<USActionComponent>(OtherActor->GetComponentByClass(USActionComponent::StaticClass()));

		if(ActionComp&&ActionComp->ActiveGameplayTags.HasTag(ParryTag))
		{
			MoveComp->Velocity=-MoveComp->Velocity;

			SetInstigator(Cast<APawn>(OtherActor));

			return;
		}

		if(USGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(),OtherActor,DamageAmount,SweepResult))
		{
			Explode();

			if(ActionComp && HasAuthority())
			{
				ActionComp->AddAction(GetInstigator(),BurningActionClass);
			}
		}
	}
}
