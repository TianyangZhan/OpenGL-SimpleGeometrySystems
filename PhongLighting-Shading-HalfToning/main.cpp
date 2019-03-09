//#include <OpenGL/gl.h> // Xcode
//#include <OpenGL/glu.h> // Xcode
//#include <GLUT/glut.h> // Xcode
#include <GL/glut.h> // csif
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <tuple>
#include "parser.h"
using namespace std;

float* PixelBufferXY;
float* ObjBufferXY;
float* PixelBufferYZ;
float* ObjBufferYZ;
float* PixelBufferXZ;
float* ObjBufferXZ;

Data parser;
string settingFile = "setting";
string dataFile = "data";
int currObj = 0; // object ID
int indexID = 0;
int* BGColor;
float scalingFactor = 1.3;
float translationX = 30;
float translationY = 30;
float translationZ = 30;
tuple<float,float,float> rotationPt1 = make_tuple(0,0,0);
tuple<float,float,float> rotationPt2 = make_tuple(100,100,100);
tuple<float,float,float> ndcrotationPt1 = make_tuple(0,0,0);
tuple<float,float,float> ndcrotationPt2 = make_tuple(1,1,1);
tuple<float,float,float> ndcLight = make_tuple(0,0,0);
float rotationAngle = 0.1;
bool displayLightSource = false;
bool halfTone = false;
float axisC[3] = {0.91,0.91,0.91};


struct sortTriangles{
    sortTriangles(vector<Shape> list, int k) { this->list = list; this->k = k;}
    bool operator () (pair<int,int> a, pair<int,int> b) {
        int i = a.first;
        int j = a.second;
        int m = b.first;
        int n = b.second;
        if(k == 0) // -X
            return max(max(list[i].X[get<0>(list[i].triangles[j])-1], list[i].X[get<1>(list[i].triangles[j])-1]), list[i].X[get<2>(list[i].triangles[j])-1])
                 < max(max(list[m].X[get<0>(list[m].triangles[n])-1], list[m].X[get<1>(list[m].triangles[n])-1]), list[m].X[get<2>(list[m].triangles[n])-1]);
        else if(k == 1) // Y
            return min(min(list[i].Y[get<0>(list[i].triangles[j])-1], list[i].Y[get<1>(list[i].triangles[j])-1]), list[i].Y[get<2>(list[i].triangles[j])-1]) > min(min(list[m].Y[get<0>(list[m].triangles[n])-1], list[m].Y[get<1>(list[m].triangles[n])-1]), list[m].Y[get<2>(list[m].triangles[n])-1]);
        else // -Z
            return max(max(list[i].Z[get<0>(list[i].triangles[j])-1], list[i].Z[get<1>(list[i].triangles[j])-1]), list[i].Z[get<2>(list[i].triangles[j])-1]) < max(max(list[m].Z[get<0>(list[m].triangles[n])-1], list[m].Z[get<1>(list[m].triangles[n])-1]), list[m].Z[get<2>(list[m].triangles[n])-1]);
    }
    int k;
    vector<Shape> list;
};

void normalize(vector<float>& v){
    float length = sqrt(pow(v[0],2) + pow(v[1],2) + pow(v[2],2));
    if (length == 0)
        return;
    v[0] /= length;
    v[1] /= length;
    v[2] /= length;
    return;
}

