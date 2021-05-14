#include "test_utils.hpp"
#include "../numeric/frac.hpp"

#pragma GCC diagnostic ignored "-Wfloat-equal"
#include "../geometry/point2d.hpp"
#include "../geometry/point3d.hpp"

template struct Point2d<frac, double>;
template struct Point2d<long, double>;
template struct Point2d<long, long double>;
template struct Point2d<__int128_t, double>;
template struct Point2d<__int128_t, long double>;
template struct Point2d<double, double>;
template struct Point2d<long double, long double>;

template struct Point3d<frac, double>;
template struct Point3d<long, double>;
template struct Point3d<long, long double>;
template struct Point3d<__int128_t, double>;
template struct Point3d<__int128_t, long double>;
template struct Point3d<double, double>;
template struct Point3d<long double, long double>;

template struct Plane<frac, double>;
template struct Plane<long, double>;
template struct Plane<long, long double>;
template struct Plane<__int128_t, double>;
template struct Plane<__int128_t, long double>;
template struct Plane<double, double>;
template struct Plane<long double, long double>;

int main() { return 0; }
