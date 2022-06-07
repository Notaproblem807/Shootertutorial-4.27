#pragma once



//eneum for counting the ammo for the player

UENUM(BlueprintType)
enum class EAmmotype :uint8 {
	EAT_9mm UMETA(DisplayName = "9mm"),
	EAT_Ar UMETA(DisplayName = "5mm"),
	EAT_Max UMETA(DisplayName = "default")
};