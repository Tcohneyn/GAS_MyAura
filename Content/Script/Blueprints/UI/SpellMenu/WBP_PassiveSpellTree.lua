--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_PassiveSpellTree_C
local M = UnLua.Class()

function M:WidgetControllerSet()
    self.Button_HaloOfProtection:SetWidgetController(self.WidgetController)
    self.Button_LifeSiphon:SetWidgetController(self.WidgetController)
    self.Button_ManaSiphon:SetWidgetController(self.WidgetController)
end
function M:Construct()
   self.Button_HaloOfProtection.OnSpellGlobeSelected:Add(self, M.OnSpellGlobeSelected)
   self.Button_LifeSiphon.OnSpellGlobeSelected:Add(self, M.OnSpellGlobeSelected)
   self.Button_ManaSiphon.OnSpellGlobeSelected:Add(self, M.OnSpellGlobeSelected)
end

function M:OnSpellGlobeSelected(SelectedGlobe)
    self.Button_HaloOfProtection:Deselect()
    self.Button_LifeSiphon:Deselect()
    self.Button_ManaSiphon:Deselect()
    SelectedGlobe:Select()
    self.OnPassiveSpellGlobeSelected:Broadcast()
end
return M
