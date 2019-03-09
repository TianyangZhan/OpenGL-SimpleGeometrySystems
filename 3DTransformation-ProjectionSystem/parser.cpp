#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <tuple>
#include "parser.h"

using namespace std;

// ******************* Shape Class *******************

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
            shape.oblX.push_back(x);
            shape.oblY.push_back(y);
            shape.oblZ.push_back(z);
        }
        input >> v;
        shape.numEdge = v;
        for (int j = 0; j < v; j++) {
            int a,b;
            input >> a >> b;
            pair<int,int> edge = make_pair(a,b);
            shape.edges.push_back(edge);
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
                output << setprecision(3) << fixed << list[i].X[j] << "  " << list[i].Y[j] << "  " << list[i].Z[j] << endl;
            }
            output << list[i].numEdge <<endl;
            for(int j = 0; j < list[i].numEdge; j++){
                output << list[i].edges[j].first << " " << list[i].edges[j].second << endl;
            }
            output<<endl;
        }
        output.close();
    }
}
