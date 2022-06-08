// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"


ABaseWeapon::ABaseWeapon():weaponfalling(false),fallresettime(0.2f) 
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//ammo gun has initially
	Ammoavailable = 0;
	//ammo maximun can been have
	Magammocapacity = 30;
	//initialize the weapon type
	Eweapontype = EWeaponType::EWT_Submachinegun;
	//initialize Ammotype
	ammotype = EAmmotype::EAT_9mm;
	//montage name for reloading section
	Montagesectionname = TEXT("Reloadsmg");

	//@Grab and replace the clip in gun for reloading animation take place
	Clipbonename = TEXT("smg_clip");
}


void ABaseWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//UE_LOG(LogTemp, Warning, TEXT("rotation of WHEN gun:%s"), *getMeshgame()->GetComponentRotation().ToString());
	if (getItemstate() == EItemState::EIS_Falling && weaponfalling == true) {
		FRotator Meshrota = FRotator(0.f, getMeshgame()->GetComponentRotation().Yaw, 0.f);
		//UE_LOG(LogTemp, Warning, TEXT("rotation of WHEN FALL gun:%s"), *Meshrota.ToString());
		getMeshgame()->SetWorldRotation(Meshrota, false, nullptr, ETeleportType::TeleportPhysics);
	}

}




void ABaseWeapon::weaponfallingdefault()
{
	getMeshgame()->SetSimulatePhysics(false);
	getMeshgame()->SetEnableGravity(false);
	weaponfalling = false;
	UE_LOG(LogTemp,Warning,TEXT("falling the weapon"))
	SetItemState(EItemState::EIS_Pickup);

}

void ABaseWeapon::throwweaponslight()
{
	Meshrotaori = FRotator(0.f, getMeshgame()->GetComponentRotation().Yaw, 0.f);
	getMeshgame()->SetWorldRotation(Meshrotaori, false, nullptr, ETeleportType::TeleportPhysics);
	const FVector MeshForward{ getMeshgame()->GetForwardVector() };
	const FVector MeshRight{ getMeshgame()->GetRightVector() };
	getMeshgame()->SetSimulatePhysics(true);
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(20.f, MeshForward);
	UE_LOG(LogTemp, Warning, TEXT("rotation of gun:%s"), *ImpulseDirection.ToString());
	//ImpulseDirection = ImpulseDirection.RotateAngleAxis(-30.f, FVector(0.f, 0.f, 1.f));
	ImpulseDirection *= 10'000;
	//set item not turning on simulating physics
	
	getMeshgame()->AddImpulse(ImpulseDirection);
	weaponfalling = true;
	GetWorldTimerManager().SetTimer(Fallingresponse, this, &ABaseWeapon::weaponfallingdefault,fallresettime);
}



//@Ammo count
void ABaseWeapon::Decrementammoavaliable()
{
	if (Ammoavailable <= 0) {
		Ammoavailable = 0;
	}
	else {
		--Ammoavailable;
	}
}

//@reloadfinish in shooter to increase the ammo count
void ABaseWeapon::Reloadammocount(int32 Amount)
{
	checkf(Ammoavailable + Amount <= Magammocapacity, TEXT("Your are existing capacity of holding Ammo"));
	Ammoavailable += Amount;
}
