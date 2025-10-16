// Fill out your copyright notice in the Description page of Project Settings.

#include "GASEnhancedInputComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GASEnhancedInputComponent)


UGASEnhancedInputComponent::UGASEnhancedInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UGASEnhancedInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}

	BindHandles.Reset();
}
