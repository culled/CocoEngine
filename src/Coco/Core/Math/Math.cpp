#include "Math.h"

#include <limits>
namespace Coco
{
	const double Math::PI = 3.14159265358979323846;
	const double Math::DoublePI = Math::PI * 2.0;
	const double Math::HalfPI = Math::PI * 0.5;
	const double Math::QuarterPI = Math::PI * 0.25;
	const double Math::OneOverPI = 1.0 / Math::PI;
	const double Math::OneOverDoublePI = 1.0 / Math::DoublePI;
	const double Math::SqrtTwo = 1.41421356237309504880;
	const double Math::SqrtThree = 1.73205080756887729352;
	const double Math::SqrtOneOverTwo = 0.70710678118654752440;
	const double Math::SqrtOneOverThree = 0.57735026918962576450;
	const double Math::Deg2RadMultiplier = Math::PI / 180.0;
	const double Math::Rad2DegMultiplier = 180.0 / Math::PI;

	const double Math::SecondsToMillisecondsMultiplier = 1000.0;
	const double Math::MillisecondsToSecondsMultiplier = 0.001;

	const double Math::Infinity = std::numeric_limits<double>::max();
	const double Math::Epsilon = std::numeric_limits<double>::epsilon();
}