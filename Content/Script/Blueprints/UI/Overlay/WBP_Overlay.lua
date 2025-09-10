--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--
require("LuaPanda").start("127.0.0.1",8818)
---@type WBP_Overlay_C
local M = UnLua.Class()

function M:WidgetControllerSet()
   UE.UAuraUserWidget.SetWidgetController(self.WBP_HealthGlobe,self.WidgetController)
   UE.UAuraUserWidget.SetWidgetController(self.WBP_ManaGlobe,self.WidgetController)
end
return M
