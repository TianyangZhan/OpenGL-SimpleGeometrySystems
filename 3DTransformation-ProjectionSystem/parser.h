#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <tuple>
#include <utility> //std::pair

using namespace std;

class Shape{
public:
    
    Shape(){}
    
    int numVertex;
    vector<float> X;
    vector<float> Y;
    vector<float> Z;
    vector<float> ndcX;
    vector<float> ndcY;
    vector<float> ndcZ;
    vector<float> oblX;
    vector<float> oblY;
    vector<float> oblZ;
    
    int numEdge;
    vector< pair<int,int> > edges;
    
    tuple<float, float, float> getCentroid();
    
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
    }
    int settings[14];
    vector<Shape> list;
    
    void loadShape(string file);
    void loadSetting(string file);
    void saveSetting(string file);
    void saveShape(string file);
};



#endif





