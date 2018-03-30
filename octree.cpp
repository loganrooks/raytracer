#include "octree.h"


#include <cstdlib>
#include <cstdio>


//definitions
#define MAX_SUBS 3
#define NUM_OBJS 2
#define TABLE_SIZE 2056
#define MIN_X -20
#define MAX_X 20
#define MIN_Y -20
#define MAX_Y 20
#define MIN_Z -20
#define MAX_Z 20
#define MAX_SIDE_LEN 40

//declarations
int hashFunc(int key);
void createList(SceneDagNode * root,List * sceneList);


//"main" funciton for octree declaration
//made this a member of raytracer to facilitate calling it from render function
Octree * Raytracer::makeTree(){
	//nothing
	//std::cout << "Hello" << "\n";
	Octree * _octree = new Octree();
	List * sceneList = new List();

	//SceneDagNode * root = getRoot();
	createList(_root, sceneList);

	//set scene dimensions
	double sceneDims[6];
	sceneDims[0] = MIN_X;
	sceneDims[1] = MAX_X;
	sceneDims[2] = MIN_Y;
	sceneDims[3] = MAX_Y;
	sceneDims[4] = MIN_Z;
	sceneDims[5] = MAX_Z;

	//add all to scene list
	HashEntry * temp = new HashEntry(0, 0, sceneDims);
	double result = temp ->subdivide(_octree, sceneList,0, MAX_SIDE_LEN);

	octreeMinSideLen = result;

	return _octree;
	
}


//very basic hash function
int hashFunc (int key){

	int hashval = key % TABLE_SIZE;
	return hashval;
}

//traverse original object tree to set up scene list
//only called once, when you are resetting the tree
void createList(SceneDagNode * root,List * sceneList){

	if(root->obj){
		sceneList->addNode(root);
		//std::cout << "Add to list \n";
	}
		SceneDagNode * temp = root;

		temp = root->child;
	    while (temp != NULL) {
	        createList(temp, sceneList);
	        temp = temp->next;
	    }
	
}

//checks if the object intersects with the subdivision
bool objIntersects(SceneDagNode * node, double * dims ){
	
	//call octree_intersects... definition in scene_object.cpp
	if(node->obj->octree_intersect(dims, node->worldToModel, node->modelToWorld)){

			return true;
	}
		

	return false;
}


//add an entry to the octree
int Octree::addEntry(HashEntry * newEntry, List * parentList){
	
	int hashVal = hashFunc(newEntry->getID());
	ListNode * temp = parentList->getHead();

	//loop through objects that parent voxel intersects with
	do {
		if (objIntersects(temp->getObject(), newEntry->getDims())) { //object intersects

			//add to voxel's list
			newEntry->addNode(temp->getObject());
			newEntry->increaseListSize();

		}
		temp = temp->getNext();

	} while (temp != NULL);
	

	if(hashTable[hashVal] != NULL){
		//duplicate entry indicator
		//if we get this far need to handle collision (already set up)
		//std::cout << "Already Exists " << newEntry->getID() << "\n";
		return 0;
	} else {
		//add to hashtable
		hashTable[hashVal] = newEntry;
	}

	return hashVal;


}

//search function for hashtable entries
HashEntry * Octree::findEntry(int ID){

	int hashVal = hashFunc(ID);

	if(hashTable[hashVal]==NULL){
		return NULL;
	} else {
		//it exists!
		if(hashTable[hashVal]->getID() == ID){
			return hashTable[hashVal];
		} else {
			//handle list
		}
	}
}

//recursive subdivision function
//set max num objects allowed here
//all calls to add entry done here (for now)
double HashEntry::subdivide(Octree * octree, List * parentList, int depth, double sidelen){
	//std::cout << "here\n";
	if (depth < MAX_SUBS){
		subdivided = true;
		double result = sidelen;
		octree->setNumEntries(octree->getNumEntries() + 8);

		//add 8 new entries
		for (int i=0; i<8; i++){
			HashEntry * newEntry = new HashEntry(ID, i+1, dims);
			int result = octree->addEntry(newEntry,parentList);
			if (newEntry->getListSize()>NUM_OBJS){
				 result = newEntry->subdivide(octree, newEntry->getList(), depth+1, sidelen/2);
			}
			
		}
		return result;
	} else {
		return sidelen;
	}
}