void phong(Shape& s, int id){
    vector<float> Lvector = {parser.lighting[0]-s.X[id], parser.lighting[1]-s.Y[id], parser.lighting[2]-s.Z[id]};
    normalize(Lvector);
    s.setNormal();
    vector<float> Nvector = s.normals[id];
    normalize(Nvector);
    vector<float> Vvector = {parser.lighting[16]-s.X[id], parser.lighting[17]-s.Y[id], parser.lighting[18]-s.Z[id]};
    normalize(Vvector);

    vector<float> Rvector(3);
    float dotNL = Nvector[0] * Lvector[0] + Nvector[1] * Lvector[1] + Nvector[2] * Lvector[2];
    for(int i = 0; i < 3; i++){
        Rvector[i] = 2 * dotNL * Nvector[i] - Lvector[i];
    }
    normalize(Rvector);
    
    float dotRV = Rvector[0] * Vvector[0] + Rvector[1] * Vvector[1] + Rvector[2] * Vvector[2];
    float dotNV = Nvector[0] * Vvector[0] + Nvector[1] * Vvector[1] + Nvector[2] * Vvector[2];
    float dist = sqrt(pow((s.ndcX[id]-get<0>(ndcLight)), 2)
                    + pow((s.ndcY[id]-get<1>(ndcLight)), 2)
                    + pow((s.ndcZ[id]-get<2>(ndcLight)), 2));  // BUG HERE
    
    vector<float> temp(3);
    for(int i = 0; i < 3; i++){ // intensity for R,G,B
        float a = parser.lighting[6+i]*dotNL;
        float b = parser.lighting[9+i]*dotRV;
        if(dotNL*dotNV < 0){ // light and eye are on different side
            a = 0;
            b = 0;
        }else if(dotNL < 0) // no specular light
            a = 0;
        else if(dotRV < 0) // no specular light
            b = 0;
        temp[i] = parser.lighting[3+i] * parser.lighting[12] +
                               (parser.lighting[13]/(float)(parser.lighting[14] + dist)) * (a + pow(b,parser.lighting[15]));
    }
    
    s.intensities[id] = temp;
    return;
}

void set_intensity(Shape& s){
    vector< vector<float> > ret;
    for(int i = 0; i < s.numVertex; i++){
        phong(s,i);
    }
    return;
}




// for randoming
vector<int> SelectN(vector<int>& grid, int n){
    vector<int> ret;
    for(int i = 0; i < n; i++){
        if(grid.size() == 0)
            return ret;
        int target = rand() % grid.size();
        ret.push_back(grid[target]);
        grid.erase(grid.begin()+target);
    }
    return ret;
}

// set the pixel buffer
void setPixel(int x, int y, int triangleID, float* PxBuff, float* ObjBuff, vector<float> intensity){
    // reset pixel to prevent overlap
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            for(int k = 0; k < 3; k++){
                PxBuff[3*(3*(y*parser.settings[0]+x)+j+i*parser.settings[0])+k] = parser.settings[2+k]/255.0;
            }
        }
    }
    
    if(!halfTone){
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                for(int k = 0; k < 3; k++){
                    PxBuff[3*(3*(y*parser.settings[0]+x)+j+i*parser.settings[0])+k] = intensity[k];
                }
            }
        }
        ObjBuff[(y*parser.settings[0]/3+x)] = (indexID==-1) ? -1 : pow(3, indexID) * pow(2, triangleID);
        return;
    }
    
    int totalPixel = (int)(9 * max(max(intensity[0],intensity[1]),intensity[2]));
    totalPixel = min(9, totalPixel);
    float sum = intensity[0]+intensity[1]+intensity[2];
    int red = (int)(intensity[0]*totalPixel/sum);
    int green = (int)(intensity[1]*totalPixel/sum);
    int blue = totalPixel - red - green;

    vector<int> temp = {0,1,2,3,4,5,6,7,8};
    vector<int> grid = SelectN(temp, totalPixel);
    int megaPos = 3*(y*parser.settings[0]+x);
    for(int k = 0; k < red; k++){
        int i = grid[k] % 3;
        int j = grid[k] / 3;
        PxBuff[3*(megaPos+i+j*parser.settings[0])+0] = 1;
        PxBuff[3*(megaPos+i+j*parser.settings[0])+1] = 0;
        PxBuff[3*(megaPos+i+j*parser.settings[0])+2] = 0;
    }
    for(int k = 0; k < green; k++){
        int i = grid[k+red] % 3;
        int j = grid[k+red] / 3;
        PxBuff[3*(megaPos+i+j*parser.settings[0])+0] = 0;
        PxBuff[3*(megaPos+i+j*parser.settings[0])+1] = 1;
        PxBuff[3*(megaPos+i+j*parser.settings[0])+2] = 0;
    }
    for(int k = 0; k < blue; k++){
        int i = grid[k+red+green] % 3;
        int j = grid[k+red+green] / 3;
        PxBuff[3*(megaPos+i+j*parser.settings[0])+0] = 0;
        PxBuff[3*(megaPos+i+j*parser.settings[0])+1] = 0;
        PxBuff[3*(megaPos+i+j*parser.settings[0])+2] = 1;
    }
    ObjBuff[(y*parser.settings[0]/3+x)] = (indexID==-1) ? -1 : pow(3, indexID) * pow(2, triangleID);
}

