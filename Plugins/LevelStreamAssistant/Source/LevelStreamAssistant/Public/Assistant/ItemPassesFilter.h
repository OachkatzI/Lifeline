// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace LevelStreamAssistant
{
	DECLARE_DELEGATE_RetVal_OneParam(bool, FItemPassesFilter, const TSharedPtr<struct FBaseItem>&);
}