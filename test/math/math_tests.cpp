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
    test_to_from_mat();
    return 0;
}
