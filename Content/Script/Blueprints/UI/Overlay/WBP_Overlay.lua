--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--
require("LuaPanda").start("127.0.0.1", 8818)
---@type WBP_Overlay_C
local M = UnLua.Class()

function M:WidgetControllerSet()
   UE.UAuraUserWidget.SetWidgetController(self.WBP_HealthGlobe, self.WidgetController)
   UE.UAuraUserWidget.SetWidgetController(self.WBP_ManaGlobe, self.WidgetController)
   --接收数据表行
   self.WidgetController:Cast(UE.UBP_OverlayWidgetController_C).MessageWidgetRowDelegate:Add(self, M.OnMessageWidgetRow)
end

function M:OnMessageWidgetRow(Row)
   --UE.UKismetSystemLibrary.PrintString(self,"Picked up a".. UE.UBlueprintGameplayTagLibrary.GetDebugStringFromGameplayTag(Row.MessageTag))
   local MyHUD = UE.UWidgetBlueprintLibrary.Create(self, Row.MessageWidget,
      UE.UGameplayStatics.GetPlayerController(self, 0))
    MyHUD:SetPositionInViewport(UE.UWidgetLayoutLibrary.GetViewportSize(self)*0.5,true)
   local HUD = MyHUD:Cast(UE.UWBP_EffectMessage_C)
   HUD:SetImageandText(Row.Image, Row.Message)
   HUD:AddToViewport()
end


function M:Construct()
   self.AttributeMenuButton.Button.OnClicked:Add(self, M.OnAttributeMenuButtonClicked)
end

function M:OnAttributeMenuButtonClicked()
   self.AttributeMenuButton.Button:SetIsEnabled(false)
   local PlayerController = UE.UGameplayStatics.GetPlayerController(self, 0)
   local AttributeMenu = UE.UWidgetBlueprintLibrary.Create(self, self.AttributeMenu, PlayerController)
   AttributeMenu:AddToViewport()
   AttributeMenu:SetPositionInViewport(UE.FVector2D(25.0,25.0),true)
   AttributeMenu.AttributeMenuClosed:Add(self, M.OnAttributeMenuClosed)  --蓝图中的委托本质是动态多播委托
end

function M:OnAttributeMenuClosed()
   self.AttributeMenuButton.Button:SetIsEnabled(true)
end
return M


