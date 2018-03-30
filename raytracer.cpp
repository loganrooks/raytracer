/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		Implementations of functions in raytracer.h,
		and the main function which specifies the
		scene to be rendered.

***********************************************************/

#include "octree.h"
// octree.h includes raytracer.h so we no longer need to include it
//#include "raytracer.h"
#include "bmp_io.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <assert.h>

Raytracer::Raytracer() : _lightSource(NULL) {
	_root = new SceneDagNode();
	use_cubemap = false;
	octreeUsed = false;
	antialias = false;
	reflection_depth = 10;
}	

Raytracer::~Raytracer() {
	delete _root;
}

SceneDagNode* Raytracer::addObject( SceneDagNode* parent,
		SceneObject* obj, Material* mat ) {
	
	SceneDagNode* node = new SceneDagNode( obj, mat );
	node->parent = parent;
	node->next = NULL;
	node->child = NULL;

	// Add the object to the parent's child list, this means
	// whatever transformation applied to the parent will also
	// be applied to the child.
	if (parent->child == NULL) {
		parent->child = node;
	}
	else {
		parent = parent->child;
		while (parent->next != NULL) {
			parent = parent->next;
		}
		parent->next = node;
	}

	return node;;
}

LightListNode* Raytracer::addLightSource( LightSource* light ) {
	LightListNode* tmp = _lightSource;
	_lightSource = new LightListNode( light, tmp );
	return _lightSource;
}

void Raytracer::rotate( SceneDagNode* node, char axis, double angle ) {
	Matrix4x4 rotation;
	double toRadian = 2*M_PI/360.0;
	int i;

	for (i = 0; i < 2; i++) {
		switch(axis) {
			case 'x':
				rotation[0][0] = 1;
				rotation[1][1] = cos(angle*toRadian);
				rotation[1][2] = -sin(angle*toRadian);
				rotation[2][1] = sin(angle*toRadian);
				rotation[2][2] = cos(angle*toRadian);
				rotation[3][3] = 1;
			break;
			case 'y':
				rotation[0][0] = cos(angle*toRadian);
				rotation[0][2] = sin(angle*toRadian);
				rotation[1][1] = 1;
				rotation[2][0] = -sin(angle*toRadian);
				rotation[2][2] = cos(angle*toRadian);
				rotation[3][3] = 1;
			break;
			case 'z':
				rotation[0][0] = cos(angle*toRadian);
				rotation[0][1] = -sin(angle*toRadian);
				rotation[1][0] = sin(angle*toRadian);
				rotation[1][1] = cos(angle*toRadian);
				rotation[2][2] = 1;
				rotation[3][3] = 1;
			break;
		}
		if (i == 0) {
		    node->trans = node->trans*rotation;
			angle = -angle;
		}
		else {
			node->invtrans = rotation*node->invtrans;
		}
	}
}

void Raytracer::translate( SceneDagNode* node, Vector3D trans ) {
	Matrix4x4 translation;

	translation[0][3] = trans[0];
	translation[1][3] = trans[1];
	translation[2][3] = trans[2];
	node->trans = node->trans*translation;
	translation[0][3] = -trans[0];
	translation[1][3] = -trans[1];
	translation[2][3] = -trans[2];
	node->invtrans = translation*node->invtrans;
}

void Raytracer::scale( SceneDagNode* node, Point3D origin, double factor[3] ) {
	Matrix4x4 scale;

	scale[0][0] = factor[0];
	scale[0][3] = origin[0] - factor[0] * origin[0];
	scale[1][1] = factor[1];
	scale[1][3] = origin[1] - factor[1] * origin[1];
	scale[2][2] = factor[2];
	scale[2][3] = origin[2] - factor[2] * origin[2];
	node->trans = node->trans*scale;
	scale[0][0] = 1/factor[0];
	scale[0][3] = origin[0] - 1/factor[0] * origin[0];
	scale[1][1] = 1/factor[1];
	scale[1][3] = origin[1] - 1/factor[1] * origin[1];
	scale[2][2] = 1/factor[2];
	scale[2][3] = origin[2] - 1/factor[2] * origin[2];
	node->invtrans = scale*node->invtrans;
}

