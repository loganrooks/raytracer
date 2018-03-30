/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements scene_object.h

***********************************************************/

#include <cmath>
#include <iostream>
#include "scene_object.h"


bool UnitSquare::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	// TODO: implement intersection code for UnitSquare, which is
	// defined on the xy-plane, with vertices (0.5, 0.5, 0),
	// (-0.5, 0.5, 0), (-0.5, -0.5, 0), (0.5, -0.5, 0), and normal
	// (0, 0, 1).
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point,
	// intersection.normal, intersection.none, intersection.t_value.
	//
	// HINT: Remember to first transform the ray into object space
	// to simplify the intersection test.

	Point3D e = worldToModel * ray.origin;
	Vector3D d = worldToModel * ray.dir;

	Point3D p1 = Point3D(0, 0, 0);
	Vector3D z = Vector3D(0, 0, 1);

	if (z.dot(d) != 0)
	{
		//ray.intersection.none = false;
		double t_value = z.dot(p1 - e) / z.dot(d);

		Point3D intersection_point = (e + t_value * d);
		bool in_bounds = true;

		in_bounds = in_bounds && intersection_point[0] > -0.5 && intersection_point[0] < 0.5;
		in_bounds = in_bounds && intersection_point[1] > -0.5 && intersection_point[1] < 0.5;

		// note - can't check that z == 0.0 because of floating point errors
		in_bounds = in_bounds && intersection_point[2] > -0.5 && intersection_point[2] < 0.5;

		//if ray is not reflected we should consider the intersection closest to the viewpoint.
		if (in_bounds && t_value > 0 && t_value < ray.intersection.t_value)
		{

			// in this case, it intersects with something
			ray.intersection.none = false;
			ray.intersection.t_value = t_value;
			// convert points and normals appropriately
			ray.intersection.point = modelToWorld * intersection_point;
			ray.intersection.normal = worldToModel.transpose() * z;
			ray.intersection.normal.normalize();
			
			// we do this because the bmp coordinates given by bmp_io are flipped
			ray.intersection.uv_coords = Point3D( (intersection_point[1] + 0.5), (1 - (intersection_point[0] + 0.5)), 0);
			
			// otherwise the correct formula would be:
			// ray.intersection.uv_coords = Point3D( (intersection_point[0] + 0.5), (1 - (intersection_point[1] + 0.5)), 0);
			return true;

		} else {
			return false;
		}

	}
	else {
		return false;
	}
}

bool UnitSphere::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	// TODO: implement intersection code for UnitSphere, which is centred
	// on the origin.
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point,
	// intersection.normal, intersection.none, intersection.t_value.
	//
	// HINT: Remember to first transform the ray into object space
	// to simplify the intersection test.

	Point3D e = worldToModel * ray.origin;
	Vector3D d = worldToModel * ray.dir;
	Point3D c = Point3D(0,0,0);

	// equation is from textbook + the Wikipedia entry on Line-sphere intersection
	double discriminant = pow(d.dot(e-c),2) - (d.dot(d)) * ((e-c).dot(e -c) - 1);


	if (discriminant >= 0) {
		double a = -d.dot(e - c);
		double t_value = fmin(a + sqrt(discriminant), a - sqrt(discriminant));
		t_value = t_value / d.dot(d);

		if (t_value > 0 && t_value < ray.intersection.t_value)
		{
			Point3D intersection_point = e + t_value * d;
			ray.intersection.none = false;
			ray.intersection.t_value = t_value;
			ray.intersection.point = modelToWorld* intersection_point;
			ray.intersection.normal = worldToModel.transpose() * (intersection_point - c);
			ray.intersection.normal.normalize();

			// This is the formula from wikipedia
			// where we have replaced (u,v) -> (1-v,1-u) 
			// because the .bmp texture coordinates are flipped
			ray.intersection.uv_coords = Point3D(1 - (0.5 - asin(intersection_point[1]) / 3.14159265), 1 - (0.5 + atan2(intersection_point[2], intersection_point[0]) / 6.2831853), 0 );
			
			// otherwise, the correct formula would be:
			//ray.intersection.uv_coords = Point3D((0.5 + atan2(intersection_point[2], intersection_point[0]) / 6.2831853),(0.5 - asin(intersection_point[1]) / 3.14159265),  0 );

			return true;
		}
		else {
	
			return false;
		}
	}
	else {

		return false;
	}
}

//checks if the square intersects the specified voxel (specified by dims)
bool UnitSquare::octree_intersect(double dims[],const Matrix4x4& worldToModel,
			const Matrix4x4& modelToWorld) {

	Point3D topLeft = Point3D(-0.5, 0.5, 0);
	Point3D bottomLeft  = Point3D(-0.5, -0.5, 0);
	Point3D bottomRight = Point3D(0.5, -0.5, 0);
	Point3D topRight = Point3D(0.5,0.5,0);


	Point3D planeCorners[4];

	//determine square coordinates in world space
	planeCorners[0] = modelToWorld*topRight;
	planeCorners[1] = modelToWorld*topLeft;
	planeCorners[2] = modelToWorld*bottomRight;
	planeCorners[3] = modelToWorld*bottomLeft;

	//check if any corner is contained
	for(int i=0; i<4; i++){
		if(planeCorners[i][0] < dims[1] && planeCorners[i][0] > dims[0]){
			if(planeCorners[i][1] < dims[3] && planeCorners[i][1] > dims[2]){
				if(planeCorners[i][2] <= dims[5] && planeCorners[i][2] >= dims[4]){
					return true;
				}
			}
		}
	}

	return false;

}


//checks if a bounding box containing the unit sphere intersects the specified voxel (specified by dims)
bool UnitSphere::octree_intersect(double dims[],const Matrix4x4& worldToModel,
			const Matrix4x4& modelToWorld) {

	Point3D topLeftFront = Point3D(-0.5, 0.5, 0.5);
	Point3D bottomLeftFront  = Point3D(-0.5, -0.5, 0.5);
	Point3D bottomRightFront = Point3D(0.5, -0.5, 0.5);
	Point3D topRightFront = Point3D(0.5,0.5,0.5);
	Point3D topLeftBack = Point3D(-0.5, 0.5, -0.5);
	Point3D bottomLeftBack  = Point3D(-0.5, -0.5, -0.5);
	Point3D bottomRightBack = Point3D(0.5, -0.5, -0.5);
	Point3D topRightBack = Point3D(0.5,0.5,-0.5);


	Point3D boxCorners[8];

	//determine sphere coordinates in world space
	boxCorners[0] = modelToWorld*topRightFront;
	boxCorners[1] = modelToWorld*topLeftFront;
	boxCorners[2] = modelToWorld*bottomRightFront;
	boxCorners[3] = modelToWorld*bottomLeftFront;
	boxCorners[4] = modelToWorld*topRightBack;
	boxCorners[5] = modelToWorld*topLeftBack;
	boxCorners[6] = modelToWorld*bottomRightBack;
	boxCorners[7] = modelToWorld*bottomLeftBack;

	//check if any corner is contained
	for(int i=0; i<8; i++){
		if(boxCorners[i][0] < dims[1] && boxCorners[i][0] > dims[0]){
			if(boxCorners[i][1] < dims[3] && boxCorners[i][1] > dims[2]){
				if(boxCorners[i][2] < dims[5] && boxCorners[i][2] > dims[4]){
					return true;
				}
			}
		}
	}

	return false;
}
