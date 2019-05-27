// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyInterface.h"
#include "simpleLight.generated.h"

UCLASS()
class ZADANIETESTOWE_API AsimpleLight : public AActor, public IMyInterface
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* LightMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Light", meta = (AllowPrivateAccess = "true"))
		class UPointLightComponent* LightPoint;

public:	

	// Sets default values for this actor's properties
	AsimpleLight();


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "MyCategory")
		bool SimpleInteraction();
	virtual bool SimpleInteraction_Implementation() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void EnableLight();

	void DisableLight();

	UPROPERTY(ReplicatedUsing  = OnRep_LightStatus, BlueprintReadOnly)
		bool isEnableLight = false;

	UPROPERTY(ReplicatedUsing = OnRep_LinearColorStatus, BlueprintReadOnly)
		FLinearColor linearColor;

	FTimerHandle loopTimerHandler;

	UFUNCTION(BlueprintCallable)
		void SetLightColor();

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UFUNCTION()
		void OnRep_LightStatus();
	UFUNCTION()
		void OnRep_linearColorStatus();
};