Matrix4x4 Raytracer::initInvViewMatrix( Point3D eye, Vector3D view,
		Vector3D up ) {
	Matrix4x4 mat;
	Vector3D w;
	view.normalize();
	up = up - up.dot(view)*view;
	up.normalize();
	w = view.cross(up);

	mat[0][0] = w[0];
	mat[1][0] = w[1];
	mat[2][0] = w[2];
	mat[0][1] = up[0];
	mat[1][1] = up[1];
	mat[2][1] = up[2];
	mat[0][2] = -view[0];
	mat[1][2] = -view[1];
	mat[2][2] = -view[2];
	mat[0][3] = eye[0];
	mat[1][3] = eye[1];
	mat[2][3] = eye[2];

	return mat;
}

void Raytracer::computeTransforms( SceneDagNode* node )
{
    SceneDagNode *childPtr;
    if (node->parent != NULL )
    {
        node->modelToWorld = node->parent->modelToWorld*node->trans;
        node->worldToModel = node->invtrans*node->parent->worldToModel;
    }
    else
    {
        node->modelToWorld = node->trans;
        node->worldToModel = node->invtrans;
    }
    // Traverse the children.
    childPtr = node->child;
    while (childPtr != NULL) {
        computeTransforms(childPtr);
        childPtr = childPtr->next;
    }

}

Colour Texture::get_colour_at_uv(Point3D uv) {
	int i = int(uv[0] * x);
	int j = int(uv[1] * y);

	// we need to divide by 255 since the texture is stored as an int from 0 to 255
	//Colour col = Colour (rarray[i *x + j]/255.0,garray[i *x + j]/255.0,barray[i* x + j]/255.0);
	Colour col = Colour(0,0,0);
	return col;
}

void Raytracer::addTextureInfo(SceneDagNode* node, Ray3D& ray)
{
	ray.intersection.has_texture = true;
	ray.intersection.texture_colour = node->texture->get_colour_at_uv(ray.intersection.uv_coords);
	//ray.intersection.normal = ray.intersection.normal
}

void Raytracer::traverseScene( SceneDagNode* node, Ray3D& ray ) {
    SceneDagNode *childPtr;

    // Applies transformation of the current node to the global
    // transformation matrices.
    if (node->obj) {
        // Perform intersection.
        if (node->obj->intersect(ray, node->worldToModel, node->modelToWorld)) {
            ray.intersection.mat = node->mat;
			if (node->has_texture) {
				addTextureInfo(node,ray);
			}
			else {
				ray.intersection.has_texture = false;
			}
        }
    }
    // Traverse the children.
    childPtr = node->child;
    while (childPtr != NULL) {
        traverseScene(childPtr, ray);
        childPtr = childPtr->next;
    }

}

void Raytracer::computeShading( Ray3D& ray ) {
    LightListNode* curLight = _lightSource;
    for (;;) {
        if (curLight == NULL) break;
        	Ray3D shadow_ray;
			if (curLight->light->soft_shadows == false) {

				shadow_ray.dir = curLight->light->get_position() - ray.intersection.point;

				// move epsilon=0.001 along the ray so that we do not consider the intersection
				// that occurs precisely at ray.intersection.point

				shadow_ray.origin = ray.intersection.point + 0.001 * shadow_ray.dir;
				shadow_ray.intersection.t_value = std::numeric_limits<double>::max();

				if(octreeUsed==false){
					traverseScene(_root, shadow_ray);
				} else {
					traverseTree(shadow_ray);
				}

				//if ray intersects an object set inShadow so only the ambient component is added
				if (!shadow_ray.intersection.none)
				{
					ray.inShadow = true;
				} 

				curLight->light->shade(ray);
			}
			
			
			else {
				//  soft shadows are implemented herehere
				
				// these will be modified by the get_samples function
				// Point3D is the array of sample positions used for checking shadows
				int num_samples = 0;
				Point3D* sample_positions;
			
				curLight->light->get_samples(&sample_positions,&num_samples);
	

				for (int i = 0;i < num_samples;i++)
				{
					shadow_ray.dir = curLight->light->get_position() - ray.intersection.point;
					shadow_ray.dir = sample_positions[i] - ray.intersection.point;

					// move epsilon=0.001 along the ray so that we do not consider the intersection
					// that occurs precisely at ray.intersection.point

					shadow_ray.origin = ray.intersection.point + 0.001 * shadow_ray.dir;
					shadow_ray.intersection.t_value = std::numeric_limits<double>::max();

					traverseScene(_root, shadow_ray);

					if (shadow_ray.intersection.none || shadow_ray.intersection.t_value > 1.0)
					{
						// this gets called num_samples times
						// the AreaLight keeps an internal count of which sample we are on
						curLight->light->shade(ray);
					}
					else {
						// else shade will not be called for this sample pair
						// we need to increment samples_so_far here
						curLight->light->samples_so_far++;
					}
				}
			}
		


        curLight = curLight->next;
    }

}

