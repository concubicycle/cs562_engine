#ifndef __CS562_QUAT_HPP_
#define __CS562_QUAT_HPP_

#include <Eigen/Core>
#include <Eigen/Dense>

namespace math
{
  template <typename T>
  class quat
  {
    using VectorT = Eigen::Matrix<T, 3, 1>;
    using EigenMap3 = Eigen::Map<VectorT>;

  public:
    static const quat zero;
    static const quat one;
    static const quat identity;

    quat()
      : _sxyz{ 1, 0, 0, 0 }
      , _v(_sxyz + 1) {}

    quat(const quat& other)
      : _sxyz{ other._sxyz[0], other._sxyz[1], other._sxyz[2], other._sxyz[3] }
      , _v(_sxyz + 1) 
    {
      int i = 0;
    }

    quat& operator= (const quat& other)
    {
      for (size_t i = 0; i < 4; ++i) _sxyz[i] = other._sxyz[i];
      _v = EigenMap3(_sxyz + 1);
      return *this;
    };

    ~quat() = default;

    quat(T s, T x, T y, T z)
      : _sxyz{ s, x, y, z }
      , _v(_sxyz + 1) {}

    quat(const Eigen::Vector3f& axis, T angle)
      : quat(std::cos(angle / 2), std::sin(angle / 2)* axis)
    {
    }

    quat(const Eigen::Matrix<T, 4, 4>& rotation)
    {
      /*
      Eigen is column major:
      If the storage order is not specified, then Eigen defaults to
      storing the entry in column - major.This is also the case if
      one of the convenience typedefs(Matrix3f, ArrayXXd, etc.) is used.

      Math notation is usually row-major. Here it is in row-major:
      s = 0.5*sqrt(m00+m11+m22+1)
      x = (m21−m12) / (4 * s)
      y = (m02−m20) / (4 * s)
      z = (m10−m01) / (4 * s)
      */

      auto m00 = rotation[0][0];
      auto m11 = rotation[1][1];
      auto m22 = rotation[2][2];
      auto m21 = rotation[1][2];
      auto m12 = rotation[2][1];
      auto m02 = rotation[2][0];
      auto m20 = rotation[0][2];
      auto m10 = rotation[0][1];
      auto m01 = rotation[1][0];

      auto s = _sxyz[0] = 0.5 * std::sqrt(m00 + m11 + m22 + 1);
      _sxyz[1] = (m21 - m12) / (4 * s);
      _sxyz[2] = (m02 - m20) / (4 * s);
      _sxyz[3] = (m10 - m10) / (4 * s);
    }

    static quat z_to_v(VectorT v)
    {
      return quat(
        VectorT::UnitZ().cross(v),
        std::acos(VectorT::UnitZ().dot(v)) / std::sqrt(v.dot(v)));
    }




    quat operator* (const quat& other) const
    {
      return quat(
        s() * other.s(),
        s() * other.v() + other.s() * v() + v().cross(other.v()));
    }

    quat operator* (T scalar) const
    {
      return quat(s() * scalar, _v * scalar);
    }

    quat operator/ (T scalar) const
    {
      return *this * (1.f / scalar);
    }

    VectorT operator* (const VectorT& r) const
    {
      return
        (s() * s() - _v.dot(_v)) * r +
        2 * _v.dot(r) * _v +
        2 * s() * _v.cross(r);
    }

    quat operator+ (const quat& other) const
    {
      return quat(s() + other.s(),
        _v + other.v());
    }

    quat operator- (const quat& other) const
    {
      return quat(s() - other.s(),
        _v - other.v());
    }

    inline bool operator==(const quat& rhs)
    {
      return
        _sxyz[0] == rhs[0] &&
        _sxyz[1] == rhs[1] &&
        _sxyz[2] == rhs[2] &&
        _sxyz[3] == rhs[3];
    }

    inline bool operator!=(const quat& rhs)
    {
      return !(*this == rhs);
    }

