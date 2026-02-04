--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_SpellMenu_C
local M = UnLua.Class()

function M:Construct()
    --处理关闭按钮点击事件
    self.CloseButton.Button.OnClicked:Add(self, M.OnCloseButtonClicked)
    --法术球已选择
    self.WBP_OffensiveSpellTree.OnOffensiveSpellGlobeSelected:Add(self, M.OnOffensiveSpellGlobeSelected)
    self.WBP_PassiveSpellTree.OnPassiveSpellGlobeSelected:Add(self, M.OnPassiveSpellGlobeSelected)
    --设置法术菜单控件控制器
    local SpellMenuWC = UE.UAuraAbilitySystemLibrary.GetSpellMenuWidgetController(self)
    self.BPSpellMenuWC = SpellMenuWC:Cast(UE.UBP_SpellMenuWidgetController_C)
    self:SetWidgetController(self.BPSpellMenuWC)
    --根据技能状态和技能点启用相应按钮
    self.BPSpellMenuWC.SpellGlobeSelectedDelegate:Add(self, M.OnSpellGlobeSelected)
    --等待装备委托
    self.BPSpellMenuWC.WaitForEquipDelegate:Add(self, M.OnWaitForEquip)
    --​停止等待装备委托
    self.BPSpellMenuWC.StopWaitingForEquipDelegate:Add(self, M.OnStopWaitingForEquip)
    --​加点按钮已按下
    self.Button_SpendPoint.Button.OnClicked:Add(self, M.OnSpendPointClicked)
    --准备按钮按下
    self.Button_Equip.Button.OnClicked:Add(self, M.OnEquipButtonClicked)
    --设置子控件（Widget）控制器
    self.WBP_EquippedSpellRow:SetWidgetController(self.WidgetController)
    self.WBP_OffensiveSpellTree:SetWidgetController(self.WidgetController)
    self.WBP_PassiveSpellTree:SetWidgetController(self.WidgetController)
    --print("Spell Menu Widget Controller Set in Spell Menu" .. UE.UKismetSystemLibrary.GetObjectName(BPSpellMenuWC))
    --技能点数变更时
    self.BPSpellMenuWC.SpellPointsChanged:Add(self, M.SpellPointsChanged)
    --广播初始值
    self.BPSpellMenuWC:BroadcastInitialValues()
end

--处理关闭按钮点击事件
function M:OnCloseButtonClicked()
    self:RemoveFromParent()
end

--法术球已选择
function M:OnOffensiveSpellGlobeSelected()
    self.WBP_PassiveSpellTree:DeselectAll()
end

function M:OnPassiveSpellGlobeSelected()
    self.WBP_OffensiveSpellTree:DeselectAll()
end

--根据技能状态和技能点启用相应按钮
function M:OnSpellGlobeSelected(bSpendPointsButtonEnabled, bEquipButtonEnabled, DescriptionString,
                                NextLevelDescriptionString)
    self:SetButtonsEnabled(bSpendPointsButtonEnabled, bEquipButtonEnabled)
    self.RichText_Description:SetText(DescriptionString)
    self.RichText_NextLevelDescription:SetText(NextLevelDescriptionString)
end

--等待装备委托
function M:OnWaitForEquip(AbilityType)
    local bIsOffensive = UE.UBlueprintGameplayTagLibrary.MatchesTag(AbilityType, self.OffensiveTag, true)
    self:SetButtonsEnabled(false, false)
    if bIsOffensive then
        self.WBP_EquippedSpellRow:PlayAnimation(self.WBP_EquippedSpellRow.OffensiveSelectionAnimation,0.0,0)
    else
        self.WBP_EquippedSpellRow:PlayAnimation(self.WBP_EquippedSpellRow.PassiveSelectionAnimation)
    end
end

--​停止等待装备委托
function M:OnStopWaitingForEquip(AbilityType)
    local bIsOffensive = UE.UBlueprintGameplayTagLibrary.MatchesTag(AbilityType, self.OffensiveTag, true)
    self.WBP_EquippedSpellRow:StopAllAnimations()
    if bIsOffensive then
        self.WBP_EquippedSpellRow:PlayAnimation(self.WBP_EquippedSpellRow.HideOffensiveBox)
    else
        self.WBP_EquippedSpellRow:PlayAnimation(self.WBP_EquippedSpellRow.HidePassiveBox)
    end
end
--​加点按钮已按下
function M:OnSpendPointClicked()
    self.BPSpellMenuWC:SpendPointButtonPressed()
end

--准备按钮按下
function M:OnEquipButtonClicked()
    self.BPSpellMenuWC:EquipButtonPressed()
end

--技能点数变更时
function M:SpellPointsChanged(SpellPoints)
    self.FramedValue_SpellPoints.TextBlock_Value:SetText(SpellPoints)
end

function M:Destruct()
    self.SpellMenuClosed:Broadcast()
    self.BPSpellMenuWC.SpellGlobeSelectedDelegate:Remove(self, M.OnSpellGlobeSelected)
    self.BPSpellMenuWC.WaitForEquipDelegate:Remove(self, M.OnWaitForEquip)
    self.BPSpellMenuWC.StopWaitingForEquipDelegate:Remove(self, M.OnStopWaitingForEquip)
end

return M
