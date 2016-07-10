// ========================================================================
// COSC 363  Computer Graphics
// Raytracing Assignment 2
// Name: Mathew Hylkema
// ID: 	 35742180
// ========================================================================

#include <iostream>
#include <cmath>
#include <vector>
#include "Vector.h"
#include "Sphere.h"
#include "Color.h"
#include "Object.h"
#include "Plane.h"
#include <GL/glut.h>
//--added--//
#include "Cone.h"
#include "Cylinder.h"

using namespace std;

const float WIDTH = 20.0;  
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int PPU = 50;  
const int MAX_STEPS = 4;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;
bool refractiveBool = false;

vector<Object*> sceneObjects;

Vector light = Vector(3, 19, -20.0);
Color backgroundCol = Color::GRAY;

//A useful struct
struct PointBundle   
{
	Vector point;
	int index;
	float dist;
};

/*
* This function compares the given ray with all objects in the scene
* and computes the closest point  of intersection.
*/
PointBundle closestPt(Vector pos, Vector dir)
{
    Vector  point(0, 0, 0);
	float min = 10000.0;

	PointBundle out = {point, -1, 0.0};

    for(unsigned int i = 0;  i < sceneObjects.size();  i++)
	{
        float t = sceneObjects[i]->intersect(pos, dir);
		if(t > 0)        //Intersects the object
		{
			point = pos + dir*t;
			if(t < min)
			{
				out.point = point;
				out.index = i;
				out.dist = t;
				min = t;
			}
		}
	}

	return out;
}

/*
* Computes the colour value obtained by tracing a ray.
* If reflections and refractions are to be included, then secondary rays will 
* have to be traced from the point, by converting this method to a recursive
* procedure.
*/

Color trace(Vector pos, Vector dir, int step)
{
	PointBundle q = closestPt(pos, dir);
	Color colorSum;
	Color refractCol = Color::BLACK;
    
    if(q.index == -1) return backgroundCol;	//no intersection
	
	Color col = sceneObjects[q.index]->getColor(); //Object's colour
	
	float non_color = 1.0;
    float Object = 1.01;
	
	//--vectors--//
    Vector n = sceneObjects[q.index]->normal(q.point);
    Vector l = light - q.point;
    float lightDist = l.length();
	l.normalise();
    float lDotn = l.dot(n);
   
	//----Object Intersection vectors----//
	Vector r = ((n * 2) * lDotn) - l;		
	r.normalise();
	Vector viewVector(-dir.x, -dir.y, -dir.z);
	float rDotv = r.dot(viewVector);
	float spec;
	if(rDotv < 0) spec = 0.0;
	else spec = pow(rDotv, 10);;	
	
	PointBundle s = closestPt(q.point, l);
	
	//--Reflection vectors--//
	float vDotn = viewVector.dot(n);
	Vector reflectionVector = ((n*2) * vDotn) - viewVector;
	reflectionVector.normalise();
	
	//--Textured Floor--//
	if (q.index == 5) {
		if ((int(q.point.x) - int(q.point.z)) % 2 == 1)
			col = Color::BLACK;
		else 
			col = Color::WHITE;
    }
    
    if (q.index == 2) {
		if (int(q.point.x) % 7 == 0)
			col = Color::RED;
		else if (int(q.point.x) % 7 == 1)
			col = Color::ORANGE;
		else if (int(q.point.x) % 7 == 2)
			col = Color::YELLOW;
		else if (int(q.point.x) % 7 == 3)
			col = Color::GREEN;
		else if (int(q.point.x) % 7 == 4)
			col = Color::BLUE;
		else if (int(q.point.x) % 7 == 5)
			col = Color(0.3f,0,0.5);
		else col = Color(0.55,0,1);
    }
    
    
    //--Refraction--//
    if (q.index == 0 && step < MAX_STEPS) {
		float ratio = non_color/Object;
		float cos;
		if (refractiveBool) {
			ratio = Object/non_color;
			cos = sqrt(1 - ((ratio) * (ratio)) * (1 - (dir.dot(n) * dir.dot(n))));
			n = n * (-1);
			refractiveBool = !refractiveBool;
		} else {
			cos = sqrt(1 - ((ratio) * (ratio)) * (1 - (dir.dot(n) * dir.dot(n))));
			refractiveBool = !refractiveBool;
		}
		
		Vector refractionVector = dir * ratio - n * ((ratio) * (dir.dot(n)) + cos);
		refractionVector.normalise();
		refractCol = trace(q.point, refractionVector, step+1);
    }  
    
	//--Background--//
    if((s.index>-1 && s.dist < lightDist)|| lDotn <= 0)
		colorSum = col.phongLight(backgroundCol, 0.0, 0.0);
	//--Shadow--//
	else {
		if (s.dist > lightDist) {
			colorSum = col.phongLight(backgroundCol, lDotn, 0);
		} else {
			colorSum = col.phongLight(backgroundCol, lDotn, spec);
		}
	}
	
	//--Reflection--//
	// Spheres
	if(((q.index == 1) || (q.index == 3) || (q.index == -1)) && step < MAX_STEPS)
	{
		float reflCoeff = 1;
		Color reflectionCol = trace(q.point, reflectionVector, step+1);
		colorSum.combineColor(reflectionCol, reflCoeff);
	}
	
	// Roof
	if(((q.index == 9)) && step < MAX_STEPS)
	{
		float reflCoeff = 0.7;
		Color reflectionCol = trace(q.point, reflectionVector, step+1);
		colorSum.combineColor(reflectionCol, reflCoeff);
	}
	
	// Floor
	if(((q.index == 5)) && step < MAX_STEPS)
	{
		float reflCoeff = 0.5;
		Color reflectionCol = trace(q.point, reflectionVector, step+1);
		colorSum.combineColor(reflectionCol, reflCoeff);
	}
	
	colorSum.combineColor(refractCol, 1);
    return colorSum;
}

