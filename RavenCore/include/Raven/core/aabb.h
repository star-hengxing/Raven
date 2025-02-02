﻿#ifndef _RAVEN_CORE_AABB_H_
#define _RAVEN_CORE_AABB_H_

#include<Raven/core/base.h>
#include<Raven/core/math.h>
#include<Raven/core/ray.h>

namespace Raven {
	using Bound3f = AABB3<double>;
	using Bound3i = AABB3<int>;
	using Bound2f = AABB2<double>;
	using Bound2i = AABB2<int>;

	/// <summary>
	/// Three dimensional axis aligned bounding box
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<class T>
	class AABB3 {
	public:
		Point3<T> pMin, pMax;

		AABB3() {
			//defual constructor set pmin>pmax to make sure this aabb do not exist
			T minNum = std::numeric_limits<T>::lowest();
			T maxNum = std::numeric_limits<T>::max();
			pMin = Point3<T>(maxNum, maxNum, maxNum);
			pMax = Point3<T>(minNum, minNum, minNum);
		}

		AABB3<T>(const AABB3<T>& box) { this->pMax = box.pMax; this->pMin = box.pMin; }

		AABB3<T>(const Point3<T>& p) { pMax = p; pMin = p; }

		AABB3<T>(const Point3<T>& p1, const Point3<T> p2) {
			pMin = Point3<T>(Min(p1.x, p2.x), Min(p1.y, p2.y), Min(p1.z, p2.z));
			pMax = Point3<T>(Max(p1.x, p2.x), Max(p1.y, p2.y), Max(p1.z, p2.z));
		}

		const Point3<T> operator[](int i)const {
			if (i == 0)return pMin;
			return pMax;
		}

		Point3<T>& operator[](int i) {
			if (i == 0)return pMin;
			return pMax;
		}

		Point3<T> corner(int corner)const {
			return Point3<T>((*this)[corner & 1].x,
				(*this)[corner & 2].y,
				(*this)[corner & 4].z
				);
		}

		Vector3<T> diagoal()const {
			return pMax - pMin;
		}

		T surfaceArea() const {
			T x = pMax.x - pMin.x;
			T y = pMax.y - pMin.y;
			T z = pMax.z - pMin.z;
			return 2 * (x * y + y * z + z * x);
		}

		T volume() const {
			Vector3<T> d = diagoal();
			return d.x * d.y * d.z;
		}

		int maxExtent()const {
			Vector3<T> d = diagoal();
			if (d[0] > d[1] && d[0] > d[2])
				return 0;
			else if (d[1] > d[0] && d[1] > d[2])
				return 1;
			else
				return 2;
		}

		Point3<T> lerp(const Point3<T>& t)const {
			return Point3<T>(
				Lerp(t.x, pMax.x, pMin.x),
				Lerp(t.y, pMax.y, pMin.y),
				Lerp(t.x, pMax.z, pMin.z)
				);
		}

		Vector3<T> offset(const Point3<T>& p)const {
			Vector3<T> o = p - pMin;
			if (pMax.x > pMin.x) o.x /= pMax.x - pMin.x;
			if (pMax.y > pMin.y) o.y /= pMax.y - pMin.y;
			if (pMax.z > pMin.z)o.z /= pMax.z - pMin.z;
			return o;
		}

		void boundingSphere(Point3<T>& center, double& radius)const {
			center = (pMax + pMin) / 2;
			radius = (pMax - center).length();
		}

		//��������Ƿ����bounding box�ཻ������ཻ������tMax��tMin
		bool hit(
			const Ray& r_in,
			double* tMin,
			double* tMax
		) const {
			double t0 = 0, t1 = r_in.tMax;
			Vector3f invDir(1.0 / r_in.dir[0], 1.0 / r_in.dir[1], 1.0 / r_in.dir[2]);
			for (int i = 0; i < 3; i++) {
				//
				double tHitNear = (pMin[i] - r_in.origin[i]) * invDir[i];
				double tHitFar = (pMax[i] - r_in.origin[i]) * invDir[i];
				if (tHitNear > tHitFar)
					std::swap(tHitNear, tHitFar);
				t0 = (tHitNear > t0) ? tHitNear : t0;
				t1 = (tHitFar < t1) ? tHitFar : t1;
				if (t0 > t1)return false;
			}

			*tMin = t0;
			*tMax = t1;
			return true;
		}

