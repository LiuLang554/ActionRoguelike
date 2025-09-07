// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SInteractionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SAttributeComponent.h"
#include "SDashProjectile.h"
#include "SProjectileBase.h"
#include "Action/SActionComponent.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp=CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->bUsePawnControlRotation=true;
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp=CreateDefaultSubobject<UCameraComponent>("Camera");
	CameraComp->SetupAttachment(SpringArmComp);

	ActionComp=CreateDefaultSubobject<USActionComponent>("Action");

	GetCharacterMovement()->bOrientRotationToMovement=true;

	bUseControllerRotationYaw=false;

	InteractionComp=CreateDefaultSubobject<USInteractionComponent>("InterAction");

	AttributeComp=CreateDefaultSubobject<USAttributeComponent>("Attribute");

	TimeToHitParamName="TimeOnHit";

}
// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("Forward",this,&ASCharacter::MoveInput);

	PlayerInputComponent->BindAxis("Right",this,&ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn",this,&APawn::AddControllerYawInput);

	PlayerInputComponent->BindAxis("LookUp",this,&APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump",EInputEvent::IE_Pressed,this,&ACharacter::Jump);

	PlayerInputComponent->BindAction("Fire",EInputEvent::IE_Pressed,this,&ASCharacter::PrimaryAttack);

	PlayerInputComponent->BindAction("Interaction",EInputEvent::IE_Pressed,this,&ASCharacter::PrimaryInteract);

	PlayerInputComponent->BindAction("Skill_BlackHole",EInputEvent::IE_Pressed,this,&ASCharacter::BlackHoleAttack);

	PlayerInputComponent->BindAction("Skill_DashProjectile",EInputEvent::IE_Pressed,this,&ASCharacter::Dash);

	PlayerInputComponent->BindAction("Sprint",EInputEvent::IE_Pressed,this,&ASCharacter::SprintStart);

	PlayerInputComponent->BindAction("Sprint",EInputEvent::IE_Released,this,&ASCharacter::SprintStop);

}

void ASCharacter::MoveInput(float Target)
{
	FRotator ControlRot=GetControlRotation();
	ControlRot.Pitch=0.0f;
	ControlRot.Roll=0.0f;

	AddMovementInput(ControlRot.Vector(),Target);
}

void ASCharacter::MoveRight(float Target)
{
	FRotator ControlRot=GetControlRotation();
	ControlRot.Pitch=0.0f;
	ControlRot.Roll=0.0f;

	FVector RightVector=FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
	AddMovementInput(RightVector,Target);
}

void ASCharacter::PrimaryAttack()
{
	ActionComp->StartActionByName(this,"PrimaryAttack");
}

void ASCharacter::PrimaryInteract()
{
	if(InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}

void ASCharacter::BlackHoleAttack()
{
	ActionComp->StartActionByName(this,"BlackHole");
}

void ASCharacter::Dash()
{
	ActionComp->StartActionByName(this,"Dash");
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor,USAttributeComponent* OtherComp,float NewHealth,float Delta)
{
	if(Delta<0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName,GetWorld()->TimeSeconds);

		float RageDelta=FMath::Abs(Delta);

		AttributeComp->ApplyRage(InstigatorActor,RageDelta);
	}

	if(NewHealth<=0.0f&&Delta<0.0f)
	{
		APlayerController* PC=Cast<APlayerController>(GetController());
		DisableInput(PC);

		SetLifeSpan(5.0f);
	}
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this,&ASCharacter::OnHealthChanged);
}

void ASCharacter::HealthSelf(float Amount)
{
	AttributeComp->ApplyHealthChanger(this,Amount);
}

FVector ASCharacter::GetPawnViewLocation() const 
{
	return CameraComp->GetComponentLocation();
}

void ASCharacter::SprintStart()
{
	ActionComp->StartActionByName(this,"Sprint");
}

void ASCharacter::SprintStop()
{
	ActionComp->StopActionByName(this,"Sprint");
}