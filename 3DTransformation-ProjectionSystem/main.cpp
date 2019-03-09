//#include <OpenGL/gl.h> // Xcode
//#include <OpenGL/glu.h> // Xcode
//#include <GLUT/glut.h> // Xcode
#include <GL/glut.h> // csif
#include <iostream>
#include <vector>
#include <cmath>
#include <tuple>
#include "parser.h"
using namespace std;

Data parser;
string settingFile = "setting";
string dataFile = "data";
int currObj = 0; // object ID
int* BGColor;
float scalingFactor = 1.1;
float translationX = 5;
float translationY = 5;
float translationZ = 5;
tuple<float,float,float> rotationPt1 = make_tuple(0,0,0);
tuple<float,float,float> rotationPt2 = make_tuple(100,100,100);
tuple<float,float,float> ndcrotationPt1 = make_tuple(0,0,0);
tuple<float,float,float> ndcrotationPt2 = make_tuple(1,1,1);
tuple<float,float,float> oblrotationPt1 = make_tuple(0,0,0);
tuple<float,float,float> oblrotationPt2 = make_tuple(1,1,1);
float rotationAngle = 0.05;
bool displayRotAxis = false;
float axisC[3] = {0.91,0.91,0.91};

void Bounding(){
    // check for out-of-bound
    float Xmin = min(get<0>(rotationPt1),get<0>(rotationPt2));
    float Xmax = max(get<0>(rotationPt1),get<0>(rotationPt2));
    float Ymin = min(get<1>(rotationPt1),get<1>(rotationPt2));
    float Ymax = max(get<1>(rotationPt1),get<1>(rotationPt2));
    float Zmin = min(get<2>(rotationPt1),get<2>(rotationPt2));
    float Zmax = max(get<2>(rotationPt1),get<2>(rotationPt2));
    for(int i = 0; i < (int)parser.list.size(); i++){
        for(int j = 0; j < (int)parser.list[i].numVertex; j++){
            Xmin = Xmin > parser.list[i].X[j] ? parser.list[i].X[j] : Xmin;
            Xmax = Xmax < parser.list[i].X[j] ? parser.list[i].X[j] : Xmax;
            Ymin = Ymin > parser.list[i].Y[j] ? parser.list[i].Y[j] : Ymin;
            Ymax = Ymax < parser.list[i].Y[j] ? parser.list[i].Y[j] : Ymax;
            Zmin = Zmin > parser.list[i].Z[j] ? parser.list[i].Z[j] : Zmin;
            Zmax = Zmax < parser.list[i].Z[j] ? parser.list[i].Z[j] : Zmax;
        }
    }
    float lowBound = min(min(Xmin,Ymin),Zmin) - parser.settings[0]*0.005;
    float highBound = max(max(Xmax,Ymax),Zmax) + parser.settings[0]*0.005;
    get<0>(ndcrotationPt1) = ((get<0>(rotationPt1) - lowBound) / (float)(highBound - lowBound));
    get<1>(ndcrotationPt1) = ((get<1>(rotationPt1) - lowBound) / (float)(highBound - lowBound));
    get<2>(ndcrotationPt1) = ((get<2>(rotationPt1) - lowBound) / (float)(highBound - lowBound));
    get<0>(ndcrotationPt2) = ((get<0>(rotationPt2) - lowBound) / (float)(highBound - lowBound));
    get<1>(ndcrotationPt2) = ((get<1>(rotationPt2) - lowBound) / (float)(highBound - lowBound));
    get<2>(ndcrotationPt2) = ((get<2>(rotationPt2) - lowBound) / (float)(highBound - lowBound));
    for(int i = 0; i < (int)parser.list.size(); i++){
        for(int j = 0; j < (int)parser.list[i].numVertex; j++){
            parser.list[i].ndcX[j] = ((parser.list[i].X[j] - lowBound) / (float)(highBound - lowBound));
            parser.list[i].ndcY[j] = ((parser.list[i].Y[j] - lowBound) / (float)(highBound - lowBound));
            parser.list[i].ndcZ[j] = ((parser.list[i].Z[j] - lowBound) / (float)(highBound - lowBound));
        }
    }
}

