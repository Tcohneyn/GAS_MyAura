--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_OffensiveSpellTree_C
local M = UnLua.Class()

function M:WidgetControllerSet()
    self.WBP_SpellGlobe_Button:SetWidgetController(self.WidgetController)
    self.WBP_SpellGlobe_Button_1:SetWidgetController(self.WidgetController)
    self.WBP_SpellGlobe_Button_2:SetWidgetController(self.WidgetController)
    self.WBP_SpellGlobe_Button_3:SetWidgetController(self.WidgetController)
    self.WBP_SpellGlobe_Button_4:SetWidgetController(self.WidgetController)
    self.WBP_SpellGlobe_Button_5:SetWidgetController(self.WidgetController)
    self.WBP_SpellGlobe_Button_6:SetWidgetController(self.WidgetController)
    self.WBP_SpellGlobe_Button_7:SetWidgetController(self.WidgetController)
    self.WBP_SpellGlobe_Button_8:SetWidgetController(self.WidgetController)
end

function M:Construct()
    --self:WidgetControllerSet()
    self.WBP_SpellGlobe_Button.OnSpellGlobeSelected:Add(self, M.OnSpellGlobeSelected)
    self.WBP_SpellGlobe_Button_1.OnSpellGlobeSelected:Add(self, M.OnSpellGlobeSelected)
    self.WBP_SpellGlobe_Button_2.OnSpellGlobeSelected:Add(self, M.OnSpellGlobeSelected)
    self.WBP_SpellGlobe_Button_3.OnSpellGlobeSelected:Add(self, M.OnSpellGlobeSelected)
    self.WBP_SpellGlobe_Button_4.OnSpellGlobeSelected:Add(self, M.OnSpellGlobeSelected)
    self.WBP_SpellGlobe_Button_5.OnSpellGlobeSelected:Add(self, M.OnSpellGlobeSelected)
    self.WBP_SpellGlobe_Button_6.OnSpellGlobeSelected:Add(self, M.OnSpellGlobeSelected)
    self.WBP_SpellGlobe_Button_7.OnSpellGlobeSelected:Add(self, M.OnSpellGlobeSelected)
    self.WBP_SpellGlobe_Button_8.OnSpellGlobeSelected:Add(self, M.OnSpellGlobeSelected)
end

function M:OnSpellGlobeSelected(SelectedGlobe)
    self.WBP_SpellGlobe_Button:Deselect()
    self.WBP_SpellGlobe_Button_1:Deselect()
    self.WBP_SpellGlobe_Button_2:Deselect()
    self.WBP_SpellGlobe_Button_3:Deselect()
    self.WBP_SpellGlobe_Button_4:Deselect()
    self.WBP_SpellGlobe_Button_5:Deselect()
    self.WBP_SpellGlobe_Button_6:Deselect()
    self.WBP_SpellGlobe_Button_7:Deselect()
    self.WBP_SpellGlobe_Button_8:Deselect()
    SelectedGlobe:Select()
    self.OnOffensiveSpellGlobeSelected:Broadcast()
end

return M