    T& operator [] (size_t index) { return _sxyz[index]; }
    const T& operator [] (size_t index) const { return _sxyz[index]; }

    T length()
    {
      return std::sqrt(s() * s() + _v.dot(_v));
    }

    T length_sq()
    {
      return s() * s() + _v.dot(_v);
    }

    quat conjugate()
    {
      return quat(s(), -_v);
    }

    quat inverse()
    {
      conjugate() / length_sq();
    }

    Eigen::Matrix<T, 4, 4> matrix() const
    {
      auto s = _sxyz[0];
      auto x = _sxyz[1];
      auto y = _sxyz[2];
      auto z = _sxyz[3];

      auto xsq = x * x;
      auto ysq = y * y;
      auto zsq = z * z;

      Eigen::Matrix<T, 4, 4> m;
      m << 1 - 2 * (ysq + zsq), 2 * (x * y - s * z), 2 * (x * z + s * y), 0,
        2 * (x * y + s * z), 1 - 2 * (xsq + zsq), 2 * (y * z - s * x), 0,
        2 * (x * z - s * y), 2 * (y * z + s * x), 1 - 2 * (xsq + ysq), 0,
        0, 0, 0, 1;

      return m;
    }

    T magnitude() const
    {
      return std::sqrt(
        _sxyz[0] * _sxyz[0] +
        _sxyz[1] * _sxyz[1] +
        _sxyz[2] * _sxyz[2] +
        _sxyz[3] * _sxyz[3]);
    }

    quat normalized() const
    {
      auto mag = magnitude();
      return quat(
        _sxyz[0] / mag,
        _sxyz[1] / mag,
        _sxyz[2] / mag,
        _sxyz[3] / mag);
    }

    void normalize()
    {
      auto mag = magnitude();
      _sxyz[0] /= mag;
      _sxyz[1] /= mag;
      _sxyz[2] /= mag;
      _sxyz[3] /= mag;
    }

    T dot(const quat& other) const
    {
      return
        _sxyz[0] * other._sxyz[0] +
        _sxyz[1] * other._sxyz[1] +
        _sxyz[2] * other._sxyz[2] +
        _sxyz[3] * other._sxyz[3];
    }

    quat slerp(const quat& other, float t) const
    {
      auto d = dot(other);
      auto d_abs = std::abs(d);

      if (d_abs > 0.9995f)
      {
        const quat& q0 = *this;
        const quat& q1 = other;
        return (q0 + t * (q1 - q0)).normalized();
      }

      if (d < 0)
      {
        return slerp_operation(-d, *this * -1.f, other, t);
      }

      return slerp_operation(d, *this, other, t);
    }

    const EigenMap3& v() const { return _v; }
    EigenMap3& v() { return _v; }
    T s() const { return _sxyz[0]; }

  private:
    T _sxyz[4];
    EigenMap3 _v;

    // private constructors, to prevent user from mistaking them for the 
    // angle axis constructor 
    quat(T s, const VectorT& xyz)
      : _sxyz{ s, xyz[0], xyz[1], xyz[2] }
      , _v(_sxyz + 1) {}

    quat(T s, const VectorT&& xyz) noexcept
      : _sxyz{ s, xyz[0], xyz[1], xyz[2] }
      , _v(_sxyz + 1) {}

    quat slerp_operation(T d, const quat& q0, const quat& q1, float t) const
    {
      auto acosd = std::acos(d);
      auto numerator = q0 * std::sin((1.f - t) * acosd) + q1 * (std::sin(t * acosd));
      return numerator / std::sin(acosd);
    }

  };

  template<typename T>
  const quat<T> quat<T>::zero{ 0,0,0,0 };

  template<typename T>
  const quat<T> quat<T>::one{ 1,1,1,1 };

  template<typename T>
  const quat<T> quat<T>::identity{ 1,0,0,0 };

  template<typename T>
  quat<T> operator*(T scalar, const quat<T>& q)
  {
    return q * scalar;
  }
}

#endif