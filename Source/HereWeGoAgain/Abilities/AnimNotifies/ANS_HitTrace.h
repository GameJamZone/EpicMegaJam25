// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_HitTrace.generated.h"

namespace EDrawDebugTrace
{
	enum Type : int;
}

class UGameplayEffect;
/**
 * 
 */
UCLASS(Blueprintable, meta = (DisplayName = "Hit Trace"))
class HEREWEGOAGAIN_API UANS_HitTrace : public UAnimNotifyState
{
	GENERATED_UCLASS_BODY()

public:
	
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,  const FAnimNotifyEventReference& EventReference) override;
	void PerformSphereTraceDeferred(USkeletalMeshComponent* MeshComp, const FVector& WorldStart,
	                                const FVector& WorldEnd);

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "HitTrace")
	FName StartSocketName;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "HitTrace")
	FName EndSocketName;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "HitTrace")
	float Radius = 30.f;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "HitTrace")
	bool bDoHitStun = true;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "HitTrace")
	bool bShouldHitOnce = true;
	
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "HitTrace")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "HitTrace")
	float DamageAmount;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "HitTrace")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "HitTrace")
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType;
	
private:
	UPROPERTY()
	TArray<AActor*> ActorsToIgnore;
	
};
