//----------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#include <math.h>
#include <stdio.h>      
#include <gl\gl.h>      // Header file for the OpenGL32 library
#include <gl\glu.h>     // Header file for the GLu32 library
#include <gl\glaux.h>   // Header file for the GLaux library
#pragma hdrstop

#define	MAP_SIZE 1608            // ������ ����� ����� Terrain.RAW
#define	STEP_SIZE 1            // ��� ����������� ����� ������
int STEP_GRID = 804;  // ��� ����������� ����� ������
//----------------------------------------------------------------------------
#pragma argsused
//----------------------------------------------------------------------------
HGLRC hRC = NULL;
HDC hDC = NULL;
HWND hWnd = NULL;
HINSTANCE hInstance = NULL;

bool keys[256];
bool active = true;
bool fullscreen = true;

//----------------------------------------------------------------------------
BYTE g_HeightMap[ MAP_SIZE*MAP_SIZE ];	// ����� ������� � ��������

GLfloat xrot = -50.4;	        // X ��������� ���� ��������
GLfloat yrot = 0.3;		        // Y
GLfloat zrot = 60.4;	        	  // Z
GLuint texture[ 1 ];      	     // ������ � ������ ��������(���������)
//float points[ STEP_GRID ][ STEP_GRID][ 3 ];  // ������ ������� ����� ��������
float points[ MAP_SIZE ][ MAP_SIZE][ 3 ];  // ������ ������� ����� ��������

