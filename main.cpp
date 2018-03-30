/***********************************************************
Starter code for Assignment 3

This code was originally written by Jack Wang for
CSC418, SPRING 2005

***********************************************************/

#include "bmp_io.h"
#include "raytracer.h"
#include <cstdlib>

// These functions render the different scenes for our 
// demo images
int default_scene(int width, int height);
int reflections_scene(int width, int height);
int shadows_scene(int width, int height);
int cube_map_scene(int width, int height);
int texture_scene(int width, int height);
int area_light_scene(int width, int height);
int octree_scene(int width, int height);
int antialias_scene(int width, int height);
int plane_texture_scene(int width, int height);

int main(int argc, char* argv[])
{
	// Build your scene and setup your camera here, by calling 
	// functions from Raytracer.  The code here sets up an example
	// scene and renders it from two different view points, DO NOT
	// change this if you're just implementing part one of the 
	// assignment.  

	int width = 960;
	int height = 640;

	if (argc == 3) {
		width = atoi(argv[1]);	
		height = atoi(argv[2]);
	}

	printf("Look in a3/raytracer/output_images for output\n");

	// these will all dump out images into the directory
	// a3/raytracer/output_images
	// the default scene will be in view1.bmp, view2.bmp

	
	cube_map_scene(width,height);
	area_light_scene(width,height);
	reflections_scene(width,height);
	//texture_scene(width,height);
	//octree_scene(width,height);
	//antialias_scene(width,height);
	//plane_texture_scene(width,height);

	return default_scene(width, height);
}


int texture_scene(int width, int height) {
	Raytracer raytracer;

	Point3D eye(-3, 2, 3);
	Vector3D view(0, -0.5, -1);
	Vector3D up(0, 1, 0);

	double fov = 60;

	Material gold(Colour(0.0, 0.0, 0.0), Colour(0.1, 0.1, 0.1),
		Colour(0.99, 0.99, 0.99),
		51.2);

	Material green(Colour(0.1, 0.2, 0.1), Colour(0.54, 0.89, 0.63),
		Colour(0.316228, 0.316228, 0.316228),
		12.8);

	Material red(Colour(0.2, 0.1, 0.1), Colour(0.89, 0.63, 0.54),
		Colour(0.316228, 0.316228, 0.316228),
		12.8);

	Material blue(Colour(0.1, 0.1, 0.2), Colour(0.63, 0.54, 0.89),
		Colour(0.316228, 0.316228, 0.316228),
		12.8);

	LightSource* a = new PointLight(Point3D(5, 5, 5),
		Colour(0.9, 0.9, 0.9));
	
	raytracer.addLightSource(a);

	double factor2[3] = { 9.0, 9.0, 9.0 };
	SceneDagNode* plane4 = raytracer.addObject(new UnitSquare(), &red);


	raytracer.translate(plane4, Vector3D(-3, 0, 0));
	raytracer.rotate(plane4, 'x', -90);
//	raytracer.rotate(plane4, 'z', 90);
	raytracer.scale(plane4, Point3D(0, 0, 0), factor2);

	SceneDagNode* sphere = raytracer.addObject(new UnitSphere(), &green);
	raytracer.translate(sphere, Vector3D(-3, 1, 0));

	sphere->texture = new Texture();
	sphere->has_texture = true;
	sphere->texture->loadBitmap("textures/earth.bmp");

	raytracer.render(width, height, eye, view, up, fov, "output_images/texture_view1.bmp");

	return 0;
}

