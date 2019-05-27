// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "zadanieTestoweCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include <DrawDebugHelpers.h>
#include <Kismet/KismetSystemLibrary.h>
#include "MyInterface.h"
#include "Grenade.h"
#include <Engine/World.h>
#include <GameFramework/Character.h>
//////////////////////////////////////////////////////////////////////////
// AzadanieTestoweCharacter

AzadanieTestoweCharacter::AzadanieTestoweCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "NONE";
	case ROLE_SimulatedProxy:
		return "SImulated Proxy";
	case ROLE_AutonomousProxy:
		return "Autonomus Proxy";
	case ROLE_Authority:
		return "Authority";
	case ROLE_MAX:
		return "NONE";
	default:
		return "NONE";
	}
}
void AzadanieTestoweCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(Role), this, FColor::White, DeltaTime);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AzadanieTestoweCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("G_Throw", IE_Pressed, this, &AzadanieTestoweCharacter::Throw_a_Grenate);

	PlayerInputComponent->BindAction("LPM_Action", IE_Pressed, this, &AzadanieTestoweCharacter::Fire);

	PlayerInputComponent->BindAxis("MoveForward", this, &AzadanieTestoweCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AzadanieTestoweCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AzadanieTestoweCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AzadanieTestoweCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AzadanieTestoweCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AzadanieTestoweCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AzadanieTestoweCharacter::OnResetVR);
}



void AzadanieTestoweCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AzadanieTestoweCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AzadanieTestoweCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AzadanieTestoweCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AzadanieTestoweCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AzadanieTestoweCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AzadanieTestoweCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
void AzadanieTestoweCharacter::ServerThrow_a_Grenate_Implementation()
{
	if (WhatToSpawn != NULL)
	{
		//Check for a valid World
		UWorld* const World = GetWorld();
		if (World)
		{
			//Set the spawn parameters
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;

			//Get a random location to spawn at
			FVector SpawnLocation = GetMesh()->GetSocketLocation("hand_r");
			SpawnLocation += this->GetActorForwardVector() * 50;

			//Get a random rotation for spawned item
			FRotator SpawnRotation = FRotator(0, 0, 0);

			AGrenade* grenate = World->SpawnActor<AGrenade>(WhatToSpawn, SpawnLocation, SpawnRotation, SpawnParams);

			grenate->SetGrenateVelocity(FVector(GetActorForwardVector()+GetActorUpVector()).GetSafeNormal(), 1000);
		}
	}
}
void AzadanieTestoweCharacter::Throw_a_Grenate()
{
	ServerThrow_a_Grenate();
}

bool AzadanieTestoweCharacter::ServerThrow_a_Grenate_Validate()
{
	return true;
}
void AzadanieTestoweCharacter::ServerFire_Implementation(FVector Start, FVector End)
{
	FHitResult OutHit;
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	bool isHit = GetWorld()->LineTraceSingleByObjectType(OutHit, Start, End, FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldDynamic), TraceParameters);
	if (isHit)
	{
		bool hasInterface = OutHit.GetActor()->GetClass()->ImplementsInterface(UMyInterface::StaticClass());
		if (hasInterface)
		{
			IMyInterface::Execute_SimpleInteraction(OutHit.GetActor());
		}
	}
}

void AzadanieTestoweCharacter::Fire()
{
	FVector Start = FollowCamera->GetComponentLocation();
	FVector ForwardVector = FollowCamera->GetForwardVector();
	FVector End = Start + ForwardVector * 1000;
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 1);

	ServerFire(Start, End);
}

bool AzadanieTestoweCharacter::ServerFire_Validate(FVector start, FVector end)
{
	return true;
}
