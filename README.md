# LZJ.criminisi.4


几个可以改变的参数：

1.头文件中 Patch类
static const int _length = 9; //正方形Patch的边长

2.calculate中 getSimilarVal函数的返回值

3.border中

void completion(Patch & prior) 
{
int chDist = 100 * prior.infoEdge() + 7;//参数;
这个为搜索范围，换为很大很大即进行全局搜索

4.border中 Patch类构造函数中 confidence计算方法
以及在completion中的 更新值 newConfidence




待解决的问题：
1.边缘处理
2.估值
3.修补顺序



//【src上有部分图像是残缺的，如何计算边界点的梯度？！】

//传参留白范围blank？
void initBond(set<BondPoint> & points, multiset<Patch> &priorLevel)


17:21 2015/6/6