#include "header.4.0.h"

//功能：对选中区域边界处理



Patch::Patch(const BondPoint & point):_x(point._x - _length/2),_y(point._y - _length/2)
{
    //double grad = calcGrad(point._x,point._y); //计算中心点梯度，返回值为0~255
    
    double grad = 0; //暂定取5*5方格中的最大梯度值
    for(int i = -2; i < 3; i++)
        for(int j = -2; j < 3; j++)
        {
            if(mask.at<uchar>(point._x + i,point._y + j) == 255) //剪枝
                continue;
            double tmp = calcGrad(point._x + i,point._y + j); //已经归一化
            if(tmp > grad)
                grad = tmp;
        }
    _infoEdge = std::exp(grad*grad - 1);
    //cout << _infoEdge << endl;
    
    _averConfidence = confidence.getAverConfidence(_x,_y,_length);
    
    _priorVal = 0.4 * _averConfidence + 0.6 * _infoEdge;
}

//传参留白范围blank？
void initBond(set<BondPoint> & points, multiset<Patch> &priorLevel)
{
    points.clear();
    priorLevel.clear();
    
    Mat showRes(mask.rows,mask.cols,mask.type(),cv::Scalar(0)); //showRes
    
    int row = mask.rows;
    int col = mask.cols;
    int blank = Patch::_length / 2 + 3; //留白范围
    
    for(int i = blank; i < row - blank; i++)
        for(int j = blank; j < col - blank; j++)
        {
            if(mask.at<uchar>(i,j) == 0)
                continue;
            
            //255为被选中区域
            if(!mask.at<uchar>(i-1,j) || !mask.at<uchar>(i+1,j)
               || !mask.at<uchar>(i,j-1) || !mask.at<uchar>(i,j+1))
            {
                showRes.at<uchar>(i,j) = 255; //showRes
                
                BondPoint point(i,j);
                
                multiset<Patch>::iterator iter = priorLevel.insert(Patch(point));
                point.setPtr(iter);
                points.insert(point);
            }
        }
    
    imshow("boundary",showRes);
    waitKey(0);
}

void renewBond(set<BondPoint> & points,multiset<Patch> & priorLevel,Patch & prior)
{
    set<BondPoint>::iterator iterPoint;
    multiset<Patch>::iterator iterPatch;
    
    int beginX = prior._x - 1,endX = prior._x + Patch::_length,
    beginY = prior._y - 1,endY = prior._y + Patch::_length;
    
    for(int i = beginX; i <= endX; i++)
        for(int j = beginY; j <= endY; j++)
        {
            if(i == beginX || i == endX || j == beginY || j == endY)
            {
                if(mask.at<uchar>(i,j) == 255) //是新加入的边界
                {
                    BondPoint point(i,j);
                    iterPoint = points.find(point);
                    if(iterPoint == points.end()) //未曾被加入进边界
                    {
                        iterPatch = priorLevel.insert(point);
                        point.setPtr(iterPatch);
                        points.insert(point);
                    };
                }
                continue;
            }
            
            if(mask.at<uchar>(i,j) == 255) //已被更新
            {
                mask.at<uchar>(i,j) = 0;
                
                BondPoint point(i,j);
                iterPoint = points.find(point);
                if(iterPoint != points.end()) //找到
                {
                    priorLevel.erase(iterPoint->_ptr);
                    points.erase(iterPoint);
                }
            }
        }
    
    //cv::imshow(">",mask);
    //cv::waitKey(0);
    //cv::imshow("src",src);
    //cv::waitKey(0);
}

void completion(Patch & prior)
{
    int chDist = 100 * prior.infoEdge() + 7;//参数;
    
    //搜索范围(记录将匹配框的左上角点范围)
    int xZeroROI = prior._x - chDist;
    if (xZeroROI < 0) xZeroROI = 0; //之后要再取5*5的方格
    int yZeroROI = prior._y - chDist;
    if (yZeroROI < 0) yZeroROI = 0;
    
    int rowROI = (prior._x + chDist + Patch::_length) < src.rows - 2 ? chDist : src.rows - Patch::_length - prior._x - 2; //搜索尺寸
    int colROI = (prior._y + chDist + Patch::_length) < src.cols - 2 ? chDist : src.cols - Patch::_length - prior._y - 2;
    
    
    //将prior用范围内的最匹配点替换.并更新confidence,src
    double matchBest = 1e30;
    int xBest = 0,yBest = 0;
    
    for (int i = xZeroROI; i < xZeroROI + rowROI; i++) //枚举左上角
        for (int j = yZeroROI; j < yZeroROI + colROI; j++)
        {
            double matchVal  = getSimilarVal(i,j,prior);
            //matchVal表示两图的距离
            
            if(matchVal < 0) //剪枝
                continue;
            
            if (matchVal < matchBest) //取最小的matchVal
            {
                matchBest = matchVal;
                xBest = i, yBest = j;//记录左上角点;
            }
        }
    
    //confidence计算时记得转换到中间点
    double newConfidence = confidence.getAverConfidence(xBest + Patch::_length/2,yBest + Patch::_length/2,Patch::_length) * std::exp(-matchBest*matchBest);
    //cout << confidence.getAverConfidence(xBest + Patch::_length/2,yBest + Patch::_length/2,Patch::_length) << endl;
    //cout << "matchBest" << matchBest << endl;
    //cout << "newConficence" << newConfidence << endl;
    //cout << matchBest << endl;
    
    //填充该点，并更新置信度
    for (int i = prior._x; i < prior._x + Patch::_length; i++)
        for (int j = prior._y; j < prior._y + Patch::_length; j++)
        {
            if (mask.at<uchar>(i,j) == 255)
            {
                //更新src
                src.at<cv::Vec3b>(i,j) = src.at<cv::Vec3b>(xBest + i - prior._x, yBest + j - prior._y);
                confidence[i][j] = newConfidence;
            }
            //更新confidence
        }
    
    //Mat maskROI(src,Rect(yBest,xBest,Patch::_length,Patch::_length));
    //cv::imshow("maskROI",maskROI);
    //cv::waitKey(0);
}