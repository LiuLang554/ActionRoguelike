// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SAnimInstance.generated.h"

class USActionComponent;

UCLASS()
class ACTIONROGUELIKE_API USAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
	protected:

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Animal")
	bool bIsStunned;

	void NativeInitializeAnimation()override;

	void NativeUpdateAnimation(float DeltaSeconds)override;

	UPROPERTY(BlueprintReadOnly,Category="Animal")
	TObjectPtr<USActionComponent> ActionComp;

};
