#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "ArenaWidget.generated.h"

class USizeBox;

/**
 * 
 */
UCLASS()
class HEREWEGOAGAIN_API UArenaWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	USizeBox* ImageSizeBox;
	
	// The image widget (binds to the image in the UMG designer)
	UPROPERTY(meta = (BindWidget))
	UImage* ArenaActivationImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Image")
	UTexture2D* DefaultTexture;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Image")
	UTexture2D* ActiveArenaTexture;
	
	UFUNCTION(BlueprintCallable, Category = "Image")
	void SetImageTexture(UTexture2D* NewTexture);

protected:
	virtual void NativeConstruct() override;
};
