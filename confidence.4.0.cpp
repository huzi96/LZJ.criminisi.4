#include "header.4.0.h"


Confidence::Confidence():_row(0),_col(0),_val(NULL){}

void Confidence::showImg() const
{
    Mat show(_row,_col,0,cv::Scalar(0));
    
    cv::Mat_<uchar>::iterator it = show.begin<uchar>();
    
    for(int i = 0; i < _row * _col; i++,it++)
    {
        *it = 255 * (*(_val+i));
    }
    
    cv::imshow("Confidence",show);
    waitKey(0);
    return;
}

void Confidence::init(const cv::Mat & mask)
{
    if(_val != NULL)
        delete []_val;
    
    _row = mask.rows;
    _col = mask.cols;
    
    _val = new double[_row * _col];
    memset(_val,0,sizeof(0));
    
    cv::Mat_<uchar>::const_iterator it = mask.begin<uchar>();
    cv::Mat_<uchar>::const_iterator itEnd = mask.end<uchar>();
    
    double * ptrVal = _val;
    
    for(;it != itEnd; it++, ptrVal++)
    {
        if(*it == 255) //被选中
            *ptrVal = 0;
        else
            *ptrVal = 1;
    }
}

double * Confidence::operator[](int r) const
{
    return _val + r*_col;
}

Confidence::~Confidence()
{
    if(_val != NULL)
        delete [] _val;
}

double Confidence::getAverConfidence(int x,int y,int length) const
{
    double res = 0;
    for(int i = 0; i < length; i++)
    {
        double * ptr = _val + (x+i)*_col + y;
        for(int j = 0; j < length; j++, ptr++)
            res += *ptr;
    }
    
    res /= (length*length); //均值
    return res;
}
