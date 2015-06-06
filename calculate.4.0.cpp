#include "header.4.0.h"

const double sqrt2 = std::sqrt(2.0);
const double operatorX[3][3] = {
    {-1, 0, 1},
    {-sqrt2, 0, sqrt2},
    {-1, 0, 1}
};
const double operatorY[3][3] = {
    {-1, -sqrt2, -1},
    {0, 0, 0},
    {1, sqrt2, 1}
};

double calcGrad(int x,int y) //返回值为0~1
{
    //传参为Patch的中心点，默认不会出界
    
    Mat srcROI(src,Rect(y-1,x-1,3,3)); //注意坐标反向变化！
    Mat maskROI(mask,Rect(y-1,x-1,3,3));
    
    Mat graySrc;
    cvtColor(srcROI, graySrc, CV_BGR2GRAY);
    
    //【src上有部分图像是残缺的，如何计算边界点的梯度？！】
    
    uchar center = 0;
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
            if(maskROI.at<uchar>(i,j) == 0) //找一个原有图形点
            {
                center = graySrc.at<uchar>(i,j);
                break;
            }
    }
    
    double gx = 0;
    double gy = 0;
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            if(maskROI.at<uchar>(i,j) == 0) //原有图像区域
            {
                gx += graySrc.at<uchar>(i,j) * operatorX[i][j]; //注意是计算gray图的
                gy += graySrc.at<uchar>(i,j) * operatorY[i][j];
            }
            else
            {
                gx += center * operatorX[i][j];
                gy += center * operatorY[i][j];
            }
        }
    }
    //考虑值比较小，故没有使用衰减因子
    //gx /= 4 + 2 * sqrt2;
    //gy /= 4 + 2 * sqrt2;
    double grad = std::sqrt(gx * gx + gy * gy);
    grad = 255 < grad? 255 : grad;
    return grad / 255; //归一化
}


double getSimilarVal(int x,int y,Patch & prior)
{
    int length = Patch::_length;
    
    double dis = 0;
    
    for(int i = 0; i < length; i++)
        for(int j = 0; j < length; j++)
        {
            if(mask.at<uchar>(prior._x + i,prior._y + j) == 255) //未知区域
            {
                if(mask.at<uchar>(x + i,y + j) == 255) //未知区域
                    return -1;
                continue;
            }
            
            for (int k = 0; k < 3; k++)
            {
                double tmp = src.at<Vec3b>(x + i,y + j)[k] - src.at<Vec3b>(prior._x + i,prior._y + j)[k];
                dis += tmp*tmp;
            }
            
        }
    
    //计算方差
    double averPrior[3] = {0},averMatch[3] = {0};
    int cntPrior = 0,cntMatch = 0;
    
    for(int i = 2; i < 7; i++)
        for(int j = 2; j < 7; j++)
        {
            if(mask.at<uchar>(x + i,y + j) == 0)
            {
                for(int k = 0; k < 3; k++)
                    averMatch[k] += src.at<Vec3b>(x + i,y + j)[k];
                cntMatch++;
            }
            
            if(mask.at<uchar>(prior._x + i,prior._y + j) == 0)
            {
                for(int k = 0; k < 3; k++)
                    averPrior[k] += src.at<Vec3b>(prior._x + i,prior._y + j)[k];
                cntPrior++;
            }
        }
    for(int k = 0; k < 3; k++)
    {
        averMatch[k] /= cntMatch;
        averPrior[k] /= cntPrior;
    }
    
    double resMatch[3] = {0},resPrior[3] = {0};
    
    for(int i = 2; i < 7; i++)
        for(int j = 2; j < 7; j++)
        {
            if(mask.at<uchar>(x + i,y + j) == 0)
            {
                for(int k = 0; k < 3; k++)
                {
                    double tmp = src.at<Vec3b>(x + i,y + j)[k] - averMatch[k];
                    resMatch[k] += tmp * tmp;
                }
            }
            
            if(mask.at<uchar>(prior._x + i,prior._y + j) == 0)
            {
                for(int k = 0; k < 3; k++)
                {
                    double tmp = src.at<Vec3b>(prior._x + i,prior._y + j)[k] - averPrior[k];
                    resPrior[k] += tmp * tmp;
                } 
            }   
        }
    
    double sMatch = 0;
    double sPrior = 0;
    for(int k = 0; k < 3; k++)
    {
        sMatch += std::sqrt(resMatch[k] / cntMatch);
        sPrior += std::sqrt(resPrior[k] / cntPrior);
    }
    
    return dis + abs(sMatch/3 - sPrior/3);
}