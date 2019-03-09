#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include "parser.h"

using namespace std;

// ******************* Shape Class *******************

void Shape::setYMinandMax(){
    
    if(numVertex ==0)
        return ;
    
    float min = cordinates[0].second;
    float max = cordinates[0].second;
    for(int i = 0; i < numVertex; i++){
        if(cordinates[i].second < min)
        min = cordinates[i].second;
        if(cordinates[i].second > max)
        max = cordinates[i].second;
    }
    yMin = min;
    yMax = max;
}

void Shape::setExtreme(){
    
    if(numVertex ==0)
        return;
    
    if(numVertex <= 2){
        for(int i = 0; i < numVertex; i++){
            extremes.push_back(make_pair(round(cordinates[i].first), round(cordinates[i].second)));
        }
        return;
    }
    for(int i = 1; i < numVertex; i++){
        float yCur = cordinates[i].second;
        float yPre = cordinates[i-1].second;
        float yNext;
        if(i == numVertex - 1)
            yNext = cordinates[0].second;
        else
            yNext = cordinates[i+1].second;
        
        int temp = round((yPre - yCur)) * round((yCur - yNext));
        if(temp <= 0){
            extremes.push_back(make_pair(round(cordinates[i].first), round(cordinates[i].second)));
        }
    }
}

bool Shape::isExtreme(int x, int y){
    for(int i = 0; i < (int)extremes.size(); i++){
        if(x == extremes[i].first && y == extremes[i].second){
            return true;
        }
    }
    return false;
}

pair<float, float> Shape::getCentroid(){
    pair<float,float> centroid;
    centroid.first = 0;
    centroid.second = 0;
    
    if(numVertex ==0)
        return centroid;
    
    if (numVertex == 1) {
        centroid = cordinates[0];
        return centroid;
    }
    if (numVertex == 2) {
        centroid.first = (cordinates[0].first + cordinates[1].first)/2;
        centroid.second = (cordinates[0].second + cordinates[1].second)/2;
        return centroid;
    }
    
    float area = 0.0;
    // For all vertices except last
    for (int i = 0; i < numVertex; ++i){
        float x1 = cordinates[i].first;
        float y1 = cordinates[i].second;
        float x2;
        float y2;
        if(i == numVertex - 1){
            x2 = cordinates[0].first;
            y2 = cordinates[0].second;
        }else{
            x2 = cordinates[i+1].first;
            y2 = cordinates[i+1].second;
        }
        float temp = x1 * y2 - x2 * y1;
        area += temp;
        centroid.first += (x1 + x2)*temp;
        centroid.second += (y1 + y2)*temp;
    }
    
    centroid.first /= (3.0*area);
    centroid.second /= (3.0*area);
    return centroid;
}


void Shape::translate(float x, float y){
    for (int i = 0; i < numVertex; i++) {
        cordinates[i].first += x;
        cordinates[i].second += y;
    }
}

void Shape::rotate(float degree){
    pair<float,float> centroid = getCentroid();
    for (int i = 0; i < numVertex; i++) {
        float first = (cordinates[i].first - centroid.first) * cos(degree) - (cordinates[i].second - centroid.second) * sin(degree) + centroid.first;
        float second = (cordinates[i].first - centroid.first) * sin(degree) + (cordinates[i].second - centroid.second) * cos(degree) + centroid.second;
        cordinates[i].first = first;
        cordinates[i].second = second;
    }
}

void Shape::scale(float size){
    pair<float,float> centroid = getCentroid();
    for (int i = 0; i < numVertex; i++) {
        float first = (cordinates[i].first - centroid.first) * size + centroid.first;
        float second = (cordinates[i].second - centroid.second) * size + centroid.second;
        cordinates[i].first = first;
        cordinates[i].second = second;
    }
}


void Shape::reflect(){ // with respect to y-axis
    pair<float,float> centroid = getCentroid();
    for (int i = 0; i < numVertex; i++) {
        float first = 2 * centroid.first - cordinates[i].first;
        //float second = 2 * centroid.second - cordinates[i].second;
        cordinates[i].first = first;
        //cordinates[i].second = second;
    }
}


