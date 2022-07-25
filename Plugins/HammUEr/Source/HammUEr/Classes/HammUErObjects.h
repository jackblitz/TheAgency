// Copyright 2015-2021 Turfster / NT Entertainment
// This is very early days stuff.
// Some of these are still kinda broken at the moment, so don't go expecting too much

#pragma once

#include "CoreMinimal.h"
#include "HammUErRuntimeClasses.h"
#include "Runtime/Engine/Classes/Components/PrimitiveComponent.h"
#include "HammUErObjects.generated.h"


UCLASS()
class UHammuerPathComponent : public UPrimitiveComponent
{
	GENERATED_UCLASS_BODY()
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
};

UCLASS()
class AHammuerPathNode : public AActor
{
	GENERATED_UCLASS_BODY()
	
public: 
	virtual void Tick(float DeltaSeconds) override;
	virtual bool ShouldTickIfViewportsOnly() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HammUEr")
	TObjectPtr<AHammuerPathNode> nextTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HammUEr")
	FColor pathColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HammUEr")
	float pathThickness;

	UHammuerPathComponent* myPathComponent;

	FString myTargetName;

	UFUNCTION()
	bool CanDraw()
	{
		return (nextTarget != NULL);
	}
};