//constructors
Octree::Octree(){
	//initialize hashtable
	hashTable = new HashEntry *[TABLE_SIZE]();
	numEntries = 0;
	//hashTable [4] = new HashEntry();
	

}


//determine dimensions for new subdivision
//helper function for HashEntry constructor
void computeDims(double dims[], double parentDims[], int newID){

	
	if (newID == 1){

		dims[0] = parentDims[0];
		dims[1] = (parentDims[1] + parentDims[0])/2;
		dims[2] = (parentDims[2] + parentDims[3])/2;
		dims[3] = parentDims[3];
		dims[4] = (parentDims[4] + parentDims[5])/2;
		dims[5] = parentDims[5];
		
	} else if (newID == 2){

		dims[0] = (parentDims[1] + parentDims[0])/2;
		dims[1] = parentDims[1];
		dims[2] = (parentDims[2] + parentDims[3])/2;
		dims[3] = parentDims[3];
		dims[4] = (parentDims[4] + parentDims[5])/2;
		dims[5] = parentDims[5];
		
	} else if (newID == 3){
		
		dims[0] = parentDims[0];
		dims[1] = (parentDims[1] + parentDims[0])/2;
		dims[2] = (parentDims[2] + parentDims[3])/2;
		dims[3] = parentDims[3];
		dims[4] = parentDims[4];
		dims[5] = (parentDims[4] + parentDims[5])/2;


	} else if (newID == 4){

		dims[0] = (parentDims[1] + parentDims[0])/2;
		dims[1] = parentDims[1];
		dims[2] = (parentDims[2] + parentDims[3])/2;
		dims[3] = parentDims[3];
		dims[4] = parentDims[4];
		dims[5] = (parentDims[4] + parentDims[5])/2;

		
	} else if (newID == 5){
		
		dims[0] = parentDims[0];
		dims[1] = (parentDims[1] + parentDims[0])/2;
		dims[2] = parentDims[2];
		dims[3] = (parentDims[2] + parentDims[3])/2;
		dims[4] = (parentDims[4] + parentDims[5])/2;
		dims[5] = parentDims[5];

	} else if (newID == 6){

		dims[0] = (parentDims[1] + parentDims[0])/2;
		dims[1] = parentDims[1];
		dims[2] = parentDims[2];
		dims[3] = (parentDims[2] + parentDims[3])/2;
		dims[4] = (parentDims[4] + parentDims[5])/2;
		dims[5] = parentDims[5];
		
	} else if (newID == 7){
		dims[0] = parentDims[0];
		dims[1] = (parentDims[1] + parentDims[0])/2;
		dims[2] = parentDims[2];
		dims[3] = (parentDims[2] + parentDims[3])/2;
		dims[4] = parentDims[4];
		dims[5] = (parentDims[4] + parentDims[5])/2;
		
	} else if (newID == 8){
		
		dims[0] = (parentDims[1] + parentDims[0])/2;
		dims[1] = parentDims[1];
		dims[2] = parentDims[2];
		dims[3] = (parentDims[2] + parentDims[3])/2;
		dims[4] = parentDims[4];
		dims[5] = (parentDims[4] + parentDims[5])/2;

	} else {
		for (int i=0;i<6;i++){
		dims[i] = parentDims[i];
		}
	}


}

//HashEntry constructor
HashEntry::HashEntry(int parentID, int newID, double parentDims[6]){
	//only called from subdivide


	//original split parentID==0
	ID = (10*parentID)+newID;

	//compute dimensions for new voxel
	computeDims(dims, parentDims, newID);

	//initialize empty list
	objectList = new List();

	subdivided = false;
	listSize = 0;
	HashEntry * prev;
	HashEntry * next;
}