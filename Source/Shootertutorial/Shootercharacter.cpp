// Fill out your copyright notice in the Description page of Project Settings.


#include "Shootercharacter.h"
#include "CoreMinimal.h"
#include "Engine/Engine.h" 
#include "Engine/World.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Baseitem.h"
#include "Components/WidgetComponent.h"
#include "BaseWeapon.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

//editdefaultonly will only first choice of editor values

// Sets default values   and when we reduce blend weights in layered blend per bone recoil anim works and additive will cause error
AShootercharacter::AShootercharacter():Baseturnrate(45.f),Baselookuprate(45.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//setting spring arm 
	Springarm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Springarm"));
	Springarm->SetupAttachment(RootComponent);
	Springarm->TargetArmLength = 300.f;
	Springarm->bUsePawnControlRotation = true;
	//socket offset to add crosshair to shooting profiles and for better visuals
	Springarm->SocketOffset = FVector(0.f,50.f, 80.f);
	//setting camera 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(Springarm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	//controller rotation not to affect character
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = true;//turning true will not rotate the mesh
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement =false;//going to blend with blend space
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);


	//zooming initialization
	Cameradefaultview = 0.f;
	Zoomedview = 45.f;
	//sub of this for smooth view
	Cameracurrentview = 0.f;
	zoominterpspeed = 15.f;

	//arrow torn rate and look rate 
	hipturnrate = 90.f;
	hiplookrate = 90.f;
	aimturnrate = 20.f;
	aimlookrate = 20.f;
	//mouse turn rate and lookup rate
	mousehipturnrate = 1.0f;
	mousehiplookrate = 1.0f;
	mouseaimturnrate = 0.2f;
	mouseaimlookrate = 0.2f;
	//cross hair multiplier initializer
	CrosshairinAirfactor = 0.1f;
	CrosshairAimfacctor = 0.1f;
	CrosshairnotAimfactor = 0.5f;
	//timing when bullet fired to spread
	Crosshairshootfactor = 0.f;
	Shootspreadtime = 0.05f;
	//auto firing system initialize
	bfirebuttonpress = false;
	//it should be higher than crosshair spred delay timer
	autofireradelay = 0.1f;

	//.................................Trace initialze
	OverlappedItemcount = 0;

	//camera interpolation
	Cameraforwarddistance = 230.f;
	CameraUpwardDistance = 70.f;

	//ammomap initaialize @Ammomap
	starting9mmcount = 100;
	starting5mmcount = 80;
	//..............................

	//ecfs intialize @ECFS
	Ecombatfirestate = ECombatfirestate::ECFS_Unoccupied;



	//@Grab and replace the clip in gun for reloading animation take place
	lefthandbonename=TEXT("Hand_L");
	Handclipcomponent = CreateDefaultSubobject<USceneComponent>(TEXT("Handclipcomponnet"));
	camarmrotater= FVector(0.f, 50.f, 80.f);
}

// Called when the game starts or when spawned
void AShootercharacter::BeginPlay()
{
	Super::BeginPlay();
	if (Camera) {
		Cameradefaultview = getCamera()->FieldOfView;
		//UE_LOG(LogTemp, Warning, TEXT("cameradefaultview:%f"), Cameradefaultview);
		Cameracurrentview = Cameradefaultview;
	}
	//.........................................................
	//Spawning the default weapon and equipping the new weapon
	Spawnandequip(getcharacterholdingweapon());
	//........................................................

	//@Ammomap initialize
	InitializeAmmomap();
	//@Ammomap end
	
}

// Called every frame
void AShootercharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("frame in tick%f"), DeltaTime);
	//calling the function to interp between default to zoomed view and zoomed to default view
	Camerazoomtick(DeltaTime);
	//..............................................................................
	//calling for cross hair multiplier which spreads the cross hair
	CrosshairsetterFac(DeltaTime);
	//............................................................................
	//AUTO FIRING
	//if (bshouldfire) {
		//Shootingfunc();
	//}
	//................................................................................

	//for tracing the weapon in world
	traceintick();
	//..................................................
}