// DDA Algorithm to draw line
void dda(Shape& s, float x1, float y1, float x2, float y2, int triangleID, vector<float> I1, vector<float> I2, float* PxBuff, float* ObjBuff){
    float dx = x2 - x1;
    float dy = y2 - y1;
    float increment = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    
    // calculate increment in x & y for each steps
    float xStep = dx / increment;
    float yStep = dy / increment;
    
    // set pixel for each step
    float x = x1;
    float y = y1;
    
    vector<float> intensity(3);
    for (int i = 0; i < increment; i++){
        // Gourand shading
        for(int j = 0; j < 3; j++){
            intensity[j] = (sqrt(pow((x2-x),2) + pow((y2-y),2))/(float)sqrt(pow(dx,2) + pow(dy,2)))*I1[j]
            + (sqrt(pow((x-x1),2) + pow((y-y1),2))/(float)sqrt(pow(dx,2) + pow(dy,2)))*I2[j];
        }
        s.intensityDict[make_pair(round(x), round(y))] = intensity;
        
        setPixel(round(x), round(y), triangleID, PxBuff, ObjBuff, intensity);
        x += xStep;
        y += yStep;
    }
}

bool check(int x, int y, int id, float* ObjBuff){
    if(ObjBuff[(y*parser.settings[0]/3+x)] == pow(3, indexID) * pow(2, id)){
        return true;
    }
    return false;
}

void fillTriangle(Shape& s, int id, float* PixBuff, float* ObjBuff){
    int pt1 = get<0>(s.triangles[id])-1;
    int pt2 = get<1>(s.triangles[id])-1;
    int pt3 = get<2>(s.triangles[id])-1;
    float yMin = min(min(s.ndcY[pt1],s.ndcY[pt2]),s.ndcY[pt3]);
    float yMax = max(max(s.ndcY[pt1],s.ndcY[pt2]),s.ndcY[pt3]);
    float zMin = min(min(s.ndcZ[pt1],s.ndcZ[pt2]),s.ndcZ[pt3]);
    float zMax = max(max(s.ndcZ[pt1],s.ndcZ[pt2]),s.ndcZ[pt3]);
    
    float min = 0;
    float max = 0;
    if(ObjBuff == ObjBufferXY){
        min = yMin*(parser.settings[0]/3-1);
        max = yMax*(parser.settings[0]/3-1);
    }else{
        min = zMin*(parser.settings[0]/3-1);
        max = zMax*(parser.settings[0]/3-1);
    }
    
    for(int j = round(min); j <= round(max); j++){
        int group = 0;
        for(int i = 0; i < parser.settings[0]/3; i++){
            if(check(i, j, id, ObjBuff)){
                if(check(i+1, j, id, ObjBuff)){
                    continue;
                }else{
                    group++;
                }
            }
        }
        
        int left = -1;
        int right = -1;
        int intersectCount = 0;
        vector<float> leftI = {0,0,0};
        vector<float> rightI = {0,0,0};
        for(int i = 0; i < parser.settings[0]/3; i++){
            
            if(check(i,j,id,ObjBuff)){
                if(group == 1) //straight line
                    break;
                if(check(i+1,j,id,ObjBuff)) // double counting line segment
                    continue;
                intersectCount++;
            }
            
            if(left == -1 && intersectCount % 2 == 1){
                leftI = s.intensityDict[make_pair(i, j)];
                left = i;
            }else if(intersectCount > 0 && intersectCount % 2 == 0){
                rightI = s.intensityDict[make_pair(i, j)];
                right = i;
            }
            if(left != -1 && right != -1){
                for (int k = left+1; k < right; k++){
                    // Gourand shading
                    vector<float> intensity = {0,0,0};
                    for(int l = 0; l < 3; l++){
                        intensity[l] = (abs(right-k)/(float)abs(right-left)) * leftI[l]
                                     + (abs(k-left)/(float)abs(right-left)) * rightI[l];
                    }
                    setPixel(k, j, id, PixBuff, ObjBuff, intensity);
                }
                left = -1;
                right = -1;
            }
        }
    }
}

