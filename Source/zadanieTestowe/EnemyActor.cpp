// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyActor.h"
#include <../Plugins/Runtime/ApexDestruction/Source/ApexDestruction/Public/DestructibleComponent.h>
#include <Components/BoxComponent.h>
#include <DrawDebugHelpers.h>
#include <UnrealNetwork.h>



// Sets default values
AEnemyActor::AEnemyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("myBox"));
	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DestructibleComponent"));
	RootComponent = BoxComponent;
	DestructibleComponent->SetupAttachment(RootComponent);
	DestructibleComponent->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator(0, 0, 0));

	this->SetReplicates(true);
	DestructibleComponent->SetIsReplicated(true);
	DestructibleComponent->SetGenerateOverlapEvents(false);

	Healt = 3;
}

bool AEnemyActor::SimpleInteraction_Implementation()
{
	Healt = Healt - 1;
	OnRep_LifeStatus();
	return true;
}

// Called when the game starts or when spawned
void AEnemyActor::BeginPlay()
{
	Super::BeginPlay();
}


float AEnemyActor::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Healt = Healt - DamageAmount;
	OnRep_LifeStatus();

	return DamageAmount;
}

// Called every frame
void AEnemyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawDebugString(GetWorld(), FVector(0, 0, 100), FString::FromInt(Healt), this, FColor::White, DeltaTime);

}
void AEnemyActor::OnRep_LifeStatus()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("enemy attack"));
	if (Healt <= 0)
	{
		DestructibleComponent->ApplyDamage(5, GetActorLocation(), FVector(0, 0, -1), 1000);
		DestructibleComponent->SetEnableGravity(false);
	}
}

void AEnemyActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemyActor, Healt);
}

