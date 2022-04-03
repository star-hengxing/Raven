#ifndef _RAVEN_CORE_INTERACTION_H_
#define _RAVEN_CORE_INTERACTION_H_
#include"base.h"
#include"math.h"
#include"ray.h"
namespace Raven {
	struct Interaction {

	};

	struct SurfaceInteraction :public Interaction {
		double t;//光线的传播距离参数t
		double eta = 1;//refract index
		Point2f uv;//texture coodinate
		Point3f p;//intersected point 
		Normal3f n;//surface normal
		Material* mate_ptr;//surface material
		Shape* shape;
		std::shared_ptr<BSDF> bsdf;//generated by surface material
		bool hitLight;
		const Light* light;
		Vector3f emit;
		//partial derivatives of intersected plane
		Vector3f dpdu, dpdv;
		Vector3f dndu, dndv;
		Vector3f wo;
		//properties to filter texture
		Vector3f dpdx, dpdy;
		double dudx, dudy, dvdx, dvdy;

		//shading geometry
		struct {
			Normal3f n;
			Vector3f dpdu, dpdv;
			Vector3f dndu, dndv;
		}shading;

		void SetShadingGeometry(const Vector3f& dpdu, const Vector3f& dpdv,
			const Normal3f& dndu, const Normal3f& dndv);

		//compute dudx dudy dvdx dvdy
		void computeDifferential(const RayDifferential& rd);

		Ray scartterRay(const Vector3f& dir)const;
	};
}

#endif