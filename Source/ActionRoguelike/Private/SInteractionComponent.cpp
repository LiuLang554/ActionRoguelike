// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"
#include "SGameplayInterface.h"
#include "SWorldUserWidget.h"

static TAutoConsoleVariable<bool> CVerDebugDrawInteraction(TEXT("AC.InteractionDebugDraw"),false,TEXT("Enable Debug Lines for Interact Component."),ECVF_Cheat);

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	CollisionChannel=ECC_WorldDynamic;

	TraceDistance=500.0f;

	TraceRadius=30.0f;
}

void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USInteractionComponent::TickComponent(float DeltaTime,ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime,TickType,ThisTickFunction);

	APawn* MyPawn=Cast<APawn>(GetOwner());

	if(MyPawn->IsLocallyControlled())
	{
		FindBestInteractable();
	}
}

void USInteractionComponent::FindBestInteractable()
{
	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(CollisionChannel);

	//FHitResult OnHit;

	AActor* MyOwner=GetOwner();
	FVector EyesLocation;
	FRotator EyesRotation;
	MyOwner->GetActorEyesViewPoint(EyesLocation,EyesRotation);
	FVector End=EyesLocation+(EyesRotation.Vector()*TraceDistance);

	//bool bBlockingHit=GetWorld()->LineTraceSingleByObjectType(OnHit,EyesLocation,End,QueryParams);

	TArray<FHitResult>Hits;

	FCollisionShape Shape;
	Shape.SetSphere(TraceRadius);

	bool bBlockingHit=GetWorld()->SweepMultiByObjectType(Hits,EyesLocation,End,FQuat::Identity,QueryParams,Shape);

	FColor LineColor= bBlockingHit?FColor::Green:FColor::Red;

	FocusedActor=nullptr;

	for(FHitResult Hit : Hits)
	{
		
	if(CVerDebugDrawInteraction.GetValueOnGameThread())
	{
		DrawDebugSphere(GetWorld(),Hit.ImpactPoint,TraceRadius,32,LineColor,false,3.0f);
	}

	AActor* HitActor=Hit.GetActor();

	if(HitActor)
	{
		if(HitActor->Implements<USGameplayInterface>())
		{
			FocusedActor=HitActor;
			
			break;
		}
	}
	}

	if(FocusedActor)
	{
		if(DefaultWidgetInstance==nullptr&&ensure(DefaultWidgetClass))
		{
			DefaultWidgetInstance=CreateWidget<USWorldUserWidget>(GetWorld(),DefaultWidgetClass);
		}
		
		if(DefaultWidgetInstance)
		{
			DefaultWidgetInstance->AttachedActor=FocusedActor;

			if(!DefaultWidgetInstance->IsInViewport())
			{
				DefaultWidgetInstance->AddToViewport();
			}
		}
	}
	else
	{
		if(DefaultWidgetInstance)
		{
			DefaultWidgetInstance->RemoveFromParent();
		}
	}

	if(CVerDebugDrawInteraction.GetValueOnGameThread())
	{
		DrawDebugLine(GetWorld(),EyesLocation,End,LineColor,false,3.0f);
	}
}

void USInteractionComponent::PrimaryInteract()
{
	ServerInteract(FocusedActor);
}

void USInteractionComponent::ServerInteract_Implementation(AActor* InFocus)
{
	if(InFocus==nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1,1.0f,FColor::Red,"No Focus Actor to Interact");

		return;
	}

	APawn* PawnOwner=Cast<APawn>(GetOwner());

	ISGameplayInterface::Execute_Interact(InFocus,PawnOwner);
}