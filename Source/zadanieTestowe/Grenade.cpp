// Fill out your copyright notice in the Description page of Project Settings.

#include "Grenade.h"
#include <Components/StaticMeshComponent.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/Actor.h>
#include <Engine/Engine.h>
#include <UnrealString.h>
#include <GameFramework/DamageType.h>
#include <UnrealNetwork.h>


// Sets default values
AGrenade::AGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GreanateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenateMesh"));
	RootComponent = GreanateMesh;

	this->SetReplicates(true);
	this->SetReplicateMovement(true);
	GreanateMesh->SetIsReplicated(true);
	GreanateMesh->SetSimulatePhysics(true);
	GreanateMesh->SetEnableGravity(true);
	GreanateMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GreanateMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	ExplosiveRange = 500;
	DamageAmount = 10;
}

// Called when the game starts or when spawned
void AGrenade::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(5);

}

void AGrenade::Destroyed()
{
	Super::Destroyed();
	if (HasAuthority())
	{
		float SquaredExplosiveRange = FMath::Pow(ExplosiveRange, 2);
		TArray<AActor*> AllActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
		TArray<AActor*> AvailableActors;

		for (AActor* actor : AllActors)
		{
			float Range = FVector::DistSquared(GetActorLocation(), actor->GetActorLocation());
			if (Range < SquaredExplosiveRange)
			{
				AvailableActors.Add(actor);
			}
		}
		int a = AvailableActors.Num();
		GEngine->AddOnScreenDebugMessage(-1, 30, FColor::Yellow, FString::Printf(TEXT("Overlaped Actors %d"), a));

		for (AActor* actor : AvailableActors)
		{
			TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
			FDamageEvent DamageEvent(ValidDamageTypeClass);

			if (GetInstigator() != nullptr)
			{
				actor->TakeDamage(DamageAmount, DamageEvent, GetInstigator()->GetController(), GetInstigator());
			}
		}
	}
}

// Called every frame
void AGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGrenade::SetGrenateVelocity(FVector direction, float speed)
{
	GreanateMesh->SetPhysicsLinearVelocity(direction*speed);
}
void AGrenade::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGrenade, ExplosiveRange);
	DOREPLIFETIME(AGrenade, DamageAmount);
}

