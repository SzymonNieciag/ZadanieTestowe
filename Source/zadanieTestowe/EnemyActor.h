// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyInterface.h"
#include "EnemyActor.generated.h"

UCLASS()
class ZADANIETESTOWE_API AEnemyActor : public AActor, public IMyInterface
{
	GENERATED_BODY()
public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
		class UBoxComponent* BoxComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
		class UDestructibleComponent* DestructibleComponent;
	// Sets default values for this actor's properties
	AEnemyActor();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "MyCategory")
		bool SimpleInteraction();
	virtual bool SimpleInteraction_Implementation() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(ReplicatedUsing = OnRep_LifeStatus, BlueprintReadOnly, Category = "Enemy Stat")
		int Healt;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION()
		void OnRep_LifeStatus();
};
