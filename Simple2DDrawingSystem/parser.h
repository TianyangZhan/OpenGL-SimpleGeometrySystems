#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <utility> //std::pair

using namespace std;

class Shape{
public:
    
    Shape(){yMin = 0; yMax = 0; filled = false;}
    
    int numVertex;
    vector< pair<float,float> > cordinates;
    
    float yMin;
    float yMax;
    void setYMinandMax();
    
    vector< pair<int,int> > extremes;
    void setExtreme();
    
    pair<float, float> getCentroid();
    bool filled;
    
    bool isExtreme(int x, int y);
    
    void lineClip(int low, int high, int left, int right);
    void polyClip(int low, int high, int left, int right);
    void clipping(int low, int high, int left, int right);
    
    void translate(float x, float y);
    void rotate(float degree);
    void scale(float a);
    void reflect();
};

class Data{
public:
    Data(){
        // width, height
        settings[0] = 700; settings[1] = 500;
        // ViewPort(R,G,B)
        settings[2] = 0; settings[3] = 0; settings[4] = 0;
        // Shape(R,G,B)
        settings[5] = 255; settings[6] = 0; settings[7] = 0;
        // Line Algorithm
        settings[8] = 0;
        // WindowSize
        settings[9] = 800; settings[10] = 600;
        // Background(R,G,B)
        settings[11] = 1; settings[12] = 1; settings[13] = 1;
        
        algo[0] = "dda";
        algo[1] = "bresenham";
    }
    int settings[14];
    string algo[2]; // "dda" or "bresenham"
    vector<Shape> list;
    
    void loadShape(string file);
    void loadSetting(string file);
    void saveSetting(string file);
    void saveShape(string file);
};



#endif





