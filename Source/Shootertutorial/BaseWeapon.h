// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Baseitem.h"
#include "BaseWeapon.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERTUTORIAL_API ABaseWeapon : public ABaseitem
{
	GENERATED_BODY()
public:
	ABaseWeapon();
	//tis is to making falling impulse of gun slightly forward
protected:
	//reset the state
	void weaponfallingdefault();
	virtual void Tick(float DeltaSeconds) override;
private:
	FTimerHandle Fallingresponse;

	float fallresettime;

	bool weaponfalling;
    
	FRotator Meshrotaori;
public:
	void throwweaponslight();

	//........................................................


	//@Ammo counting procedure and shooing stop
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammoavailable", meta = (AllowPrivateAccess = "true"))
	int32 Ammoavailable;

public:
	FORCEINLINE int32 getAmmoavaliable() const { return Ammoavailable;}
	 
	void Decrementammoavaliable();


	//@Ammo counting ends here

};