void Raytracer::initPixelBuffer() {
    int numbytes = _scrWidth * _scrHeight * sizeof(unsigned char);
    _rbuffer = new unsigned char[numbytes];
    std::fill_n(_rbuffer, numbytes,0);
    _gbuffer = new unsigned char[numbytes];
    std::fill_n(_gbuffer, numbytes,0);
    _bbuffer = new unsigned char[numbytes];
    std::fill_n(_bbuffer, numbytes,0);
}

void Raytracer::flushPixelBuffer( std::string file_name ) {
    bmp_write( file_name.c_str(), _scrWidth, _scrHeight, _rbuffer, _gbuffer, _bbuffer );
    delete _rbuffer;
    delete _gbuffer;
    delete _bbuffer;
}

Colour Raytracer::shadeRay( Ray3D& ray, int depth ) {
    Colour col(0.0, 0.0, 0.0);
    traverseScene(_root, ray);

    // Don't bother shading if the ray didn't hit
    // anything.

    if (!ray.intersection.none) {

        computeShading(ray);

		if (depth < reflection_depth){
			// if we haven't reached the max recursion depth,
			// set up the reflected ray and compute its shading

			Ray3D reflected_ray;

			reflected_ray.dir =  (ray.dir -  2 * ray.dir.dot(ray.intersection.normal) * ray.intersection.normal);

			reflected_ray.reflected = true;
			reflected_ray.dir.normalize();
			reflected_ray.origin = ray.intersection.point + 0.001 * reflected_ray.dir;
			reflected_ray.intersection.t_value = std::numeric_limits<double>::max();
			reflected_ray.intersection.has_texture = false;

			// We re-use the specular component of the material as the coefficient for reflection
			// This is the way it is done in the textbook for the course
			// We acknowledge that it is more physically appropriate to have a separate coefficient 
			// for reflection but we chose to follow the textbook

			ray.col = ray.col + ray.intersection.mat->specular * shadeRay(reflected_ray, depth + 1);
			ray.col.clamp();
		}

        col = ray.col;
    }
	else {

		// else it intersects nothing, get cube map value for the ray direction
		if (use_cubemap){
			col = cube_map->query_bmp_cube_map(ray.dir);
		}
	}

    return col;
}


void Raytracer::render( int width, int height, Point3D eye, Vector3D view,
        Vector3D up, double fov, std::string fileName) {
    computeTransforms(_root);
    Matrix4x4 viewToWorld;
    _scrWidth = width;
    _scrHeight = height;
    double factor = (double(height)/2)/tan(fov*M_PI/360.0);

    //printf("octreeUsed:%d\n",octreeUsed );
    initPixelBuffer();
    viewToWorld = initInvViewMatrix(eye, view, up);

	if(octreeUsed){
	    Octree * _octree = makeTree();
		octree = _octree;
	}
	//loadOBJFile( _root, "bunny.obj", _root->child->mat);
	
    if(use_cubemap) {
        cube_map = new CubeMap;
        cube_map->set_face_images();
    }

    // Construct a ray for each pixel.
    for (int i = 0; i < _scrHeight; i++) {
        for (int j = 0; j < _scrWidth; j++) {

			Colour col;
			if (antialias == false) {
				//pixel color
				col = traceRay(-double(_scrHeight) / 2 + 0.5 + i, -double(_scrWidth) / 2 + 0.5 + j, factor, viewToWorld);
			}
			else {
				// take multiple samples per pixel to do antialiasing
				// based off formula in textbook, p 230
				double n = 5;
				double noise;
				for (int p = 0; p < n; p++) {
					for (int p = 0; p < n; p++) {
						// noise term varies between 0 and 1
						noise = (double) rand() / RAND_MAX;
						col = col + traceRay(-double(_scrHeight) / 2 + (p+noise)/n + i, -double(_scrWidth) / 2 + (p + noise)/n + j, factor, viewToWorld);
					}
				}
				col = (1.0 / pow(n, 2)) * col;
			}

			_rbuffer[i*width+j] = int(col[0]*255);
			_gbuffer[i*width+j] = int(col[1]*255);
			_bbuffer[i*width+j] = int(col[2]*255);
		}
	}

	flushPixelBuffer(fileName);
}

