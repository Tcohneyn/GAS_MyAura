// Tcohneyn All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#define CUSTOM_DEPTH_RED 250
// 这是一个预处理器宏定义，用于为自定义的碰撞通道创建一个易于理解和记忆的别名。
// 它的作用是将标识符 'ECC_Projectile' 定义为 'ECollisionChannel::ECC_GameTraceChannel1'。
// 这样，在代码中就可以使用更具语义的 'ECC_Projectile' 来指代第一个自定义游戏追踪通道，
// 从而避免直接使用含义不明确的 'ECC_GameTraceChannel1'，极大地提高了代码的可读性和可维护性。
#define ECC_Projectile ECollisionChannel::ECC_GameTraceChannel1