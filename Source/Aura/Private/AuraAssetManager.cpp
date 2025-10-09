// Tcohneyn All Rights Reserved


#include "AuraAssetManager.h"
#include "AuraGameplayTagsController.h"

// 返回当前项目中唯一的 UAuraAssetManager 实例
UAuraAssetManager& UAuraAssetManager::Get()
{
    check(GEngine);
	
    UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
    return *AuraAssetManager;
}
// AssetManager 初始化时的钩子函数
// 在引擎启动加载时会被调用一次
void UAuraAssetManager::StartInitialLoading()
{
    Super::StartInitialLoading();

    FAuraGameplayTags::InitializeNativeGameplayTags();
}