// Called to bind functionality to input
void AShootercharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Moveforward", this, &AShootercharacter::Moveforward);
	PlayerInputComponent->BindAxis("Moveright", this, &AShootercharacter::Moveright);
	PlayerInputComponent->BindAxis("Baseturnrate", this, &AShootercharacter::Baseturn);
	PlayerInputComponent->BindAxis("Baselookup", this, &AShootercharacter::Baselookup);
	//default are changed to our func 
	PlayerInputComponent->BindAxis("mouseturn", this, &AShootercharacter::mouseturn);
	PlayerInputComponent->BindAxis("mouselookup", this, &AShootercharacter::mouselookup);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//functionality to shoot the bullet 
	PlayerInputComponent->BindAction("Firebutton", IE_Pressed, this, &AShootercharacter::Firebuttonpressed);
	PlayerInputComponent->BindAction("Firebutton", IE_Released, this, &AShootercharacter::Firebuttonreleased);

	//functionality for zooming func
	PlayerInputComponent->BindAction("Aimzoom", IE_Pressed, this, &AShootercharacter::Aimzoomgunpress);
	PlayerInputComponent->BindAction("Aimzoom", IE_Released, this, &AShootercharacter::Aimzoomgunrelease);

	//functionality for the selecting weapon and dropping weapon
	PlayerInputComponent->BindAction("SelectWeapon", IE_Pressed, this, &AShootercharacter::selectweaponpressed);
	PlayerInputComponent->BindAction("SelectWeapon", IE_Released, this, &AShootercharacter::selectweaponreleased);

	//fuctionality of reloading the weapon
	PlayerInputComponent->BindAction("Reloadgun", IE_Pressed, this, &AShootercharacter::reloadbuttonpressed);


}

