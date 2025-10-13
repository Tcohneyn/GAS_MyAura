--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_TextValueRow_C
local M = UnLua.Class()

function M:SetLabelText(LabelText)
    self.TextBlock_Label:SetText(LabelText)
end

function M:SetNumericalValueInt(NewValue)
    self.WBP_FramedValue.TextBlock_Value:SetText(UE.UKismetTextLibrary.Conv_IntToText(NewValue))
end

function M:Construct()
   local AMController = UE.UAuraAbilitySystemLibrary.GetAttributeMenuWidgetController(self)
   AMController.AttributeInfoDelegate:Add(self, M.OnAttributeInfoReceived)
end

function M:OnAttributeInfoReceived(Info)
  if UE.UBlueprintGameplayTagLibrary.MatchesTag(Info.AttributeTag, self.AttributeTag, true) then
   --print(UE.UBlueprintGameplayTagLibrary.GetTagName(self.AttributeTag))
   self:SetLabelText(Info.AttributeName)
   self:SetNumericalValueInt(UE.UKismetMathLibrary.FTrunc(Info.AttributeValue))
  end
end 
return M
