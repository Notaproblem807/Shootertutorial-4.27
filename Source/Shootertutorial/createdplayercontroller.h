// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "createdplayercontroller.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERTUTORIAL_API Acreatedplayercontroller : public APlayerController
{
	GENERATED_BODY()
	
public:
	Acreatedplayercontroller();
private:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "controller|sethudammo", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> Ammoverlayclass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "controller|hudammo", meta = (AllowPrivateAccess = "true"))
	UUserWidget* Ammooverlay;
};
