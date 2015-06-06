#include "header.4.0.h"

void testGrad() //返回值为0~1
{
    //传参为Patch的中心点，默认不会出界
    
    Mat showRes(mask.rows,mask.cols,mask.type(),cv::Scalar(0)); //showRes
    
    //【src上有部分图像是残缺的，如何计算边界点的梯度？！】
    
    for(int r = 5; r < mask.rows - 5; r++)
        for(int c = 5; c < mask.cols - 5; c++)
        {
            //showRes.at<uchar>(r,c) = calcGrad(r,c) * 255;
            
            if(mask.at<uchar>(r,c) == 255)
            {
                double grad = 0;
                for(int i = -2; i < 3; i++)
                    for(int j = -2; j < 3; j++)
                    {
                        if(mask.at<uchar>(r+i,c+j) == 255) //剪枝
                            continue;
                        double tmp = calcGrad(r + i,c + j) * 255;
                        if(tmp > grad)
                            grad = tmp;
                    }
                showRes.at<uchar>(r,c) = grad;
                if(grad != 0)
                    cout << r << " " << c << " " << grad << endl;
            }
            
        }
    
    Mat graySrc;
    cvtColor(src, graySrc, CV_BGR2GRAY);
    cv::imshow("graySrc",graySrc);
    cv::waitKey(0);
    
    cv::imshow("grad",showRes);
    cv::waitKey(0);
    
    cv::imshow("src",src);
    cv::waitKey(0);
}


void ConfidenceTest(Confidence & c)
{
    //c.showImg();
    
    int size = Patch::_length;
    
    for(int x = 20; x < mask.rows - 20; x++)
        for(int y = 20; y < mask.cols - 20; y++)
        {
            
            double tt = c.getAverConfidence(x,y,size);
            
            double res = 0;
            for(int i = 0; i < size; i++)
                for(int j = 0; j < size; j++)
                    res += c[i+x][j+y];
            res /= size*size;
            
            if(tt != res)
                cout << "no" << endl;
        }
    
    return;
}
