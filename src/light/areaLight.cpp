#include"areaLight.h"

namespace Raven {
	Vector3f DiffuseAreaLight::L(const SurfaceInteraction& p, const Vector3f& wi)const {
		return Dot(p.n, wi) > 0.0 ? emittedRadiance : Vector3f(0.0);
	}

	Vector3f DiffuseAreaLight::sample_Li(const SurfaceInteraction& inter, const Point2f& uv,
		Vector3f* wi, double* pdf,Point3f* lightSample)const {
		//在光源表面采样一个点，算出从该点射向点p的方向向量
		SurfaceInteraction lightInter = shape_ptr->sample(inter, uv);
		*wi = Normalize(lightInter.p - inter.p);
		//调用Shape的pdf函数求出采样该点的pdf
		*pdf = shape_ptr->pdf(inter, *wi);
		*lightSample = lightInter.p;
		return	L(lightInter, -*wi);
	}

	Vector3f DiffuseAreaLight::power()const {
		return emittedRadiance * area * M_PI;
	}
}