//initialize the display window
void init(){
    glClearColor(parser.settings[11], parser.settings[12], parser.settings[13], 0); //clears the buffer of OpenGL
    for(int x = 0; x < parser.settings[0]; x++){
        for(int y = 0; y < parser.settings[1]; y++){
            for(int i = 0; i < 3; i++){
                PixelBufferXY[3*(y*parser.settings[0]+x)+i] = parser.settings[2+i]/255.0;
                PixelBufferYZ[3*(y*parser.settings[0]+x)+i] = parser.settings[2+i]/255.0;
                PixelBufferXZ[3*(y*parser.settings[0]+x)+i] = parser.settings[2+i]/255.0;
            }
            ObjBufferXY[(y*parser.settings[0]+x)] = -1;
            ObjBufferYZ[(y*parser.settings[0]+x)] = -1;
            ObjBufferXZ[(y*parser.settings[0]+x)] = -1;
        }
    }
}

void Bounding(){
    // check for out-of-bound
    float Xmin = parser.lighting[0];
    float Xmax = parser.lighting[0];
    float Ymin = parser.lighting[1];
    float Ymax = parser.lighting[1];
    float Zmin = parser.lighting[2];
    float Zmax = parser.lighting[2];
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
    float lowBound = min(min(Xmin,Ymin),Zmin) - parser.settings[0]*0.1;
    float highBound = max(max(Xmax,Ymax),Zmax) + parser.settings[0]*0.1;
    get<0>(ndcrotationPt1) = ((get<0>(rotationPt1) - lowBound) / (float)(highBound - lowBound));
    get<1>(ndcrotationPt1) = ((get<1>(rotationPt1) - lowBound) / (float)(highBound - lowBound));
    get<2>(ndcrotationPt1) = ((get<2>(rotationPt1) - lowBound) / (float)(highBound - lowBound));
    get<0>(ndcrotationPt2) = ((get<0>(rotationPt2) - lowBound) / (float)(highBound - lowBound));
    get<1>(ndcrotationPt2) = ((get<1>(rotationPt2) - lowBound) / (float)(highBound - lowBound));
    get<2>(ndcrotationPt2) = ((get<2>(rotationPt2) - lowBound) / (float)(highBound - lowBound));
    get<0>(ndcLight) = ((parser.lighting[0] - lowBound) / (float)(highBound - lowBound));
    get<1>(ndcLight) = ((parser.lighting[1] - lowBound) / (float)(highBound - lowBound));
    get<2>(ndcLight) = ((parser.lighting[2] - lowBound) / (float)(highBound - lowBound));
    for(int i = 0; i < (int)parser.list.size(); i++){
        for(int j = 0; j < (int)parser.list[i].numVertex; j++){
            parser.list[i].ndcX[j] = ((parser.list[i].X[j] - lowBound) / (float)(highBound - lowBound));
            parser.list[i].ndcY[j] = ((parser.list[i].Y[j] - lowBound) / (float)(highBound - lowBound));
            parser.list[i].ndcZ[j] = ((parser.list[i].Z[j] - lowBound) / (float)(highBound - lowBound));
        }
    }
}

void printText(string str){
    for(int i = 0; i < (int)str.size(); ++i)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15,str[i]);
}

