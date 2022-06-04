// Fill out your copyright notice in the Description page of Project Settings.


#include "createdplayercontroller.h"
#include "Blueprint/UserWidget.h"


Acreatedplayercontroller::Acreatedplayercontroller() {

}

void Acreatedplayercontroller::BeginPlay()
{
	Super::BeginPlay();
	if (Ammoverlayclass) {
		Ammooverlay = CreateWidget<UUserWidget>(this, Ammoverlayclass);
		Ammooverlay->AddToViewport(0);
		Ammooverlay->SetVisibility(ESlateVisibility::Visible);
	}
}
