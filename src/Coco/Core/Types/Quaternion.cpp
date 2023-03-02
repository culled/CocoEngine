#include "Quaternion.h"

namespace Coco
{
	const Quaternion Quaternion::Identity = Quaternion(0.0, 0.0, 0.0, 1.0);

	Quaternion::Quaternion() : Quaternion(0.0, 0.0, 0.0, 1.0)
	{
	}

	Quaternion::Quaternion(double x, double y, double z, double w) :
		_data(x, y, z, w)
	{
	}

	double Quaternion::Dot(const Quaternion& a, const Quaternion& b)
	{
		return a.Dot(b);
	}

	double Quaternion::Normal() const
	{
		return _data.GetLength();
	}

	void Quaternion::Normalize()
	{
		_data.Normalize();
	}
	Quaternion Quaternion::Conjugate() const
	{
		return Quaternion(-this->_data.X, -this->_data.Y, -this->_data.Z, this->_data.W);
	}
	Quaternion Quaternion::Inverted() const
	{
		Quaternion inv = Conjugate();
		inv.Normalize();
		return inv;
	}

	double Quaternion::Dot(const Quaternion& other) const
	{
		return this->_data.Dot(other._data);
	}

	Quaternion Quaternion::operator*(const Quaternion& other) const
	{
		Quaternion result;

		result._data.X = this->_data.X * other._data.W +
			this->_data.Y * other._data.Z -
			this->_data.Z * other._data.Y +
			this->_data.W * other._data.X;

		result._data.Y = -this->_data.X * other._data.Z +
			this->_data.Y * other._data.W +
			this->_data.Z * other._data.X +
			this->_data.W * other._data.Y;

		result._data.Z = this->_data.X * other._data.Y -
			this->_data.Y * other._data.X +
			this->_data.Z * other._data.W +
			this->_data.W * other._data.Z;

		result._data.W = -this->_data.X * other._data.X -
			this->_data.Y * other._data.Y -
			this->_data.Z * other._data.Z +
			this->_data.W * other._data.W;

		return result;
	}
}