void AShootercharacter::Moveforward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.f)) {
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator Yawrotation{ FRotator(0.f,Rotation.Yaw,0.f) };
		FVector Direction = FRotationMatrix(Yawrotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AShootercharacter::Moveright(float Value)
{
	if ((Controller != nullptr) && (Value != 0.f)) {
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator Yawrotation{ FRotator(0.f,Rotation.Yaw,0.f) };
		FVector Direction{ FRotationMatrix(Yawrotation).GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);
	}
}

void AShootercharacter::setarrowturnlookup()
{
	if (baimZoom) {
		Baseturnrate = aimturnrate;
		Baselookuprate = aimlookrate;
	}
	else {
		Baseturnrate = hipturnrate;
		Baselookuprate = hiplookrate;
	}
}

void AShootercharacter::mouseturn(float Value)
{
	float mouselocalturn{0.f};
	if (baimZoom) {
		mouselocalturn = mouseaimturnrate;
	}
	else {
		mouselocalturn = mousehipturnrate;
	}
	//UE_LOG(LogTemp, Warning, TEXT("mousetrun:%f"), Value);
	AddControllerYawInput(Value * mouselocalturn);
}

void AShootercharacter::mouselookup(float Value)
{
	float mouselocallookup{0.f};
	if (baimZoom) {
		mouselocallookup = mouseaimlookrate;
	}
	else {
		mouselocallookup = mousehiplookrate;
	}
	//UE_LOG(LogTemp, Warning, TEXT("mouselook:%f"), Value);
	AddControllerPitchInput(Value * mouselocallookup);
}

void AShootercharacter::Baseturn(float Rate)
{
	AddControllerYawInput(Rate * Baseturnrate * GetWorld()->DeltaTimeSeconds);//deg/sec * sec/frame
	//UE_LOG(LogTemp, Warning, TEXT("Basetrun:%f"), Rate);
	//UE_LOG(LogTemp, Warning, TEXT("tine in baseturn:%f"), GetWorld()->DeltaTimeSeconds);
	
}

void AShootercharacter::Baselookup(float Rate)
{
	AddControllerPitchInput(Rate * Baselookuprate * GetWorld()->DeltaTimeSeconds);
	//UE_LOG(LogTemp, Warning, TEXT("Baselookup:%f"), Rate);
	//UE_LOG(LogTemp, Warning, TEXT("tine in baselook:%f"), GetWorld()->DeltaTimeSeconds);

}

ABaseWeapon* AShootercharacter::getcharacterholdingweapon()
{
	if (Defaultweaponclass) {
		return GetWorld()->SpawnActor<ABaseWeapon>(Defaultweaponclass);
	}
	return nullptr;
}

void AShootercharacter::Spawnandequip(ABaseWeapon* getWeapon)
{
	if (getWeapon) {
		//no need
		//setting collision to the weapon
		//getWeapon->getBoxCollision()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		//getWeapon->getSpherecollision()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		//socket location to spawn the gun
		const USkeletalMeshSocket* socketgun = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (socketgun) {
			//this is better when we drop gun and reattaching the same gun
			getWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget,true), FName("RightHandSocket"));
			//may use this cause the problem when we are free hands
			//socketgun->AttachActor(getWeapon, GetMesh());
		}
		EquippedWeapon = getWeapon;
		//for playing the sound cue
		if (EquippedWeapon->getEquippedsound()) {
			UGameplayStatics::PlaySound2D(this, EquippedWeapon->getEquippedsound());
		}
		//setting the enum itemstate 
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}

//.............................................................
void AShootercharacter::selectweaponpressed()
{
	if (hititemorWeapon) {
		//pickupprop(hititemorWeapon);
		hititemorWeapon->Startitemcurve(this);
		//play the equipping and in zcurve
		if (hititemorWeapon->getEquippingsound()) {
			UGameplayStatics::PlaySound2D(this, hititemorWeapon->getEquippingsound());
		}
	}
	else {
		//may be includeded for better gameplay
		DropGun();
	}
	
}

void AShootercharacter::selectweaponreleased()
{
	
}

void AShootercharacter::DropGun()
{
	FDetachmentTransformRules Detachmenttransformrules(EDetachmentRule::KeepWorld,true);
	EquippedWeapon->DetachFromActor(Detachmenttransformrules);
	EquippedWeapon->SetItemState(EItemState::EIS_Falling);
	EquippedWeapon->throwweaponslight();
	
}
void AShootercharacter::Swapgun(ABaseWeapon* Weapontoswap)
{
	DropGun();
	Spawnandequip(Weapontoswap);
	hititemorWeapon = nullptr;
	Traceforweaponclass = nullptr;

}
//starts for iterping the weapon
 FVector AShootercharacter::camerainterplotion()
{
	const FVector Cameralocation{ getCamera()->GetComponentLocation() };
	const FVector CameraForward{ getCamera()->GetForwardVector() };
	return Cameralocation + (CameraForward * Cameraforwarddistance) + FVector(0.f, 0.f, CameraUpwardDistance);
}
void AShootercharacter::pickupprop(ABaseitem* itemseen)
{
	ABaseWeapon* Wepref = Cast<ABaseWeapon>(itemseen);
	if (Wepref) {
		Swapgun(Wepref);
	}
	
}
//.....................................................................
void AShootercharacter::gunshotandparticle() {
	if (gunshotsound) {
		UGameplayStatics::PlaySound2D(this, gunshotsound);
	}
	
}

void AShootercharacter::getvalsubshootfunc() {
	//for spawnemitter function parameters
	const USkeletalMeshSocket* Barelsocket = EquippedWeapon->getMeshgame()->GetSocketByName("BarrelSocket");
	const FTransform Bareltransform = Barelsocket->GetSocketTransform(EquippedWeapon->getMeshgame());
	if (gunshotparticle) {
		//to spawn the particle in socket place
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), gunshotparticle, Bareltransform);
	}
	FVector Beamendpoint;
	//pass to function and get the value
	if (subshootingfunc(Bareltransform.GetLocation(), Beamendpoint)) {

		//hit pointed in object to spawn the particle
		if (gunshotimpactparticle) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), gunshotimpactparticle, Beamendpoint);
		}


		//line beam to show path on gunfiring bullet trace

		if (Beamparticle) {
			UParticleSystemComponent* Beamparticlesystem = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Beamparticle, Bareltransform);
			if (Beamparticlesystem) {
				//for travelling beam up to hitted object
				Beamparticlesystem->SetVectorParameter(FName("Target"), Beamendpoint);
			}
		}
	}
}

void AShootercharacter::firingmontageplay() {
	if (montagegunfire) {
		UAnimInstance* Animinstance = GetMesh()->GetAnimInstance();
		if (Animinstance) {
			Animinstance->Montage_Play(montagegunfire, 1.0f);
			Animinstance->Montage_JumpToSection(FName("Fire"));
		}
	}
}


