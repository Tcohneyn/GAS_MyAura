--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type BP_DamageTextComponent_C
local M = UnLua.Class()

function M:SetDamageText(Damage)
   local WObject = self:GetUserWidgetObject()
   local DamageText = WObject:Cast(UE.UWBP_DamageText_C)
   DamageText:UpdateDamageText(Damage)
   coroutine.resume(coroutine.create(
        function()    
            --UE.UKismetSystemLibrary.Delay(self,5.0)
            UE.UKismetSystemLibrary.Delay(self,2.0)
            --print("Destroy Damage Text Component")
            UE.UActorComponent.K2_DestroyComponent(self,self)
        end
    ))
end

return M
