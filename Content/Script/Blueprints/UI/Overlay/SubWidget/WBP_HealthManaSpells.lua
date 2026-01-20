--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_HealthManaSpells_C
local M = UnLua.Class()

function M:WidgetControllerSet()
   self.WBP_HealthGlobe:SetWidgetController(self.WidgetController)
   self.WBP_ManaGlobe:SetWidgetController(self.WidgetController)
   self:SetGlobeWidgetControllers()
end


return M
