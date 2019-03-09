
//#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
//#include <GLUT/glut.h>

#include <GL/glut.h> // csif
#include <iostream>
#include <vector>
#include <cmath>
#include "parser.h"
using namespace std;

float* PixelBuffer;
float* ObjBuffer;
Data parser;
string settingFile = "setting";
string dataFile = "data";
int currObj = 0; // object ID
int indexID = 0;

int* BGColor;

float scalingFactor = 1.1;
float translationX = 5;
float translationY = 5;
float rotationAngle = 0.1;

int clipLeft = 0, clipRight = 0, clipLow = 0, clipHigh = 0;

// set the pixel buffer
void setPixel(int x, int y){
    for(int i = 0; i < 3; i++){
        PixelBuffer[3*(y*parser.settings[0]+x)+i] = parser.settings[5+i];
    }
    ObjBuffer[(y*parser.settings[0]+x)] = indexID;
}

//initialize the display window
void init(){
    glClearColor(parser.settings[11], parser.settings[12], parser.settings[13], 0); //clears the buffer of OpenGL
    for(int x = 0; x < parser.settings[0]; x++){
        for(int y = 0; y < parser.settings[1]; y++){
            for(int i = 0; i < 3; i++){
                PixelBuffer[3*(y*parser.settings[0]+x)+i] = parser.settings[2+i];
            }
            ObjBuffer[(y*parser.settings[0]+x)] = -1;
        }
    }
}

// Bresenham Algorithm to draw line
void bresenham(float x1, float y1, float x2, float y2){
    float dx = x2 - x1;
    float dy = y2 - y1;
    int signx = dx > 0 ? 1 : (dx < 0 ? -1 : 0);
    int signy = dy > 0 ? 1 : (dy < 0 ? -1 : 0);
    dx = round(dx*signx); // abs(dx)
    dy = round(dy*signy); // abs(dy)
    
    // handle m = 0 or m = infinite
    int x = round(x1), y = round(y1);
    if(signx == 0){
        for(int i = 0; i <= dy; i++){
            setPixel(x, y);
            y += signy;
        }
        return;
    }
    if(signy == 0){
        for(int i = 0; i <= dx; i++){
            setPixel(x, y);
            x += signx;
        }
        return;
    }
    
    // handle m > 1
    bool flipped = false;
    if(dy > dx){
        int temp = dx;
        dx = dy;
        dy = temp;
        flipped = true;
    }
    
    // generalized algorithm
    int p = 2*dy - dx;
    for(int i = 0; i <= dx; i++){
        setPixel(x,y);
        if(!flipped)
            x += signx;
        else
            y += signy;
        if(p < 0){
            p += 2*dy;
        }else{
            if(!flipped)
                y += signy;
            else
                x += signx;
            p += 2*(dy-dx);
        }
    }
}

// DDA Algorithm to draw line
void dda(float x1, float y1, float x2, float y2){
    float dx = x2 - x1;
    float dy = y2 - y1;
    float increment = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    
    // calculate increment in x & y for each steps
    float xStep = dx / increment;
    float yStep = dy / increment;
    
    // set pixel for each step
    float x = x1;
    float y = y1;

    for (int i = 0; i < increment; i++){
        setPixel(round(x),round(y));
        x += xStep;
        y += yStep;
    }
}

bool check(int x, int y){
    if(x < parser.settings[0] && ObjBuffer[(y*parser.settings[0]+x)] == indexID){
        return true;
    }
    return false;
}

