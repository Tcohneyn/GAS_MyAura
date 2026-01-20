// Tcohneyn All Rights Reserved


#include "AbilitySystem/AbilityTasks/WaitCooldownChange.h"
#include "AbilitySystemComponent.h"

/**
 * 静态工厂函数：用于创建并初始化异步任务实例
 * @param AbilitySystemComponent (ASC) - 需要监听的目标能力系统组件
 * @param InCooldownTag - 需要监听的具体冷却标签（例如 Cooldown.Fire.Firebolt）
 */
UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& InCooldownTag)
{
    // 1. 创建任务对象实例
	UWaitCooldownChange* WaitCooldownChange = NewObject<UWaitCooldownChange>();
	WaitCooldownChange->ASC = AbilitySystemComponent;
	WaitCooldownChange->CooldownTag = InCooldownTag;
    // 2. 有效性检查：如果 ASC 无效或标签无效，直接结束任务并返回空
	if (!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid())
	{
		WaitCooldownChange->EndTask();
		return nullptr;
	}
    // 3. 监听冷却结束（Tag 移除）
    // RegisterGameplayTagEvent 用于监听特定 Tag 的计数变化。
    // EGameplayTagEventType::NewOrRemoved 表示我们只关心 Tag 被添加（首次出现）或被移除（完全消失）的情况。
    // 这里主要利用它来检测 Tag 计数变为 0 的时刻（即冷却结束）。
	// 用于知晓一个技能的冷却时间何时结束（即其对应的冷却标签已被移除）
	AbilitySystemComponent->RegisterGameplayTagEvent(
		InCooldownTag,
		EGameplayTagEventType::NewOrRemoved).AddUObject(
			WaitCooldownChange,
			&UWaitCooldownChange::CooldownTagChanged);
    // 4. 监听冷却开始（GE 应用）
    // OnActiveGameplayEffectAddedDelegateToSelf 是一个广播委托，当任何拥有“持续时间”的 Gameplay Effect (GE) 被应用到自己身上时触发。
    // 我们需要利用这个回调来捕获冷却开始的瞬间，并计算剩余时间。
	// 知晓或判定一个技能的冷却效果何时被成功施加到目标上
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownChange, &UWaitCooldownChange::OnActiveEffectAdded);
    // 返回任务对象，蓝图节点的 Execution Pin 会在此处继续执行
	return WaitCooldownChange;
}
/**
 * 结束任务并清理资源
 * 通常在 UI 被销毁或不再需要监听时调用
 */
void UWaitCooldownChange::EndTask()
{
	if (!IsValid(ASC)) return;
    // 移除 Tag 变化的监听绑定，防止内存泄漏或无效调用
	ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
    // 标记任务为“准备销毁”，允许垃圾回收系统回收此对象
	SetReadyToDestroy();
	MarkAsGarbage();
}
/**
 * 回调函数：当冷却标签的数量发生变化时触发
 * @param NewCount 新的标签数量
 */
void UWaitCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		CooldownEnd.Broadcast(0.f);
	}
}
/**
 * 回调函数：当有新的持续性 GE 应用到 ASC 时触发
 * 用于检测冷却是否开始
 */
void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
    // 1. 获取新应用 GE 的所有标签
    FGameplayTagContainer AssetTags;
    SpecApplied.GetAllAssetTags(AssetTags); // 获取 GE 资产标签

    FGameplayTagContainer GrantedTags;
    SpecApplied.GetAllGrantedTags(GrantedTags); // 获取 GE 授予给角色的标签

    // 2. 检查此 GE 是否包含我们要监听的 CooldownTag
    // 冷却 GE 通常会将冷却标签放在 GrantedTags 中（授予给角色），或者作为 AssetTags 标记自身
    if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
    {
        // 3. 构建查询：查找所有包含此冷却标签的活跃 GE
        // MakeQuery_MatchAnyOwningTags 表示只要拥有该 Tag 就匹配
        FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
       
        // 4. 获取剩余时间数组
        // 因为理论上可能存在多个 GE 提供同一个冷却标签（虽然少见，但要处理）
        TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
       
        if (TimesRemaining.Num() > 0)
        {
            // 5. 寻找最大值
            // 我们只关心最长的那个冷却时间作为最终显示
            float TimeRemaining = TimesRemaining[0];
            for (int32 i = 0; i < TimesRemaining.Num(); i++)
            {
                if (TimesRemaining[i] > TimeRemaining)
                {
                    TimeRemaining = TimesRemaining[i];
                }
            }
          
            // 6. 广播 CooldownStart 委托，将剩余时间传给 UI
            CooldownStart.Broadcast(TimeRemaining);
        }
    }
}
