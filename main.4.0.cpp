#include "header.4.0.h"

Mat src;
//src(source)记录原图
Mat mask;
//mask 单通道矩阵,255为待填补区域,0为已知区域
Confidence confidence;
//confidence 记录每一点信任值(0~1)

void test();

int main()
{
    src = cv::imread("test3.jpg");
    if(!src.data)
    {
        cout << "read error." << endl;
        system("pause");
        return 1;
    }
    //以上需要封装
    
    mask = Mat(src.rows, src.cols, CV_8UC1, cv::Scalar(0));
    //建立一个原图大小的无符号单通道图像矩阵
    chooseMask(mask,src,20); //在src上选中相应区域,输出至mask,255为待填补区域,0为已知区域
    //第三个参数为选择点半径
    
    //mask = cv::imread("333mask.png",0);
    
    confidence.init(mask);
    confidence.showImg();
    
    ////////////////////////////////////
    //testGrad(); //测试使用
    //ConfidenceTest(confidence); //测试
    ////////////////////////////////////
    
    
    set<BondPoint> points; //边界点集合(按x,y排序)
    multiset<Patch> priorLevel; //边界点集合(按优先级排序) 且优先值有可能相同,用multiset
    
    initBond(points,priorLevel); //初始化边界
    
    while(!points.empty())
    {
        //cout << "size " << points.size() << endl;
        
        Patch prior = *priorLevel.begin();
        
        completion(prior);
        //修补优先级最高的Patch,并更新confidence
        
        renewBond(points,priorLevel,prior);
        //更新mask,points,priorLevl
    }
    
    cv::imshow("result",src);
    cv::waitKey(0);
    
    return 0;
}