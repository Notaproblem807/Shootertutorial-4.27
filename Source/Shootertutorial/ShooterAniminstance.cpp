// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAniminstance.h"
#include "Shootercharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

UShooterAniminstance::UShooterAniminstance() {
	speed = 0.f;
	//@turn in place
	Rotationyaw = 0.f;
	Rotationyawlastframe = 0.f;
	Characteryaw = 0.f;
	characteryawlastframe = 0.f;
	Rootoffsetyaw = 0.f;
	bReloading = false;
	//@Lean
	LeanCharacteryawLastframe = FRotator(0.f);
	LeanCharacteryaw = FRotator(0.f);
	YawDelta = 0.f;
	//@BlendWeights
	bTurninplace = false;
	BlendWeights = 1.0f;
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
		//@Crouch value 
		bCrouching = Shootercharref->GetCrouchingState();
		//@Sprint value
		bSprinting = Shootercharref->bSprint;
		Forward = Shootercharref->Forward;
		Right = Shootercharref->Right;
		//Pitch value 
		Pitch = Shootercharref->GetBaseAimRotation().Pitch;
		bReloading = Shootercharref->getCombatfirestate() == ECombatfirestate::ECFS_reloadstate;
		FVector Velocity = Shootercharref->GetVelocity();
		// set to zero for the only movement not the flying or falling
		Velocity.Z = 0.f;
		speed = Velocity.Size();
		if (speed == 0.f) {
			//set Sprinting false
			Shootercharref->bSprint = false;
		}
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

		//@ enum spexifiers Estateofffire
		if (bReloading) {
			Stateinenum = EStateoffire::ESF_Reload;
		}
		else if (bisinair) {
			Stateinenum = EStateoffire::ESF_Isinair;
		}
		else if (Shootercharref->baimZoom) {
			Stateinenum = EStateoffire::ESF_Aiming;
		}
		else {
			Stateinenum = EStateoffire::ESF_Hip;
		}
		//to turn in place 
		Turninplace();

		//lean while running
		Lean(Deltatime);
	}
}

void UShooterAniminstance::Turninplace()
{
	if (Shootercharref == nullptr)return;
	if (speed > 0) {
		Rootoffsetyaw = 0.f;
		Characteryaw = Shootercharref->GetActorRotation().Yaw;
		characteryawlastframe = Characteryaw;
		Rotationyaw = 0.f;
		Rotationyawlastframe = 0.f;
	}
	else {
		characteryawlastframe = Characteryaw;
		Characteryaw = Shootercharref->GetActorRotation().Yaw;
		//if (characteryawlastframe == Characteryaw) {

		//}
		//else {
			//UE_LOG(LogTemp, Warning, TEXT("characteryawlast %f"), characteryawlastframe);
		//}
		const float yawoffset{ Characteryaw - characteryawlastframe };
		//nomalise between the ange 180 ,-180
		Rootoffsetyaw -= yawoffset;
		//Rootoffsetyaw = UKismetMathLibrary::NormalizeAxis(Rootoffsetyaw-yawoffset);
		//Rootoffsetyaw=UKismetMathLibrary::NormalizeAxis(Rootoffsetyaw - yawoffset);
		FString offsetyaw = FString::Printf(TEXT("characteryaaw:%f"), Characteryaw);
		FString offsete = FString::Printf(TEXT("characteryaawlast:%f"), characteryawlastframe);
		FString offsetr = FString::Printf(TEXT("rootoff:%f"), Rootoffsetyaw);
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Red, offsetyaw);
			GEngine->AddOnScreenDebugMessage(2, 0.f, FColor::Green, offsete);
			GEngine->AddOnScreenDebugMessage(3, 0.f, FColor::Purple, offsetr);

		}
		//turn in 90 while moving turn the body 
		const float trackerval{ GetCurveValue(TEXT("trackerval")) };
		//if trackerval > 0 not  animation not running turn in anim
		if (trackerval > 0) {
			bTurninplace = true;
			Rotationyawlastframe = Rotationyaw;
			Rotationyaw = GetCurveValue(TEXT("Rotationturn"));
			//UE_LOG(LogTemp, Warning, TEXT("Rotationyaw%f"), Rotationyaw);
			//UE_LOG(LogTemp, Warning, TEXT("Rotationyawlast%f"), Rotationyawlastframe);
			const float Rotationyawoffset{ Rotationyaw - Rotationyawlastframe };
			//UE_LOG(LogTemp, Warning, TEXT("Rotationyaw%f"), Rotationyaw);
			//UE_LOG(LogTemp, Warning, TEXT("Rotationyawoffest%f"), Rotationyawoffset);
			Rootoffsetyaw > 0.f ? Rootoffsetyaw -= Rotationyawoffset : Rootoffsetyaw = Rootoffsetyaw - FMath::Lerp<float, float>(0, Rootoffsetyaw - Rotationyawlastframe, 1.0f);
			const float ABSRootoffsetyaw{ FMath::Abs(Rootoffsetyaw) };
			if (ABSRootoffsetyaw > 90.f) {
				const float yawexcess{ ABSRootoffsetyaw - 90.f };
				Rootoffsetyaw > 0.f ? Rootoffsetyaw -= yawexcess : Rootoffsetyaw += yawexcess;
			}
			UE_LOG(LogTemp, Warning, TEXT("finRotation%f"), Rootoffsetyaw);
		}
		if (bTurninplace) {
			if (bCrouching) {
				BlendWeights = 0.1f;
			}
			else if (bisinair) {
				BlendWeights = 0.3f;
			}
			else {
				BlendWeights = 0.2f;
			}
		}
		else {
			BlendWeights = 1.f;
		}
	}
}

void UShooterAniminstance::Lean(float Deltatime)
{
	if (Shootercharref != nullptr) {
		LeanCharacteryawLastframe = LeanCharacteryaw;
		LeanCharacteryaw = Shootercharref->GetActorRotation();
		const FRotator DeltaRot=UKismetMathLibrary::NormalizedDeltaRotator(LeanCharacteryaw, LeanCharacteryawLastframe);
		const float Target{ DeltaRot.Yaw / Deltatime };
		float Rinterp = FMath::FInterpTo(YawDelta, Target, Deltatime, 6.f);
		YawDelta = FMath::Clamp(Rinterp, -90.f, 90.f);
	}
}
