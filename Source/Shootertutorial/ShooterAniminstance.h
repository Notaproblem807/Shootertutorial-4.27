// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAniminstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERTUTORIAL_API UShooterAniminstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UShooterAniminstance();

	virtual void NativeInitializeAnimation() override;
	
	UFUNCTION(BlueprintCallable,Category="Animinstance|animfunc")
	void UpdatecAnimation(float Deltatime);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animinstace|stats")
		float speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animinstace|stats")
		bool bisinair;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animinstace|stats")
		bool bisacelerating;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animinstace|stats")
	class AShootercharacter* Shootercharref;

	//movement yaw off to set strafing animation value in blend space
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animinstace|blendspacestrafing")
	float Movementoffsetyaw;

	//to store movement offset yaw before velocity goes to zero for blendspace stop animation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animinstace|blendspacestrafing")
	float lastmovementoffsetyaw;

    // property to change hip to fire state in the animation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hip to fire|status")
	bool banimaiming;
};
