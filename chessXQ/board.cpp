#include "board.h"
#include <QDebug>

Board::Board(QWidget *parent)
    : QWidget(parent)
{
    _r = 20;

    for(int i = 0;i<32;++i)
    {
        _s[i].init(i);
    }
    _selectId = -1;
}

Board::~Board()
{

}

void Board::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    int d = 2*_r;
    //绘制10条横线
    for(int i = 1;i<=10;++i)
    {
        painter.drawLine(d,d*i,9*d,i*d);
    }

    //绘制9条竖线
    for(int i = 1;i<=9;++i)
    {
        if(i==1 || i==9)
            painter.drawLine(i*d,d,i*d,10*d);
        else
        {
            painter.drawLine(i*d,d,i*d,5*d);
            painter.drawLine(i*d,6*d,i*d,10*d);
        }
    }
    painter.drawText(QRect(3*d,5*d,2*d,d),Qt::AlignCenter,"楚河");
    painter.drawText(QRect(5*d,5*d,2*d,d),Qt::AlignCenter,"汉界");

    //绘制九宫格
    painter.drawLine(4*d,d,6*d,3*d);
    painter.drawLine(4*d,3*d,6*d,d);
    painter.drawLine(4*d,8*d,6*d,10*d);
    painter.drawLine(4*d,10*d,6*d,8*d);

    //绘制棋子
    for(int i = 0;i<32;++i)
    {
        drawStone(painter,i);
    }
}

void Board::drawStone(QPainter &painter, int id)
{
    //棋子已死亡，不需要绘制
    if(_s[id]._dead)
        return;

    //画圆
    //获取圆心点
    QPoint p = center(id);

    //根据棋子选中状态，设置画刷
    if(id == _selectId)
        painter.setBrush(Qt::darkYellow);
    else
        painter.setBrush(Qt::yellow);
    painter.setPen(Qt::black);
    painter.drawEllipse(p,_r,_r);

    //写文字
    painter.setFont(QFont("MS Shell Dlg 2",_r));//字体大小为_r棋子半径
    if(id < 16)
        painter.setPen(Qt::red);
    QRect rect(p.x()-_r,p.y()-_r,_r*2,_r*2);
    painter.drawText(rect,Qt::AlignCenter,_s[id].getText());
}
//根据棋子id  获取其初始坐标点
QPoint Board::center(int id)
{
    return center(_s[id]._row,_s[id]._col);
}

QPoint Board::center(int row, int col)
{
    //行相当于坐标系中的y,列相当于x 并且因为棋子行列从0记录，因此为与坐标系相对应要+1
    QPoint p((col+1)*2*_r,(row+1)*2*_r);
    return p;
}

bool Board::getRowCol(QPoint p, int &row, int &col)
{
    int x = p.x();
    int y = p.y();
    //遍历整个棋盘，获取有效位置
    for(row = 0;row<10;row++)
    {
        for(col = 0;col<9;col++)
        {
            int disc = ((col+1)*2*_r-x)*((col+1)*2*_r-x)+
                    ((row+1)*2*_r-y)*((row+1)*2*_r-y);
            if(disc <= _r*_r)
                return true;
        }
    }
    return false;
}

bool Board::canMove(int selectId, int clickId, int row, int col)
{
    if(clickId != -1)//若点击的位置有棋子 判断两颗棋子是否同一个颜色
    {
        bool r = isSameColor(selectId,clickId);
        if(r)
            return false;
    }
    //根据不同棋子类型，进行规则判断
    switch (_s[selectId]._type)
    {
        case Stone::CHE:
             return canMoveChe(selectId,row,col);
        case Stone::MA:
            return canMoveMa(selectId,row,col);
        case Stone::XIANG:
            return canMoveXiang(selectId,row,col);
        case Stone::SHI:
            return canMoveShi(selectId,row,col);
        case Stone::JIANG:
            return canMoveJiang(selectId,clickId,row,col);
        case Stone::PAO:
            return canMovePao(selectId,clickId,row,col);
        case Stone::BING:
            return canMoveBing(selectId,row,col);
    }
    return false;
}