void AShootercharacter::Shootingfunc()
{
	if (EquippedWeapon == nullptr) return;
	if (Ecombatfirestate != ECombatfirestate::ECFS_Unoccupied) return;
	if (WeaponhasAmmo()) {
		gunshotandparticle();
	    getvalsubshootfunc();
		firingmontageplay();
		//to shoot
		shouldfiresetter();
	}
	//@Ammocount
	EquippedWeapon->Decrementammoavaliable();
	//...................@Ammocount end 
	//set firing to true to tweak crosshair
	crossspreadfiring();
}

bool AShootercharacter::subshootingfunc(const FVector& Barrelsocloc, FVector& setbeamendpoint)
{	
	//trace the weapon and the item in the world to show the widget
	//FHitResult crosstraceweap;


	//for line trace to hit world actor,and they are any object in between to check spawn particle and change bea endpoint
	FHitResult Bullethitreturn;
	TraceWeaponinworld(Bullethitreturn, setbeamendpoint);
	//initialize the barrel soc loc
	const FVector Start{ Barrelsocloc };


	//const FQuat Rotation{ Bareltransform.GetRotation() };
	//const FVector Rotationaxis{ Rotation.GetAxisX() };

	FVector starttoend = setbeamendpoint - Start;
	const FVector End = Start + starttoend * 1.25f;
	GetWorld()->LineTraceSingleByChannel(Bullethitreturn, Start, End, ECollisionChannel::ECC_Visibility);
	//beam vector 
	//FVector Beamendpoint{ End };
	//if hit hitted any object to draw line and point
	if (Bullethitreturn.bBlockingHit) {
		//DrawDebugLine(GetWorld(), Start, End, FColor::Purple, false, 3.f);
		//DrawDebugPoint(GetWorld(), Bullethitreturn.Location, 5.f, FColor::Green, false, 5.0f);
		//to change ending beam trace while they hit the object
		setbeamendpoint = Bullethitreturn.Location;
		return true;
	}
	return false;
	//line trace function finished for any object between to spawn particle
}

void AShootercharacter::traceintick()
{
	if (bShouldtrace) {
		FHitResult weapongettrace;
		FVector  getbeamendpoint;
		if (TraceWeaponinworld(weapongettrace, getbeamendpoint)) {
			hititemorWeapon = Cast<ABaseitem>(weapongettrace.GetActor());
			if (hititemorWeapon && hititemorWeapon->getpickupwidget())
			{
				//UE_LOG(LogTemp, Warning, TEXT("hitworks:%f"), weapongettrace.Distance);
				hititemorWeapon->getpickupwidget()->SetVisibility(true);
			}
			if (Traceforweaponclass) {
				if (hititemorWeapon != Traceforweaponclass) {
					Traceforweaponclass->getpickupwidget()->SetVisibility(false);
				}
			}
			Traceforweaponclass = hititemorWeapon;
		}
	}
	else if (Traceforweaponclass) {
		Traceforweaponclass->getpickupwidget()->SetVisibility(false);
	}
}

