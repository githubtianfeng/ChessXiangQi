#ifndef BOARD_H
#define BOARD_H

#include <QWidget>
#include <QPaintEvent>
#include "stone.h"
#include <QPainter>
#include <QMouseEvent>

class Board : public QWidget
{
    Q_OBJECT

public:
    Board(QWidget *parent = 0);
    ~Board();

private:
    //棋子半径
    int _r;

    Stone _s[32];//棋子

    int _selectId;//当前选中的棋子
public:
    void drawStone(QPainter &painter,int id);

    QPoint center(int id);//根据id获取中心点

    QPoint center(int row,int col);//根据行列号获取中心点

    //获取坐标系指定点，在棋盘里面的行列号
    bool getRowCol(QPoint p,int &row,int &col);

    bool canMove(int selectId, int clickId, int row, int col);

    //判断是否同一颜色
    bool isSameColor(int selectId,int clickId);

    //判断在一条线上selectId和clickId之间有几颗棋子
    int getStoneCountAtLine(int selectId,int row,int col);

    //判读row col位置是否有棋子
    int getStoneId(int row,int col);

    bool canMoveChe(int selectId,int row,int col);
    bool canMoveMa(int selectId,int row,int col);
    bool canMoveXiang(int selectId,int row,int col);
    bool canMoveShi(int selectId,int row,int col);
    bool canMoveJiang(int selectId,int clickId,int row,int col);
    bool canMovePao(int selectId,int clickId,int row,int col);//炮需要clickId进行联合判断走棋还是吃子
    bool canMoveBing(int selectId,int row,int col);
    int relation(int row1,int col1,int row,int col);
protected:
    void paintEvent(QPaintEvent*);//重写绘图事件
    void mouseReleaseEvent(QMouseEvent*);
};

#endif // BOARD_H
