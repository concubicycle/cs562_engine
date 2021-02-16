#include "quat_tests.hpp"

void identity()

{
  using quat = math::quat<float>;

  quat q;
  quat q2(1, 2, 3, 4);
  quat q3 = q * q2;
  quat q4 = q2 * q;

  if (q2 != q3) throw std::runtime_error("identity not identity-ish");
  if (q2 != q4) throw std::runtime_error("identity not identity-ish");

  Eigen::Matrix4f m = q.matrix();
  Eigen::Vector3f x = q * Eigen::Vector3f::UnitX();
  Eigen::Vector3f y = q * Eigen::Vector3f::UnitY();
  Eigen::Vector3f z = q * Eigen::Vector3f::UnitZ();

  if (!m.isApprox(Eigen::Matrix4f::Identity()))
    throw std::runtime_error("identity");

  if (!x.isApprox(Eigen::Vector3f::UnitX()))
    throw std::runtime_error("x identity wrong");
  if (!y.isApprox(Eigen::Vector3f::UnitY()))
    throw std::runtime_error("y identity wrong");
  if (!z.isApprox(Eigen::Vector3f::UnitZ()))
    throw std::runtime_error("z identity wrong");
}

void add()
{
  using quat = math::quat<int>;

  quat q(1, 2, 3, 4);
  quat q2(2, 3, 4, 5);
  quat q3 = q + q2;

  if (q3 != quat(3, 5, 7, 9)) throw std::runtime_error("add wrong");
}

void subtract()
{
  using quat = math::quat<int>;

  quat q(1, 2, 3, 4);
  quat q2 = q - q;

  if (q2 != quat::zero) throw std::runtime_error("subtraction wrong");
}

void length()
{
  using quat = math::quat<int>;

  quat q(4, 4, 4, 4);
  auto len = q.length();
  if (len != 8) throw std::runtime_error("length wrong");
}

void conjugate()
{
  using quat = math::quat<float>;
  quat q(1, -1, -1, -1);
  auto conjugate = q.conjugate();
  if (conjugate != quat::one) throw std::runtime_error("conjugate wrong");
}

void compare_rotations()
{
  using quat = math::quat<float>;

  Eigen::Vector3f randomvec = Eigen::Vector3f(2, 7, 4).normalized();
  float randomf = 7.13423f;

  Eigen::Transform<float, 3, Eigen::TransformTraits::Affine> actual_rotation(
    Eigen::AngleAxisf(util::HalfPi_f, Eigen::Vector3f::UnitX()));

  Eigen::Transform<float, 3, Eigen::TransformTraits::Affine> actual_rotation2(
    Eigen::AngleAxisf(randomf, randomvec));

  quat q(Eigen::Vector3f::UnitX(), util::HalfPi_f);
  quat q2(randomvec, randomf);

  Eigen::Matrix4f qm = q.matrix();
  Eigen::Matrix4f qm2 = q2.matrix();

  auto d1_base = actual_rotation.data();
  auto d1_q = qm.data();

  auto d2_base = actual_rotation2.data();
  auto d2_q = qm2.data();

  auto epsilon = 0.0001;

  for (size_t i = 0; i < 16; ++i)
  {
    if (std::abs(d1_base[i] - d1_q[i]) > epsilon)
      throw new std::runtime_error("rotation matrix not close enough to baseline");

    if (std::abs(d2_base[i] - d2_q[i]) > epsilon)
      throw new std::runtime_error("rotation matrix not close enough to baseline");
  }
}

void rotate_vector()
{
  using quat = math::quat<float>;

  quat about_x(Eigen::Vector3f::UnitX(), -util::HalfPi_f);
  Eigen::Vector3f result = about_x * Eigen::Vector3f::UnitZ();

  if (!(result.isApprox(Eigen::Vector3f::UnitY())))
  {
    throw std::runtime_error("rotation not working");
  }

  quat about_x_90(0.70710678f, 0.70710678f, 0, 0);
  Eigen::Vector3f x = about_x_90 * Eigen::Vector3f::UnitX();
  Eigen::Vector3f y = about_x_90 * Eigen::Vector3f::UnitY();
  Eigen::Vector3f z = about_x_90 * Eigen::Vector3f::UnitZ();

  if (!(x.isApprox(Eigen::Vector3f::UnitX())))
    throw std::runtime_error("rotation not working");

  if (!(y.isApprox(Eigen::Vector3f::UnitZ())))
    throw std::runtime_error("rotation not working");

  if (!(z.isApprox(-Eigen::Vector3f::UnitY())))
    throw std::runtime_error("rotation not working");

  Eigen::Matrix4f m = about_x_90.matrix();
  std::cout << m << std::endl;

  std::cout << "----" << std::endl;

  quat q_half(0.5, 0.5, 0.5, 0.5);
  x = q_half * Eigen::Vector3f::UnitX();
  y = q_half * Eigen::Vector3f::UnitY();
  z = q_half * Eigen::Vector3f::UnitZ();

  if (!(x.isApprox(Eigen::Vector3f::UnitY())))
    throw std::runtime_error("rotation not working");

  if (!(y.isApprox(Eigen::Vector3f::UnitZ())))
    throw std::runtime_error("rotation not working");

  if (!(z.isApprox(Eigen::Vector3f::UnitX())))
    throw std::runtime_error("rotation not working");

  m = q_half.matrix();
  std::cout << m << std::endl;

  std::cout << "----" << std::endl;

  quat q_four_thirds(-0.5, 0.5, 0.5, 0.5);
  x = q_four_thirds * Eigen::Vector3f::UnitX();
  y = q_four_thirds * Eigen::Vector3f::UnitY();
  z = q_four_thirds * Eigen::Vector3f::UnitZ();

  if (!(x.isApprox(Eigen::Vector3f::UnitZ())))
    throw std::runtime_error("rotation not working");

  if (!(y.isApprox(Eigen::Vector3f::UnitX())))
    throw std::runtime_error("rotation not working");

  if (!(z.isApprox(Eigen::Vector3f::UnitY())))
    throw std::runtime_error("rotation not working");

  m = q_four_thirds.matrix();
  std::cout << m << std::endl;

  std::cout << "----" << std::endl;

  quat second_identity(-1, 0, 0, 0);
  x = second_identity * Eigen::Vector3f::UnitX();
  y = second_identity * Eigen::Vector3f::UnitY();
  z = second_identity * Eigen::Vector3f::UnitZ();

  if (!(x.isApprox(Eigen::Vector3f::UnitX())))
    throw std::runtime_error("rotation not working");

  if (!(y.isApprox(Eigen::Vector3f::UnitY())))
    throw std::runtime_error("rotation not working");

  if (!(z.isApprox(Eigen::Vector3f::UnitZ())))
    throw std::runtime_error("rotation not working");

  m = second_identity.matrix();
  std::cout << m << std::endl;


  std::cout << "rotation test done" << std::endl;
}

void test_to_from_mat()
{
  using namespace math;

  quat<float> q_half(0.5, 0.5, 0.5, 0.5);
  Eigen::Matrix4f m = q_half.matrix();
  quat<float> q_hopefully_half(m);

  if (q_hopefully_half != q_half)
    throw std::runtime_error("from matrix bad");
}
