--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_TextValueButtonRow_C
local M = UnLua.Class("Blueprints.UI.AttributeMenu.WBP_TextValueRow")

-- function M:Construct()
--   self.Super.Construct(self)
--   --  local AMController = UE.UAuraAbilitySystemLibrary.GetAttributeMenuWidgetController(self)
--   --  AMController.AttributeInfoDelegate:Add(self, M.OnAttributeInfoReceived)
-- end

-- function M:OnAttributeInfoReceived(Info)
--   --self.Super.OnAttributeInfoReceived(self)
--   -- if UE.UBlueprintGameplayTagLibrary.MatchesTag(Info.AttributeTag, self.AttributeTag, true) then
--   --  print(UE.UBlueprintGameplayTagLibrary.GetTagName(self.AttributeTag))
--   --  self:SetLabelText(Info.AttributeName)
--   --  self:SetNumericalValueInt(UE.UKismetMathLibrary.FTrunc(Info.AttributeValue))
--   -- end
-- end 
return M
