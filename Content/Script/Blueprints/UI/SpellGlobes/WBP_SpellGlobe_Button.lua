--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_SpellGlobe_Button_C
local M = UnLua.Class()

function M:WidgetControllerSet()
    --Set Widget Controller
    self.BPSpellMenuWidgetController = self.WidgetController:Cast(UE.UBP_SpellMenuWidgetController_C)
    --Bind to Ability Info Delegate
    self.BPSpellMenuWidgetController.AbilityInfoDelegate:Add(self, M.ReceiveAbilityInfo)
    self.BPSpellMenuWidgetController.SpellGlobeReassignedDelegate:Add(self, M.SpellGlobeReassigned)
    self.Button_Ring.OnClicked:Add(self, M.OnRingClicked)
end

function M:ReceiveAbilityInfo(AbilityInfo)
    if (UE.UBlueprintGameplayTagLibrary.MatchesTag(AbilityInfo.AbilityTag, self.AbilityTag, true)) then
        if (UE.UBlueprintGameplayTagLibrary.MatchesTag(AbilityInfo.StatusTag, self.Locked, true)) then
            self:SetGlobeLocked()
        else
            if (UE.UBlueprintGameplayTagLibrary.MatchesTag(AbilityInfo.StatusTag, self.Equipped, true) or UE.UBlueprintGameplayTagLibrary.MatchesTag(AbilityInfo.StatusTag, self.Unlocked, true)) then
                print("Receiving Ability Info in Spell Globe Button for " ..
                    UE.UBlueprintGameplayTagLibrary.GetTagName(AbilityInfo.StatusTag))
                self:SetGlobeEquippedorUnlocked(AbilityInfo.Icon, AbilityInfo.BackgroundMaterial)
            else
                if (UE.UBlueprintGameplayTagLibrary.MatchesTag(AbilityInfo.StatusTag, self.Eligible, true)) then
                    self:SetGlobeEligible(AbilityInfo.Icon)
                end
            end
        end
    end
end

function M:OnRingClicked()
    if self.Selected then
        self:Deselect()
        self.BPSpellMenuWidgetController:GlobeDeselect()
        UE.UGameplayStatics.PlaySound2D(self, self.CancelSound)
    else
        self.OnSpellGlobeSelected:Broadcast(self)
        self.BPSpellMenuWidgetController:SpellGlobeSelected(self.AbilityTag)
    end
end

function M:SpellGlobeReassigned(AbilityTag)
    if (UE.UBlueprintGameplayTagLibrary.MatchesTag(AbilityTag, self.AbilityTag, true)) then
        self.Image_Selection:SetRenderOpacity(0.0)
        UE.UGameplayStatics.PlaySound2D(self, self.ReassignSound)
        self.Selected = false
    end
end

function M:Destruct()
    self.BPSpellMenuWidgetController.AbilityInfoDelegate:Remove(self, M.ReceiveAbilityInfo)
    self.BPSpellMenuWidgetController.SpellGlobeReassignedDelegate:Remove(self, M.SpellGlobeReassigned)
end
return M
