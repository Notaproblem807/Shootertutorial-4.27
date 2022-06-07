// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Ammotype.h"
#include "Shootercharacter.generated.h"



UENUM(BlueprintType)
enum class ECombatfirestate :uint8 {
	ECFS_Unoccupied UMETA(DisplayName="unoccupiedstate"),
	ECFS_Firetimerinprogress UMETA(DisplayName="firetimeprogress"),//its the timer set to fire again bullet
	ECFS_reloadstate UMETA(DisplayName="reloadstate"),

	ECFS_Max UMETA(DisplayName="default")
};



UCLASS()
class SHOOTERTUTORIAL_API AShootercharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShootercharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpringArm", meta = (AllowPrivateAccess="true"))
	class USpringArmComponent* Springarm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

public:
	FORCEINLINE USpringArmComponent* Getspringarm() const { return Springarm; }
	FORCEINLINE UCameraComponent* getCamera() const { return Camera; }

	//character movement
	void Moveforward(float Value);
	void Moveright(float Value);
	//turning rate for normal
	float Baseturnrate;
	float Baselookuprate;
	//while not aiming and aiming turnrate and lookuprate for arrow keys
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="rate|turn")
	float hipturnrate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "rate|lookup")
	float hiplookrate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "rate|aimturn")
	float aimturnrate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "rate|aimlookup")
	float aimlookrate;
	//this is turnrate and lookuprate for mouse input 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "rate|mouseturn",meta=(ClampMin="0.0",ClampMax="1.0"),meta=(UIMin="0.0",UIMax="2.0"))
	float mousehipturnrate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "rate|mouseaimturn", meta = (ClampMin = "0.0", ClampMax = "1.0"), meta = (UIMin = "0.0", UIMax = "2.0"))
	float mouseaimturnrate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "rate|mouselookup", meta = (ClampMin = "0.0", ClampMax = "1.0"), meta = (UIMin = "0.0", UIMax = "2.0"))
	float mousehiplookrate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "rate|mouseaimlook", meta = (ClampMin = "0.0", ClampMax = "1.0"), meta = (UIMin = "0.0", UIMax = "2.0"))
	float mouseaimlookrate;
	//setting turnrate for the arrow keys
	void setarrowturnlookup();
	//func for mouse turnrate while aiming and not
	void mouseturn(float Value);
	void mouselookup(float Value);
	//func for normal turn and lookup rate
	void Baseturn(float Rate);
	void Baselookup(float Rate);
	//EQUIPING THE GUN and SPAWNING THE GUN And DROPPING
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,category="Equippedweapon")
	class ABaseWeapon* EquippedWeapon;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Equipinggun|default")
	TSubclassOf<class ABaseWeapon> Defaultweaponclass;

	ABaseWeapon* getcharacterholdingweapon();

	void Spawnandequip(ABaseWeapon* getWeapon);

	//action mapping
	void selectweaponpressed();
	void selectweaponreleased();


	//droppin the gun
	void DropGun();

	//for only tracing purpose when swapping
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trace|setting_visiblity")
	class ABaseitem* hititemorWeapon;
	
	//swapping the gun
	void Swapgun(ABaseWeapon* Weapontoswap);

	//......................................................................

	//this is method and prop for the item to hover over the camera for few seconds and equippped or to be in our inventory@itemhover
	//section interping the weapon
	UFUNCTION()
	FVector camerainterplotion();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Iteminterp|Cameradistance")
	float Cameraforwarddistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Iteminterp|Cameradistance")
	float CameraUpwardDistance;

	//when the E Key is pressed
	void pickupprop(ABaseitem* itemseen);

	
	//.....................................end of @itemhover
	

	//...................//


	//................................................................this is for tracing and shooting with key pressed
	//shooting input func
	void Shootingfunc();

	//sub of shooting func
	bool subshootingfunc(const FVector& Barrelsocloc, FVector& setbeamendpoint);

	//this function takes in traceinworld func
	void traceintick();

	//to trace any weapon and item in world to show widget //cpoy of endpoint
	bool TraceWeaponinworld(FHitResult& traceweaponhit, FVector& setbeamendpointref);

	        //..............sub prop for tracing to done when only satisfies..............
	        bool bShouldtrace;
			int8 OverlappedItemcount;
			//for to trace in tick only when prop satisfies
			void IncrementItemcount(int8 Amount);

    //TRACE FOR ITEM AND HIDE WIDGET
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Trace|setting_visiblity")
	class ABaseitem* Traceforweaponclass;
	//...........................................................................

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Main|gunshot")
	class USoundCue* gunshotsound;

	//before adding gunshot particle socket to change to facing x-axis
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main|gunshotparticle")
	class UParticleSystem* gunshotparticle;

	//montage to play when gun fires
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main|gunshotmontage")
	class UAnimMontage* montagegunfire;

	//after hitted the object particle to shwn where impacted
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main|gunshotimpactparticle")
	class UParticleSystem* gunshotimpactparticle;

	//beam particle to show line path of gun firing
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main|Beamlineparticle")
	class UParticleSystem* Beamparticle;


	//function and properties for zoom aiming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "zoom")
	bool baimZoom;
	void Aimzoomgunpress();
	void Aimzoomgunrelease();
	float Cameradefaultview;
	float Zoomedview;
	//sub function to slow down the speed of zoom
	void Camerazoomtick(float Deltatime);
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="zoom|interpspeed")
	float zoominterpspeed;
	//to change the value of camera field of view 
	float Cameracurrentview;


	//.....................................................................................................
	//FUNCTION AND PROPERTY TO THE CROSSHAIR AIM COMPONENTS WHILE AIMING,SHOOTING,INAIR
	//basic multiplier to crosshair shape smaller to the bigger
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshair|basic")
	float Crosshairmultiplier;

	//cross hair multiplier according to the velocity of character
	float CrosshairVelocityfactor;
	//while aiming
	float CrosshairAimfacctor;
	//while shooting
	float Crosshairshootfactor;
	//while jumping
	float CrosshairinAirfactor;
	//while not shooting and not aiming
	float CrosshairnotAimfactor;

	//function to setup these prop
	void CrosshairsetterFac(float Deltatime);

	UFUNCTION(BlueprintCallable)
	float GetCrosshairmultiplier() const;

	//cross hair spread when firing
	void crossspreadfiring();
	float Shootspreadtime;
	bool bfiringbullet;
    FTimerHandle Shootspreadhandle;
	void crossspreaddef();

	//............................................................................................................
	


	//automatic firing for  certain time and their properties

	//user input fire the weapon func
	void Firebuttonpressed();
	//user input fire bullet release func
	void Firebuttonreleased();
	//func to fire automatically for certain time
	void shouldfiresetter();
	//func to reset the after the timer
	UFUNCTION()
	void autofirereset();
	//maintenance of firing 
	bool bfirebuttonpress;
	//timer to reset auto fire
	FTimerHandle Autofiretimer;
	//auto fire bool
	//noneed bool bshouldfire;
	//delay for timer
	float autofireradelay;


	//@map of ammo start here
	//for ammo counting 
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ammomap", meta = (AllowPrivateAccess = "true"))
	TMap<EAmmotype, int32> AmmoMap;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ammomap|9mm", meta = (AllowPrivateAccess = "true"))
	int32 starting9mmcount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ammomap|5mm", meta = (AllowPrivateAccess = "true"))
	int32 starting5mmcount;

protected:
	void InitializeAmmomap();

	//...................................................


	//@Ammo count func
	bool WeaponhasAmmo();
	//...................

	//@ECFS
	//enum of ECFS
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="enumCombatstate")
	ECombatfirestate Ecombatfirestate;

	//@ECFS..........end

	//make shooting func easily readable
	void gunshotandparticle();
	void getvalsubshootfunc();
	void firingmontageplay();
	//....................................end



	//@reload the gun func start
	void reloadbuttonpressed();
	void Reloadweapon();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "reload")
	UAnimMontage* Reloadmontage;

	UFUNCTION(BlueprintCallable)
	void Finishreloading();

	//.........@reload end

	//@Ammo check for having ammorto reload
	bool carrysameammo();
	//@Ammocheck ends.............



};//for stting cross hair screen x will be integer positive not negative and same for screen y
