--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_ValueGlobe_C
local M = UnLua.Class()

function M:WidgetControllerSet()
    --Set Widget Controller
    self.BPOverlayWidgetController = self.WidgetController:Cast(UE.UBP_OverlayWidgetController_C)
    --Bind to Ability Info Delegate
    self.BPOverlayWidgetController.OnPlayerLevelChangedDelegate:Add(self, M.OnPlayerLevelChangedDelegate_Event)
end

function M:OnPlayerLevelChangedDelegate_Event(NewValue)
    self.Text_Value:SetText(UE.UKismetTextLibrary.Conv_IntToText(NewValue))
end

function M:Destruct()
    self.BPOverlayWidgetController.OnPlayerLevelChangedDelegate:Remove(self, M.OnPlayerLevelChangedDelegate_Event)
end
return M
