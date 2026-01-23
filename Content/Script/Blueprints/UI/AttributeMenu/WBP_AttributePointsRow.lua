--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_AttributePointsRow_C
local M = UnLua.Class()

function M:WidgetControllerSet()
   --接收数据表行
   self.BPAttributeMenuController = self.WidgetController:Cast(UE.UBP_AttributeMenuWidgetController_C)
   if self.BPAttributeMenuController then
        print("Attribute Menu Widget Controller Set Successfully")
     else
        print("Failed to Set Attribute Menu Widget Controller")
        return
     end
   self.BPAttributeMenuController.AttributePointsChangedDelegate:Add(self, M.OnAttributePointsReceived)
end

function M:OnAttributePointsReceived(NewAttributePoints)
   print("Attribute Points Changed: "..NewAttributePoints)
   self.WBP_FramedValue.TextBlock_Value:SetText(UE.UKismetTextLibrary.Conv_IntToText(NewAttributePoints))
end
return M