Colour Raytracer::traceRay(double coord1, double coord2, double factor, Matrix4x4 viewToWorld) {

	Point3D origin(0, 0, 0);
	// Sets up ray origin and direction in view space,
	// image plane is at z = -1.
	Point3D imagePlane;

	imagePlane[0] = (coord2) / factor;
	imagePlane[1] = (coord1) / factor;
	imagePlane[2] = -1;

	Ray3D ray;

	ray.dir = viewToWorld * (imagePlane - origin);
	ray.dir.normalize();
	ray.origin = viewToWorld * imagePlane;
	
	ray.intersection.has_texture = false;

	// we want to compare t values and keep the lowest one,
	// so we start the t value off at the maximum possible value for a double
	ray.intersection.t_value = std::numeric_limits<double>::max();

	if(octreeUsed == false){
		return shadeRay(ray, 0);
	} else {
		return octreeShadeRay(ray, 0);
		
	}

}


#define X 5;
#define Y 5;

// We need to make these constants so that we can assign 
// C-style arrays at compile time

// This was used to create a simple checkerboard texture for 
// debugging
void Texture::makeCheckerboard(){

	x = X;
	y = Y;
	/*
	rarray = new unsigned char[25];
	garray = new unsigned char[25];
	barray = new unsigned char[25];
	
	for(unsigned long int i = 0; i < 5; i++){
		for(unsigned int j = 0; j < 5; j++){
			if((i%2)^(j%2)){
				rarray[i * 5 + j] = 255;
				garray[i * 5 + j] = 255;
				barray[i * 5 + j] = 255;
			}
			else {
				rarray[i * 5 + j] = 0;
				garray[i * 5 + j] = 0;
				barray[i * 5 + j] = 0;
			}
		}
	}
	*/
}

// load a bitmap into the Texture class's internal
// variables
void Texture::loadBitmap(const char * filename){
	//bmp_read(filename, &x, &y, &rarray, &garray, &barray);
}

Point3D CubeMap::direction_to_cube_map_uv(Vector3D direction, int* face) {

	// Used the wikipedia article for cube mapping as a guide
	// As well as the textbook
	// Use the convention from the wikipedia article since in the book, +z is the up-direction
	// In the cube mapping code I assume that positive y is the up-direction, as given in main.cpp

	double u; double v;
	double x = direction[0];
	double y = direction[1];
	double z = direction[2];

	double abs_x = fabs(x);
	double abs_y = fabs(y);
	double abs_z = fabs(z);

	// find the which face of the infinitely
	// large cube we will intersect with
	// by checking which component has biggest magnitude
	bool x_is_biggest = (abs_x >= abs_y) && (abs_x >= abs_z);
	bool y_is_biggest = (abs_y >= abs_x) && (abs_y >= abs_z);
	bool z_is_biggest = (abs_z >= abs_x) && (abs_z >= abs_y);

	// then, use the appropriate formula
	// for that face to get (u,v) from the
	// ray's direction
	if (x_is_biggest)
	{
		if (x > 0)
		{
			u = (-z + x) / (2 * x);
			v = (-y + x) / (2 * x);
			*face = 0;
		}
		else {
			u = (-	z + x) / (2 * x);
			v = (y + x) / (2 * x);
			*face = 1;
		}
	}
	else if (y_is_biggest)
	{
		if (y > 0)
		{
			u = (x + y) / (2 * y);
			v = (z + y) / (2 * y);
			*face = 2;
		}
		else {
			u = (-x + y) / (2 * y);
			v = (z + y) / (2 * y);
			*face = 3;
		}
	}
	else if (z_is_biggest) {
		if (z > 0)
		{
			u = (x + z) / (2 * z);
			v = (-y + z) / (2 * z);
			*face = 4;
		}
		else {
			u = (x + z) / (2 * z);
			v = (y + z) / (2 * z);
			*face = 5;
		}

	}

	// We do this flipping because 
	// The .bmp file coordinates 
	// are flipped and stored as Y,X 

	//otherwise we would do Point3D(u,v,0)
	return Point3D(1-v, 1-u, 0);
}