float xa=0, ya=0, za=0;
float ScaleMap = -37;   // ������� �����
float ScaleHeight = 0.335; // ������� ������
float hMov = 54.5f, wMov = 54.5f;
//----------------------------------------------------------------------------
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);   
//----------------------------------------------------------------------------
AUX_RGBImageRec *LoadBMP(char *Filename)
{
	FILE *File = NULL;
	if (!Filename)
		return NULL;
	File = fopen(Filename,"r");
	if (File)
	{
		fclose(File);
		return auxDIBImageLoad(Filename);
	}
	return NULL;
}
//----------------------------------------------------------------------------
int LoadGLTextures()
{
	int Status = false;
	AUX_RGBImageRec *TextureImage[1];
	memset(TextureImage,0,sizeof(void *)*1); 
	if (TextureImage[0] = LoadBMP("Data/Map.bmp"))
	{
		Status = true;
		glGenTextures(1, &texture[0]);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	}
	if (TextureImage[0])
	{
		if (TextureImage[0]->data)
			free(TextureImage[0]->data);
		free(TextureImage[0]);
	}
	return Status;
}
//----------------------------------------------------------------------------
GLvoid ReSizeGLScene(GLsizei width, GLsizei height)
{
   if (height == 0)
      height = 1;          

   glViewport(0, 0, width, height);
   glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// ������ �����������
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();                  
}
//----------------------------------------------------------------------------
// �������� ����� ������ �� terrain.raw
void LoadRawFile(LPSTR strName, int nSize, BYTE *pHeightMap)
{
	FILE *pFile = NULL;
	pFile = fopen( strName, "rb" );
	if ( pFile == NULL )
	{
		MessageBox(NULL, "��� ����� �����", "Error", MB_OK);
		return;
	}
	fread( pHeightMap, 1, nSize, pFile );
	int result = ferror( pFile );
	if (result)
	{
		MessageBox(NULL, "�� ���� ���������", "Error", MB_OK);
	}
	fclose(pFile);
}
//----------------------------------------------------------------------------
int InitGL(GLvoid)      // ������������� ���������� OpenGL
{
   if (!LoadGLTextures())
		return false;
	glEnable(GL_TEXTURE_2D);                // ������������� �������
	glShadeModel(GL_SMOOTH);                // �������� ��� ����������������
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // ���� �������
	glClearDepth(1.0f);                     // ������
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);      // ��������� �����������

   glPolygonMode(GL_BACK, GL_FILL);	   // ���������� �������
	glPolygonMode(GL_FRONT, GL_LINE);	// ���������� �����

	for(int x = 0; x < STEP_GRID; x++)
		for(int y = 0; y < STEP_GRID; y++)
		{
			points[x][y][0] = float((x/5.0f)-4.5f); //����������
			points[x][y][1] = float((y/5.0f)-4.5f);
			points[x][y][2] = 1;                    // ��������� ������� ������
		}
   LoadRawFile("Data/Terrain.raw", MAP_SIZE * MAP_SIZE, g_HeightMap);
	return true;
}
//----------------------------------------------------------------------------
int Height(BYTE *pHeightMap, int X, int Y)
{
	int x = X % MAP_SIZE;
	int y = Y % MAP_SIZE;
	if(!pHeightMap)
      return 0;
   BYTE temp;
   temp = pHeightMap[(y * MAP_SIZE)+x];
  
	return temp;
}
//----------------------------------------------------------------------------
int DrawGLScene(GLvoid)
{
   int x, y, z;
	float float_x, float_y, float_xb, float_yb;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //CLS
	glLoadIdentity();                       // ����� ������
      glTranslatef(0.0f,0.0f,ScaleMap);     // �����������

	glRotatef(xrot,1.0f,0.0f,0.0f);		// �������� X
	glRotatef(yrot,0.0f,1.0f,0.0f);		// Y
	glRotatef(zrot,0.0f,0.0f,1.0f);		// Z

	glBindTexture(GL_TEXTURE_2D, texture[0]);		// ����� ��������
      glBegin(GL_QUADS);
	for( x = 0; x <(STEP_GRID-1); x++ )
		for( y = 0; y <(STEP_GRID-1); y++ )
		{
         float_x = float(x)/(STEP_GRID-1);
			float_y = float(y)/(STEP_GRID-1);
			float_xb = float(x+1)/(STEP_GRID-1);
			float_yb = float(y+1)/(STEP_GRID-1);
         glTexCoord2f(float_x, float_y);	// ����� ������
			glVertex3f( points[x][y][0], points[x][y][1], points[x][y][2]);

			glTexCoord2f( float_x, float_yb );	// ����� �������
			glVertex3f(points[x][y+1][0], points[x][y+1][1], points[x][y+1][2]);

			glTexCoord2f( float_xb, float_yb );	// ������ �������
			glVertex3f( points[x+1][y+1][0], points[x+1][y+1][1], points[x+1][y+1][2]);

			glTexCoord2f( float_xb, float_y );	// ������ ������
			glVertex3f( points[x+1][y][0], points[x+1][y][1], points[x+1][y][2]);
		}
	glEnd();

	int X = 0, Y = 0;
   glBegin( GL_QUADS );		// ��������� ��������
	glBegin( GL_LINES );		// ��������� �����
  	for ( X = 0; X < (STEP_GRID); X = X + STEP_SIZE)
		for ( Y = 0; Y < (STEP_GRID); Y = Y + STEP_SIZE)
		{
			// ��������� �������� �� ����� �����
		 	x = X;
			y = Y;
			z = Height(g_HeightMap, X*(MAP_SIZE/STEP_GRID), MAP_SIZE-Y*(MAP_SIZE/STEP_GRID));

         points[x][y][0] = float((x/5.0f)-wMov);
			points[x][y][1] = float((y/5.0f)-hMov);
         points[X][Y][2] = float(z * ScaleHeight);   

			//glVertex3i(float(x/5.0f)-wMov, float(y/5.0f)-hMov, float(z * ScaleHeight));	// ��������� ������(����� ���������, �� ��� �������)

      }
    	glEnd();
 
   xrot+=xa;
   yrot+=ya;
   zrot+=za;
	return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/*	This Code Creates Our OpenGL Window.  Parameters Are:
 *	title			- Title To Appear At The Top Of The Window
 *	width			- Width Of The GL Window Or Fullscreen Mode
 *	height			- Height Of The GL Window Or Fullscreen Mode
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)*/

GLvoid KillGLWindow(GLvoid)
{
	if (fullscreen)
	{
		ChangeDisplaySettings(NULL,0);
		ShowCursor(true);
	}
	if (hRC)
	{
		if (!wglMakeCurrent(NULL,NULL))
		{
			MessageBox(NULL,"Release of DC and RC failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))             // Are we able to delete the RC?
		{
			MessageBox(NULL,"Release rendering context failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;             // Set RC to NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))        // Are we able to release the DC
	{
		MessageBox(NULL,"Release device context failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC = NULL;             // Set DC to NULL
	}

	if (hWnd && !DestroyWindow(hWnd))       // Are we able to destroy the window?
	{
		MessageBox(NULL,"Could not release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;            // Set hWnd to NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))       // Are we able to unregister class
	{
		MessageBox(NULL,"Could not unregister class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;       // Set hInstance to NULL
	}
}

BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;		// Holds the results after searching for a match
	WNDCLASS	wc;		        // Windows class structure
	DWORD		dwExStyle;              // Window extended style
	DWORD		dwStyle;                // Window style
	RECT		WindowRect;             // Grabs rctangle upper left / lower right values
	WindowRect.left = (long)0;              // Set left value to 0
	WindowRect.right = (long)width;		// Set right value to requested width
	WindowRect.top = (long)0;               // Set top value to 0
	WindowRect.bottom = (long)height;       // Set bottom value to requested height

	fullscreen = fullscreenflag;              // Set the global fullscreen flag

	hInstance               = GetModuleHandle(NULL);		// Grab an instance for our window
	wc.style                = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;   // Redraw on size, and own DC for window
	wc.lpfnWndProc          = (WNDPROC) WndProc;			// WndProc handles messages
	wc.cbClsExtra           = 0;					// No extra window data
	wc.cbWndExtra           = 0;					// No extra window data
	wc.hInstance            = hInstance;				// Set the Instance
	wc.hIcon                = LoadIcon(NULL, IDI_WINLOGO);		// Load the default icon
	wc.hCursor              = LoadCursor(NULL, IDC_ARROW);		// Load the arrow pointer
	wc.hbrBackground        = NULL;					// No background required for GL
	wc.lpszMenuName		= NULL;					// We don't want a menu
	wc.lpszClassName	= "OpenGL";				// Set the class name

	if (!RegisterClass(&wc))					// Attempt to register the window class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);

		return false;   // Return FALSE
	}

	if (fullscreen)         // Attempt fullscreen mode?
	{
		DEVMODE dmScreenSettings;                                       // Device mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	        // Makes sure memory's cleared
		dmScreenSettings.dmSize         = sizeof(dmScreenSettings);     // Size of the devmode structure
		dmScreenSettings.dmPelsWidth	= width;                        // Selected screen width
		dmScreenSettings.dmPelsHeight	= height;                       // Selected screen height
		dmScreenSettings.dmBitsPerPel	= bits;	                        // Selected bits per pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try to set selected mode and get results. NOTE: CDS_FULLSCREEN gets rid of start bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If the mode fails, offer two options. Quit or use windowed mode.
			if (MessageBox(NULL,"The requested fullscreen mode is not supported by\nyour video card. Use windowed mode instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen = false;       // Windowed mode selected. Fullscreen = FALSE
			}
			else
			{
				// Pop up a message box letting user know the program is closing.
				MessageBox(NULL,"Program will now close.","ERROR",MB_OK|MB_ICONSTOP);
				return false;           // Return FALSE
			}
		}
	}

	if (fullscreen)                         // Are We Still In Fullscreen Mode?
	{
		dwExStyle = WS_EX_APPWINDOW;    // Window extended style
		dwStyle = WS_POPUP;		// Windows style
		ShowCursor(false);		// Hide mouse pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;           // Window extended style
		dwStyle=WS_OVERLAPPEDWINDOW;                            // Windows style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);     // Adjust window to true requested size

	// Create the window
	if (!(hWnd = CreateWindowEx(dwExStyle,          // Extended Style For The Window
                "OpenGL",				// Class name
		title,					// Window title
		dwStyle |				// Defined window style
		WS_CLIPSIBLINGS |			// Required window style
		WS_CLIPCHILDREN,			// Required window style
		0, 0,					// Window position
		WindowRect.right-WindowRect.left,	// Calculate window width
		WindowRect.bottom-WindowRect.top,	// Calculate window height
		NULL,					// No parent window
		NULL,					// No menu
		hInstance,				// Instance
		NULL)))					// Dont pass anything to WM_CREATE
	{
		KillGLWindow();                         // Reset the display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;                           // Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd =             // pfd tells windows how we want things to be
	{
		sizeof(PIXELFORMATDESCRIPTOR),          // Size of this pixel format descriptor
		1,					// Version number
		PFD_DRAW_TO_WINDOW |			// Format must support window
		PFD_SUPPORT_OPENGL |			// Format must support OpenGL
		PFD_DOUBLEBUFFER,			// Must support double buffering
		PFD_TYPE_RGBA,				// Request an RGBA format
		bits,					// Select our color depth
		0, 0, 0, 0, 0, 0,			// Color bits ignored
		0,					// No alpha buffer
		0,					// Shift bit ignored
		0,					// No accumulation buffer
		0, 0, 0, 0,				// Accumulation bits ignored
		16,					// 16Bit Z-Buffer (Depth buffer)
		0,					// No stencil buffer
		0,					// No auxiliary buffer
		PFD_MAIN_PLANE,				// Main drawing layer
		0,					// Reserved
		0, 0, 0					// Layer masks ignored
	};

	if (!(hDC=GetDC(hWnd)))         // Did we get a device context?
	{
		KillGLWindow();         // Reset the display
		MessageBox(NULL,"Can't create a GL device context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;           // Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did windows find a matching pixel format?
	{
		KillGLWindow();         // Reset the display
		MessageBox(NULL,"Can't find a suitable pixelformat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;           // Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))       // Are we able to set the pixel format?
	{
		KillGLWindow();         // Reset the display
		MessageBox(NULL,"Can't set the pixelformat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;           // Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))               // Are we able to get a rendering context?
	{
		KillGLWindow();         // Reset the display
		MessageBox(NULL,"Can't create a GL rendering context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;           // Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))    // Try to activate the rendering context
	{
		KillGLWindow();         // Reset the display
		MessageBox(NULL,"Can't activate the GL rendering context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;           // Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);       // Show the window
	SetForegroundWindow(hWnd);      // Slightly higher priority
	SetFocus(hWnd);                 // Sets keyboard focus to the window
	ReSizeGLScene(width, height);   // Set up our perspective GL screen

	if (!InitGL())                  // Initialize our newly created GL window
	{
		KillGLWindow();         // Reset the display
		MessageBox(NULL,"Initialization failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;           // Return FALSE
	}

	return true;                    // Success
}

LRESULT CALLBACK WndProc(HWND hWnd,     // Handle for this window
                        UINT uMsg,      // Message for this window
			WPARAM wParam,  // Additional message information
			LPARAM lParam)  // Additional message information
{
	switch (uMsg)                           // Check for windows messages
	{
		case WM_ACTIVATE:               // Watch for window activate message
		{
			if (!HIWORD(wParam))    // Check minimization state
			{
				active = true;  // Program is active
			}
			else
			{
				active = false; // Program is no longer active
			}

			return 0;               // Return to the message loop
		}

		case WM_SYSCOMMAND:             // Intercept system commands
		{
			switch (wParam)         // Check system calls
			{
				case SC_SCREENSAVE:     // Screensaver trying to start?
				case SC_MONITORPOWER:	// Monitor trying to enter powersave?
				return 0;       // Prevent from happening
			}
			break;                  // Exit
		}

		case WM_CLOSE:                  // Did we receive a close message?
		{
			PostQuitMessage(0);     // Send a quit message
			return 0;               // Jump back
		}

		case WM_KEYDOWN:                // Is a key being held down?
		{
			keys[wParam] = true;    // If so, mark it as TRUE
//����������---------------------------------------------
	      if (wParam == 38)
            xrot+=0.3f;
         if (wParam == 40)
            xrot-=0.3f;
         if (wParam == 39)
            yrot+=0.2f;
         if (wParam == 37)
	         yrot-=0.2f;
         if (wParam == 34)
	         zrot+=0.4f;
         if (wParam == 46)
	         zrot-=0.4f;
         if (wParam == 107)
         {
            ScaleMap+=0.2f;
         }
         if (wParam == 109)
         {
            ScaleMap-=0.2f;
         }
         if (wParam == 36)
            ScaleHeight += 0.001;
         if (wParam == 35)
            ScaleHeight -= 0.001;
         if (wParam ==104)
            hMov-=0.1;
         if (wParam ==98)
            hMov+=0.1;
         if (wParam ==102)
            wMov-=0.1;
         if (wParam ==100)
            wMov+=0.1;

         if (wParam == 81)
            xa+=0.1;
         if (wParam == 87)
            xa-=0.1;
         if (wParam == 65)
            ya+=0.1;
         if (wParam == 83)
            ya-=0.1;
         if (wParam == 90)
            za+=0.1;
         if (wParam == 88)
            za-=0.1;
         if (wParam == 222)
         {
            STEP_GRID += 5;
            if (STEP_GRID > MAP_SIZE) STEP_GRID = MAP_SIZE;
         }
         if (wParam == 191)
         {
            STEP_GRID -= 5;
            if (STEP_GRID < 45) STEP_GRID = 45;
         }
         if (wParam == 32)
         {
            xrot = -40.4;
            yrot = 0.3;
            zrot = 15.4;
            xa=0, ya=0, za=0;
            ScaleMap = -12;
            ScaleHeight = 0.025;
            hMov = 4.5f, wMov = 4.5f;
            STEP_GRID = 45;
         }

//----------------------------------------------------------------
			return 0;               // Jump back
		}

		case WM_KEYUP:                  // Has a key been released?
		{
			keys[wParam] = false;   // If so, mark it as FALSE
			return 0;               // Jump back
		}

		case WM_SIZE:                   // Resize the OpenGL window
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord = Width, HiWord = Height
			return 0;               // Jump back
		}
	}

	// Pass all unhandled messages to DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
        MSG msg;                // Windows message structure
	bool done = false;      // Bool variable to exit loop
   fullscreen = false;

	// Create our OpenGL window
	if (!CreateGLWindow("3D Map",800,600,16,fullscreen))
	{
		return 0;               // Quit if window was not created
	}

	while(!done)            // Loop that runs while done = FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is there a message waiting?
		{
			if (msg.message == WM_QUIT)             // Have we received a quit message?
			{
				done = true;                    // If so done = TRUE
			}
			else                                    // If not, deal with window messages
			{
				TranslateMessage(&msg);         // Translate the message
				DispatchMessage(&msg);          // Dispatch the message
			}
		}
		else            // If there are no messages
		{
			// Draw the scene.  Watch for ESC key and quit messages from DrawGLScene()
			if (active)                             // Program active?
			{
				if (keys[VK_ESCAPE])            // Was ESC pressed?
				{
					done = true;            // ESC signalled a quit
				}
				else                            // Not time to quit, Update screen
				{
					DrawGLScene();          // Draw the scene
					SwapBuffers(hDC);       // Swap buffers (Double buffering)
				}
			}

			if (keys[VK_F1])                        // Is F1 being pressed?
			{
				keys[VK_F1] = false;            // If so make key FALSE
				KillGLWindow();                 // Kill our current window
				fullscreen =! fullscreen;       // Toggle fullscreen / windowed mode
				// Recreate our OpenGL window
				if (!CreateGLWindow("3D Map",800,600,16,fullscreen))
				{
					return 0;               // Quit if window was not created
				}
			}
		}
	}

	// Shutdown
	KillGLWindow();         // Kill the window
	return (msg.wParam);    // Exit the program
}
//---------------------------------------------------------------------------
