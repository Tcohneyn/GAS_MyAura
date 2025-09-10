// Tcohneyn All Rights Reserved


#include "UI/Widget/AuraUserWidget.h"

void UAuraUserWidget::SetWidgetController(UObject* InWidgetController)
{
    WidgetController = InWidgetController;
    if (WidgetController != nullptr)
    {
        WidgetControllerSet();
        //UE_LOG(LogTemp, Display, TEXT("WidgetController: %s"), *WidgetController->GetName());
    }
}