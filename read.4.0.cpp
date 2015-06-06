#include "header.4.0.h"

//功能：与用户对接，载入图片，选择修补区域


//鼠标事件调用函数，用于用鼠标在param指向的原图上绘制黑色色块，同时在全局的mask上绘制255的区域
void onMouse(int event, int x, int y, int flags, void * Param)
{
    static int Radius = *(int *)Param;
    
    static bool s_bMoushowMaskButtonDown = false;//是否已按下左键，用来检测拖动
    static Point s_cvPrePoint = Point(0, 0);
    switch (event)
    {
        case CV_EVENT_LBUTTONDOWN://OpenCV highgui部分
            s_bMoushowMaskButtonDown = true;
            s_cvPrePoint = Point(x, y);//记录左键按下时的坐标，即直线起点
            break;
            
        case  CV_EVENT_LBUTTONUP:
            s_bMoushowMaskButtonDown = false;
            break;
            
        case CV_EVENT_MOUSEMOVE:
            if (s_bMoushowMaskButtonDown)
            {
                Point cvCurrPoint = Point(x, y);//记下鼠标移动时坐标
                //在开始按下鼠标的地方到开始移动的地方画一条直线
                //事实上由于只有移动一点点就会画直线，画出的直线是一个微分,所以相当于画曲线
                int thickness = Radius; //【半径】
                line(src, s_cvPrePoint, cvCurrPoint, Scalar(255, 255, 255), thickness);//在原图上画白线
                line(mask, s_cvPrePoint, cvCurrPoint, Scalar(255), thickness);//在mask上画白线
                s_cvPrePoint = cvCurrPoint;//更新下一次绘制时的起始坐标为现在的终止坐标
                imshow("source",src);//实时更新屏幕显示
            }
            break;
    }
}

//选择修补区域
void chooseMask(Mat & mask, Mat & src, int radius)
{
    namedWindow("source"); //在该窗口上绘制需填补部分(mask)
    imshow("source", src);
    
    int param = radius; //单次选择区域的半径
    setMouseCallback("source", onMouse, &param);//鼠标事件监测(结果输出至mask)
    //param为传给onMouse函数的最后一个参数
    
    waitKey(0);
    imshow("mask", mask);
    waitKey(0);
}