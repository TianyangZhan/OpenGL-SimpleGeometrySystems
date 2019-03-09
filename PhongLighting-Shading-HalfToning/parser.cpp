#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <tuple>
#include "parser.h"

using namespace std;

// ******************* Shape Class *******************

vector<float> Shape::cross(int start, int first, int second){
    vector<float> ret(3);
    vector<float> v1(3);
    vector<float> v2(3);
    
    v1[0] = X[first] - X[start];
    v1[1] = Y[first] - Y[start];
    v1[2] = Z[first] - Z[start];
    v2[0] = X[second] - X[start];
    v2[1] = Y[second] - Y[start];
    v2[2] = Z[second] - Z[start];
    
    ret[0] = v1[1] * v2[2] - v1[2] * v2[1];
    ret[1] = v1[2] * v2[0] - v1[0] * v2[2];
    ret[2] = v1[0] * v2[1] - v1[1] * v2[0];
    float length = sqrt(pow(ret[0],2)+pow(ret[1],2)+pow(ret[2],2));
    ret[0] /= length;
    ret[1] /= length;
    ret[2] /= length;
    return ret;
}

void Shape::setNormal() {
    for (int i = 0; i < numVertex; i++) {
        vector<float> temp = {0,0,0};
        int count = 0;
        for (int j = 0; j < numTriangle; j++){
            vector<float> crossProduct = {0,0,0};
            if((get<0>(triangles[j])-1) == i){
                crossProduct = cross(i, get<1>(triangles[j])-1, get<2>(triangles[j])-1);
                count++;
            }else if((get<1>(triangles[j])-1) == i){
                crossProduct = cross(i, get<2>(triangles[j])-1, get<0>(triangles[j])-1);
                count++;
            }else if((get<2>(triangles[j])-1) == i){
                crossProduct = cross(i, get<0>(triangles[j])-1, get<1>(triangles[j])-1);
                count++;
            }
            temp[0] += crossProduct[0];
            temp[1] += crossProduct[1];
            temp[2] += crossProduct[2];
        }
        float length = sqrt(pow(temp[0],2)+pow(temp[1],2)+pow(temp[2],2));
        temp[0] /= length;
        temp[1] /= length;
        temp[2] /= length;
        normals[i] = temp;
    }
}

tuple<float, float, float> Shape::getCentroid(){
    
    tuple<float,float,float> centroid;
    get<0>(centroid) = 0;
    get<1>(centroid) = 0;
    get<2>(centroid) = 0;
    
    if(numVertex == 0)
        return centroid;
    
    for(int i = 0; i < numVertex; i++){
        get<0>(centroid) += X[i];
        get<1>(centroid) += Y[i];
        get<2>(centroid) += Z[i];
    }
    get<0>(centroid) /= numVertex;
    get<1>(centroid) /= numVertex;
    get<2>(centroid) /= numVertex;
    
    return centroid;
}

void Shape::translate(float x, float y, float z){
    for(int i = 0; i < numVertex; i++){
        X[i] += x;
        Y[i] += y;
        Z[i] += z;
    }
}

void Shape::scale(float size){
    tuple<float,float,float> centroid = getCentroid();
    for(int i = 0; i < numVertex; i++){
        float x = (X[i] - get<0>(centroid)) * size + get<0>(centroid);
        float y = (Y[i] - get<1>(centroid)) * size + get<1>(centroid);
        float z = (Z[i] - get<2>(centroid)) * size + get<2>(centroid);
        X[i] = x;
        Y[i] = y;
        Z[i] = z;
    }
}

vector< vector<float> > matMult(vector< vector<float> > A, vector< vector<float> > B){
    vector< vector<float> > C (A.size(),vector<float>(A[0].size(),0));
    for(int i = 0; i < (int)A.size(); i++){
        for(int j = 0; j < (int)B[i].size();j++){
            for(int k=0; k < (int)A[i].size();k++){
                C[i][j] += A[i][k]*B[k][j];
            }
        }
    }
    return C;
}

