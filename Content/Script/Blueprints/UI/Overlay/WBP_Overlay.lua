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
   UE.UAuraUserWidget.SetWidgetController(self.WBP_HealthManaSpells, self.WidgetController)
   UE.UAuraUserWidget.SetWidgetController(self.WBP_XPBar, self.WidgetController)
   UE.UAuraUserWidget.SetWidgetController(self.ValueGlobe_Level, self.WidgetController)
   --接收数据表行
   self.BPOverlayWidgetController = self.WidgetController:Cast(UE.UBP_OverlayWidgetController_C)
   self.BPOverlayWidgetController.MessageWidgetRowDelegate:Add(self, M.OnMessageWidgetRow)
   self.BPOverlayWidgetController.OnPlayerLevelChangedDelegate:Add(self, M.OnPlayerLevelChanged)
end

function M:OnMessageWidgetRow(Row)
   --UE.UKismetSystemLibrary.PrintString(self,"Picked up a".. UE.UBlueprintGameplayTagLibrary.GetDebugStringFromGameplayTag(Row.MessageTag))
   local MyHUD = UE.UWidgetBlueprintLibrary.Create(self, Row.MessageWidget,
      UE.UGameplayStatics.GetPlayerController(self, 0))
   MyHUD:SetPositionInViewport(UE.UWidgetLayoutLibrary.GetViewportSize(self) * 0.5, true)
   local HUD = MyHUD:Cast(UE.UWBP_EffectMessage_C)
   HUD:SetImageandText(Row.Image, Row.Message)
   HUD:AddToViewport()
end

function M:OnPlayerLevelChanged(NewLevel)
   if self.MyHUD and self.MyHUD:IsValid() then
      self.MyHUD:RemoveFromParent()
   else
      self.MyHUD = UE.UWidgetBlueprintLibrary.Create(self, self.Class,
         self.BPOverlayWidgetController.PlayerController)
      self.MyHUD.Text_Level:SetText(UE.UKismetTextLibrary.Conv_IntToText(NewLevel))
      self.MyHUD:AddToViewport()
   end
end

function M:Construct()
   self.AttributeMenuButton.Button.OnClicked:Add(self, M.OnAttributeMenuButtonClicked)
   self.SpellMenuButton.Button.OnClicked:Add(self, M.OnSpellMenuButtonClicked)
end

function M:OnAttributeMenuButtonClicked()
   self.AttributeMenuOpen = true
   self.AttributeMenuButton.Button:SetIsEnabled(false)
   local PlayerController = UE.UGameplayStatics.GetPlayerController(self, 0)
   local AttributeMenu = UE.UWidgetBlueprintLibrary.Create(self, self.AttributeMenu, PlayerController)
   AttributeMenu:AddToViewport()
   AttributeMenu:SetPositionInViewport(UE.FVector2D(self.MenuPadding, self.MenuPadding), true)
   AttributeMenu.AttributeMenuClosed:Add(self, M.OnAttributeMenuClosed) --蓝图中的委托本质是动态多播委托
   UE.UWidgetBlueprintLibrary.SetInputMode_UIOnlyEx(UE.UGameplayStatics.GetPlayerController(self, 0))
end

function M:OnSpellMenuButtonClicked()
   self.SpellMenuOpen = true
   self.SpellMenuButton.Button:SetIsEnabled(false)
   local PlayerController = UE.UGameplayStatics.GetPlayerController(self, 0)
   local SpellMenu = UE.UWidgetBlueprintLibrary.Create(self, self.SpellMenu, PlayerController)
   SpellMenu:AddToViewport()
   local ViewportSize = UE.UWidgetLayoutLibrary.GetViewportSize(self)
   local MenuSize = UE.FVector2D(ViewportSize.x - SpellMenu.SizeBox_Root.WidthOverride - self.MenuPadding,
      self.MenuPadding)
   SpellMenu:SetPositionInViewport(MenuSize, true)
   SpellMenu.SpellMenuClosed:Add(self, M.OnSpellMenuClosed) --蓝图中的委托本质是动态多播委托
   --UE.UWidgetBlueprintLibrary.SetInputMode_UIOnlyEx(UE.UGameplayStatics.GetPlayerController(self, 0))
end

function M:OnAttributeMenuClosed()
   self.AttributeMenuButton.Button:SetIsEnabled(true)
   self.AttributeMenuOpen = false
   if not self.AttributeMenuOpen then
      UE.UWidgetBlueprintLibrary.SetInputMode_GameAndUIEx(UE.UGameplayStatics.GetPlayerController(self, 0), nil,
         UE.EMouseLockMode.DoNotLock, false)
   end
end

function M:OnSpellMenuClosed()
   self.SpellMenuButton.Button:SetIsEnabled(true)
   -- self.SpellMenuOpen = false
   -- if not self.SpellMenuOpen then
   --    UE.UWidgetBlueprintLibrary.SetInputMode_GameAndUIEx(UE.UGameplayStatics.GetPlayerController(self, 0), nil,
   --             UE.EMouseLockMode.DoNotLock, false)
   -- end
end

return M