void fill(Shape& s){
    
    if(s.numVertex <= 2) return;
    
    s.setYMinandMax();
    //s.setExtreme();
    for(int j = round(s.yMin); j <= round(s.yMax); j++){
        int group = 0;
        for(int i = 0; i < parser.settings[0]; i++){
            if(check(i,j)){
                if(check(i+1, j)){
                    continue;
                }else{
                    group++;
                }
            }
        }

        bool turnOn = false;
        int intersectCount = 0;
        for(int i = 0; i < parser.settings[0]; i++){

            if(check(i,j)){
                
                if(group == 1){ //straight line
                    setPixel(i, j);
                    continue;
                }
                
                if(check(i+1, j)){ // double counting line segment
                    setPixel(i, j);
                    continue;
                }
                
                intersectCount++;
            }
            
            if(intersectCount % 2 == 1)
                turnOn = true;
            else
                turnOn = false;

            if(turnOn){
                setPixel(i, j);
            }
        }
    }
}

// draw one polygon
void draw(Shape& s){
    if(s.numVertex ==0)
        return;
    if(s.numVertex == 1){
        setPixel(s.cordinates[0].first, s.cordinates[0].second);
        return;
    }
    for(int i = 0; i <= s.numVertex-1; i++){
        int x1 = (s.cordinates[i].first);
        int y1 = (s.cordinates[i].second);
        int x2;
        int y2;
        if(i == s.numVertex-1){
            if(s.numVertex == 2) break;
            x2 = (s.cordinates[0].first);
            y2 = (s.cordinates[0].second);
        }else{
            x2 = (s.cordinates[i+1].first);
            y2 = (s.cordinates[i+1].second);
        }
        parser.algo[parser.settings[8]] == "dda" ? dda(x1, y1, x2, y2) : bresenham(x1, y1, x2, y2);
    }

}

// draw all poygons in the data file
void drawAll(){
    for(int i = 0; i < (int)parser.list.size(); i++){
        indexID = i;
        parser.list[i].clipping(clipLow,clipHigh,clipLeft,clipRight); // low, high, left, right
        draw(parser.list[i]);
        if(parser.list[i].filled)
            fill(parser.list[i]);
    }
}

//main display loop, this function will be called again and again by OpenGL
void display(){
    
    init();
    drawAll();
    
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    int xSpace = (parser.settings[9] - parser.settings[0]) / 2;
    int ySpace = (parser.settings[10] - parser.settings[1]) / 2;
    glViewport(xSpace,ySpace,parser.settings[0],parser.settings[1]);
    glRasterPos2i(-1,-1);
    //draws pixel on screen, width and height must match pixel buffer dimension
    glDrawPixels(parser.settings[0], parser.settings[1], GL_RGB, GL_FLOAT, PixelBuffer);
    
    //window refresh
    glFlush();
}

// keyboard interaction
void keyPressed (unsigned char key, int x, int y) {
    
    if(key == 'w'){
        parser.list[currObj].translate(0, translationY);
    }else if(key == 's'){
        parser.list[currObj].translate(0, -1 * translationY);
    }else if(key == 'd'){
        parser.list[currObj].translate(translationX, 0);
    }else if(key == 'a'){
        parser.list[currObj].translate(-1 * translationX, 0);
    }else if(key == 't'){
        parser.list[currObj].translate(translationX, translationY);
    }else if (key == 'q'){
        parser.list[currObj].rotate(rotationAngle);
    }else if (key == 'e'){
        parser.list[currObj].rotate(-1 * rotationAngle);
    }else if (key == 'r'){
        parser.list[currObj].reflect();
    }else if (key == 'x'){
        parser.list[currObj].scale(scalingFactor);
    }else if (key == 'z'){
        parser.list[currObj].scale(1/scalingFactor);
    }else if (key == ','){
        if(currObj > 0)
            currObj--;
        else
            currObj = (int)parser.list.size() - 1;
    }else if (key == '.'){
        if(currObj < (int)parser.list.size() - 1)
            currObj++;
        else
            currObj = 0;
    }else if (key == 'l'){ // switch line drawing algorithm
        parser.settings[8] = (parser.settings[8] == 0 ? 1 : 0);
    }else if (key == 'f'){
        parser.list[currObj].filled = parser.list[currObj].filled ? false : true;
    }else if (key == 'v'){ // set line drawing algorithm to dda
        parser.settings[8] = 0;
    }else if (key == 'b'){ // set line drawing algorithm to bresenham
        parser.settings[8] = 1;
    }else if (key == 27){ //escape key
        exit(0);
    }
    glutPostRedisplay();
}


