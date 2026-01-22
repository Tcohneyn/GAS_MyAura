--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_LevelUpMessage_C
local M = UnLua.Class()


function M:Construct()
   UE.UGameplayStatics.PlaySound2D(self, self.LevelUpSound)
   self:PlayAnimation(self.MessageAnimation)
       coroutine.resume(coroutine.create(
     function()    
            UE.UKismetSystemLibrary.Delay(self,3.0)
            self:RemoveFromParent()
        end
    ))
end


return M