void Shape::rotate(tuple<float,float,float> pt1, tuple<float,float,float> pt2, float degree){
    float x1 = get<0>(pt1);
    float y1 = get<1>(pt1);
    float z1 = get<2>(pt1);
    float x2 = get<0>(pt2);
    float y2 = get<1>(pt2);
    float z2 = get<2>(pt2);
    
    float d = sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) +(z2-z1)*(z2-z1));
    float a = (x2-x1) / d;
    float b = (y2-y1) / d;
    float c = (z2-z1) / d;
    d = sqrt(b*b + c*c);
    
    vector< vector<float> > M1 = {{1,0,0,(-1*x1)},{0,1,0,(-1*y1)},{0,0,1,(-1*z1)},{0,0,0,1}};
    vector< vector<float> > M2 = {{1,0,0,0},{0,(c/d),(-1*b/d),0},{0,(b/d),(c/d),0},{0,0,0,1}};
    vector< vector<float> > M3 = {{d,0,(-1*a),0},{0,1,0,0},{a,0,d,0},{0,0,0,1}};
    vector< vector<float> > M4 = {{cos(degree),-1*sin(degree),0,0},{sin(degree),cos(degree),0,0},{0,0,1,0},{0,0,0,1}};
    vector< vector<float> > M5 = {{d,0,a,0},{0,1,0,0},{(-1*a),0,d,0},{0,0,0,1}};
    vector< vector<float> > M6 = {{1,0,0,0},{0,(c/d),(b/d),0},{0,(-b/d),(c/d),0},{0,0,0,1}};
    vector< vector<float> > M7 = {{1,0,0,x1},{0,1,0,y1},{0,0,1,z1},{0,0,0,1}};
    
    vector< vector<float> > result = matMult(matMult(matMult(matMult(matMult(matMult(M7,M6), M5),M4),M3),M2),M1);
    
    for(int i = 0; i < numVertex; i++){
        vector< vector<float> > points = matMult(result, {{X[i]},{Y[i]},{Z[i]},{1}});
        X[i] = points[0][0];
        Y[i] = points[1][0];
        Z[i] = points[2][0];
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
            float x, y, z;
            input >> x >> y >> z;
            shape.X.push_back(x);
            shape.Y.push_back(y);
            shape.Z.push_back(z);
            shape.ndcX.push_back(x);
            shape.ndcY.push_back(y);
            shape.ndcZ.push_back(z);
        }
        input >> v;
        shape.numTriangle = v;
        for (int j = 0; j < v; j++) {
            int a,b,c;
            input >> a >> b >> c;
            tuple<int,int,int> tri{a,b,c};
            shape.triangles.push_back(tri);
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
    while(count < 14 && input >> temp){
        settings[count] = temp;
        count++;
    }
    count = 0;
    float t = 0;
    while(count < 18 && input >> t){
        lighting[count] = t;
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
        output << settings[8]<<endl;
        output << settings[9] << " " << settings[10] << endl;
        output << settings[11] << " " << settings[12] << " " << settings[13]<<endl;
        output << setprecision(3) << fixed;
        output << lighting[0] << " " << lighting[1] << " " << lighting[2]<<endl;
        output << lighting[3] << " " << lighting[4] << " " << lighting[5]<<endl;
        output << lighting[6] << " " << lighting[7] << " " << lighting[8]<<endl;
        output << lighting[9] << " " << lighting[10] << " " << lighting[11]<<endl;
        output << lighting[12]<<endl;
        output << lighting[13]<<endl;
        output << lighting[14]<<endl;
        output << lighting[15]<<endl;
        output << lighting[16] << " " << lighting[17] << " " << lighting[18];
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
                output << setprecision(3) << fixed << list[i].X[j] << "  " << list[i].Y[j] << "  " << list[i].Z[j]<< endl;
            }
            output << list[i].numTriangle <<endl;
            for(int j = 0; j < list[i].numTriangle; j++){
                output << get<0>(list[i].triangles[j]) << " " << get<1>(list[i].triangles[j]) << " " << get<2>(list[i].triangles[j]) << endl;
            }
            output<<endl;
        }
        output.close();
    }
}
