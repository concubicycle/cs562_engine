#ifndef __LOOKAT_HPP_
#define __LOOKAT_HPP_

#include <Eigen/Core>

namespace renderer
{
	// https://bitbucket.org/drtaglia/eigen-opengl/commits/85537e2c643fe63651aa4260ca3a748c48509f34?at=default
	/// @brief Returns a view transformation matrix like the one from glu's lookAt
	/// @see http://www.opengl.org/sdk/docs/man2/xhtml/gluLookAt.xml
	/// @see glm::lookAt
	template<typename Derived>
	Eigen::Matrix<typename Derived::Scalar, 4, 4> lookat(Derived const& eye, Derived const& center, Derived const& up) {
		typedef Eigen::Matrix<typename Derived::Scalar, 4, 4> Matrix4;
		typedef Eigen::Matrix<typename Derived::Scalar, 3, 1> Vector3;
		Vector3 f = (center - eye).normalized();
		Vector3 u = up.normalized();
		Vector3 s = f.cross(u).normalized();
		u = s.cross(f);
		Matrix4 mat;
		mat(0, 0) = s.x();
		mat(0, 1) = s.y();
		mat(0, 2) = s.z();
		mat(0, 3) = -s.dot(eye);
		mat(1, 0) = u.x();
		mat(1, 1) = u.y();
		mat(1, 2) = u.z();
		mat(1, 3) = -u.dot(eye);
		mat(2, 0) = -f.x();
		mat(2, 1) = -f.y();
		mat(2, 2) = -f.z();
		mat(2, 3) = f.dot(eye);
		mat.row(3) << 0, 0, 0, 1;
		return mat;
	}
}

#endif