/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		   light source classes

***********************************************************/

#include "util.h"

// Base class for a light source.  You could define different types
// of lights here, but point light is sufficient for most scenes you
// might want to render.  Different light sources shade the ray
// differently.
class LightSource {
public:
	LightSource(bool soft_shadows) : soft_shadows(soft_shadows), samples_so_far(0) {}
	virtual void shade( Ray3D& ) = 0;
	virtual Point3D get_position() const = 0;
	bool soft_shadows;
	
	// only makes sense for the Area Light
	int samples_so_far;

	virtual void get_samples(Point3D** sample_positions, int*num_samples) = 0;
};

// A point light is defined by its position in world space and its
// colour.
class PointLight : public LightSource {
public:
	PointLight( Point3D pos, Colour col ) : _pos(pos), _col_ambient(col),
	_col_diffuse(col), _col_specular(col), LightSource(false) {
	}

	PointLight( Point3D pos, Colour ambient, Colour diffuse, Colour specular )
	: _pos(pos), _col_ambient(ambient), _col_diffuse(diffuse),
	_col_specular(specular), LightSource(false) {}

	void shade( Ray3D& ray );
	Point3D get_position() const { return _pos; }
	void get_samples(Point3D** sample_positions, int*num_samples) { ; }

private:
	Point3D _pos;
	Colour _col_ambient;
	Colour _col_diffuse;
	Colour _col_specular;
};

class AreaLight : public LightSource {
public:
	AreaLight( Point3D pos, Colour col, Vector3D a, Vector3D b) : _pos(pos), _col_ambient(col),
	_col_diffuse(col), _col_specular(col), _a(a), _b(b), LightSource(true) {}

	void shade( Ray3D& ray );
	Point3D get_position() const { return _pos; }
	void get_samples(Point3D** sample_positions, int*num_samples);

	// store two arrays of Points representing 
	// the jittered sample points
	Point3D* shadow_samples;
	Point3D* ray_samples;

private:
	// This light is defined by a corner point
	// And two edge vectors _a and _b
	Point3D _pos;
	Vector3D _a;
	Vector3D _b;

	// We approximate the area light as an array of point lights
	Colour _col_ambient;
	Colour _col_diffuse;
	Colour _col_specular;

};

void shuffle_array(Point3D* a, int n_squared);