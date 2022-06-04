// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Baseitem.generated.h"


//ENUM CLASS FOR SETTING STAR IN UMG of pickup widget


UENUM(BlueprintType)
enum class EItemRarity : uint8 {
	EIR_Damaged UMETA(DisplayName="Damaged"),
	EIR_Common UMETA(DisplayName = "common"),
	EIR_Uncommon UMETA(DisplayName = "uncommon"),
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Legendary UMETA(DisplayName = "Legendary"),

	EIR_Default UMETA(DisplayName="Default")
};


UENUM(BlueprintType)
enum class EItemState : uint8 {
	EIS_Pickup UMETA(DisplayName = "Pickup"),
	EIS_Equipinterp UMETA(DisplayName = "Equipandinterp"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Pickedup UMETA(DisplayName = "Pickedup"),
	EIS_Falling UMETA(DisplayName = "Falling"),

	EIS_Default UMETA(DisplayName = "Default")
};


UCLASS()
class SHOOTERTUTORIAL_API ABaseitem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseitem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//this is base class for all weapons and items

	

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Baseitem|Mesh", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh;


	//widget to show on the screen
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Baseitem|Pickupwidget", meta = (AllowPrivateAccess = "true"))
    class UWidgetComponent* PickupWidget;

     //spherical component to show widget when the character interfere
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Baseitem|Sphere", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* Sphere;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Baseitem|Box", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* Box;


	//..........................................................
	//name to be showed on gun when traced
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "itemTrace|Itemname", meta = (AllowPrivateAccess = "true"))
	FString ItemName;

	//no of bullet in the gun to show in widget
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "itemTrace|Itemcount", meta = (AllowPrivateAccess = "true"))
	int32 ItemAmmocount;

	//setting Itemstate enum
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Enum|ItemState",meta=(AllowPrivateAccess="true"))
	EItemState Itemstate;

	//for interping the weapon and the pickup
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "Iteminterp|Zcurve",meta=(AllowPrivateAccess="true"))
	class UCurveFloat* ItemZcurve;

	//for reducing the scale of the gun 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "Iteminterp|Zcurve", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* itemScalecurve;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Iteminterp|charref", meta = (AllowPrivateAccess = "true"))
	class AShootercharacter* Shooterchar;

	//..........................................
public:
	FORCEINLINE UWidgetComponent* getpickupwidget() const { return PickupWidget; }
	//...........................
	//this is to trace weapon in scene to activate tracing in tick
	UFUNCTION()
	void oncharacteroverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void oncharacterendoverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	//...........................

	//for star opacity
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="itemTrace|ItemStarcount")
	EItemRarity Itemrarity;

	void SetActivestarinwidget();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "itemTrace|arrayforstar")
	TArray<bool>ActiveStars;

	//GETTER PROPERTIES
	FORCEINLINE USphereComponent* getSpherecollision() const { return Sphere; }
	FORCEINLINE UBoxComponent* getBoxCollision() const { return Box; }
	FORCEINLINE USkeletalMeshComponent* getMeshgame() const { return Mesh; }

	//...........................................................................
	//getter for enum itemstate
	FORCEINLINE  EItemState getItemstate() const { return Itemstate; }
	void SetItemState(EItemState state);

	void SetItemProperties(EItemState setstate);
	//.............................................................................

	//......................//


	//setting for interping the weapon and the item
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "Iteminterp|startactorloc")
	FVector ItemStartinterploc;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "Iteminterp|cameraloc")
	FVector Camerainterploc;

	class  AShootercharacter* Shooter;

	void  Startitemcurve(AShootercharacter* Chare);

	//in tick to float the gunn and item in the screen for some time
	void interpZItem(float Deltatime);

	float ZCurvetime;
	
	FTimerHandle Zcurvehandle;

	bool binterpItem;


	//for pupose of rotating the weapon relevent to the camera
	FRotator  charcamerarot;
	FRotator  weaponrotation;
	float Initialrotyawoffset;


	UFUNCTION()
	void Endcurvehandle();

	//......................................................//

};