void oblBounding(){
    // check for out-of-bound
    float Xmin = min((get<0>(rotationPt1)-sqrt(2)*get<2>(rotationPt1)/(float)parser.settings[8]),(get<0>(rotationPt2)-sqrt(2)*get<2>(rotationPt2)/(float)parser.settings[8]));
    float Xmax = max((get<0>(rotationPt1)-sqrt(2)*get<2>(rotationPt1)/(float)parser.settings[8]),(get<0>(rotationPt2)-sqrt(2)*get<2>(rotationPt2)/(float)parser.settings[8]));
    float Ymin = min((get<0>(rotationPt1)-sqrt(2)*get<2>(rotationPt1)/(float)parser.settings[8]),(get<0>(rotationPt2)-sqrt(2)*get<2>(rotationPt2)/(float)parser.settings[8]));
    float Ymax = max((get<1>(rotationPt1)-sqrt(2)*get<2>(rotationPt1)/(float)parser.settings[8]),(get<1>(rotationPt2)-sqrt(2)*get<2>(rotationPt2)/(float)parser.settings[8]));
    for(int i = 0; i < (int)parser.list.size(); i++){
        for(int j = 0; j < (int)parser.list[i].numVertex; j++){
            Xmin = Xmin > (parser.list[i].X[j]-sqrt(2)*parser.list[i].Z[j]/(float)parser.settings[8]) ? (parser.list[i].X[j]-sqrt(2)*parser.list[i].Z[j]/(float)parser.settings[8]) : Xmin;
            Xmax = Xmax < (parser.list[i].X[j]-sqrt(2)*parser.list[i].Z[j]/(float)parser.settings[8]) ? (parser.list[i].X[j]-sqrt(2)*parser.list[i].Z[j]/(float)parser.settings[8]) : Xmax;
            Ymin = Ymin > (parser.list[i].Y[j]-sqrt(2)*parser.list[i].Z[j]/(float)parser.settings[8]) ? (parser.list[i].Y[j]-sqrt(2)*parser.list[i].Z[j]/(float)parser.settings[8]) : Ymin;
            Ymax = Ymax < (parser.list[i].Y[j]-sqrt(2)*parser.list[i].Z[j]/(float)parser.settings[8]) ? (parser.list[i].Y[j]-sqrt(2)*parser.list[i].Z[j]/(float)parser.settings[8]) : Ymax;
        }
    }
    float lowBound = min(Xmin,Ymin) - parser.settings[0]*0.005;
    float highBound = max(Xmax,Ymax) + parser.settings[0]*0.005;
    get<0>(oblrotationPt1) = (((get<0>(rotationPt1)-sqrt(2)*get<2>(rotationPt1)/(float)parser.settings[8]) - lowBound) / (float)(highBound - lowBound));
    get<1>(oblrotationPt1) = (((get<1>(rotationPt1)-sqrt(2)*get<2>(rotationPt1)/(float)parser.settings[8]) - lowBound) / (float)(highBound - lowBound));
    get<2>(oblrotationPt1) = 0;
    get<0>(oblrotationPt2) = (((get<0>(rotationPt2)-sqrt(2)*get<2>(rotationPt1)/(float)parser.settings[8]) - lowBound) / (float)(highBound - lowBound));
    get<1>(oblrotationPt2) = (((get<1>(rotationPt2)-sqrt(2)*get<2>(rotationPt1)/(float)parser.settings[8]) - lowBound) / (float)(highBound - lowBound));
    get<2>(oblrotationPt2) = 0;
    for(int i = 0; i < (int)parser.list.size(); i++){
        for(int j = 0; j < (int)parser.list[i].numVertex; j++){
            parser.list[i].oblX[j] = (((parser.list[i].X[j]-sqrt(2)*parser.list[i].Z[j]/(float)parser.settings[8]) - lowBound) / (float)(highBound - lowBound));
            parser.list[i].oblY[j] = (((parser.list[i].Y[j]-sqrt(2)*parser.list[i].Z[j]/(float)parser.settings[8]) - lowBound) / (float)(highBound - lowBound));
            parser.list[i].oblZ[j] = 0;
        }
    }
}

void printText(string str){
    for(int i = 0; i < (int)str.size(); ++i)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15,str[i]);
}

