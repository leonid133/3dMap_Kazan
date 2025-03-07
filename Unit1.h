//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>    // Header file for windows
#include <math.h>	// For the Sin() function
#include <stdio.h>      // Header file for standard Input/Output
#include <gl\gl.h>      // Header file for the OpenGL32 library
#include <gl\glu.h>     // Header file for the GLu32 library
#include <gl\glaux.h>   // Header file for the GLaux library
//---------------------------------------------------------------------------
#define	MAP_SIZE 1024           // Size of our .RAW height map
#define	STEP_SIZE 16            // Width and height of each quad
#define	HEIGHT_RATIO 1.5f       // Ratio that the Y is scaled according to the X and Z
//---------------------------------------------------------------------------

HGLRC hRC = NULL;               // Permanent rendering context
HDC hDC = NULL;                 // Private GDI device context
HWND hWnd = NULL;               // Holds our window handle
HINSTANCE hInstance = NULL;     // Holds the instance of the application

bool keys[256];                 // Array used for the keyboard routine
bool active = true;             // Window active flag set to TRUE by default
bool fullscreen = true;         // Fullscreen flag set to fullscreen mode by default

BYTE g_HeightMap[MAP_SIZE*MAP_SIZE];	// Holds the height map data
float scaleValue = 0.15f;		// Scale value for the terrain

GLfloat xrot;	                // X rotation
GLfloat yrot;		        // Y rotation
GLfloat zrot;	        	// Z rotation

GLuint texture[1];      	// Storage for one texture

float points[ 45 ][ 45 ][3];	// The array for the points on the grid of our "Wave"
int wiggle_count = 0;		// Counter used to control how fast flag waves
GLfloat hold;			// Temporarily holds a floating point value

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);   // Declaration for WndProc

//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#endif
 