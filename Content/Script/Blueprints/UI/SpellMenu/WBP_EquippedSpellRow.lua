--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_EquippedSpellRow_C
local M = UnLua.Class()

function M:WidgetControllerSet()
    self:SetWidgetControllers()
end

function M:SetWidgetControllers()
    self.Globe_LMB:SetWidgetController(self.WidgetController)
    self.Globe_RMB:SetWidgetController(self.WidgetController)
    self.Globe_1:SetWidgetController(self.WidgetController)
    self.Globe_2:SetWidgetController(self.WidgetController)
    self.Globe_3:SetWidgetController(self.WidgetController)
    self.Globe_4:SetWidgetController(self.WidgetController)
    self.Globe_Passive_1:SetWidgetController(self.WidgetController)
    self.Globe_Passive_2:SetWidgetController(self.WidgetController)
end

return M