//main display loop, this function will be called again and again by OpenGL
void display(){
    glClearColor(parser.settings[11], parser.settings[12], parser.settings[13], 0); //clears the buffer of OpenGL
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glViewport(0, 0, parser.settings[9], parser.settings[10]); //display text on main screen
    glLoadIdentity();
    glOrtho(0, parser.settings[9], 0, parser.settings[10],-1,1);
    glColor3f(parser.settings[5]/255.0, parser.settings[6]/255.0, parser.settings[7]/255.0);
    glRasterPos2f(10+parser.settings[1]/2-90, 5);
    printText("XY Projection: --> X");
    glRasterPos2f(10+parser.settings[0]/2-90, 25 + parser.settings[1]);
    printText("XZ Projection: --> X");
    glRasterPos2f(30+3*(parser.settings[0]-1)/2-90, 25 + parser.settings[1]);
    printText("YZ Projection: --> Y");
    glRasterPos2f(30+3*(parser.settings[0]-1)/2-90, 5);
    printText(" Oblique Projection:");
    
    Bounding(); //handle out-of-bound condition
    oblBounding();
    
    // Projection onto 3 planes
    // XZ Plane
    glViewport(20, parser.settings[1]+40, parser.settings[0], parser.settings[1]);
    //Color Viewport Background
    glScissor(20, parser.settings[1]+40, parser.settings[0], parser.settings[1]);
    glEnable(GL_SCISSOR_TEST);
    glClearColor(parser.settings[2], parser.settings[3], parser.settings[4], 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
    
    glLoadIdentity();
    glOrtho(0, parser.settings[0], 0, parser.settings[1],-1,1);
    glColor3f(axisC[0],axisC[1],axisC[2]);
    if(displayRotAxis){
        glLineWidth(2.5);
        glBegin(GL_LINES);
        glVertex2f(get<0>(ndcrotationPt1)*(parser.settings[0]-1), get<2>(ndcrotationPt1)*(parser.settings[0]-1));
        glVertex2f(get<0>(ndcrotationPt2)*(parser.settings[0]-1), get<2>(ndcrotationPt2)*(parser.settings[0]-1));
        glEnd();
        glLineWidth(1.0);
    }
    glColor3f(parser.settings[5]/255.0, parser.settings[6]/255.0, parser.settings[7]/255.0);
    for(int i = 0; i < (int)parser.list.size(); i++){
        for(int j = 0; j < (int)parser.list[i].numEdge; j++){
            int pt1 = parser.list[i].edges[j].first-1;
            int pt2 = parser.list[i].edges[j].second-1;
            glBegin(GL_LINES);
            glVertex2f(parser.list[i].ndcX[pt1]*(parser.settings[0]-1), parser.list[i].ndcZ[pt1]*(parser.settings[0]-1));
            glVertex2f(parser.list[i].ndcX[pt2]*(parser.settings[0]-1), parser.list[i].ndcZ[pt2]*(parser.settings[0]-1));
            glEnd();
        }
    }
    
    // YZ Plane
    glViewport(parser.settings[0]+40, parser.settings[1]+40, parser.settings[0], parser.settings[1]);
    //Color Viewport Background
    glScissor(parser.settings[0]+40, parser.settings[1]+40, parser.settings[0], parser.settings[1]);
    glEnable(GL_SCISSOR_TEST);
    glClearColor(parser.settings[2], parser.settings[3], parser.settings[4], 0); //clears the buffer of OpenGL
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
    
    glLoadIdentity();
    glOrtho(0, parser.settings[0], 0, parser.settings[1],-1,1);
    glColor3f(axisC[0],axisC[1],axisC[2]);
    if(displayRotAxis){
        glLineWidth(2.5);
        glBegin(GL_LINES);
        glVertex2f(get<0>(ndcrotationPt1)*(parser.settings[0]-1), get<2>(ndcrotationPt1)*(parser.settings[0]-1));
        glVertex2f(get<0>(ndcrotationPt2)*(parser.settings[0]-1), get<2>(ndcrotationPt2)*(parser.settings[0]-1));
        glEnd();
        glLineWidth(1.0);
    }
    glColor3f(parser.settings[5]/255.0, parser.settings[6]/255.0, parser.settings[7]/255.0);
    for(int i = 0; i < (int)parser.list.size(); i++){
        for(int j = 0; j < (int)parser.list[i].numEdge; j++){
            int pt1 = parser.list[i].edges[j].first-1;
            int pt2 = parser.list[i].edges[j].second-1;
            glBegin(GL_LINES);
            glVertex2f(parser.list[i].ndcY[pt1]*(parser.settings[0]-1), parser.list[i].ndcZ[pt1]*(parser.settings[0]-1));
            glVertex2f(parser.list[i].ndcY[pt2]*(parser.settings[0]-1), parser.list[i].ndcZ[pt2]*(parser.settings[0]-1));
            glEnd();
        }
    }
    
    // XY Plane
    glViewport(20, 20, parser.settings[0], parser.settings[1]);
    //Color Viewport Background
    glScissor(20, 20, parser.settings[0], parser.settings[1]);
    glEnable(GL_SCISSOR_TEST);
    glClearColor(parser.settings[2], parser.settings[3], parser.settings[4], 0); //clears the buffer of OpenGL
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
    
    glLoadIdentity();
    glOrtho(0, parser.settings[0], 0, parser.settings[1],-1,1);
    glColor3f(axisC[0],axisC[1],axisC[2]);
    if(displayRotAxis){
        glLineWidth(2.5);
        glBegin(GL_LINES);
        glVertex2f(get<0>(ndcrotationPt1)*(parser.settings[0]-1), get<2>(ndcrotationPt1)*(parser.settings[0]-1));
        glVertex2f(get<0>(ndcrotationPt2)*(parser.settings[0]-1), get<2>(ndcrotationPt2)*(parser.settings[0]-1));
        glEnd();
        glLineWidth(1.0);
    }
    glColor3f(parser.settings[5]/255.0, parser.settings[6]/255.0, parser.settings[7]/255.0);
    for(int i = 0; i < (int)parser.list.size(); i++){
        for(int j = 0; j < (int)parser.list[i].numEdge; j++){
            int pt1 = parser.list[i].edges[j].first-1;
            int pt2 = parser.list[i].edges[j].second-1;
            glBegin(GL_LINES);
            glVertex2f(parser.list[i].ndcX[pt1]*(parser.settings[0]-1), parser.list[i].ndcY[pt1]*(parser.settings[0]-1));
            glVertex2f(parser.list[i].ndcX[pt2]*(parser.settings[0]-1), parser.list[i].ndcY[pt2]*(parser.settings[0]-1));
            glEnd();
        }
    }
    
    // Oblique Projection
    glViewport(40+parser.settings[0], 20, parser.settings[0], parser.settings[1]);
    //Color Viewport Background
    glScissor(40+parser.settings[0], 20, parser.settings[0], parser.settings[1]);
    glEnable(GL_SCISSOR_TEST);
    glClearColor(parser.settings[2], parser.settings[3], parser.settings[4], 0); //clears the buffer of OpenGL
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
    
    glLoadIdentity();
    glOrtho(0, parser.settings[0], 0, parser.settings[1],-1,1);
    glColor3f(axisC[0],axisC[1],axisC[2]);
    if(displayRotAxis){
        glLineWidth(2.5);
        glBegin(GL_LINES);
        glVertex2f(get<0>(oblrotationPt1)*(parser.settings[0]-1), get<1>(oblrotationPt1)*(parser.settings[0]-1));
        glVertex2f(get<0>(oblrotationPt2)*(parser.settings[0]-1), get<1>(oblrotationPt2)*(parser.settings[0]-1));
        glEnd();
        glLineWidth(1.0);
    }
    glColor3f(parser.settings[5]/255.0, parser.settings[6]/255.0, parser.settings[7]/255.0);
    for(int i = 0; i < (int)parser.list.size(); i++){
        for(int j = 0; j < (int)parser.list[i].numEdge; j++){
            int pt1 = parser.list[i].edges[j].first-1;
            int pt2 = parser.list[i].edges[j].second-1;
            glBegin(GL_LINES);
            glVertex2f(parser.list[i].oblX[pt1]*(parser.settings[0]-1),parser.list[i].oblY[pt1]*(parser.settings[0]-1));
            glVertex2f(parser.list[i].oblX[pt2]*(parser.settings[0]-1),parser.list[i].oblY[pt2]*(parser.settings[0]-1));
            glEnd();
        }
    }
    
    //window refresh
    glFlush();
}

void specialKey(int key, int x, int y){
    if(key == GLUT_KEY_UP){
        parser.list[currObj].translate(0, 0, (translationZ));
    }else if(key == GLUT_KEY_DOWN){
        parser.list[currObj].translate(0, 0, -1 * (translationZ));
    }
    glutPostRedisplay();
}

// keyboard interaction
void keyPressed (unsigned char key, int x, int y) {
    if(key == 'w'){
        parser.list[currObj].translate(0, translationY, 0);
    }else if(key == 's'){
        parser.list[currObj].translate(0, -1 * (translationY), 0);
    }else if(key == 'd'){
        parser.list[currObj].translate((translationX), 0, 0);
    }else if(key == 'a'){
        parser.list[currObj].translate(-1 * (translationX), 0, 0);
    }else if(key == 't'){
        parser.list[currObj].translate(translationX, translationY, translationZ);
    }else if (key == 'q'){
        parser.list[currObj].rotate(rotationPt1,rotationPt2,rotationAngle);
    }else if (key == 'e'){
        parser.list[currObj].rotate(rotationPt1,rotationPt2,-1 * rotationAngle);
    }else if (key == 'l'){
        displayRotAxis = !displayRotAxis;
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
        cout<<"Setting Translation Vector (ex: 5.0 5.0 5.0): ";
        cin >> translationX >> translationY >> translationZ;
    }else if(id == 2){ // roattion axis
        cout<<"Setting Rotation Axis Point 1 (ex: 10 20 100): ";
        cin >> get<0>(rotationPt1) >> get<1>(rotationPt1) >> get<2>(rotationPt1);
        cout<<"Setting Rotation Axis Point 2 (ex: 0 100 100): ";
        cin >> get<0>(rotationPt2) >> get<1>(rotationPt2) >> get<2>(rotationPt2);
    }else if(id == 3){ // roattion angle
        cout<<"Setting Rotation Angle (ex: 0.1): ";
        cin >> rotationAngle;
    }else{ //default all
        scalingFactor = 1.1;
        translationX = 5.0;
        translationY = 5.0;
        translationZ = 5.0;
        rotationAngle = 0.1;
        rotationPt1 = make_tuple(0,0,0);
        rotationPt2 = make_tuple(100,100,100);
    }
    glutPostRedisplay();
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
        cout<<"Setting Background Color (ex: 255 0 0): ";
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
        cout<<"Setting Line Color (ex: 255 0 0): ";
        cin >> parser.settings[5] >> parser.settings[6] >> parser.settings[7];
        glutPostRedisplay();
    }
}
void axiscolorMenu (int id){
    if(id == 0){ // black
        axisC[0] = 0;
        axisC[1] = 0;
        axisC[2] = 0;
        glutPostRedisplay();
    }else if(id == 1){ //white
        axisC[0] = 255;
        axisC[1] = 255;
        axisC[2] = 255;
        glutPostRedisplay();
    }else if(id == 2){ //red
        axisC[0] = 255;
        axisC[1] = 0;
        axisC[2] = 0;
        glutPostRedisplay();
    }else if(id == 3){ //green
        axisC[0] = 0;
        axisC[1] = 255;
        axisC[2] = 0;
        glutPostRedisplay();
    }else if(id == 4){ //blue
        axisC[0] = 0;
        axisC[1] = 0;
        axisC[2] = 255;
        glutPostRedisplay();
    }else if(id == 5){ //Yellow
        axisC[0] = 255;
        axisC[1] = 223;
        axisC[2] = 0;
        glutPostRedisplay();
    }else{
        int r,g,b;
        cout<<"Setting Rotation Axis Color (ex: 255 0 0): ";
        cin >> r >> g >> b;
        axisC[0] = r;
        axisC[1] = g;
        axisC[2] = b;
        glutPostRedisplay();
    }
}
void windowMenu (int id){
    if(id == 0){
        parser.settings[9] = 660;
        parser.settings[10] = 660;
    }else if(id == 1){
        parser.settings[9] = 1000;
        parser.settings[10] = 1000;
    }else{
        cout<<"Setting Window Size (ex: 800 800): ";
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
        parser.settings[0] = 300;
        parser.settings[1] = 300;
    }else if(id == 1){
        parser.settings[0] = 400;
        parser.settings[1] = 400;
    }else{
        cout<<"Setting ViewPort Length (Width == Height) (ex: 500): ";
        cin >> parser.settings[0] >> parser.settings[1];
    }
    
    glutPostRedisplay();
}

