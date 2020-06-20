#ifndef STONE_H
#define STONE_H

#include <QString>

/**
 * @brief The Stone class
 * 棋盘一共10行 9列
 * 棋子编号这样记录：上下正好相反对应
 * 0 1 2 3 ... 8
 *
 *   9  10
 *
 * 11 12 ... 15
 *
 * 15 ... 12 11
 *
 *   10  9
 *
 * 8 ... 3 2 1 0
 */
class Stone
{
public:
    Stone();

    enum TYPE{CHE,MA,XIANG,SHI,JIANG,PAO,BING};

    int _id;//编号
    bool _dead;
    bool _red;//红黑方
    int _row,_col;//行列号 相对于32个棋子，不是相对于棋盘 从0行0列开始

    TYPE _type;//类型

    //棋子初始化
    void init(int i)
    {
        struct {
            int x,y;
            Stone::TYPE t;
        }pos[16] = {
        {0,0,CHE},
        {0,1,MA},
        {0,2,XIANG},
        {0,3,SHI},
        {0,4,JIANG},
        {0,5,SHI},
        {0,6,XIANG},
        {0,7,MA},
        {0,8,CHE},

        {2,1,PAO},
        {2,7,PAO},

        {3,0,BING},
        {3,2,BING},
        {3,4,BING},
        {3,6,BING},
        {3,8,BING}
        };

        _id = i;
        _dead = false;
        _red = (i<16);

        if(i < 16)
        {
            _row = pos[i].x;
            _col = pos[i].y;
            _type = pos[i].t;
        }
        else
        {
            _row = 9 - pos[i-16].x;
            _col = 8 - pos[i-16].y;
            _type = pos[i-16].t;
        }
    }

    //根据类型获取棋子的文本
    QString getText()
    {
        switch (this->_type) {
        case CHE:
            return "车";
        case MA:
            return "马";
        case PAO:
            return "炮";
        case XIANG:
            return "象";
        case SHI:
            return "士";
        case JIANG:
            return "将";
        case BING:
            return "兵";
        default:
            return "*";
        }
    }
};

#endif // STONE_H