//helper function for intersect()
pair<float,float> boxIntersect(float x1, float y1, float x2, float y2, float line, bool horizontal){
    float x,y;
    if(horizontal){
        y = line;
        x = (y-(y1-(y2-y1)/(x2-x1)*x1))/((y2-y1)/(x2-x1));
    }else{
        x = line;
        y = (y2-y1)/(x2-x1)*x+(y1-(y2-y1)/(x2-x1)*x1);
    }
    return make_pair(x, y);
}

vector< pair<float,float> > intersection(pair<float,float> A, pair<float,float> B, float left, float right, float low, float high){
    
    float x1 = A.first;
    float y1 = A.second;
    float x2 = B.first;
    float y2 = B.second;
    
    vector< pair<float,float> > intersects;
    
    if(round(x1) == round(x2)){
        //vertical parallel line
        if((y1-low)*(y2-low) < 0)
            intersects.push_back(make_pair(x1,low));
        if((y1-high)*(y2-high) < 0)
            intersects.push_back(make_pair(x1,high));
        return intersects;
    }
    if(round(y1) == round(y2)){
        //horizontal parallel line
        if((x1-left)*(x2-left) < 0)
            intersects.push_back(make_pair(left,y1));
        if((x1-right)*(x2-right) < 0)
            intersects.push_back(make_pair(right,y1));
        return intersects;
    }
    
    float lst[4] = {low,high,left,right};
    pair<float,float> temp;
    for(int i = 0; i < 4; i++){
        temp = boxIntersect(x1, y1, x2, y2, lst[i], (i<2));
        if((temp.first >= left && temp.first <= right && temp.second >= low && temp.second <= high)
           && (x1-temp.first)*(x2-temp.first) < 0 &&
           (y1-temp.second)*(y2-temp.second) < 0)
        {
            intersects.push_back(temp);
        }
    }
    return intersects;
}

void Shape::lineClip(int low, int high, int left, int right){
    vector< pair<float,float> > temp;
    vector<int> position;
    for(int i = 0; i < numVertex; i++){
        int pos = 0;
        if(cordinates[i].first < left)
            pos += 1;
        if(cordinates[i].first > right)
            pos += 2;
        if(cordinates[i].second < low)
            pos += 4;
        if(cordinates[i].second > high)
            pos += 8;
        position.push_back(pos);
    }

    if(numVertex ==0)
        return;
    // single point
    if(numVertex == 1){
        if(position[0] != 0){
            cordinates.clear();
            numVertex = 0;
        }
        return;
    }
    
    //line
        int pos1 = 0, pos2 = 1;
        if(position[pos1] == 0 && position[pos2] == 0){ // both inside
            return;
        }else if((position[pos1] & position[pos2]) == 1){ // both out and rejected
            cordinates.clear();
            numVertex = 0;
            return;
        }else{ // both out Or one in one out
            if(position[pos1] == 0)
                temp.push_back(cordinates[pos1]);
            if(position[pos2] == 0)
                temp.push_back(cordinates[pos2]);
            vector< pair<float,float> > pts = intersection(cordinates[pos1], cordinates[pos2], left,right,low,high);
            for(int j = 0; j < (int)pts.size(); j++){
                if (find(temp.begin(), temp.end(), pts[j]) == temp.end())
                    temp.push_back(pts[j]);
            }
        }
    cordinates.clear();
    for(int i = 0; i < (int)temp.size(); i++)
        cordinates.push_back(temp[i]);
    numVertex = (int)cordinates.size();
}

pair<float,float> lineIntersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4){
    float x = ((x1*y2-y1*x2)*(x3-x4)-(x1-x2)*(x3*y4-y3*x4))/((x1-x2)*(y3-y4)-(y1-y2)*(x3-x4));
    float y = ((x1*y2-y1*x2)*(y3-y4)-(y1-y2)*(x3*y4-y3*x4))/((x1-x2)*(y3-y4)-(y1-y2)*(x3-x4));
    return make_pair(x, y);
}