int reflections_scene(int width, int height) {
	Raytracer raytracer;

	Point3D eye(0, 0, 1);
	Vector3D view(0, 0, -1);
	Vector3D up(0, 1, 0);

	double fov = 60;

	Material gold(Colour(0.05, 0.05, 0.05), Colour(0.001, 0.001, 0.001),
		Colour(0.99, 0.99, 0.99),
		120);

	Material green(Colour(0.1, 0.2, 0.1), Colour(0.54, 0.89, 0.63),
		Colour(0.316228, 0.316228, 0.316228),
		12.8);

	Material red(Colour(0.2, 0.1, 0.1), Colour(0.89, 0.63, 0.54),
		Colour(0.316228, 0.316228, 0.316228),
		12.8);

	Material blue(Colour(0.1, 0.1, 0.2), Colour(0.63, 0.54, 0.89),
		Colour(0.316228, 0.316228, 0.316228),
		12.8);


	LightSource* a = new PointLight(Point3D(0, 0, 5),
		Colour(0.9, 0.9, 0.9));
	
	raytracer.addLightSource(a);

	double factor1[3] = { 1.0, 2.0, 1.0 };
	double factor2[3] = { 9.0, 9.0, 9.0 };
	SceneDagNode* plane2 = raytracer.addObject(new UnitSquare(), &gold);
	raytracer.translate(plane2, Vector3D(1, 0, -10));
	raytracer.rotate(plane2, 'y', -45);
	raytracer.scale(plane2, Point3D(0, 0, 0), factor2);

	SceneDagNode* plane3 = raytracer.addObject(new UnitSquare(), &gold);
	raytracer.translate(plane3, Vector3D(-1, 0, -10));
	raytracer.rotate(plane3, 'y', 45);
	raytracer.scale(plane3, Point3D(0, 0, 0), factor2);

	SceneDagNode* plane4 = raytracer.addObject(new UnitSquare(), &gold);
	raytracer.translate(plane4, Vector3D(0, -9.0, -10));
	raytracer.rotate(plane4, 'x', 90);
	raytracer.rotate(plane4, 'z', 90);
	raytracer.scale(plane4, Point3D(0, 0, 0), factor2);


	SceneDagNode* sphere = raytracer.addObject(new UnitSphere(), &green);
	raytracer.translate(sphere, Vector3D(0, 0, -7));
	SceneDagNode* sphere2 = raytracer.addObject(new UnitSphere(), &red);
	raytracer.translate(sphere2, Vector3D(-1, 1, -6));
	SceneDagNode* sphere3 = raytracer.addObject(new UnitSphere(), &blue);
	raytracer.translate(sphere3, Vector3D(-1, -1, -6));
	
	double factor3[3] = { 0.8, 0.8, 0.8 };
	raytracer.scale(sphere, Point3D(0, 0, 0), factor3);
	raytracer.scale(sphere2, Point3D(0, 0, 0), factor3);
	raytracer.scale(sphere3, Point3D(0, 0, 0), factor3);
	raytracer.reflection_depth = 10;

	// Render the scene, feel free to make the image smaller for
	// testing purposes.	
	raytracer.render(width, height, eye, view, up, fov, "output_images/reflections_view1.bmp");

	return 0;
}



int cube_map_scene(int width, int height){	
	Raytracer raytracer;

	// Camera parameters.
	Point3D eye(0, 0, 1);
	Vector3D view(0, 0, -1);
	Vector3D up(0, 1, 0);
	double fov = 60;

	// Defines a material for shading.
	Material shiny(Colour(0.0, 0.0, 0.0), Colour(0.2, 0.2, 0.1),
		Colour(0.8, 0.8, 0.8),
		200.);

	// Defines a point light source.
	raytracer.addLightSource(new PointLight(Point3D(1, 3, 5),
		Colour(0.3, 0.3, 0.3)));

	raytracer.use_cubemap = true;
	//raytracer.antialias = true;

	SceneDagNode* sphere = raytracer.addObject(new UnitSphere(), &shiny);

	double factor1[3] = { 1.0, 2.0, 1.0 };
	double factor2[3] = { 6.0, 6.0, 6.0 };
	raytracer.translate(sphere, Vector3D(0, 0, -2));

	raytracer.render(width, height, eye, view, up, fov, "output_images/cube_map_view1.bmp");

	Point3D eye2(4, 2, 1);
	Vector3D view2(-4, -2, -6);
	raytracer.render(width, height, eye2, view2, up, fov, "output_images/cube_map_view2.bmp");
	return 0;}

