// Fill out your copyright notice in the Description page of Project Settings.

#include "simpleLight.h"
#include <Components/StaticMeshComponent.h>
#include <Components/PointLightComponent.h>

#include "Net/UnrealNetwork.h"
#include <GameFramework/Actor.h>


// Sets default values
AsimpleLight::AsimpleLight()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	LightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MyMesh"));
	RootComponent = LightMesh;
	LightPoint = CreateDefaultSubobject<UPointLightComponent>(TEXT("LightPoint"));
	LightPoint->SetupAttachment(RootComponent);
	LightPoint->SetRelativeLocation(FVector(0,0,0));
	LightPoint->Intensity = 2000;
	LightPoint->AttenuationRadius = 500;
	isEnableLight = 0;
	LightPoint->SetHiddenInGame(true);
}

bool AsimpleLight::SimpleInteraction_Implementation()
{
	isEnableLight = !isEnableLight;
	if (isEnableLight)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("lampka zaswiec"));
		OnRep_LightStatus();
		return true;

	}
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("lampka nie swieci"));
	OnRep_LightStatus();
	return false;
}

//	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("lampka"));

void AsimpleLight::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(loopTimerHandler, this, &AsimpleLight::SetLightColor, 2.f, true);

}
void AsimpleLight::EnableLight()
{
	LightPoint->SetHiddenInGame(false);
	if (Role != ROLE_AutonomousProxy)
	{
		isEnableLight = true;
	}
}

void AsimpleLight::DisableLight()
{
	LightPoint->SetHiddenInGame(true);
	if (Role != ROLE_AutonomousProxy)
	{
		isEnableLight = false;
	}
}


void AsimpleLight::OnRep_LightStatus()
{
	if (isEnableLight)
	{
		EnableLight();
	}
	else
		DisableLight();
}

void AsimpleLight::OnRep_linearColorStatus()
{
	LightPoint->SetLightColor(linearColor);
}

void AsimpleLight::SetLightColor()
{
	if ((Role == ROLE_Authority) && (isEnableLight))
	{
		float R, G, B;

		R = FMath::FRandRange(0, 1);
		G = FMath::FRandRange(0, 1);
		B = FMath::FRandRange(0, 1);

		linearColor = FLinearColor(R, G, B);

		OnRep_linearColorStatus();
	}
}

// Called every frame
void AsimpleLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AsimpleLight::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AsimpleLight, isEnableLight);
	DOREPLIFETIME(AsimpleLight, linearColor);
}