bool Board::isSameColor(int selectId, int clickId)
{
    if(_s[clickId]._dead)//判断棋子是否已死
        return false;
    if(_s[selectId]._red == _s[clickId]._red)//如果是同一个角色
        return true;
    return false;
}

int Board::getStoneCountAtLine(int selectId, int row, int col)
{
    //获取selectId对应的row col
    int row1 = _s[selectId]._row;
    int col1 = _s[selectId]._col;
    //比较
    if(row1 != row && col1 != col)//说明选中的和点击的棋子不在同一行 也不在同一列 肯定不能移动
        return -1;
    if(row1 == row && col1 == col)//说明选中的和点击的棋子是同一个 那也不能移动
        return -1;
    if(row1 == row)//在同一行 那就比较列
    {
        int max = (col1 > col) ? col1 : col;
        int min = (col1 < col) ? col1 : col;
        int count = 0;//用来记录selectId和clickId之间的棋子数
        for(min = min+1;min <= max-1;min++)
        {
            if(getStoneId(row,min) != -1)//说明他们之间有棋子
            {
                count++;
                if(count > 1)
                    break;//如果它们之间的棋子数超过1 ，那么不管车还是炮 都不能走 直接break 省的浪费时间
            }
        }
        return count;
    }
    else//在同一列
    {
        int max = (row1 > row) ? row1 : row;
        int min = (row1 < row) ? row1 : row;
        int count = 0;
        for(min = min+1;min <= max-1;min++){
            if(getStoneId(min,col) != -1)
            {
                count++;
                if(count > 1)
                    break;
            }
        }
        return count;
    }
}

int Board::getStoneId(int row, int col)
{
    for(int i=0;i<32;i++)
    {
        if(_s[i]._row == row && _s[i]._col == col && _s[i]._dead == false)
            return i;
    }
    return -1;
}

bool Board::canMoveChe(int selectId, int row, int col)
{
    int num = getStoneCountAtLine(selectId,row,col);
    if(num == 0)
        return true;
    return false;
}

/**
 * 马的走棋公式：qAbs(r1-r)*10 + qAbs(c1-c) 等于12或21 r和c分别表示row col
 * 象的走棋公式：qAbs(r1-r)*10 + qAbs(c1-c) 等于22
 * 将的走棋公式：qAbs(r1-r)*10 + qAbs(c1-c) 等于10或1
 * 兵的走棋公式：qAbs(r1-r)*10 + qAbs(c1-c) 等于10或1
 * 士的走棋公式：qAbs(r1-r)*10 + qAbs(c1-c) 等于11
 */
bool Board::canMoveMa(int selectId, int row, int col)
{
    int row1 = _s[selectId]._row;
    int col1 = _s[selectId]._col;
    int value = relation(row1,col1,row,col);
    if(value !=12 && value != 21)
        return false;
    if(value == 12)
    {
        //憋了马腿的情况
        if(getStoneId(row1,(col1 + col)/2) != -1)
            return false;
    }
    else if(value == 21)
    {
        //憋了马腿的情况
        if(getStoneId((row1 + row)/2,col1) != -1)
            return false;
    }
    return true;
}

bool Board::canMoveXiang(int selectId, int row, int col)
{
    int row1 = _s[selectId]._row;
    int col1 = _s[selectId]._col;
    int value = relation(row1,col1,row,col);
    if(value != 22)
        return false;
    if(getStoneId((row1 + row)/2,(col1 + col)/2) != -1)//堵象眼了
        return false;
    qDebug() << selectId << " " << row;
    //不能过河
    if(selectId < 16)//红方
    {
        if(row > 4)
            return false;
    }
    else//黑方
    {
        if(row < 5)
            return false;
    }
    return true;
}