int area_light_scene(int width, int height){

	Raytracer raytracer;

	// Camera parameters.
	Point3D eye(0, 0, 1);
	Vector3D view(0, 0, -1);
	Vector3D up(0, 1, 0);
	double fov = 60;

	// Defines a material for shading.
	Material gold(Colour(0.3, 0.3, 0.3), Colour(0.75164, 0.60648, 0.22648),
		Colour(0.628281, 0.555802, 0.366065),
		51.2);

	Material jade(Colour(0, 0, 0), Colour(0.54, 0.89, 0.63),
		Colour(0.316228, 0.316228, 0.316228),
		12.8);

	Material clay(Colour(0.3, 0.3, 0.3), Colour(0.6, 0.6, 0.6),
		Colour(0.316228, 0.316228, 0.316228),
		12.8);

	raytracer.addLightSource(new AreaLight(Point3D(0, 0, 2), Colour(0.9, 0.9, 0.9) , Vector3D(0,0,0.5), Vector3D(0, 0.5, 0)));

	// Add a unit square into the scene with material mat.
	SceneDagNode* sphere = raytracer.addObject(new UnitSphere(), &gold);
	SceneDagNode* plane = raytracer.addObject(new UnitSquare(), &jade);

	double factor1[3] = { 1.0, 2.0, 1.0 };
	double factor2[3] = { 6.0, 6.0, 6.0 };
	raytracer.translate(sphere, Vector3D(0, 0, -5));
	raytracer.rotate(sphere, 'x', -45);
	raytracer.rotate(sphere, 'z', 45);
	raytracer.scale(sphere, Point3D(0, 0, 0), factor1);

	raytracer.translate(plane, Vector3D(0, 0, -7));
	raytracer.rotate(plane, 'z', 45);
	raytracer.scale(plane, Point3D(0, 0, 0), factor2);
	raytracer.reflection_depth = 1;

	// Render the scene, feel free to make the image smaller for
	// testing purposes.	
	raytracer.render(width, height, eye, view, up, fov, "output_images/area_light_view1.bmp");

	// Render it from a different point of view.
	Point3D eye2(4, 2, 1);
	Vector3D view2(-4, -2, -6);
	raytracer.render(width, height, eye2, view2, up, fov, "output_images/area_light_view2.bmp");
	return 0;}


int octree_scene(int width,int height) {
	Raytracer raytracer;

	// To get the image used in the report use these parameters 
	// width = 1000; height = 300;

	// The only reason the eye isn't exactly at 0 in this scene
	// is because it makes the picture nicer 
	Point3D eye(0, 0, -0.3);

	Vector3D view(0, 0, -1);
	Vector3D up(0, 1, 0);
	double fov = 60;

	Material shiny(Colour(0.2, 0.0, 0.0), Colour(0.2, 0.2, 0.1),
		Colour(0.8, 0.8, 0.8),
		200.);

	// Defines a point light source.
	raytracer.addLightSource(new PointLight(Point3D(0, 10, 0),
		Colour(0.9, 0.9, 0.9)));

	SceneDagNode* sphere ;
	double factor1[3] = { 0.5, 0.5, 0.5 };

	int count=1;

	sphere = raytracer.addObject(new UnitSphere(), &shiny);
	raytracer.scale(sphere, Point3D(0,0,0), factor1);
	raytracer.translate(sphere, Vector3D(0, 0,  -5));


	for(int i=-1.5; i < 1.1; i = i + 1){ 
		for(int j=-1.5; j < 1.1; j = j + 1){
			for(int k=-1.5; k < 1.1; k = k + 1){ 
				sphere = raytracer.addObject(new UnitSphere(), &shiny); 
				raytracer.scale(sphere, Point3D(0,0,0), factor1);
				raytracer.translate(sphere, Vector3D(i*5, j*5, k*5)); 
				count++;
			} 
		} 
	}
	//printf("%d\n",count);
	raytracer.render(width, height, eye, view, up, fov, "output_images/octree_view1.bmp");
	return 0;
}

int antialias_scene(int width,int height) {
	Raytracer raytracer;
	Point3D eye(0, 0, 1);
	Vector3D view(0, 0, -1);
	Vector3D up(0, 1, 0);
	double fov = 60;
	Material gold(Colour(0.3, 0.3, 0.3), Colour(0.75164, 0.60648, 0.22648),
		Colour(0.628281, 0.555802, 0.366065),
		51.2);
	Material jade(Colour(0, 0, 0), Colour(0.54, 0.89, 0.63),
		Colour(0.316228, 0.316228, 0.316228),
		12.8);

	raytracer.addLightSource(new PointLight(Point3D(0, 0, 5),
		Colour(0.9, 0.9, 0.9)));
	SceneDagNode* sphere = raytracer.addObject(new UnitSphere(), &gold);
	SceneDagNode* plane = raytracer.addObject(new UnitSquare(), &jade);
	raytracer.reflection_depth = 1;
	double factor1[3] = { 1.0, 2.0, 1.0 };
	double factor2[3] = { 6.0, 6.0, 6.0 };
	raytracer.translate(sphere, Vector3D(0, 0, -5));
	raytracer.rotate(sphere, 'x', -45);
	raytracer.rotate(sphere, 'z', 45);
	raytracer.scale(sphere, Point3D(0, 0, 0), factor1);
	raytracer.translate(plane, Vector3D(0, 0, -7));
	raytracer.rotate(plane, 'z', 45);
	raytracer.scale(plane, Point3D(0, 0, 0), factor2);

	// Turn on antialiasing
	raytracer.antialias = true;

	raytracer.render(width, height, eye, view, up, fov, "output_images/antialias_view1.bmp");
	Point3D eye2(4, 2, 1);
	Vector3D view2(-4, -2, -6);
	raytracer.render(width, height, eye2, view2, up, fov, "output_images/antialias_view2.bmp");
	return 0;
}