Color Average(Vector eye, Vector dir, float pixel, float x1, float y1)
{
    int size = 4; // Sampling size (Between 4 and 8)
    
    float R = 0, G = 0, B = 0;
    float part_pixel = pixel/size;
    
	Color PixelSet[size];
    Vector direction;
    
    int object = 0;
    
    for (int x=0; x < size; x++) {
		for (int y=0; y <size; y++) {
			if (((x % 2) == 1) && ((y % 2) == 1)) {
				direction = Vector(x1 + (part_pixel * x), y1 + (part_pixel * y), -EDIST);
				direction.normalise();
				PixelSet[object] = trace(eye, direction, 1);
				object++;
			}
		}
	}
 
    for (int i=0; i<size; i++){
        R += PixelSet[i].r;
        G += PixelSet[i].g;
        B += PixelSet[i].b;
    }
    
    Color col(R/size,G/size,B/size);
    return col;
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each pixel as quads.
//---------------------------------------------------------------------------------------
void display()
{
	int widthInPixels = (int)(WIDTH * PPU);
	int heightInPixels = (int)(HEIGHT * PPU);
	float pixelSize = 1.0/PPU;
	float halfPixel = pixelSize/2.0;
	float x1, y1, xc, yc;
	Vector eye(10, 10, 0.);

	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_QUADS);  //Each pixel is a quad.

	for(int i = 0; i < widthInPixels; i++)	//Scan every "pixel"
	{
		x1 = XMIN + i*pixelSize;
		xc = x1 + halfPixel;
		for(int j = 0; j < heightInPixels; j++)
		{
			y1 = YMIN + j*pixelSize;
			yc = y1 + halfPixel;

		    Vector dir(xc, yc, -EDIST);	//direction of the primary ray
		    
		    //--Super Sampling--//
			Color col_ss = Average(eye,dir,pixelSize,x1,y1);
			Color col = col_ss;
			
			// Used if Supersampling isnt in use. 
		    //dir.normalise();			//Normalise this direction
		    //Color col = trace (eye, dir, 1); //Trace the primary ray and get the colour value
			
			glColor3f(col.r, col.g, col.b);
			glVertex2f(x1, y1);				//Draw each pixel with its color value
			glVertex2f(x1 + pixelSize, y1);
			glVertex2f(x1 + pixelSize, y1 + pixelSize);
			glVertex2f(x1, y1 + pixelSize);
        }
    }

    glEnd();
    glFlush();
}

