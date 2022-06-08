// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Baseitem.h"
#include "Ammotype.h"
#include "BaseWeapon.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EWeaponType :uint8 {
	EWT_Submachinegun UMETA(DisplayName="Submachinegun"),
	EWT_ArGun       UMETA(DisplayName="Argun"),
	EWT_Default    UMETA(DisplayName="default")
};



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
	

	//capacity of gun to hold and bullet and increse while reloading
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maxammocancarry", meta = (AllowPrivateAccess = "true"))
	int32 Magammocapacity;

	void Reloadammocount(int32 Amount);
	//......ends capacity

	//@WEapontype enum specs
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "weapontype", meta = (AllowPrivateAccess = "true"))
	EWeaponType Eweapontype;

public:
	FORCEINLINE EWeaponType getweapontype() const { return Eweapontype; }
	//@weapontype enum ends here

	//@Ammotype weapon holds
private:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="weapon|Ammotype", meta = (AllowPrivateAccess = "true"))
	EAmmotype ammotype;

public:
	FORCEINLINE EAmmotype getammotype() const { return ammotype; }

	//@Ammotype ends

	//montage section for diffeernet weapon
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="montagesectionname")
	FName Montagesectionname;
	//..........end


	//@Grab and replace the clip in gun for reloading animation take place
private:
	FName Clipbonename;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "grabandclip", meta = (AllowPrivateAccess = "true"))
	bool bMovingclip;

public:
	FORCEINLINE FName Getclipbonename()const { return Clipbonename; }
	FORCEINLINE void Setmovingclip(bool move) { bMovingclip = move; }

	//@Grab and replace ends


};