// save settings and polygons to file
void saveScreen(){
    parser.saveSetting(settingFile);
    parser.saveShape(dataFile);
}


// menu items
void objectMenu (int id){
    currObj = id;
}
void transformationMenu (int id){
    if(id == 0){ // scaling factor
        cout<<"Setting Scaling Factor (ex: 1.1): ";
        cin >> scalingFactor;
    }else if(id == 1){ // translation vector
        cout<<"Setting Translation Vector (ex: 5.0 5.0): ";
        cin >> translationX >> translationY;
    }else if(id == 2){ // roattion angle
        cout<<"Setting Rotation Angle (ex: 0.1): ";
        cin >> rotationAngle;
    }else{ //default all
        scalingFactor = 1.1;
        translationX = 5.0;
        translationY = 5.0;
        rotationAngle = 0.1;
    }
}
void algorithmMenu (int id){
    if(id == 0){
        parser.settings[8] = 0;
        glutPostRedisplay();
    }else{
        parser.settings[8] = 1;
        glutPostRedisplay();
    }
}
void backgroundMenu (int id){
    if(id == 0){ // black
        parser.settings[2] = 0;
        parser.settings[3] = 0;
        parser.settings[4] = 0;
        glutPostRedisplay();
    }else if(id == 1){ //white
        parser.settings[2] = 255;
        parser.settings[3] = 255;
        parser.settings[4] = 255;
        glutPostRedisplay();
    }else{
        cout<<"Setting Background Color (ex: 0 0 0): ";
        cin >> parser.settings[2] >> parser.settings[3] >> parser.settings[4];
        glutPostRedisplay();
    }
}
void colorMenu (int id){
    if(id == 0){ // black
        parser.settings[5] = 0;
        parser.settings[6] = 0;
        parser.settings[7] = 0;
        glutPostRedisplay();
    }else if(id == 1){ //white
        parser.settings[5] = 255;
        parser.settings[6] = 255;
        parser.settings[7] = 255;
        glutPostRedisplay();
    }else if(id == 2){ //red
        parser.settings[5] = 255;
        parser.settings[6] = 0;
        parser.settings[7] = 0;
        glutPostRedisplay();
    }else if(id == 3){ //green
        parser.settings[5] = 0;
        parser.settings[6] = 255;
        parser.settings[7] = 0;
        glutPostRedisplay();
    }else if(id == 4){ //blue
        parser.settings[5] = 0;
        parser.settings[6] = 0;
        parser.settings[7] = 255;
        glutPostRedisplay();
    }else if(id == 5){ //Yellow
        parser.settings[5] = 255;
        parser.settings[6] = 223;
        parser.settings[7] = 0;
        glutPostRedisplay();
    }else{
        cout<<"Setting Line Color (ex: 0 0 0): ";
        cin >> parser.settings[5] >> parser.settings[6] >> parser.settings[7];
        glutPostRedisplay();
    }
}
void windowMenu (int id){
    if(id == 0){
        parser.settings[9] = 800;
        parser.settings[10] = 600;
    }else if(id == 1){
        parser.settings[9] = 1200;
        parser.settings[10] = 800;
    }else{
        cout<<"Setting Window Size (ex: 800 600): ";
        cin >> parser.settings[9] >> parser.settings[10];
    }
    glutReshapeWindow(parser.settings[9], parser.settings[10]);
    glutPostRedisplay();
}
void windowBGCMenu (int id){
    if(id == 0){ // black
        parser.settings[11] = parser.settings[12] = parser.settings[13] = 0;
        glutPostRedisplay();
    }else if(id == 1){ //white
        parser.settings[11] = parser.settings[12] = parser.settings[13] = 1;
        glutPostRedisplay();
    }
}
void viewMenu (int id){
    if(id == 0){
        parser.settings[0] = 700;
        parser.settings[1] = 500;
    }else if(id == 1){
        parser.settings[0] = 1100;
        parser.settings[1] = 700;
    }else{
        cout<<"Setting ViewPort Size (ex: 800 600): ";
        cin >> parser.settings[0] >> parser.settings[1];
    }
    delete[] PixelBuffer;
    delete[] ObjBuffer;
    PixelBuffer = new float[parser.settings[0] * parser.settings[1] * 3];
    ObjBuffer = new float[parser.settings[0] * parser.settings[1]];
    
    clipRight = parser.settings[0]-1;
    clipHigh = parser.settings[1]-1;
    
    glutPostRedisplay();
}
void clipwindowMenu (int id){
    if(id == 0){
        clipLeft = 0;
        clipRight = parser.settings[0]-1;
        clipLow = 0;
        clipHigh = parser.settings[1]-1;
    }else{
        cout<<"Setting Clipping Window Size [xMin, xMax, yMin, yMax](ex: 0 800 0 600): ";
        cin >> clipLeft >> clipRight >> clipLow >> clipHigh;
        if(clipLeft < 0)
            clipLeft = 0;
        if(clipLow < 0)
            clipLow = 0;
        if(clipRight >= parser.settings[0])
            clipRight = parser.settings[0]-1;
        if(clipHigh >= parser.settings[1])
            clipHigh = parser.settings[1]-1;
    }
    glutPostRedisplay();
}
void mainMenu (int id){
    if(id == 0){
        Shape obj;
        cout<<"Enter the number of vertices for this object (ex: 3): ";
        cin >> obj.numVertex;
        for(int i = 0; i < obj.numVertex; i++){
            cout<<"Enter the " << i <<"th pair of vertices for this object (ex: 50.0 100.0): ";
            float x,y;
            cin >> x >> y;
            pair<float,float> points = make_pair(x, y);
            obj.cordinates.push_back(points);
        }
        parser.list.push_back(obj);
        glutSetMenu(6); // object menu
        const char* tag = to_string((int)parser.list.size() - 1).c_str();
        glutAddMenuEntry (tag, (int)parser.list.size() - 1);
        currObj = (int)parser.list.size() - 1;
        glutPostRedisplay();
    }else if(id == 1){
        exit (0);
    }
}