void initialize()
{
	//--Glass Sphere--//
	Sphere *sphere0 = new Sphere(Vector(10, 6, -50), 3.0, Color::BLACK); // 11, 3, -60
	sceneObjects.push_back(sphere0);
	
	//--Spheres--//
	Sphere *sphere1 = new Sphere(Vector(12, 8, -92), 8.0, Color::BLUE);
	sceneObjects.push_back(sphere1);
	Sphere *sphere2 = new Sphere(Vector(5, 5, -75), 5.0, Color::GREEN);
	sceneObjects.push_back(sphere2);
	Sphere *sphere3 = new Sphere(Vector(6, 2, -55), 2.0, Color::BLACK);
	sceneObjects.push_back(sphere3);
	
	//--Cone--//
	Cone *cone4 = new Cone(Vector(16, 8, -52), 1.5, 5.0, Color::RED);
    sceneObjects.push_back(cone4);
	
	//--Ground--//
	Plane *plane5 = new Plane(Vector(0, 0, -40), Vector(20, 0, -40), Vector(20, 0, -100), Vector(0, 0, -100), Color::WHITE);
	sceneObjects.push_back(plane5); //Ground
	Plane *plane6 = new Plane(Vector(0, 0, -100), Vector(20, 0, -100), Vector(20, 20, -100), Vector(0, 20, -100), Color::PURPLE);
	sceneObjects.push_back(plane6); //Background
	Plane *plane7 = new Plane(Vector(0, 0, -40), Vector(0, 0, -100), Vector(0, 20, -100), Vector(0, 20, -40), Color::PURPLE);
	sceneObjects.push_back(plane7); //Leftside
	Plane *plane8 = new Plane(Vector(20, 0, -100), Vector(20, 0, -40), Vector(20, 20, -40), Vector(20, 20, -100), Color::PURPLE);
	sceneObjects.push_back(plane8); //Rightside
	Plane *plane9 = new Plane(Vector(0, 20, -100), Vector(20, 20, -100), Vector(20, 20, -40), Vector(0, 20, -40), Color::BLACK);
	sceneObjects.push_back(plane9); //Roof
	
	//--Box--//
	Plane *plane10 = new Plane(Vector(14, 4, -50), Vector(18, 4, -50), Vector(18, 4, -54), Vector(14, 4, -54), Color::YELLOW);
	sceneObjects.push_back(plane10); //Top side
	Plane *plane11 = new Plane(Vector(14, 0, -54), Vector(14, 0, -50), Vector(14, 4, -50), Vector(14, 4, -54), Color::YELLOW);
	sceneObjects.push_back(plane11); //Left side
	Plane *plane12 = new Plane(Vector(14, 0, -50), Vector(18, 0, -50), Vector(18, 4, -50), Vector(14, 4, -50), Color::YELLOW);
	sceneObjects.push_back(plane12); //front side
	Plane *plane13 = new Plane(Vector(18, 0, -50), Vector(18, 0, -54), Vector(18, 4, -54), Vector(18, 4, -50), Color::YELLOW);
	sceneObjects.push_back(plane13); //right side
	Plane *plane14 = new Plane(Vector(18, 0, -54), Vector(14, 0, -54), Vector(14, 4, -54), Vector(18, 4, -54), Color::YELLOW);
	sceneObjects.push_back(plane14); //back side
	
	//--Cylinder--//
	Cylinder *cylinder15 = new Cylinder(Vector(16, 4, -52), 1.5, 4.0, Color::GREEN);
    sceneObjects.push_back(cylinder15);
    
    //--Sphere--//
    Sphere *sphere16 = new Sphere(Vector(16, 14.5, -52), 2.0, Color::WHITE);
	sceneObjects.push_back(sphere16);
	
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0, 0, 0, 1);
}

int main(int argc, char *argv[]) 
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(1000, 1000);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracing - Matt Hylkema");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
