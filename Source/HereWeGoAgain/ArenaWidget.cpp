#include "ArenaWidget.h"

#include "Components/Image.h"
#include "Components/SizeBox.h"

void UArenaWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ImageSizeBox)
	{
		// These override width/height on the sizebox
		ImageSizeBox->SetWidthOverride(50.f);
		ImageSizeBox->SetHeightOverride(50.f);
	}
	
	if (ArenaActivationImage)
	{
		if (DefaultTexture)
        {
        	ArenaActivationImage->SetBrushFromTexture(DefaultTexture);
        }
	}
}

void UArenaWidget::SetImageTexture(UTexture2D* NewTexture)
{
	if (ArenaActivationImage && NewTexture)
	{
		ArenaActivationImage->SetBrushFromTexture(NewTexture);
	}
}