bool Board::canMoveShi(int selectId, int row, int col)
{
    int row1 = _s[selectId]._row;
    int col1 = _s[selectId]._col;
    int value = relation(row1,col1,row,col);
    if(value != 11)
        return false;
    //不管黑方还是红方 列都必须在3-5之间
    if(col < 3 || col >5)
        return false;
    //判断row的取值
    if(selectId < 16)//红方
    {
        if(row > 2)
            return false;
    }
    else
    {
        if(row < 7)
            return false;
    }
    return true;
}

bool Board::canMoveJiang(int selectId, int clickId, int row, int col)
{
    int row1 = _s[selectId]._row;
    int col1 = _s[selectId]._col;
    //老将对面 可以对杀  若要杀的是将 并且对方没死 并且两个将之间的棋子数为0 满足条件
    if(_s[clickId]._type == Stone::JIANG && _s[clickId]._dead == false
            && getStoneCountAtLine(selectId,row,col) == 0)
        return true;

    int value = relation(row1,col1,row,col);
    if(value != 10 && value != 1)
        return false;
    //不管黑方还是红方 列都必须在3-5之间
    if(col < 3 || col >5)
        return false;
    //判断row的取值
    if(selectId < 16)//红方
    {
        if(row > 2)
            return false;
    }
    else
    {
        if(row < 7)
            return false;
    }
    return true;
}

bool Board::canMovePao(int selectId, int clickId, int row, int col)
{
    int num = getStoneCountAtLine(selectId,row,col);
    //之间没有棋子且为走棋 或者有一个棋子且为吃子
    if((num == 0 && clickId == -1) || (num == 1 && clickId != -1))
        return true;
    return false;
}

bool Board::canMoveBing(int selectId, int row, int col)
{
    int row1 = _s[selectId]._row;
    int col1 = _s[selectId]._col;
    int value = relation(row1,col1,row,col);
    if(value != 10 && value != 1)
        return false;

    //不能后退 并且过河以后才能左右移动
    if(selectId < 16)//红方
    {
        if(row1 < 5)//没过河
            if(row == row1) return false;
        if(row < row1)  return false;
    }
    else//黑方
    {
        if(row1 >= 5)//没过河
            if(row == row1)    return false;
        if(row > row1)  return false;
    }
    return true;
}

//获取关系值
int Board::relation(int row1, int col1, int row, int col)
{
    return qAbs(row1-row)*10 + qAbs(col1-col);
}

void Board::mouseReleaseEvent(QMouseEvent *e)
{
    //1.获取鼠标点击位置
    QPoint p = e->pos();

    //2.获取点击位置在棋盘里面的行列号
    int row,col;
    bool bRet = getRowCol(p,row,col);
    if(!bRet)
        return;
    //点到了棋盘上的有效位置
    //3.获取指定行列上棋子的id
    int clickId = -1;
    int i = -1;
    if((i = getStoneId(row,col)) != -1)
    {
        clickId = i;
    }
    //点击的位置是否有棋子
    //4.相关处理
    //如果之前没有选中棋子，并且点击的位置有棋子，那么seletId就是clickId
    if(_selectId == -1)
    {
        if(clickId != -1)//点击了一个棋子
        {
            _selectId = clickId;
            update();
        }
        //else 就是没有点到棋子
    }
    else //之前已经选中了一颗棋子
    {
        bool ret = canMove(_selectId,clickId,row,col);//判断是否能移动
        if(!ret) //不能移动
        {
            if(clickId == -1)//说明点击的位置没有棋子且不能走棋 那就把选中的棋子置不选中
                ;//_selectId = -1;
            else//点击的位置有棋子且不能吃子
                _selectId = clickId;
            update();
            return;
        }

        //走棋
        if(clickId == -1)
        {
            _s[_selectId]._row = row;
            _s[_selectId]._col = col;
            _selectId = -1;
            update();
        }
        //吃子
        else
        {
            _s[clickId]._dead = true;
            _s[_selectId]._row = row;
            _s[_selectId]._col = col;

            _selectId = -1;
            update();
        }
    }
}
