#include "raytracer.h"

#ifndef _OCTREE_
#define _OCTREE_

#include <iostream>
#include <cmath>


//forward declarations
class Octree;
class List;
class ListNode;

//class for a List node, points to an object and the next node in the list
class ListNode {
public:
	ListNode(){
		object = NULL;
		next = NULL;
	}
	ListNode(SceneDagNode * node){
		object = node;
		next= NULL;
	}
	SceneDagNode * getObject(){
		return object;
	}
	ListNode * getNext(){
		return next;
	}
	void setNext(ListNode * _next){
		next = _next;
	}
	void setObject(SceneDagNode * obj){
		object = obj;
	}
private:
	SceneDagNode * object;
	ListNode * next;

};


//class for a list, head points to first item in list
class List {
public:
	List(){
		head = NULL;
	}

	ListNode * getHead(){
		return head;
	}

	void setHead(ListNode* _head){
		head = _head;
	}

	void addNode (SceneDagNode * newNode) {
		ListNode * temp;
		if (head == NULL){
			head = new ListNode(newNode);
			head->setObject(newNode);
		} else {
			temp = head;
			while (temp->getNext() != NULL ){
				temp = temp->getNext();
			}
			ListNode * temp2 = new ListNode(newNode);
			temp2->setObject(newNode);
			temp->setNext(temp2);

		}
	}

private:
	ListNode * head;
};




//class for hash table entry
//contains a list of objects that intersect as well as dimensions in world space
class HashEntry {

public:

	HashEntry(int parentID, int newID, double parentDims[6]);
	double subdivide(Octree * octree, List * parentList, int depth, double sideLen);

	int getID(){
		return ID;
	}
	int getSubdivided(){
		return subdivided;
	}
	int getListSize(){
		return listSize;
	}
	List * getList(){
		return objectList;
	}
	double * getDims(){
		return dims;
	}
	void increaseListSize(){
		listSize++;
	}
	void addNode (SceneDagNode * newNode) {
		ListNode * temp;
		ListNode * temp2;
		if (objectList->getHead() == NULL){
			temp2 = new ListNode(newNode);
			temp2->setObject(newNode);
			objectList->setHead(temp2);
		} else {
			temp = objectList->getHead();
			while (temp->getNext() != NULL ){
				temp = temp->getNext();
			}
			temp2 = new ListNode(newNode);
			temp2->setObject(newNode);
			temp->setNext(temp2);
			}
	}

private:

	int ID;
	bool subdivided;
	int listSize;
	List * objectList;

	//dims[0] = min_x, dims[1] = max_x, dims[2] = min_y, you get the idea
	double dims[6];

	//never use these! for hash table collision
	HashEntry * prev;
	HashEntry * next;

};



//class for an octree
//basically just a hash table
class Octree {
public:
	Octree();
	int addEntry(HashEntry * newEntry, List * parentList);
	HashEntry * findEntry(int ID);
	int getNumEntries(){
		return numEntries;
	}
	void setNumEntries(int num){
		numEntries = num;
	}
private:
	int numEntries;
	HashEntry ** hashTable;


};












#endif
