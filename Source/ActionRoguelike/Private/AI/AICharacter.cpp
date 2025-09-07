// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AICharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "SAttributeComponent.h"
#include "BrainComponent.h"
#include "SWorldUserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Action/SActionComponent.h"

// Sets default values
AAICharacter::AAICharacter()
{
	PawnSensingComp=CreateDefaultSubobject<UPawnSensingComponent>("PawnSensing");

	AttributeComp=CreateDefaultSubobject<USAttributeComponent>("Attribute");

	ActionComp=CreateDefaultSubobject<USActionComponent>("ActionComp");

	AutoPossessAI=EAutoPossessAI::PlacedInWorldOrSpawned;

	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Ignore);

	GetMesh()->SetGenerateOverlapEvents(true);

	TimeToHitParamName="TimeToHit";
}

void AAICharacter::OnPawnSeen(APawn* Pawn)
{
	if(GetInstigator()!=Pawn)
	{
		SetTargetActor(Pawn);

		MuliticastPawnSeen();
	}

	//DrawDebugString(GetWorld(),GetActorLocation(),"PLAYER SPOTTED",nullptr,FColor::White,4.0f,true);
}

void AAICharacter::MuliticastPawnSeen_Implementation()
{
	USWorldUserWidget* NewWidget=CreateWidget<USWorldUserWidget>(GetWorld(),SpottedwidgetClass);

	if(NewWidget)
	{
		NewWidget->AttachedActor=this;

		NewWidget->AddToViewport(10);
	}
}

void AAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this,&AAICharacter::OnPawnSeen);

	AttributeComp->OnHealthChanged.AddDynamic(this,&AAICharacter::OnHealthChanged);
}

void AAICharacter::OnHealthChanged(AActor* InstigatorActor,USAttributeComponent* OtherComp,float NewHealth,float Delta)
{
	if(Delta<0.0f)
	{
		if(InstigatorActor!=this)
		{
			SetTargetActor(InstigatorActor);
		}

		if(ActiveWidget==nullptr)
		{
			ActiveWidget=CreateWidget<USWorldUserWidget>(GetWorld(),HealthBarWidgetClass);
		
			if(ActiveWidget)
			{
				ActiveWidget->AttachedActor=this;
				ActiveWidget->AddToViewport();
			}
		}
		
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName,GetWorld()->TimeSeconds);

		//Died
		if(NewHealth<=0.0f)
		{
			AAIController* AIC=Cast<AAIController>(GetController());
			if(AIC)
			{
				AIC->GetBrainComponent()->StopLogic("killed");
			}

			GetMesh()->SetAllBodiesSimulatePhysics(true);
			GetMesh()->SetCollisionProfileName("Ragdoll");

			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCharacterMovement()->DisableMovement();

			SetLifeSpan(10.0f);
		}
	}
}

void AAICharacter::SetTargetActor(AActor* NewTargetActor)
{
	AAIController* AIC=Cast<AAIController>(GetController());
	if(AIC)
	{
		AIC->GetBlackboardComponent()->SetValueAsObject("TargetActor",NewTargetActor);
	}
}