void CubeMap::set_face_images() {
	// we store the cube map
	// as six square .bmp images
	face0 = new Texture();
	face0->loadBitmap("side1.bmp"); // pos_x
	face1 = new Texture();
	face1->loadBitmap("side3.bmp"); // neg_x
	face2 = new Texture();
	face2->loadBitmap("side6.bmp"); // pos_y
	face3 = new Texture();
	face3->loadBitmap("side5.bmp"); // neg_y
	face4 = new Texture();
	face4->loadBitmap("side2.bmp"); // pos_z
	face5 = new Texture();
	face5->loadBitmap("side4.bmp"); // neg_z
}

Colour CubeMap::query_bmp_cube_map(Vector3D direction)
{
	int face = -1;
	Colour col = Colour(0, 0, 0);
	
	Point3D uv = direction_to_cube_map_uv(direction, &face);
	Texture* txt;
	switch (face)
	{
	case 0:
		txt = face0;
		break;
	case 1:
		txt = face1;
		break;
	case 2:
		txt = face2;
		break;
	case 3:
		txt = face3;
		break;
	case 4:
		txt = face4;
		break;
	case 5:
		txt = face5;
		break;
	}
	col = txt->get_colour_at_uv(uv);
	return col;
}


// the rest is only called when octreeUsed == true

///////////////////This function replaces shaderay//////////////////////////////
/////////Basically the same, but calls traverse tree and itself recursively/////////
Colour Raytracer::octreeShadeRay(Ray3D& ray, int depth){
	Colour col(0.0, 0.0, 0.0);

	//for now this call searches entire octree, each object in each
    traverseTree(ray);

    // Don't bother shading if the ray didn't hit
    // anything.
    if (!ray.intersection.none) {
    	
        computeShading(ray);
        
		if (depth < reflection_depth){
			// if we haven't reached the max recursion depth,
			// set up the reflected ray and compute its shading

			Ray3D reflected_ray;

			reflected_ray.dir = ray.dir + ( -2 * ray.dir.dot(ray.intersection.normal) )* ray.intersection.normal;
			
			reflected_ray.reflected = true;
			reflected_ray.dir.normalize();
			reflected_ray.origin = ray.intersection.point + 0.001 * reflected_ray.dir;
			reflected_ray.intersection.t_value = std::numeric_limits<double>::max();
			reflected_ray.intersection.has_texture = false;

			ray.col = ray.col + ray.intersection.mat->specular * octreeShadeRay(reflected_ray, depth + 1);
			ray.col.clamp();
		}
		
		
        col = ray.col;
    }
	else {

	}
	
    return col;
}


////////////This function is used to traverse the octree////////////////
////////////It makes calls to helper functions to find optimal search route///////////////
void Raytracer::traverseTree(Ray3D& ray){
	HashEntry * curEntry;
	List * curList;
	ListNode * temp;
	Ray3D extendedRay;
	extendedRay.origin = ray.origin;
	extendedRay.dir = ray.dir;
	
	//determine starting node for the given ray
	int best = findStartNode(ray);
	bool found = false;
	
	if (best > 0){



		//if you haven't found an intersection
		while (!found){

			//get the starting node
			curEntry = octree->findEntry(best);
			if (curEntry != NULL){
				
				curList = curEntry->getList();
				temp = curList->getHead();
			}


			//loop through the objects in this node's list to check for intersections
			while (temp != NULL){

				//if the ray intersects the object
				if(temp->getObject()->obj->intersect(ray, temp->getObject()->worldToModel, temp->getObject()->modelToWorld)){
					ray.intersection.mat = temp->getObject()->mat;
					found = true;
				}
				temp = temp->getNext();
			}

			//if you didnt find any intersections in this node
			if(!found){
				
				//create a mimic ray that starts at the point where the original ray left this node
				Point3D newOrigin = extendRay(extendedRay, curEntry);
				extendedRay.origin = newOrigin;

				//find the start node for this new ray -> next node for real ray. 
				best = findStartNode(extendedRay);
				
				if (best<0){
					//ray is leaving the scene does not intersect any objects
					found = true;
					ray.intersection.none = true;
				}
			}

			curList=NULL;
			curEntry=NULL;
			temp=NULL;
		}
	}
}	