//main display loop, this function will be called again and again by OpenGL
void display(){
    init(); //clears the buffer of OpenGL
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glViewport(0, 0, parser.settings[9], parser.settings[10]); //display text on main screen
    glLoadIdentity();
    // *************THIS IS ONLY USED FOR TEXT PRINTING. NOT FOR ACTUAL DRAWING***********
    glOrtho(0, parser.settings[9], 0, parser.settings[10],-1,1);
    // ***********************************************************************************
    glColor3f(1, 0, 0);
    glRasterPos2f(10+parser.settings[1]/2-90, 5);
    printText("XY Projection: --> X");
    glRasterPos2f(10+parser.settings[0]/2-90, 25 + parser.settings[1]);
    printText("XZ Projection: --> X");
    glRasterPos2f(30+3*(parser.settings[0]-1)/2-90, 25 + parser.settings[1]);
    printText("YZ Projection: --> Y");
    
    Bounding(); //handle out-of-bound condition
    
    //sort the objects
    vector< pair<int,int> > triangleList;
    for(int i = 0; i < (int)parser.list.size(); i++){
        set_intensity(parser.list[i]);
        for(int j = 0; j < (int)parser.list[i].triangles.size(); j++){
            triangleList.push_back(make_pair(i,j));
        }
    }
    
    // Projection onto 3 planes
    // XZ Plane
    glViewport(20, parser.settings[1]+40, parser.settings[0], parser.settings[1]);    
    glLoadIdentity();
    //displayLight
    if(displayLightSource){
        indexID = -1;
        setPixel(get<0>(ndcLight)*(parser.settings[0]/3-1), get<2>(ndcLight)*(parser.settings[0]/3-1),-1, PixelBufferXZ, ObjBufferXZ, {1,1,1});
        setPixel(get<0>(ndcLight)*(parser.settings[0]/3-1)+1, get<2>(ndcLight)*(parser.settings[0]/3-1),-1, PixelBufferXZ, ObjBufferXZ, {1,1,1});
        setPixel(get<0>(ndcLight)*(parser.settings[0]/3-1)-1, get<2>(ndcLight)*(parser.settings[0]/3-1),-1, PixelBufferXZ, ObjBufferXZ, {1,1,1});
        setPixel(get<0>(ndcLight)*(parser.settings[0]/3-1), get<2>(ndcLight)*(parser.settings[0]/3-1)+1,-1, PixelBufferXZ, ObjBufferXZ, {1,1,1});
        setPixel(get<0>(ndcLight)*(parser.settings[0]/3-1), get<2>(ndcLight)*(parser.settings[0]/3-1)-1,-1, PixelBufferXZ, ObjBufferXZ, {1,1,1});
    }

    sort(triangleList.begin(), triangleList.end(), sortTriangles(parser.list, 1));
    for(auto triangle : triangleList){
        int i = triangle.first;
        int j = triangle.second;
        indexID = i;
        
        // hidden surface removal
        vector<float> surfeceNormal = parser.list[i].cross(get<0>(parser.list[i].triangles[j])-1, get<1>(parser.list[i].triangles[j])-1, get<2>(parser.list[i].triangles[j])-1);
        if(surfeceNormal[1] >= 0)
            continue;
        
        for(int k = 0; k < 3; k++){
            int pt1, pt2;
            if(k == 0){
                pt1 = get<0>(parser.list[i].triangles[j])-1;
                pt2 = get<1>(parser.list[i].triangles[j])-1;
            }else if(k == 1){
                pt1 = get<1>(parser.list[i].triangles[j])-1;
                pt2 = get<2>(parser.list[i].triangles[j])-1;
            }else{
                pt1 = get<2>(parser.list[i].triangles[j])-1;
                pt2 = get<0>(parser.list[i].triangles[j])-1;
            }
            vector<float> I1 = parser.list[i].intensities[pt1];
            vector<float> I2 = parser.list[i].intensities[pt2];
            dda(parser.list[i],
                parser.list[i].ndcX[pt1]*(parser.settings[0]/3-1), parser.list[i].ndcZ[pt1]*(parser.settings[0]/3-1),
                parser.list[i].ndcX[pt2]*(parser.settings[0]/3-1), parser.list[i].ndcZ[pt2]*(parser.settings[0]/3-1),
                j,I1,I2,PixelBufferXZ,ObjBufferXZ);
        }
        //rasterizing
        if(parser.list[i].filled)
            fillTriangle(parser.list[i], j, PixelBufferXZ, ObjBufferXZ);
        
        parser.list[i].intensityDict.clear();
    }
    
    
    glRasterPos2i(-1,-1);
    glDrawPixels(parser.settings[0], parser.settings[1], GL_RGB, GL_FLOAT, PixelBufferXZ);
    
    // YZ Plane
    glViewport(parser.settings[0]+40, parser.settings[1]+40, parser.settings[0], parser.settings[1]);
    glLoadIdentity();
    //displayLight
    if(displayLightSource){
        indexID = -1;
        setPixel(get<1>(ndcLight)*(parser.settings[0]/3-1), get<2>(ndcLight)*(parser.settings[0]/3-1),-1, PixelBufferYZ, ObjBufferYZ, {1,1,1});
        setPixel(get<1>(ndcLight)*(parser.settings[0]/3-1)+1, get<2>(ndcLight)*(parser.settings[0]/3-1),-1, PixelBufferYZ, ObjBufferYZ, {1,1,1});
        setPixel(get<1>(ndcLight)*(parser.settings[0]/3-1)-1, get<2>(ndcLight)*(parser.settings[0]/3-1),-1, PixelBufferYZ, ObjBufferYZ, {1,1,1});
        setPixel(get<1>(ndcLight)*(parser.settings[0]/3-1), get<2>(ndcLight)*(parser.settings[0]/3-1)+1,-1, PixelBufferYZ, ObjBufferYZ, {1,1,1});
        setPixel(get<1>(ndcLight)*(parser.settings[0]/3-1), get<2>(ndcLight)*(parser.settings[0]/3-1)-1,-1, PixelBufferYZ, ObjBufferYZ, {1,1,1});
    }
    
    sort(triangleList.begin(), triangleList.end(), sortTriangles(parser.list, 0));
    for(auto triangle : triangleList){
        int i = triangle.first;
        int j = triangle.second;
        indexID = i;
        
        // hidden surface removal
        vector<float> surfeceNormal = parser.list[i].cross(get<0>(parser.list[i].triangles[j])-1, get<1>(parser.list[i].triangles[j])-1, get<2>(parser.list[i].triangles[j])-1);
        if(surfeceNormal[0] <= 0)
            continue;
        
        for(int k = 0; k < 3; k++){
            int pt1, pt2;
            if(k == 0){
                pt1 = get<0>(parser.list[i].triangles[j])-1;
                pt2 = get<1>(parser.list[i].triangles[j])-1;
            }else if(k == 1){
                pt1 = get<1>(parser.list[i].triangles[j])-1;
                pt2 = get<2>(parser.list[i].triangles[j])-1;
            }else{
                pt1 = get<2>(parser.list[i].triangles[j])-1;
                pt2 = get<0>(parser.list[i].triangles[j])-1;
            }
            vector<float> I1 = parser.list[i].intensities[pt1];
            vector<float> I2 = parser.list[i].intensities[pt2];
            dda(parser.list[i],
                parser.list[i].ndcY[pt1]*(parser.settings[0]/3-1), parser.list[i].ndcZ[pt1]*(parser.settings[0]/3-1),
                parser.list[i].ndcY[pt2]*(parser.settings[0]/3-1), parser.list[i].ndcZ[pt2]*(parser.settings[0]/3-1),
                j,I1,I2,PixelBufferYZ,ObjBufferYZ);
        }
        //rasterizing
        if(parser.list[i].filled)
            fillTriangle(parser.list[i], j, PixelBufferYZ, ObjBufferYZ);
        
        parser.list[i].intensityDict.clear();
    }
    glRasterPos2i(-1,-1);
    glDrawPixels(parser.settings[0], parser.settings[1], GL_RGB, GL_FLOAT, PixelBufferYZ);
    
    // XY Plane
    glViewport(20, 20, parser.settings[0], parser.settings[1]);
    glLoadIdentity();
    //displayLight
    if(displayLightSource){
        indexID = -1;
        setPixel(get<0>(ndcLight)*(parser.settings[0]/3-1), get<1>(ndcLight)*(parser.settings[0]/3-1),-1, PixelBufferXY, ObjBufferXY, {1,1,1});
        setPixel(get<0>(ndcLight)*(parser.settings[0]/3-1)+1, get<1>(ndcLight)*(parser.settings[0]/3-1),-1, PixelBufferXY, ObjBufferXY, {1,1,1});
        setPixel(get<0>(ndcLight)*(parser.settings[0]/3-1)-1, get<1>(ndcLight)*(parser.settings[0]/3-1),-1, PixelBufferXY, ObjBufferXY, {1,1,1});
        setPixel(get<0>(ndcLight)*(parser.settings[0]/3-1), get<1>(ndcLight)*(parser.settings[0]/3-1)+1,-1, PixelBufferXY, ObjBufferXY, {1,1,1});
        setPixel(get<0>(ndcLight)*(parser.settings[0]/3-1), get<1>(ndcLight)*(parser.settings[0]/3-1)-1,-1, PixelBufferXY, ObjBufferXY, {1,1,1});
    }
    
    sort(triangleList.begin(), triangleList.end(), sortTriangles(parser.list, 2));
    for(auto triangle: triangleList){
        int i = triangle.first;
        int j = triangle.second;
        indexID = i;
        
        // hidden surface removal
        vector<float> surfeceNormal = parser.list[i].cross(get<0>(parser.list[i].triangles[j])-1, get<1>(parser.list[i].triangles[j])-1, get<2>(parser.list[i].triangles[j])-1);
        if(surfeceNormal[2] <= 0)
            continue;
        
        for(int k = 0; k < 3; k++){
            int pt1, pt2;
            if(k == 0){
                pt1 = get<0>(parser.list[i].triangles[j])-1;
                pt2 = get<1>(parser.list[i].triangles[j])-1;
            }else if(k == 1){
                pt1 = get<1>(parser.list[i].triangles[j])-1;
                pt2 = get<2>(parser.list[i].triangles[j])-1;
            }else{
                pt1 = get<2>(parser.list[i].triangles[j])-1;
                pt2 = get<0>(parser.list[i].triangles[j])-1;
            }
            vector<float> I1 = parser.list[i].intensities[pt1];
            vector<float> I2 = parser.list[i].intensities[pt2];
            
            dda(parser.list[i],
                parser.list[i].ndcX[pt1]*(parser.settings[0]/3-1), parser.list[i].ndcY[pt1]*(parser.settings[0]/3-1),
                parser.list[i].ndcX[pt2]*(parser.settings[0]/3-1), parser.list[i].ndcY[pt2]*(parser.settings[0]/3-1),
                j,I1,I2,PixelBufferXY,ObjBufferXY);
        }
        
        //rasterizing
        if(parser.list[i].filled)
            fillTriangle(parser.list[i], j, PixelBufferXY, ObjBufferXY);
        
        parser.list[i].intensityDict.clear();
    }
    glRasterPos2i(-1,-1);
    glDrawPixels(parser.settings[0], parser.settings[1], GL_RGB, GL_FLOAT, PixelBufferXY);
    
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
        displayLightSource = !displayLightSource;
    }else if (key == 'x'){
        parser.list[currObj].scale(scalingFactor);
    }else if (key == 'z'){
        parser.list[currObj].scale(1/scalingFactor);
    }else if (key == 'f'){
        parser.list[currObj].filled = !parser.list[currObj].filled;
    }else if (key == 'h'){
        halfTone = !halfTone;
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
    delete[] PixelBufferXY;
    delete[] PixelBufferXZ;
    delete[] PixelBufferYZ;
    delete[] ObjBufferXY;
    delete[] ObjBufferXZ;
    delete[] ObjBufferYZ;
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
    
    delete[] PixelBufferXY;
    delete[] PixelBufferXZ;
    delete[] PixelBufferYZ;
    delete[] ObjBufferXY;
    delete[] ObjBufferXZ;
    delete[] ObjBufferYZ;
    PixelBufferXY = new float[parser.settings[0] * parser.settings[1] * 3];
    PixelBufferXZ = new float[parser.settings[0] * parser.settings[1] * 3];
    PixelBufferYZ = new float[parser.settings[0] * parser.settings[1] * 3];
    ObjBufferXY = new float[parser.settings[0] * parser.settings[1]];
    ObjBufferXZ = new float[parser.settings[0] * parser.settings[1]];
    ObjBufferYZ = new float[parser.settings[0] * parser.settings[1]];
    
    glutPostRedisplay();
}
void phongMenu (int id){
    if(id == 0){ // Light Source Position
        cout<<"Setting Light Source Position (ex: 100 100 100): ";
        cin >> parser.lighting[0] >> parser.lighting[1] >> parser.lighting[2];
    }else if(id == 1){ // From Point Position
        cout<<"Setting From Point Position (ex: 100 100 100): ";
        cin >> parser.lighting[16] >> parser.lighting[17] >> parser.lighting[18];
    }else if(id == 2){ // Ka
        cout<<"Setting Ambient Reflection Coefficient (ex: 0.1 0.1 0.1): ";
        cin >> parser.lighting[3] >> parser.lighting[4] >> parser.lighting[5];
    }else if(id == 3){ // Kd
        cout<<"Setting Diffuse Reflection Coefficient (ex: 0.1 0.1 0.1): ";
        cin >> parser.lighting[6] >> parser.lighting[7] >> parser.lighting[8];
    }else if(id == 4){ // Ks
        cout<<"Setting Specular Reflection Coefficient (ex: 0.1 0.1 0.1): ";
        cin >> parser.lighting[9] >> parser.lighting[10] >> parser.lighting[11];
    }else if(id == 5){ // Ia
        cout<<"Setting Ambient Light Intensity (ex: 0.8): ";
        cin >> parser.lighting[12];
    }else if(id == 6){ // IL
        cout<<"Setting Light Source Intensity (ex: 0.8): ";
        cin >> parser.lighting[13];
    }else if(id == 7){ // K
        cout<<"Setting Constant K (ex: 0.5): ";
        cin >> parser.lighting[14];
    }else if(id == 8){ // n
        cout<<"Setting Constant n (ex: 4): ";
        cin >> parser.lighting[15];
    }
    glutPostRedisplay();
}
void mainMenu (int id){
    if(id == 1){
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
        cout<<"Setting Window Size (ex: 660 660): ";
        cin >> parser.settings[9] >> parser.settings[10];
        if(parser.settings[9] < 160)
            parser.settings[9] = 160;
        if(parser.settings[10] < 160)
            parser.settings[10] = 160;
        parser.settings[0] = (min(parser.settings[9],parser.settings[10]) - 60)/2;
        parser.settings[1] = parser.settings[0];
    }
    
    // allocate new pixel buffer, need initialization!!
    PixelBufferXY = new float[parser.settings[0] * parser.settings[1] * 3];
    PixelBufferXZ = new float[parser.settings[0] * parser.settings[1] * 3];
    PixelBufferYZ = new float[parser.settings[0] * parser.settings[1] * 3];
    ObjBufferXY = new float[parser.settings[0] * parser.settings[1]];
    ObjBufferXZ = new float[parser.settings[0] * parser.settings[1]];
    ObjBufferYZ = new float[parser.settings[0] * parser.settings[1]];
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(parser.settings[9], parser.settings[10]);
    glutInitWindowPosition(50, 50);
    
    // create and set main window title
    glutCreateWindow("ECS175 Project3");
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
    
    int view_menu = glutCreateMenu(viewMenu); // 3
    glutAddMenuEntry ("300x300", 0);
    glutAddMenuEntry ("400x400", 1);
    glutAddMenuEntry ("Custom (Input in Terminal)", 2);
    
    int object_menu = glutCreateMenu (objectMenu); // 4
    for(int i = 0; i < (int)parser.list.size(); i++){
        const char* tag = to_string(i).c_str();
        glutAddMenuEntry (tag, i);
    }
    
    int window_menu = glutCreateMenu(windowMenu); // 5
    glutAddMenuEntry ("660x660", 0);
    glutAddMenuEntry ("1000x1000", 1);
    glutAddMenuEntry ("Custom (Input in Terminal)", 2);
    
    int windowColor_menu = glutCreateMenu(windowBGCMenu); // 6
    glutAddMenuEntry ("Black", 0);
    glutAddMenuEntry ("White", 1);
    
    int resize = glutCreateMenu(empty); // 7
    glutAddSubMenu ("Resize Window:", window_menu);
    glutAddSubMenu ("Resize Viewport:", view_menu);
    
    int color = glutCreateMenu(empty); // 8
    glutAddSubMenu ("ViewPort Color:", background_menu);
    glutAddSubMenu ("Background Color:", windowColor_menu);
    
    int phong = glutCreateMenu(phongMenu); // 9
    glutAddMenuEntry ("Light Source Position (Input in Terminal)", 0);
    glutAddMenuEntry ("From Point Position (Input in Terminal)", 1);
    glutAddMenuEntry ("Ka (Input in Terminal)", 2);
    glutAddMenuEntry ("Kd (Input in Terminal)", 3);
    glutAddMenuEntry ("Ks (Input in Terminal)", 4);
    glutAddMenuEntry ("Ia (Input in Terminal)", 5);
    glutAddMenuEntry ("IL (Input in Terminal)", 6);
    glutAddMenuEntry ("Constant K (Input in Terminal)", 7);
    glutAddMenuEntry ("Constant n (Input in Terminal)", 8);
    
    glutCreateMenu (mainMenu); // 0
    glutAddSubMenu ("Current Object:", object_menu);
    glutAddSubMenu ("Phong Lighting Model Parameters:", phong);
    glutAddSubMenu("Transformation Parameters:", tranformation_menu);
    glutAddSubMenu ("Resize:", resize);
    glutAddSubMenu ("Color:", color);
    glutAddMenuEntry ("Exit", 1);
    glutAttachMenu (GLUT_RIGHT_BUTTON);
    
    atexit(saveScreen);
    
    glutMainLoop();// main display loop, will display until terminate
    return 0;
}