void Shape::polyClip(int x1, int y1, int x2, int y2){
    vector< pair<float,float> > temp;
    for(int i = 0; i < numVertex; i++){
        int pos1 = i, pos2=i+1;
        if(i == numVertex-1)
            pos2 = 0;
        
        float x3 = cordinates[pos1].first;
        float y3 = cordinates[pos1].second;
        float x4 = cordinates[pos2].first;
        float y4 = cordinates[pos2].second;
        int pt1 = (x2-x1)*(y3-y1)-(y2-y1)*(x3-x1);
        int pt2 = (x2-x1)*(y4-y1)-(y2-y1)*(x4-x1);
        
        if (pt1 < 0  && pt2 < 0){ // both in
            temp.push_back(cordinates[pos2]);
        }else if(pt1 < 0  && pt2 >= 0){ // first in second out
            temp.push_back(lineIntersect(x1, y1, x2, y2, x3, y3, x4, y4));
        }else if(pt1 >= 0  && pt2 < 0){ // second in first out
            temp.push_back(lineIntersect(x1, y1, x2, y2, x3, y3, x4, y4));
            temp.push_back(cordinates[pos2]);
        }
    }
    cordinates.clear();
    for(int i = 0; i < (int)temp.size(); i++)
        cordinates.push_back(temp[i]);
    numVertex = (int)temp.size();
}

// general clipping mathod
void Shape::clipping(int low, int high, int left, int right){

    // line clipping
    if(numVertex <= 2){
        lineClip(low, high, left, right);
        return;
    }
    
    // polygon clipping
    vector< pair<int, int> > boxVertex;
    boxVertex.push_back(make_pair(left, low));
    boxVertex.push_back(make_pair(left, high));
    boxVertex.push_back(make_pair(right, high));
    boxVertex.push_back(make_pair(right, low));
    for (int i = 0; i < 4; i++){
        if(i == 3)
            polyClip(boxVertex[i].first, boxVertex[i].second, boxVertex[0].first, boxVertex[0].second);
        else
            polyClip(boxVertex[i].first, boxVertex[i].second, boxVertex[i+1].first, boxVertex[i+1].second);
    }
}
















// ******************* Data Class *******************

void Data::loadShape(string file){
    ifstream input(file);
    if (!input){
        cout <<"Error when loading file"<<endl;
        exit(1);
    }
    // number of total shapes
    int n;
    input >> n;
    
    for (int i = 0; i < n; i++){
        Shape shape;
        // number of vertices per shapes
        int v;
        input >> v;
        shape.numVertex = v;
        
        for (int j = 0; j < v; j++) {
            float x;
            float y;
            input >> x >> y;
            pair<float,float> points = make_pair(x, y);
            shape.cordinates.push_back(points);
        }
        list.push_back(shape);
    }
    input.close();
}

void Data::loadSetting(string file){
    ifstream input(file);
    if (!input){
        cout <<"Using default settings"<<endl;
        return;
    }
    // set window size and background color
    int temp;
    int count = 0;
    while(count < 8 && input >> temp){
    // Attempt read from file, return false if it fails
        settings[count] = temp;
        count++;
    }
    string lineDrawn;
    if(input >> lineDrawn){
        if(lineDrawn == "bresenham")
            settings[count] = 1;
        else
            settings[count] = 0;
        count++;
    }
    while(count < 14 && input >> temp){
        settings[count] = temp;
        count++;
    }
    input.close();
}

void Data::saveSetting(string file){
    ofstream output(file);
    if (!output.is_open()){
        cout << "Error when saving file";
        exit(1);
    }else{
        output << settings[0] << " " << settings[1] << endl;
        output << settings[2] << " " << settings[3] << " " << settings[4] << endl;
        output << settings[5] << " " << settings[6] << " " << settings[7] << endl;
        output << algo[settings[8]]<<endl;
        output << settings[9] << " " << settings[10] << endl;
        output << settings[11] << " " << settings[12] << " " << settings[13];
        output.close();
    }
}

void Data::saveShape(string file){
    ofstream output(file);
    if (!output.is_open()){
        cout << "Error when saving file";
        exit(1);
    }else{
        output << (int)list.size() << endl <<endl;
        for(int i = 0; i < (int)list.size(); i++){
            output << list[i].numVertex <<endl;
            for(int j = 0; j < list[i].numVertex; j++){
                output << setprecision(2) << fixed << list[i].cordinates[j].first << "  " << list[i].cordinates[j].second << endl;
            }
            output<<endl;
        }
        output.close();
    }
}
