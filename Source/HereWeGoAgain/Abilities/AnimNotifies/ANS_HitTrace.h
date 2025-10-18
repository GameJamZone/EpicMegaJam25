// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_HitTrace.generated.h"

/**
 * 
 */
UCLASS()
class HEREWEGOAGAIN_API UANS_HitTrace : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "HitTrace")
	FName StartSocketName;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "HitTrace")
	FName EndSocketName;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "HitTrace")
	float Radius = 30.f;
	
private:
	UPROPERTY()
	TArray<AActor*> ActorsToIgnore;
	
};
