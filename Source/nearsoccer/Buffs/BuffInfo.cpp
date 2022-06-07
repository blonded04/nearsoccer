// Nearsoccer 2022


#include "BuffInfo.h"
#include "BaseBuff.h"

FBuffInfo::FBuffInfo()
{
	BuffImage = nullptr;
}

FBuffInfo::FBuffInfo(const UBaseBuff* Buff)
{
	BuffName = Buff->GetName();
	BuffImage = Buff->GetTexture();
	FXColor = Buff->GetFXColor();
	FXRadius = Buff->GetFXRadius();
	BuffTime = Buff->GetTime();
	bIsLeftRight = Buff->IsLeftRight();
	bIsSpawnCircleEffect = Buff->IsSpawnCircleEffect();
}
