#ifndef __CS562_QUAT_HPP_
#define __CS562_QUAT_HPP_

#include <Eigen/Core>

namespace math
{
    template <typename T>
    class quat
    {
        using EigenMap3f = Eigen::Map<Eigen::Matrix<T, 3, 1>>;

    public:
        quat(T s, T x, T y, T z)
            : _sxyz {s, x, y, z}
            , _v(_sxyz) {}

        quat(const quat& other) = default;
        quat& operator= (const quat&) = default;
        ~quat() = default;

        T& operator [] (size_t index) { return _sxyz[index]; }        
        const EigenMap3f& v() { return _v; }
        T s() const { return _sxyz[0]; }    

        quat operator* (const quat& other)
        {
            return quat(
                s() * other.s() - _v.dot
        }


    private:
        T _sxyz[4];
        EigenMap3f _v;
    };
}

#endif