void empty(int id){}

int main(int argc, char *argv[]){
    // parse input file
    char key;
    cout<<"Do you want to specify the input data file? (y/n) ";
    cin>>key;
    if(key == 'y'){
        cout<<"Changing Input File to (ex: data.txt): ";
        cin >> dataFile;
    }
    parser.loadSetting(settingFile);
    parser.loadShape(dataFile);
    
    // Window Initialization
    char c;
    cout<<"Do you want to specify the window size? (y/n) ";
    cin>>c;
    if(c == 'y'){
        cout<<"Setting Window Size (ex: 800 600): ";
        cin >> parser.settings[9] >> parser.settings[10];
        if(parser.settings[9] < 150)
            parser.settings[9] = 150;
        if(parser.settings[10] < 150)
            parser.settings[10] = 150;
        parser.settings[0] = parser.settings[9] - 100;
        parser.settings[1] = parser.settings[10] - 100;
    }
    
    clipLeft = 0;
    clipRight = parser.settings[0]-1;
    clipLow = 0;
    clipHigh = parser.settings[1]-1;
    
    // allocate new pixel buffer, need initialization!!
    ObjBuffer = new float[parser.settings[0] * parser.settings[1]];
    PixelBuffer = new float[parser.settings[0] * parser.settings[1] * 3];
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(parser.settings[9], parser.settings[10]);
    glutInitWindowPosition(50, 50);
    
    // create and set main window title
    glutCreateWindow("ECS175 Project1");
    // sets display function
    glutDisplayFunc(display);
    glutKeyboardFunc(keyPressed);
    
    int tranformation_menu = glutCreateMenu (transformationMenu); // 1
    glutAddMenuEntry ("Scaling factor (Input in Terminal)", 0);
    glutAddMenuEntry ("Translation vector (Input in Terminal)", 1);
    glutAddMenuEntry ("Rotation angle (Input in Terminal)", 2);
    glutAddMenuEntry ("Set All to Default", 3);
    
    int algorithm_menu = glutCreateMenu (algorithmMenu); // 2
    glutAddMenuEntry ("DDA", 0);
    glutAddMenuEntry ("Bresenham's", 1);
    
    int background_menu = glutCreateMenu(backgroundMenu); // 3
    glutAddMenuEntry ("Black", 0);
    glutAddMenuEntry ("White", 1);
    glutAddMenuEntry ("Custom (Input in Terminal)", 2);
    
    int line_menu = glutCreateMenu(colorMenu); // 4
    glutAddMenuEntry ("Red", 2);
    glutAddMenuEntry ("Green", 3);
    glutAddMenuEntry ("Blue", 4);
    glutAddMenuEntry ("Yellow", 5);
    glutAddMenuEntry ("Black", 0);
    glutAddMenuEntry ("White", 1);
    glutAddMenuEntry ("Custom (Input in Terminal)", 6);
    
    int view_menu = glutCreateMenu(viewMenu); // 5
    glutAddMenuEntry ("700x500", 0);
    glutAddMenuEntry ("1100x700", 1);
    glutAddMenuEntry ("Custom (Input in Terminal)", 2);
    
    int object_menu = glutCreateMenu (objectMenu); // 6
    for(int i = 0; i < (int)parser.list.size(); i++){
        const char* tag = to_string(i).c_str();
        glutAddMenuEntry (tag, i);
    }
    
    int window_menu = glutCreateMenu(windowMenu); // 7
    glutAddMenuEntry ("800x600", 0);
    glutAddMenuEntry ("1200x800", 1);
    glutAddMenuEntry ("Custom (Input in Terminal)", 2);
    
    int windowColor_menu = glutCreateMenu(windowBGCMenu); // 7
    glutAddMenuEntry ("Black", 0);
    glutAddMenuEntry ("White", 1);
    
    int clipwindow_menu = glutCreateMenu(clipwindowMenu); // 7
    glutAddMenuEntry ("Set as viewport size", 0);
    glutAddMenuEntry ("Custom (Input in Terminal)", 1);
    
    int resize = glutCreateMenu(empty); // 8
    glutAddSubMenu ("Resize Window:", window_menu);
    glutAddSubMenu ("Resize Viewport:", view_menu);
    glutAddSubMenu ("Resize Clipping Window:", clipwindow_menu);
    
    int color = glutCreateMenu(empty); // 8
    glutAddSubMenu ("Line Color:", line_menu);
    glutAddSubMenu ("ViewPort Color:", background_menu);
    glutAddSubMenu ("Background Color:", windowColor_menu);
    
    glutCreateMenu (mainMenu); // 0
    glutAddSubMenu ("Current Object:", object_menu);
    glutAddSubMenu("Transformation Parameters:", tranformation_menu);
    glutAddSubMenu ("Line Drawing Algorithm:", algorithm_menu);
    glutAddSubMenu ("Resize:", resize);
    glutAddSubMenu ("Color:", color);
    glutAddMenuEntry("Add New Object (Input in Terminal)", 0);
    glutAddMenuEntry ("Exit", 1);
    glutAttachMenu (GLUT_RIGHT_BUTTON);
    
    atexit(saveScreen);
    
    glutMainLoop();// main display loop, will display until terminate
    return 0;
}