void mainMenu (int id){
    if(id == 1){
        exit (0);
    }
}
void projectionMenu (int id){
    if(id == 1){
        parser.settings[8] = 2;
    }else{
        parser.settings[8] = 4;
    }
    glutPostRedisplay();
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
        cout<<"Setting Window Size (ex: 660 660): ";
        cin >> parser.settings[9] >> parser.settings[10];
        if(parser.settings[9] < 160)
            parser.settings[9] = 160;
        if(parser.settings[10] < 160)
            parser.settings[10] = 160;
        parser.settings[0] = (min(parser.settings[9],parser.settings[10]) - 60)/2;
        parser.settings[1] = parser.settings[0];
    }
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(parser.settings[9], parser.settings[10]);
    glutInitWindowPosition(50, 50);
    
    // create and set main window title
    glutCreateWindow("ECS175 Project2");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyPressed);
    glutSpecialFunc(specialKey);
    
    int tranformation_menu = glutCreateMenu (transformationMenu); // 1
    glutAddMenuEntry ("Scaling factor (Input in Terminal)", 0);
    glutAddMenuEntry ("Translation vector (Input in Terminal)", 1);
    glutAddMenuEntry ("Rotation axis (Input in Terminal)", 2);
    glutAddMenuEntry ("Rotation angle (Input in Terminal)", 3);
    glutAddMenuEntry ("Set All to Default", 4);
    
    int background_menu = glutCreateMenu(backgroundMenu); // 2
    glutAddMenuEntry ("Black", 0);
    glutAddMenuEntry ("White", 1);
    glutAddMenuEntry ("Custom (Input in Terminal)", 2);
    
    int line_menu = glutCreateMenu(colorMenu); // 3
    glutAddMenuEntry ("Red", 2);
    glutAddMenuEntry ("Green", 3);
    glutAddMenuEntry ("Blue", 4);
    glutAddMenuEntry ("Yellow", 5);
    glutAddMenuEntry ("Black", 0);
    glutAddMenuEntry ("White", 1);
    glutAddMenuEntry ("Custom (Input in Terminal)", 6);
    
    int view_menu = glutCreateMenu(viewMenu); // 4
    glutAddMenuEntry ("300x300", 0);
    glutAddMenuEntry ("400x400", 1);
    glutAddMenuEntry ("Custom (Input in Terminal)", 2);
    
    int object_menu = glutCreateMenu (objectMenu); // 5
    for(int i = 0; i < (int)parser.list.size(); i++){
        const char* tag = to_string(i).c_str();
        glutAddMenuEntry (tag, i);
    }
    
    int window_menu = glutCreateMenu(windowMenu); // 6
    glutAddMenuEntry ("660x660", 0);
    glutAddMenuEntry ("1000x1000", 1);
    glutAddMenuEntry ("Custom (Input in Terminal)", 2);
    
    int windowColor_menu = glutCreateMenu(windowBGCMenu); // 7
    glutAddMenuEntry ("Black", 0);
    glutAddMenuEntry ("White", 1);
    
    int resize = glutCreateMenu(empty); // 8
    glutAddSubMenu ("Resize Window:", window_menu);
    glutAddSubMenu ("Resize Viewport:", view_menu);
    
    int axis_menu = glutCreateMenu(axiscolorMenu); // 9
    glutAddMenuEntry ("Red", 2);
    glutAddMenuEntry ("Green", 3);
    glutAddMenuEntry ("Blue", 4);
    glutAddMenuEntry ("Yellow", 5);
    glutAddMenuEntry ("Black", 0);
    glutAddMenuEntry ("White", 1);
    glutAddMenuEntry ("Custom (Input in Terminal)", 6);
    
    int color = glutCreateMenu(empty); // 10
    glutAddSubMenu ("Line Color:", line_menu);
    glutAddSubMenu ("Rotation Axis Color:", axis_menu);
    glutAddSubMenu ("ViewPort Color:", background_menu);
    glutAddSubMenu ("Background Color:", windowColor_menu);
    
    int projection = glutCreateMenu(projectionMenu); // 10
    glutAddMenuEntry ("Cavalier Projection", 1);
    glutAddMenuEntry ("Cabinet Projection", 2);
    
    glutCreateMenu (mainMenu); // 0
    glutAddSubMenu ("Current Object:", object_menu);
    glutAddSubMenu("Transformation Parameters:", tranformation_menu);
    glutAddSubMenu ("Oblique Projection Type:", projection);
    glutAddSubMenu ("Resize:", resize);
    glutAddSubMenu ("Color:", color);
    glutAddMenuEntry ("Exit", 1);
    glutAttachMenu (GLUT_RIGHT_BUTTON);
    
    atexit(saveScreen);
    
    glutMainLoop();// main display loop, will display until terminate
    return 0;
}
