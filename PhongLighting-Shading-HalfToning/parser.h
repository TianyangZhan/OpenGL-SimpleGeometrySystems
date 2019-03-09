#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <tuple>
#include <map>
#include <utility> //std::pair

using namespace std;

class Shape{
public:
    
    Shape(){filled = false;}
    
    int numVertex;
    vector<float> X;
    vector<float> Y;
    vector<float> Z;
    vector<float> ndcX;
    vector<float> ndcY;
    vector<float> ndcZ;
    
    int numTriangle;
    vector< tuple<int,int,int> > triangles;
    map<int, vector<float> > intensities;
    map<pair<int,int>, vector<float> > intensityDict;
    map<int, vector<float> > normals;
    
    bool filled;
    
    tuple<float, float, float> getCentroid();
    vector<float> cross(int start, int first, int second);
    void setNormal();
    void translate(float x, float y, float z);
    void rotate(tuple<float,float,float> pt1, tuple<float,float,float> pt2, float degree);
    void scale(float a);
};

class Data{
public:
    Data(){
        // width, height
        settings[0] = 300; settings[1] = 300;
        // ViewPort(R,G,B)
        settings[2] = 0; settings[3] = 0; settings[4] = 0;
        // Shape(R,G,B)
        settings[5] = 255; settings[6] = 0; settings[7] = 0;
        // Line Algorithm
        settings[8] = 2; //2->cavalier 4->cabinet
        // WindowSize
        settings[9] = 660; settings[10] = 660;
        // Background(R,G,B)
        settings[11] = 1; settings[12] = 1; settings[13] = 1;
        
        //Light Source Position X
        lighting[0] = 0; lighting[1] = 0; lighting[2] = 0;
        //Ambient(R,G,B) Ka
        lighting[3] = 0.4; lighting[4] = 0.8; lighting[5] = 0.7;
        //Diffuse(R,G,B) Kd
        lighting[6] = 1; lighting[7] = 0; lighting[8] = 0;
        //Specular(R,G,B) Ks
        lighting[9] = 0.7; lighting[10] = 0.8; lighting[11] = 0.9;
        //Ambient Intensity IA
        lighting[12] = 0.5;
        //Light Source Intensity IL
        lighting[13] = 0.7;
        //Distance Constant K
        lighting[14] = 0.5;
        // Phone Constant n
        lighting[15] = 4;
        //From Position (View position) F
        lighting[16] = 10; lighting[17] = 10; lighting[18] = 10;
    }
    int settings[14];
    float lighting[19];
    vector<Shape> list;
    
    void loadShape(string file);
    void loadSetting(string file);
    void saveSetting(string file);
    void saveShape(string file);
};



#endif





