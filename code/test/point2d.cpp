#include "../geometry/general_point2d.hpp"
#include "../geometry/general_point3d.hpp"
#include "../numeric/frac.hpp"
#include "test_utils.hpp"

template struct Point2d<frac, double>;
template struct Point2d<long, double>;
template struct Point3d<frac, double>;
template struct Point3d<long, double>;
template struct Point2d<long, long double>;
template struct Point3d<long, long double>;
template struct Point2d<double, double>;
template struct Point3d<double, double>;

int main() { return 0; }
