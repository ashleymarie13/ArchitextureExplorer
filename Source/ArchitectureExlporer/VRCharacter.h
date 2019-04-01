// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VRCharacter.generated.h"

UCLASS()
class ARCHITECTUREEXLPORER_API AVRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// Methods
	void UpdateDestinationMarker();
	void MoveForward(float throttle);
	void MoveRight(float throttle);
	void BeginTeleport();
	void FinishTeleport();
	bool FindTeleportDestination(FVector &location);
	void FadeInOrOut(float fromAlpha, float toAlpha);
private:
	UPROPERTY()
	class UCameraComponent* Camera;
	
	UPROPERTY()
	class USceneComponent* VRRoot;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* DestinationMarker;

private:
	UPROPERTY(EditAnywhere)
	float MaxTeleportDistance = 1000; //1000 = ~10 meters

	UPROPERTY(EditAnywhere)
		float TeleportFadeTime = 1.5;

	UPROPERTY(EditAnywhere)
		FVector TeleportExtent = FVector(100, 100, 100);
}; 
