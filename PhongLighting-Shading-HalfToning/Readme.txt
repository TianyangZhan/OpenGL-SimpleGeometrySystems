ECS 175 Project 3
“THE PHONG LIGHTING MODEL, GOURAUD SHADING, HALF-TONING, AND THE PAINTER’S ALGORITHM”
Author
Name: Tianyang Zhan        
SID: 913739374
Environment
* Operating System:Ubuntu 12.04 amd64
* IDE:Using Code::Blocks 10.05 for coding, Make for compiling and running
* G++ Version:Ubuntu/Linaro 4.6.3-1ubuntu5
Prerequisite
* OpenGL
* GLUT
Commands
1.Compile the program using Makefile
$   make 
2. Run the program                                                                                  
$   ./graphics
3. Clean up the folder AFTER program ends
$   make clean
Running the program
1. Setup config files (not required) (Refer to Config Files)
2. Run the program (Refer to Commands)
$   make 
$   ./graphics
3. Use Command Line to set pre-launch settings (Refer to User Menu)
4. Use Keyboard to operate on objects (Refer to Keyboard Interactions)
5. Right Click On Window for Menu to set parameters (Refer to User Menu)
6. Exit the program, outputs save to files automatically
7. Clean up the folder (Refer to Commands)
$   make clean


Config Files
1.Data file used to display shape (default file: data) (World Coordinates)


        1                         number of polygons
definition of 1st polygon:  
4                                         number of points of 1st polygon 
0.0 0.0 0.0                                 coordinates of 1st point 
100.0 0.0 0.0                                coordinates of 2nd point 
0.0 100.0 0.0                                coordinates of 3rd point 
0.0 0.0 100.0                                 coordinates of 4th point 
4                                number of triangles of 1st object
1 3 2                                        triangle vertices of 1st triangle
1 2 4                                        triangle vertices of 2nd triangle
1 4 3                                        triangle vertices of 3rd triangle
2 3 4                                        triangle vertices of 4th triangle






2. Setting file used to config the window (default file: setting)


        300 300                 Viewport width and height                 (0~) (0~)
0 0 0                         Viewport background color R,G,B         (0~255) (0~255) (0~255)
255 255 255                Obsolete Value
2                        Obsolete Value
660 660                 Window width and height                 (0~) (0~)
1 1 1                        Window background color R,G,B         (0~1) (0~1) (0~1)
500 500 500                X, Light Source Position                 (~) (~)  In World Coordinates
1 0 0                        Ka, Ambient Light R,G,B                (0~1) (0~1) (0~1)
1 0 0                        Kd, Diffuse Light R,G,B                (0~1) (0~1) (0~1)
0 1 0                        Ks, Specular Light R,G,B                (0~1) (0~1) (0~1)
0.700                        Ia, Ambient Light Intensity                (0~1)
1.000                        IL, Light Source Intensity                (0~1)
0.500                        K, Average Distant Constant                (0~)
4.000                        n, Phong Constant                        (0~)
0 0 10                        F, From Point Position                 (~) (~)  In World Coordinates
Input/Output Files
Inputs (1 OR 2 Files) : data file (REQUIRED), setting file
* User is able to specify the name of the data file when prompted
Outputs (2 Files) : data file, setting file
* Program automatically save all changes (World Coordinates) to output files at exit


User Menu
1.  Pre-launch Settings (Command Line)
* Specify input file name:
Do you want to specify the input data file? (y/n)  y
Changing Input File to (ex: data.txt) data1.txt
* Specify display window size:
Do you want to specify the window size? (y/n) y
Setting Window Size (ex: 660 660): 800 800


2.  Runtime Menu (Right Click On the Graphic Window)
** For (Input in Terminal) items, check command line prompt for custom input **
Menu Entries:
* Current Object:                                Select Object by ID
         …


* Phong Lighting Model Parameters:        Set Phong Model parameters
         Light Source Position (Input in Terminal)
        From Point Position (Input in Terminal)
        Ka (Input in Terminal)
        Kd (Input in Terminal)
        Ks (Input in Terminal)
Ia (Input in Terminal)
IL (Input in Terminal)
Constant K (Input in Terminal)
Constant n (Input in Terminal)


* Transformation Parameters:                Set Transformation parameters
         Scaling factor (Input in Terminal)
         Translation vector (Input in Terminal)
         Rotation axis (Input in Terminal)        