bool AShootercharacter::TraceWeaponinworld(FHitResult& traceweaponhit, FVector& setbeamendpointref)
{
	//we are going to use crosshair point to shoot and impact particle

	FVector2D Viewportsize;
	if (GEngine && GEngine->GameViewport) {
		GEngine->GameViewport->GetViewportSize(Viewportsize);
	}
	//to get center we have setted in HUD class of blueprint only
	FVector2D Crosshairscreenori = FVector2D(Viewportsize.X / 2.f,
		Viewportsize.Y / 2.f);
	//this not necessary it will cause aim focus change place  while aimzoom its aiming true
	//Crosshairscreenori.Y -= 50.f;

	//to set for line trace from croshair
	FVector Crosshairposition;
	FVector Crosshairdirection;
	APlayerController* Playercontroler = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	bool bscreentoworld=UGameplayStatics::DeprojectScreenToWorld(Playercontroler, Crosshairscreenori, Crosshairposition, Crosshairdirection);
	if (bscreentoworld) {
		//for line trace from the crosshair
		//FHitResult crossfirehit;
		const FVector crossstart{ Crosshairposition };
		const FVector crossend{ Crosshairposition + Crosshairdirection * 50'000.f };

		//beam to be travel from barrel gun to the end
		setbeamendpointref = crossend;


		GetWorld()->LineTraceSingleByChannel(traceweaponhit, crossstart, crossend, ECollisionChannel::ECC_Visibility);
		if (traceweaponhit.bBlockingHit) {
			DrawDebugLine(GetWorld(), crossstart, crossend, FColor::Red, false, 3.f);
			DrawDebugPoint(GetWorld(), traceweaponhit.Location, 5.f, FColor::Green, false, 5.0f);
			setbeamendpointref = traceweaponhit.Location;
			return true;
		}
	}
	

	return false;
}

void AShootercharacter::IncrementItemcount(int8 Amount)
{
	OverlappedItemcount += Amount;
	if (OverlappedItemcount <= 0) {
		bShouldtrace = false;
		OverlappedItemcount = 0;
	}
	else {
		bShouldtrace = true;
	}
}

void AShootercharacter::Aimzoomgunpress()
{
	baimZoom = true;
	//for prototyping purpose only
	//getCamera()->SetFieldOfView(Zoomedview);
}

void AShootercharacter::Aimzoomgunrelease()
{
	baimZoom = false;
	//getCamera()->SetFieldOfView(Cameradefaultview);
}

void AShootercharacter::Camerazoomtick(float Deltatime)
{
	if (baimZoom) {
		//to change from default to zoomed view smoothly
		Cameracurrentview = FMath::FInterpTo(Cameracurrentview, Zoomedview, Deltatime, zoominterpspeed);
	}
	else {
		//to change from zoomed to default view smoothly
		Cameracurrentview = FMath::FInterpTo(Cameracurrentview,Cameradefaultview, Deltatime, zoominterpspeed);
	}
	//UE_LOG(LogTemp, Warning, TEXT("Cameracurrentview:%f"), Cameracurrentview);
	getCamera()->SetFieldOfView(Cameracurrentview);
}

void AShootercharacter::CrosshairsetterFac(float Deltatime)
{
	FVector2D inputwalkspeed{ 0.f,600.f };
	FVector2D outputrange{ 0.f,1.f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f;
	CrosshairVelocityfactor=FMath::GetMappedRangeValueClamped(inputwalkspeed,outputrange,Velocity.Size());

	//checking for in air
	bool falling = GetCharacterMovement()->IsFalling();
	if (falling) {
		CrosshairinAirfactor = FMath::FInterpTo(CrosshairinAirfactor, 1.9f, Deltatime, 1.5f);
	}
	else {
		CrosshairinAirfactor = FMath::FInterpTo(CrosshairinAirfactor, 0.f, Deltatime, 1.5f);
	}
	//checking for firing
	if (bfiringbullet) {
		Crosshairshootfactor = FMath::FInterpTo(CrosshairAimfacctor, 0.4f, Deltatime, 30.f);
	}
	else {
		Crosshairshootfactor = FMath::FInterpTo(CrosshairAimfacctor, 0.f, Deltatime, 30.f);
	}
	//mulitiplier for crosshair sizee
	Crosshairmultiplier = CrosshairnotAimfactor + CrosshairVelocityfactor+CrosshairinAirfactor+Crosshairshootfactor;
	if (baimZoom) {
		Crosshairmultiplier = CrosshairAimfacctor + CrosshairVelocityfactor+Crosshairshootfactor;
	}
}

float AShootercharacter::GetCrosshairmultiplier() const
{
	return Crosshairmultiplier;
}

void AShootercharacter::crossspreadfiring()
{
	bfiringbullet = true;
	GetWorldTimerManager().SetTimer(Shootspreadhandle, this, &AShootercharacter::crossspreaddef, Shootspreadtime);
}

void AShootercharacter::crossspreaddef()
{
	bfiringbullet = false;
}

//automatic firing bullet prop
void AShootercharacter::Firebuttonpressed()
{
	bfirebuttonpress = true;
	if (WeaponhasAmmo()) {
		Shootingfunc();
	}
}

void AShootercharacter::Firebuttonreleased()
{
	bfirebuttonpress = false;
}

void AShootercharacter::shouldfiresetter()
{
	Ecombatfirestate = ECombatfirestate::ECFS_Firetimerinprogress;
	//UE_LOG(LogTemp, Warning, TEXT("now"));
	GetWorldTimerManager().SetTimer(Autofiretimer, this, &AShootercharacter::autofirereset,autofireradelay);
}

void AShootercharacter::autofirereset()
{
	//UE_LOG(LogTemp, Warning, TEXT("now in reset"));
	Ecombatfirestate = ECombatfirestate::ECFS_Unoccupied;
	//UE_LOG(LogTemp, Warning, TEXT("later in reset"));
	if (WeaponhasAmmo()) {
		if (bfirebuttonpress) {
			Shootingfunc();
		}
	}
	else {
		//reload to be happen
		Reloadweapon();
	}
}


//@ammomap starts
void AShootercharacter::InitializeAmmomap()
{
	AmmoMap.Add(EAmmotype::EAT_9mm, starting9mmcount);
	AmmoMap.Add(EAmmotype::EAT_Ar, starting5mmcount);
}
bool AShootercharacter::WeaponhasAmmo()
{
	if (EquippedWeapon == nullptr)  return false;
	return EquippedWeapon->getAmmoavaliable()>0;
}
//.........................................................................


//@Reload
void AShootercharacter::reloadbuttonpressed() {
	Reloadweapon();
}



void AShootercharacter::Reloadweapon() {
	if (Ecombatfirestate != ECombatfirestate::ECFS_Unoccupied) return;
	if (EquippedWeapon == nullptr) return;
	UAnimInstance* Anim = GetMesh()->GetAnimInstance();
	if (carrysameammo() && !EquippedWeapon->CheckforReload()) {
		Ecombatfirestate = ECombatfirestate::ECFS_reloadstate;
		//FName sectionname = TEXT("Reloadsmg");
		if (Anim && Reloadmontage) {
			Anim->Montage_Play(Reloadmontage, 1.0f);
			Anim->Montage_JumpToSection(EquippedWeapon->Montagesectionname);
		}
	}
}
//.............................reload end


void AShootercharacter::Finishreloading() {
	Ecombatfirestate = ECombatfirestate::ECFS_Unoccupied;
	if (EquippedWeapon == nullptr)return;
	EAmmotype ammosameinequip = EquippedWeapon->getammotype();
	if (AmmoMap.Contains(ammosameinequip)) {
		int32 Noammocarried = AmmoMap[ammosameinequip];
		UE_LOG(LogTemp, Warning, TEXT("ammocarried % i"), Noammocarried);
		int32 spaceavailabletocarry = EquippedWeapon->Magammocapacity - EquippedWeapon->getAmmoavaliable();
		if (spaceavailabletocarry > Noammocarried) {
			EquippedWeapon->Reloadammocount(Noammocarried);
			Noammocarried = 0;
			AmmoMap.Add(ammosameinequip, Noammocarried);
		}
		else {
			EquippedWeapon->Reloadammocount(spaceavailabletocarry);
			Noammocarried -= spaceavailabletocarry;
			AmmoMap.Add(ammosameinequip, Noammocarried);
		}
	}
	
}

bool AShootercharacter::carrysameammo()
{
	if (EquippedWeapon == nullptr) {
		return false;
	}
	EAmmotype ammowithcharacter = EquippedWeapon->getammotype();
	if (AmmoMap.Contains(ammowithcharacter)) {
		return AmmoMap[ammowithcharacter] > 0;
	}
	return false;
}


//@Grab and replace the clip in gun for reloading animation take place

void AShootercharacter::Grabclip()
{
	if (EquippedWeapon == nullptr)return;
	if (Handclipcomponent == nullptr)return;
	int32 clipboneindex=EquippedWeapon->getMeshgame()->GetBoneIndex(EquippedWeapon->Getclipbonename());
	Clipbone=EquippedWeapon->getMeshgame()->GetBoneTransform(clipboneindex);

	FAttachmentTransformRules Attachmentrules(EAttachmentRule::KeepRelative, true);
	bool re=Handclipcomponent->AttachToComponent(GetMesh(), Attachmentrules, lefthandbonename);
	if (re) {
		Handclipcomponent->SetWorldTransform(Clipbone);
		UE_LOG(LogTemp, Warning, TEXT("grab work"));
	}
	EquippedWeapon->Setmovingclip(true);
	Springarm->SocketOffset = FVector(0.f, -150.f,100.f)+camarmrotater;
}

void AShootercharacter::Replaceclip()
{
	EquippedWeapon->Setmovingclip(false);
	Springarm->SocketOffset = FVector(0.f, 50.f, 80.f);
}

//@Grab and replace ends

