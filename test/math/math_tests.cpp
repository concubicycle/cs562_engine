#include "quat_tests.hpp"
#include "curve_tests.hpp"

int main()
{
    identity();
    add();
    subtract();
    length();
    conjugate();
    compare_rotations();
    rotate_vector();

    test_b_spline();
    return 0;
}
