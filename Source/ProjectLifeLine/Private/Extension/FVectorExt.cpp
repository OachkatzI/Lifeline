// Project LifeLine


#include "Extension/FVectorExt.h"


float FVectorExt::AngleBetween(const FVector Vec1, const FVector Vec2)
{
	return (FMath::Acos(FVector::DotProduct(Vec1.GetSafeNormal(), Vec2.GetSafeNormal())) * 180) / PI;
}


float FVectorExt::AngleBetween2D(FVector Vec1, FVector Vec2)
{
	Vec1.Z = 0;
	Vec2.Z = 0;
	return (FMath::Acos(FVector::DotProduct(Vec1.GetSafeNormal(), Vec2.GetSafeNormal())) * 180) / PI;
}
