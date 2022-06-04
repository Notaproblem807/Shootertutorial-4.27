// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAniminstance.h"
#include "Shootercharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UShooterAniminstance::UShooterAniminstance() {
	speed = 0.f;
}

void UShooterAniminstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	APawn* Pawn = TryGetPawnOwner();
	Shootercharref = Cast<AShootercharacter>(Pawn);
}

void UShooterAniminstance::UpdatecAnimation(float Deltatime)
{
	if (Shootercharref != nullptr) {
		APawn* Pawn = TryGetPawnOwner();
		Shootercharref = Cast<AShootercharacter>(Pawn);
	}
	if (Shootercharref) {
		FVector Velocity = Shootercharref->GetVelocity();
		// set to zero for the only movement not the flying or falling
		Velocity.Z = 0.f;
		speed = Velocity.Size();
		//checking for jumping
		bisinair = Shootercharref->GetCharacterMovement()->IsFalling();
		//checking that character is not idle
		if (Shootercharref->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f) {
			//UE_LOG(LogTemp, Warning, TEXT("acceleration %s") ,*(Shootercharref->GetCharacterMovement()->GetCurrentAcceleration().ToString()));
			bisacelerating = true;
		}
		else {
			bisacelerating = false;
		}
		//character rotation from 0 to 180 base aimrotation
		FRotator moveyawrotation = Shootercharref->GetBaseAimRotation();
		FString moveyawmessage = FString::Printf(TEXT("Base Aim Rotation: %f"), moveyawrotation.Yaw);

		//charcter aix rotation from velocity they move
		FRotator velocityrotation = UKismetMathLibrary::MakeRotFromX(Shootercharref->GetVelocity());
		FString velocityrotamessage = FString::Printf(TEXT("VElocity rotat message:%f"),velocityrotation.Yaw);

		//normalize difference two roatation
		Movementoffsetyaw=UKismetMathLibrary::NormalizedDeltaRotator(velocityrotation, moveyawrotation).Yaw;

		if (Shootercharref->GetVelocity().Size() > 0.f) {
			lastmovementoffsetyaw = Movementoffsetyaw;
		}
		FString offsetyaw = FString::Printf(TEXT("movement off set:%f"), Movementoffsetyaw);
		if (GEngine) {
			//GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Red, moveyawmessage);
			//GEngine->AddOnScreenDebugMessage(2, 0.f, FColor::Green, velocityrotamessage);
			//GEngine->AddOnScreenDebugMessage(3, 0.f, FColor::Purple,offsetyaw);

		}
		//proerty to change the hip fire to aiming fire function
		banimaiming = Shootercharref->baimZoom;
	}
}
