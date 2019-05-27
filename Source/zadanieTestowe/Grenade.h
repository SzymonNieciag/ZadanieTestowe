// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grenade.generated.h"

UCLASS()
class ZADANIETESTOWE_API AGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrenade();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		class UStaticMeshComponent* GreanateMesh;

	UPROPERTY(Replicated,VisibleAnywhere, BlueprintReadWrite, Category = "Grenate")
		float ExplosiveRange;

	UPROPERTY(Replicated,VisibleAnywhere, BlueprintReadWrite, Category = "Grenate")
		float DamageAmount;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Destroyed()override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SetGrenateVelocity(FVector direction, float speed);
};
