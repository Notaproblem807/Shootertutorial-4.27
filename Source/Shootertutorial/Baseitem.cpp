// Fill out your copyright notice in the Description page of Project Settings.


#include "Baseitem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Shootercharacter.h"
#include "Camera/CameraComponent.h"

// Sets default values
ABaseitem::ABaseitem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(Mesh);
	Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(GetRootComponent());


	//setup the sphere to enhance trace in tick of shootercharacter
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());
	//....................Trace prop for naming and other things
	ItemName = FString("Default");
	//set state of gun and weapon and item in the world
	Itemstate = EItemState::EIS_Pickup;

	//setting the interping of the item
	binterpItem = false;
	ZCurvetime = 0.7f;
	ItemStartinterploc = FVector(0.f);
}

// Called when the game starts or when spawned
void ABaseitem::BeginPlay()
{
	Super::BeginPlay();
	//.........................................
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseitem::oncharacteroverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &ABaseitem::oncharacterendoverlap);
	getpickupwidget()->SetVisibility(false);
	//.....................................
	//setting star  in the widget
	SetActivestarinwidget();
	
	
}

// Called every frame
void ABaseitem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//interping thhe item im the z direction infront of camera
	interpZItem(DeltaTime);

}

void ABaseitem::oncharacteroverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AShootercharacter* MainChar = Cast<AShootercharacter>(OtherActor);
	if (MainChar) {
		MainChar->IncrementItemcount(1);
	}

}

void ABaseitem::oncharacterendoverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AShootercharacter* MainChar = Cast<AShootercharacter>(OtherActor);
	if (MainChar) {
		MainChar->IncrementItemcount(-1);
	}
}

void ABaseitem::SetActivestarinwidget()
{
	//intialize all stars in widget not to show
	for (int32 i = 0;i <= 5;i++) {
		ActiveStars.Add(false);
	}

	switch (Itemrarity)
	{
	case EItemRarity::EIR_Damaged:
		ActiveStars[1] = true;
		break;
	case EItemRarity::EIR_Common:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		break;
	case EItemRarity::EIR_Uncommon:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		break;
	case EItemRarity::EIR_Rare:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		break;
	case EItemRarity::EIR_Legendary:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		ActiveStars[5] = true;
	}

}

void ABaseitem::SetItemState(EItemState state)
{
	Itemstate = state;
	SetItemProperties(Itemstate);
}

void ABaseitem::SetItemProperties(EItemState setstate)
{
	switch (setstate) {
	    case EItemState::EIS_Pickup:
			//mesh settings
			getMeshgame()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			getMeshgame()->SetSimulatePhysics(false);
			getMeshgame()->SetEnableGravity(false);
			getMeshgame()->SetVisibility(true);
			//setsphere collision
			getSpherecollision()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
			getSpherecollision()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			//setbox collision
			getBoxCollision()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			getBoxCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
			getBoxCollision()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			break;
		case EItemState::EIS_Equipped:
			//pickup widget setting
			getpickupwidget()->SetVisibility(false);
			//mesh settings
			getMeshgame()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			getMeshgame()->SetSimulatePhysics(false);
			getMeshgame()->SetEnableGravity(false);
			getMeshgame()->SetVisibility(true);
			//setsphere collision
			getSpherecollision()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			getSpherecollision()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//setbox collision
			getBoxCollision()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			getBoxCollision()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;
		case EItemState::EIS_Falling:
			getMeshgame()->SetSimulatePhysics(true);
			getMeshgame()->SetEnableGravity(true);
			getMeshgame()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			getMeshgame()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			getMeshgame()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
			//sphere setting
			getSpherecollision()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			getSpherecollision()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//box setting
			getBoxCollision()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			getBoxCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
			getBoxCollision()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		case EItemState::EIS_Equipinterp:
			//pickup widget setting
			getpickupwidget()->SetVisibility(false);
			//mesh settings
			getMeshgame()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			getMeshgame()->SetSimulatePhysics(false);
			getMeshgame()->SetEnableGravity(false);
			getMeshgame()->SetVisibility(true);
			//sphere setting
			getSpherecollision()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			getSpherecollision()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//box setting
			getBoxCollision()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			getBoxCollision()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ABaseitem::Startitemcurve(AShootercharacter* Chare)
{
	Shooter = Chare;
	ItemStartinterploc = GetActorLocation();
	SetItemState(EItemState::EIS_Equipinterp);
	binterpItem = true;
	

	GetWorldTimerManager().SetTimer(Zcurvehandle, this, &ABaseitem::Endcurvehandle, ZCurvetime);

	charcamerarot = Shooter->getCamera()->GetComponentRotation();
	//UE_LOG(LogTemp, Warning, TEXT("charrotloc:%s"), *charcamerarot.ToString());
	weaponrotation = getMeshgame()->GetComponentRotation();
	//UE_LOG(LogTemp, Warning, TEXT("weaponloc:%s"), *weaponrotation.ToString());
	Initialrotyawoffset = charcamerarot.Yaw - weaponrotation.Yaw;
}

void ABaseitem::interpZItem(float Deltatime)
{
	if (!binterpItem) return;
	
	if (Shooter && binterpItem && ItemZcurve) {
		const float elapsedtime = GetWorldTimerManager().GetTimerElapsed(Zcurvehandle);
		const float Zcurvevalueintic = ItemZcurve->GetFloatValue(elapsedtime);
		//UE_LOG(LogTemp, Warning, TEXT("curve:%f"), Zcurvevalueintic);
		Camerainterploc = Shooter->camerainterplotion();
		FVector itemloc = ItemStartinterploc;
		//UE_LOG(LogTemp, Warning, TEXT("actorloc:%s"),*itemloc.ToString());
		FVector itemtocamera{ FVector(0.f,0.f,(Camerainterploc.Z-itemloc.Z)) };
		const float Deltaz = itemtocamera.Size();
		const FVector CurrentLocation{ GetActorLocation() };
		//interplaeting item in xaxis
		const float InterpXvalue = FMath::FInterpTo(CurrentLocation.X, Camerainterploc.X, Deltatime, 30.f);
		//interpleting item in yaxis
		const float InterpYvalue = FMath::FInterpTo(CurrentLocation.Y, Camerainterploc.Y, Deltatime, 30.f);
		itemloc.X = InterpXvalue;
		itemloc.Y = InterpYvalue;
		itemloc.Z += Zcurvevalueintic * Deltaz;
		if (itemScalecurve) {
			float scaleval=itemScalecurve->GetFloatValue(elapsedtime);
			SetActorScale3D(FVector(scaleval, scaleval, scaleval));
		}
		SetActorLocation(itemloc, true, nullptr, ETeleportType::TeleportPhysics);
		//for setting rotation relevant to camera if not it rotate unevenly
		// default not working as expected
		const FRotator initialyaoffset{ Shooter->getCamera()->GetComponentRotation()};
		FRotator setweaponrot{ 0.f,weaponrotation.Yaw + Initialrotyawoffset +initialyaoffset.Yaw ,0.f };
		SetActorRotation(setweaponrot, ETeleportType::TeleportPhysics);
		
	}
}


void ABaseitem::Endcurvehandle()
{
	binterpItem = false;
	if (Shooter) {
		Shooter->pickupprop(this);
	}
	SetActorScale3D(FVector(1.f, 1.f, 1.f));
}


