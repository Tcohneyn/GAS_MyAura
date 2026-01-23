--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_TextValueButtonRow_C
local M = UnLua.Class("Blueprints.UI.AttributeMenu.WBP_TextValueRow")

function M:SetLabelText(LabelText)
    self.TextBlock_Label:SetText(LabelText)
end

function M:SetNumericalValueInt(NewValue)
    self.WBP_FramedValue.TextBlock_Value:SetText(UE.UKismetTextLibrary.Conv_IntToText(NewValue))
end

function M:Construct()
   self.AMController = UE.UAuraAbilitySystemLibrary.GetAttributeMenuWidgetController(self)
   self.AMController.AttributeInfoDelegate:Add(self, M.OnAttributeInfoReceived)
    self.WBP_Button.Button.OnClicked:Add(self, M.OnButtonClicked)
end

function M:OnAttributeInfoReceived(Info)
  if UE.UBlueprintGameplayTagLibrary.MatchesTag(Info.AttributeTag, self.AttributeTag, true) then
   --print(UE.UBlueprintGameplayTagLibrary.GetTagName(self.AttributeTag))
   self:SetLabelText(Info.AttributeName)
   self:SetNumericalValueInt(UE.UKismetMathLibrary.FTrunc(Info.AttributeValue))
  end
end 

function M:OnButtonClicked()
print("Upgrade Button Clicked for Attribute: "..UE.UBlueprintGameplayTagLibrary.GetTagName(self.AttributeTag))
   if self.AMController then
      self.AMController:UpgradeAttribute(self.AttributeTag)
   end
end

return M
