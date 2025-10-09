--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_TextValueButtonRow_C
local M = UnLua.Class("Blueprints.UI.AttributeMenu.WBP_TextValueRow")

function M:Construct()
   local AMController = UE.UAuraAbilitySystemLibrary.GetAttributeMenuWidgetController(self)
   AMController.AttributeInfoDelegate:Add(self, M.OnAttributeInfoReceived)
end

function M:OnAttributeInfoReceived(Info)
  --print(UE.UBlueprintGameplayTagLibrary.GetTagName(self.AttributeTag))
  if UE.UBlueprintGameplayTagLibrary.MatchesTag(Info.AttributeTag, self.AttributeTag, true) then
   self:SetLabelText(Info.AttributeName)
   self:SetNumericalValueInt(UE.UKismetMathLibrary.FTrunc(Info.AttributeValue))
  end
end 
return M