		bool hit(
			const Ray& ray,
			const Vector3f& invDir,
			const Vector3i& isDirNeg
		)const {
			const Point3f& pMax = this->pMax;
			const Point3f& pMin = this->pMin;

			double xMin = isDirNeg[0] ? pMin.x : pMax.x;
			double xMax = isDirNeg[1] ? pMax.x : pMin.x;
			double tMinX = (xMin - ray.origin.x) * invDir.x;
			double tMaxX = (pMax.x - ray.origin.x) * invDir.x;

			double yMin = isDirNeg[1] ? pMin.y : pMax.y;
			double yMax = isDirNeg[1] ? pMax.y : pMin.y;
			double tMinY = (yMin - ray.origin.y) * invDir.y;
			double tMaxY = (yMax - ray.origin.y) * invDir.y;

			if (tMinX > tMaxY || tMaxX < tMinY)return false;
			double tMin = Min(tMinX, tMinY);
			double tMax = Max(tMaxX, tMaxY);

			double zMin = isDirNeg[2] ? pMin.z : pMax.z;
			double zMax = isDirNeg[2] ? pMax.z : pMin.z;
			double tMinZ = (zMin - ray.origin.z) * invDir.z;
			double tMaxZ = (zMax - ray.origin.z) * invDir.z;

			if (tMin > tMaxZ || tMax < tMinZ)return false;
			tMin = Min(tMin, tMinZ);
			tMax = Max(tMax, tMaxZ);

			return (tMin > 0) && (tMax < ray.tMax);
		}
	};

	/// <summary>
	/// 2ά��Χ��
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<class T>
	class AABB2 {
	public:
		Point2<T> pMin, pMax;

		AABB2() {
			//defual constructor set pmin>pmax to make sure this aabb do not exist
			T minNum = std::numeric_limits<T>::lowest();
			T maxNum = std::numeric_limits<T>::max();
			pMin = Point2<T>(maxNum, maxNum);
			pMax = Point2<T>(minNum, minNum);
		}
		AABB2(const Point2<T>& p) :pMin(p), pMax(p) {}

		AABB2(const AABB2<T>& box) :pMin(box.pMin), pMax(box.pMax) {}

		AABB2(const Point2<T>& p1, const Point2<T>& p2) {
			pMin = Point2<T>(Min(p1.x, p2.x), Min(p1.y, p2.y));
			pMax = Point2<T>(Max(p1.x, p2.x), Max(p1.y, p2.y));
		}

		const Point2<T> operator[](int i) const {
			if (i == 0)return pMin;
			return pMax;
		}

		Point2<T>& operator[](int i) {
			if (i == 0)return pMin;
			return pMax;
		}

		Point2<T> Lerp(const Point3<T>& t)const {
			return Point2<T>(
				Lerp(t.x, pMax.x, pMin.x),
				Lerp(t.y, pMax.y, pMin.y)
				);
		}
	};

	template<class T>
	inline AABB3<T> Union(const AABB3<T>& box1, const AABB3<T>& box2) {
		return AABB3<T>(
			Point3<T>(box1[0].x < box2[0].x ? box1[0].x : box2[0].x,
				box1[0].y < box2[0].y ? box1[0].y : box2[0].y,
				box1[0].z < box2[0].z ? box1[0].z : box2[0].z),
			Point3<T>(box1[1].x > box2[1].x ? box1[1].x : box2[1].x,
				box1[1].y > box2[1].y ? box1[1].y : box2[1].y,
				box1[1].z > box2[1].z ? box1[1].z : box2[1].z));
	}

	template<class T>
	inline AABB3<T> Union(const AABB3<T>& box, const Point3<T>& p) {
		return AABB3<T>(
			Point3<T>(Min(box.pMin.x, p.x), Min(box.pMin.y, p.y), Min(box.pMin.z, p.z)),
			Point3<T>(Max(box.pMax.x, p.x), Max(box.pMax.y, p.y), Max(box.pMax.z, p.z))
			);
	}

	template<class T>
	inline AABB3<T> Intersection(const AABB3<T>& box1, const AABB3<T>& box2) {
		return AABB3<T>(
			Point3<T>(box1[0].x > box2[0].x ? box1[0].x : box2[0].x,
				box1[0].y > box2[0].y ? box1[0].y : box2[0].y,
				box1[0].z > box2[0].z ? box1[0].z : box2[0].z),
			Point3<T>(box1[1].x < box2[1].x ? box1[1].x : box2[1].x,
				box1[1].y < box2[1].y ? box1[1].y : box2[1].y,
				box1[1].z < box2[1].z ? box1[1].z : box2[1].z)
			);
	}

	template<class T>
	inline bool Overlaps(const AABB3<T>& box1, const AABB3<T>& box2) {
		bool x = (box1[1].x >= box2[0].x) && (box1[0].x <= box2[1].x);
		bool y = (box1[1].y >= box2[0].y) && (box1[0].y <= box2[1].y);
		bool z = (box1[1].z >= box2[0].z) && (box1[0].z <= box2[1].z);
		return x && y && z;
	}

	template<class T>
	inline bool Inside(const Point3<T>& p, const AABB3<T>& box) {
		bool x = p.x > box[0].x && p.x < box[1].x;
		bool y = p.y > box[0].y && p.y < box[1].y;
		bool z = p.z > box[0].z && p.z < box[1].z;
		return x && y && z;
	}

	template<class T, class U>
	inline AABB3<T> Expand(const AABB3<T>& box, U delta) {
		return AABB3<T>(box.pMin - Vector3<T>(delta, delta, delta),
			box.pMax + Vector3<T>(delta, delta, delta));
	}

}

#endif