int plane_texture_scene(int width, int height) {
	Raytracer raytracer;
	Point3D eye(0, 0, 5);
	Vector3D view(0, 0, -1);
	Vector3D up(0, 1, 0);
	double fov = 60;

	Material jade(Colour(0, 0, 0), Colour(0.54, 0.89, 0.63),
		Colour(0,0,0),
		12.8);

	raytracer.addLightSource(new PointLight(Point3D(0, 0, 5),
		Colour(0.9, 0.9, 0.9)));
	SceneDagNode* plane = raytracer.addObject(new UnitSquare(), &jade);
		
	double factor1[3] = { 6.0, 6.0, 6.0 };

	raytracer.translate(plane, Vector3D(3.5, 0, -6));
	raytracer.rotate(plane, 'y', -20);
	raytracer.scale(plane, Point3D(0, 0, 0), factor1);

	plane->texture = new Texture();
	plane->has_texture = true;
	plane->texture->loadBitmap("textures/trouble_will_find_me.bmp");

	SceneDagNode* plane2 = raytracer.addObject(new UnitSquare(), &jade);
		
	double factor2[3] = { 9.0, 6.0, 6.0 };

	raytracer.translate(plane2, Vector3D(-4, 0, -9));
	raytracer.rotate(plane2, 'y', 20);
	//raytracer.rotate(plane2, 'x', 45);
	raytracer.scale(plane2, Point3D(0, 0, 0), factor2);

	plane2->texture = new Texture();
	plane2->has_texture = true;
	plane2->texture->loadBitmap("textures/the_national.bmp");

	raytracer.render(width, height, eye, view, up, fov, "output_images/plane_texture_view1.bmp");
	return 0;
}



int default_scene(int width,int height) {
	Raytracer raytracer;
	Point3D eye(0, 0, 1);
	Vector3D view(0, 0, -1);
	Vector3D up(0, 1, 0);
	double fov = 60;
	Material gold(Colour(0.3, 0.3, 0.3), Colour(0.75164, 0.60648, 0.22648),
		Colour(0.628281, 0.555802, 0.366065),
		51.2);
	Material jade(Colour(0, 0, 0), Colour(0.54, 0.89, 0.63),
		Colour(0.316228, 0.316228, 0.316228),
		12.8);
	raytracer.addLightSource(new PointLight(Point3D(0, 0, 5),
		Colour(0.9, 0.9, 0.9)));
	SceneDagNode* sphere = raytracer.addObject(new UnitSphere(), &gold);
	SceneDagNode* plane = raytracer.addObject(new UnitSquare(), &jade);
	raytracer.reflection_depth = 1;
	double factor1[3] = { 1.0, 2.0, 1.0 };
	double factor2[3] = { 6.0, 6.0, 6.0 };
	raytracer.translate(sphere, Vector3D(0, 0, -5));
	raytracer.rotate(sphere, 'x', -45);
	raytracer.rotate(sphere, 'z', 45);
	raytracer.scale(sphere, Point3D(0, 0, 0), factor1);
	raytracer.translate(plane, Vector3D(0, 0, -7));
	raytracer.rotate(plane, 'z', 45);
	raytracer.scale(plane, Point3D(0, 0, 0), factor2);
	raytracer.render(width, height, eye, view, up, fov, "output_images/view1.bmp");
	Point3D eye2(4, 2, 1);
	Vector3D view2(-4, -2, -6);
	raytracer.render(width, height, eye2, view2, up, fov, "output_images/view2.bmp");
	return 0;
}
