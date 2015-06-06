#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/gpu/gpu.hpp>
#include<iostream>
#include<string>
#include<cstdio>
#include<cstring>
#include<set>
#include<fstream>
#include<algorithm>
#include<cmath>

using namespace std;
using namespace cv;

class Confidence;
class Patch;
class BondPoint;

extern Mat src;
extern Mat mask;
extern Confidence confidence;

class Confidence //信任度
{
public:
    Confidence();
    ~Confidence();
    
    void init(const cv::Mat & mask);
    void showImg() const;
    double * operator[](int) const;
    double getAverConfidence(int x,int y,int length) const;
private:
    Confidence(const Confidence &){} //禁止公有调用复制构造函数
    
    int _row;
    int _col;
    double * _val;
};

class BondPoint
{
public:
    int _x;
    int _y;
    multiset<Patch>::iterator _ptr; //指向其所在priorLevel位置的迭代器
    
    BondPoint(int x,int y):_x(x),_y(y){}
    
    void setPtr(multiset<Patch>::iterator iter){ _ptr = iter; }
    
    bool operator < (const BondPoint & p) const
    {
        if(_x == p._x)
            return _y < p._y;
        else
            return _x < p._x;
    }
};

class Patch
{
public:
    //记录Patch左上角点坐标
    const int _x;
    const int _y;
    static const int _length = 9; //正方形Patch的边长
    
    Patch(const BondPoint & point);
    
    inline double infoEdge() { return _infoEdge; }
    
    bool operator < (const Patch & p) const
    {
        return _priorVal > p._priorVal;
    }
    
private:
    double _priorVal; //优先值 P(p) = 0.4*C(p) + 0.6*E(p) 取值为0~1
    double _infoEdge;  // E(p) = e^(d^2 - 1) 强边缘信息 取值为0~1
    double _averConfidence; //均信任值C(p) 取值为0~1
};

void chooseMask(Mat & mask, Mat & src, int radius = 20);
void initBond(set<BondPoint> & points, multiset<Patch> &priorLevel);
double calcGrad(int x,int y);
void completion(Patch & prior);
void renewBond(set<BondPoint> & points,multiset<Patch> & priorLevel,Patch & prior);
void completion(Patch & prior);
double getSimilarVal(int i,int j, Patch & prior);

void testGrad();
void ConfidenceTest(Confidence & c);