Rotation angle (Input in Terminal)
         Set All to Default


* Resize:                                        
* Resize Window:                        Set the Graphics Window size
                        ...
                        Custom (Input in Terminal)
* Resize Viewport:                        Set the Viewport size
                        ...
                        Custom (Input in Terminal)


* Color:
* ViewPort Color:                        Set ViewPort background color
                        ...
                        Custom (Input in Terminal)
* Background Color:                        Set Graphic Window background color
                        …


* Exit                                                Exit the program
Input Units:


        Light Source Position                                 (Real World Coordinates)
        From Point Position                                 (Real World Coordinates)
Ka, Ambient Light R,G,B                                (0-1) (0-1) (0-1)
Kd, Diffuse Light R,G,B                                (0-1) (0-1) (0-1)
Ks, Specular Light R,G,B                                (0-1) (0-1) (0-1)
Ia, Ambient Light Intensity                                (0-1)
IL, Light Source Intensity                                (0-1)
K, Average Distant Constant                        (0-)  
(0-1) is recommended since NDC coordinates are used in calculation
n, Phong Constant                                        (0-)
        
Color: R G B                                         (0-255) (0-255) (0-255)
Translation Vector: X Y Z                                 (Real World Coordinates)
Rotation Axis: X1 Y1 Z1, X2 Y2 Z2                 (Real World Coordinates)
Rotation Angle                                         (Unit: Radian)


DEFAULT PARAMETERS:


Light Source: 500 500 500                         (Real World Coordinates)
        From Point: 0 0 10                                         (Real World Coordinates)
Ka: 1 0 0
Kd: 1 0 0 
Ks: 0 1 0
Ia: 0.7
IL: 1.0
K: 0.5
n: 4


Translation Vector: 20.0 20.0 20.0                 (Unit: Pixels)
        Rotation Axis: Point 1: 0 0 0 Point 2: 0 100 100 (Unit: Pixels) 
Rotation Angle: 0.1                                         (Unit: Radian)
        Scaling Factor: 1.3


Keyboard Interactions
FUNCTIONAL KEYS:
1. Half Toning On/Off 
* H : Enable / Disable Half Toning Display
1. Rasterizing 
* F : Enable / Disable Rastering
1. Translation
* W : translate current object by +y defined in translation vector
* A : translate current object by -x defined in translation vector
* S : translate current object by -y defined in translation vector
* D : translate current object by +x defined in translation vector
* UP_ARROW_KEY: translate current object by +z defined in translation vector
* DOWN_ARROW_KEY: translate current object by -z defined in translation vector
* T : translate current object by +x & +y & +z defined in translation vector
1. Rotation
* Q : rotate current object clockwise by α radian around rotation axis
* E : rotate current object counterclockwise by α radian around rotation axis
1. Display Light Source 
* L : Display / Hide The Light Source
1. Scaling
* X : scale current object by (1 * scale) defined in scaling factor
* Z : scale current object by (1 / scale) defined in scaling factor
1. Object Selection
* . : Switch to the next object
* , : Switch to the previous object
1. Exit Program
* Esc : Exit the program
Project Overview
Required:


Projections Onto XY, XZ, YZ Planes        (Statue: Complete) (Location: [387-546]main.cpp)
Phong Model                                        (Statue: Complete) (Location: [60-110]main.cpp)
Gouraud Shading                                (Statue: Complete) 
(Location: [208-213, 283-289]main.cpp)
Halftone                                        (Statue: Complete) (Location: [136-190]main.cpp)
Rasterizing Polygon                                (Statue: Complete) (Location: [228-296]main.cpp)
Painter’s Algorithm                                (Statue: Complete) 
(Location: [41-58, 378-385, 401, 455, 507]main.cpp)
User Input Customization                        (Statue: Complete) (Location: Runtime Menu)
* ID of the polygon to be manipulated
* Phong model parameters
Output written to the input data file         (Statue: Complete)
Makefile and Manual                         (Statue: Complete) 




Extra Credit:
Display intersecting objects                                (Location: [137-144]main.cpp)
Separate setting of Window and Viewport size                  (Location: Runtime Menu)
Separate setting of Window, Viewport, background         (Location: Runtime Menu)
Rescale All Objects according to the bounding box        (Location: [30-95]main.cpp)
Intuitive Keyboard Interactions                                (Location: [199-244]main.cpp)
Save Settings to a setting file at exit                        (Location: [247-250]parser.cpp)