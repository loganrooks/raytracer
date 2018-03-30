/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements light_source.h

***********************************************************/

#include <cmath>
#include "light_source.h"
#include <assert.h>
#include <cstdlib>

void PointLight::shade( Ray3D& ray ) {
	// TODO: implement this function to fill in values for ray.col
	// using phong shading.  Make sure your vectors are normalized, and
	// clamp colour values to 1.0.
	//
	// It is assumed at this point that the intersection information in ray
	// is available.  So be sure that traverseScene() is called on the ray
	// before this function.

	// implement phong shading as in a2

	assert(ray.intersection.none == false);

	Material * mat = ray.intersection.mat;

	Colour ambient_col = _col_ambient * mat->ambient;
	ambient_col.clamp();

	Vector3D l = _pos - ray.intersection.point;
	l.normalize();

	double n_dot_l = 1.0;
	n_dot_l = fmax(0, ray.intersection.normal.dot(l));

	Colour diffuse_col;

	// if the ray intersects a textured object then the texture colour replaces
	// the diffuse color
	if (ray.intersection.has_texture)
	{
		diffuse_col = n_dot_l* _col_diffuse * ray.intersection.texture_colour;
	}
	else {
		diffuse_col = n_dot_l* _col_diffuse * mat->diffuse;
	}
	diffuse_col.clamp();

	double v_dot_r = 0.0;

	Vector3D v = -ray.dir;
	v.normalize();

	Vector3D r = 2 * n_dot_l * ray.intersection.normal - l;
	r.normalize();
	v_dot_r = fmax(0.0, v.dot(r));

	Colour spec_light = pow(v_dot_r, mat->specular_exp) * _col_specular * mat->specular;
	spec_light.clamp();

	// for rays in shadow we only add the ambient colour 
	if(ray.inShadow == false){
		ray.col = ray.col + ambient_col + diffuse_col + spec_light;

	} else {
		ray.col = ray.col + ambient_col;

	}

	// uncomment this to produce the scene signature
	//ray.col = mat->diffuse;
	
	ray.col.clamp();

}

// We need to make these constants so that we can assign 
// C-style arrays at compile time

#define N 10
#define N_SQUARED 100

void AreaLight::get_samples(Point3D** sample_positions, int*num_samples) {

	int n = N;
	*num_samples = N_SQUARED;

	// for each sample we need to generate two points
	// one for caculating illumination
	// one for testing shadows

	shadow_samples = new Point3D[N_SQUARED];
	ray_samples = new Point3D[N_SQUARED];

	double noise1;
	double noise2;
	
	// Use jittered sampling, as we did for anti-aliasing

	for (int p = 0; p < n; p++)
	{
		for (int q = 0; q < n; q++)
		{
			noise1 = (double)rand() / RAND_MAX;
			noise2 = (double)rand() / RAND_MAX;

			shadow_samples[p *n + q] =  Point3D(_pos + (p + noise1)/n * _a + (q + noise2)/n *_b);

			noise1 = (double)rand() / RAND_MAX;
			noise2 = (double)rand() / RAND_MAX;

			ray_samples[p*n + q] =  Point3D(_pos + (p + noise1)/n * _a + (q + noise2)/n *_b);
		}
	}

	*sample_positions = shadow_samples;

	// Because we used jittered sampling
	// but we want the random position of the area_light sample
	// used for testing shadows to be independent of
	// the position of the sample used for calculating illumination
	// we randomly shuffle one of the arrays
	shuffle_array(ray_samples, N_SQUARED);
	
	// keep track of how many of the samples have been generated for this particular intersection
	samples_so_far = 0;
}


void AreaLight::shade( Ray3D& ray ) {

	assert(ray.intersection.none == false);

	Material * mat = ray.intersection.mat;

	Colour ambient_col = _col_ambient * mat->ambient;
	ambient_col.clamp();

	Colour col;

	// this function will be called
	// n_squared times

	// each time we use a different position sample and then
	// increment samples_so_far by 1

	 Vector3D l = (_pos - ray.intersection.point);
	
	 if (samples_so_far > N_SQUARED || samples_so_far < 0)
	 {
	 	// SANITY CHECK - print log message if samples_so_far 
		// is outside the bounds of the array 
		std::cout << "Panic " << samples_so_far << std::endl;
	 }
	 else {
	 	// for each sample we change the position of the light source
		Vector3D l = (ray_samples[samples_so_far] - ray.intersection.point);
	 }
	 
	// the rest of this is the same as for the point light source

	l.normalize();
	samples_so_far++;

	double n_dot_l = 1.0;
	n_dot_l = fmax(0, ray.intersection.normal.dot(l));

	Colour diffuse_col;

	if (ray.intersection.has_texture) {
		diffuse_col = n_dot_l* _col_diffuse * ray.intersection.texture_colour;
	}
	else {
		diffuse_col = n_dot_l* _col_diffuse * mat->diffuse;
	}
	diffuse_col.clamp();

	double v_dot_r = 0.0;

	Vector3D v = -ray.dir;
	v.normalize();

	Vector3D r = 2 * n_dot_l * ray.intersection.normal - l;
	r.normalize();
	v_dot_r = fmax(0.0, v.dot(r));

	Colour spec_light = pow(v_dot_r, mat->specular_exp) * _col_specular * mat->specular;
	spec_light.clamp();

	// because there will be N_SQUARED samples, divide the contribution of this sample 
	// by N_SQUARED
	col = col + (1./N_SQUARED) * (ambient_col + diffuse_col + spec_light);

	ray.col = ray.col + col;
	ray.col.clamp();
}

// to ensure the two sets of samples are independent
void shuffle_array(Point3D* a, int n_squared)
{
	for (int i = n_squared - 1; i > 0; i--) {		
		int j = (int) round( (double) rand() / RAND_MAX * i);
		Point3D tmp = a[i];
		a[i] = a[j];
		a[j] = tmp;
	}
}