//determines the point immediately after the ray leaves the current voxel
Point3D Raytracer::extendRay(Ray3D& ray, HashEntry * node){
	
	
	
	double t_val = 0;
	bool done = false;
	double curX;
	double curY; 
	double curZ; 
	double * dims = node->getDims();

	while(!done){
		
		curX = ray.origin[0] + t_val*ray.dir[0];
		curY = ray.origin[1] + t_val*ray.dir[1];
		curZ = ray.origin[2] + t_val*ray.dir[2];
		

		if (curX < dims[0]){
			return Point3D(curX, curY, curZ);
			done=true;
		} else if (curX > dims[1]){
			return Point3D(curX, curY, curZ);
			done=true;
		} else if (curY < dims[2]){
			return Point3D(curX, curY, curZ);
			done=true;
		}else if (curY > dims[3]){
			return Point3D(curX, curY, curZ);
			done=true;
		}else if (curZ < dims[4]){
			return Point3D(curX, curY, curZ);
			done=true;
		}else if (curZ > dims[5]){
			return Point3D(curX, curY, curZ);
			done=true;
		}

		t_val = t_val + 2;
	}

	

}


// Define constants for the octree dimensions
// We need constants so we can use C-style arrays
// This means that we need to adjust these for each
// individual scene
//if changing, also change in octree.cpp
#define MIN_X -20
#define MAX_X 20
#define MIN_Y -20
#define MAX_Y 20
#define MIN_Z -20
#define MAX_Z 20


//checks which voxel the point is contained in... EG 4->45->456
int Raytracer::findStartNode (Ray3D& ray){

	double x_mid = (MAX_X + MIN_X)/2;
	double y_mid = (MAX_Y + MIN_Y)/2;
	double z_mid = (MAX_Z + MIN_Z)/2;
	bool doit=false;

	//check boudnaries for extended rays...
	if(ray.origin[0] < MAX_X && ray.origin[0] > MIN_X){
		if(ray.origin[1] < MAX_Y && ray.origin[1] > MIN_Y){
			if(ray.origin[2] < MAX_Z && ray.origin[2] > MIN_Z){
				doit = true;
			}
		}
	}
	if (doit) {
		return getQuad(ray, x_mid, y_mid, z_mid, 0);
	} else {
		return -1;
	}



}

//recursively returns best starting voxel given a ray and a midpoint
int Raytracer::getQuad(Ray3D& ray, double x, double y, double z, int parentID){
	HashEntry * finder;
	int levelID;
	int j;

	if(ray.origin[0] < x){
		if(ray.origin[1] < y){
			if (ray.origin[2] < z){
				levelID = 7;
			} else {
				levelID = 5;
			}
		} else {
			if (ray.origin[2] < z){
				levelID = 3;
			} else {
				levelID = 1;
			}
		}
	} else {
		if(ray.origin[1] < y){
			if (ray.origin[2] < z){
				levelID = 8;
			} else {
				levelID = 6;
			}
		} else {
			if (ray.origin[2] < z){
				levelID = 4;
			} else {
				levelID = 2;
			}
		}
	}
	if(parentID != 0){
	double * dims = octree->findEntry(parentID)->getDims();
	}

	
	

	int curID = 10*parentID + levelID;

	if (curID < 1000){
		finder = octree->findEntry(curID);
	} else {
		return curID;
	}
	

	if (finder != NULL){
		if (finder->getSubdivided()==true){

			double * tempDims = finder->getDims(); 

			double temp_x = (tempDims[1]+tempDims[0])/2;
			double temp_y = (tempDims[3]+tempDims[2])/2;
			double temp_z = (tempDims[5]+tempDims[4])/2;

			return getQuad(ray, temp_x, temp_y ,temp_z ,curID);
		} else {	
			return curID;
		}
	}	else {	
			return curID;
		}

}