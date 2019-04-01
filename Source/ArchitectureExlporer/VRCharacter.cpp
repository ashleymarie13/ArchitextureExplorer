// Fill out your copyright notice in the Description page of Project Settings.

#include "VRCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "Public/DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "AI/Navigation/NavigationSystem.h"

// Sets default values
AVRCharacter::AVRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	VRRoot->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);

	DestinationMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DestinationMarker"));
	DestinationMarker->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	DestinationMarker->SetVisibility(false);

}

// Called every frame
void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// (Cameras current location within this frame) - (Actors location[not updated from the previous frame])
	FVector NewCameraOffset = Camera->GetComponentLocation() - GetActorLocation();
	NewCameraOffset.Z = 0; // disable upward movement;
	AddActorWorldOffset(NewCameraOffset);
	VRRoot->AddWorldOffset(-NewCameraOffset);

	UpdateDestinationMarker();

}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &AVRCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &AVRCharacter::MoveRight);
	PlayerInputComponent->BindAction(TEXT("Teleport"), IE_Released, this, &AVRCharacter::BeginTeleport);
}

bool AVRCharacter::FindTeleportDestination(FVector &location) {
	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetForwardVector() * MaxTeleportDistance;
	// This HitResult is where it hits the world
	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);
	if (!bHit) return false;

	FNavLocation FNavLocation;
	bool bHitNavMesh = GetWorld()->GetNavigationSystem()->ProjectPointToNavigation(HitResult.Location, FNavLocation, TeleportExtent);
	if (!bHitNavMesh) return false;

	location = FNavLocation.Location;
	return true;
}

void AVRCharacter::UpdateDestinationMarker() {
	FVector location;

	if (FindTeleportDestination(location)) {
		DestinationMarker->SetVisibility(true);
		DestinationMarker->SetWorldLocation(location);
	}
	else {
		DestinationMarker->SetVisibility(false);
	}
}

void AVRCharacter::MoveForward(float throttle) {
	AddMovementInput(throttle * Camera->GetForwardVector());
}

void AVRCharacter::MoveRight(float throttle) {
	AddMovementInput(throttle * Camera ->GetRightVector());

}

void AVRCharacter::BeginTeleport() {

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &AVRCharacter::FinishTeleport, TeleportFadeTime);
	FadeInOrOut(0, 1);
}

void AVRCharacter::FinishTeleport() {
	// We are a pawn and dont have a player character
	//GetCapsuleComponent()->GetScaledCapsualHalfHeight(); TODO
	SetActorLocation(DestinationMarker->GetComponentLocation()); // add the half height of the capsual
	
	FadeInOrOut(1, 0);
}

void AVRCharacter::FadeInOrOut(float fromAlpha, float toAlpha) {
	// PC - PlayerController (could be null)
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC != nullptr) {
		PC->PlayerCameraManager->StartCameraFade(fromAlpha, toAlpha, TeleportFadeTime, FLinearColor::Black);
	}
}

