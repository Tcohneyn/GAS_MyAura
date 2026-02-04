--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_EquippedRow_Button_C
local M = UnLua.Class()

function M:WidgetControllerSet()
    --设置法术菜单控件控制器
    self.BPSpellMenuWidgetController = self.WidgetController:Cast(UE.UBP_SpellMenuWidgetController_C)
    --接受能力信息委托
    self.BPSpellMenuWidgetController.AbilityInfoDelegate:Add(self, M.ReceiveAbilityInfo)

    self.Button_Ring.OnClicked:Add(self, M.OnRingClicked)
end

function M:ReceiveAbilityInfo(AbilityInfo)
    if UE.UBlueprintGameplayTagLibrary.MatchesTag(AbilityInfo.InputTag, self.InputTag, true) then
        if UE.UBlueprintGameplayTagLibrary.MatchesTag(AbilityInfo.AbilityTag, self.NoneTag, true) then
            self:ClearGlobe()
        else
            print(UE.UBlueprintGameplayTagLibrary.GetTagName(AbilityInfo.InputTag))
            self:SetSlateBrush(AbilityInfo.Icon, AbilityInfo.BackgroundMaterial)
        end
    end
end

function M:OnRingClicked()
    self.BPSpellMenuWidgetController:SpellRowGlobePressed(self.InputTag, self.AbilityType)
end

function M:Destruct()
    self.BPSpellMenuWidgetController.AbilityInfoDelegate:Remove(self, M.ReceiveAbilityInfo)
end
return M
