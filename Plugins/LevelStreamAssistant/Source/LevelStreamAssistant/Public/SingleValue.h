// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include <CoreMinimal.h>

// Represents a single value from multiple values. returns empty value if given values are not equal.
template<typename ValueType>
struct TSingleValue
{
	TSingleValue() { }

	void AddNextValue(ValueType InNextValue)
	{
		if (bAllValuesAreEqual)
		{
			if (!SingleValue.IsSet())
			{
				SingleValue = InNextValue;
			}
			else if (SingleValue != InNextValue)
			{
				SingleValue.Reset();
				bAllValuesAreEqual = false;
			}
		}
	}

	ValueType GetValue(ValueType FallbackValue)
	{
		return SingleValue.Get(FallbackValue);
	}

	bool HasDifferentValues()
	{
		return !bAllValuesAreEqual;
	}

private:
	TOptional<ValueType> SingleValue;
	bool bAllValuesAreEqual = true;
};