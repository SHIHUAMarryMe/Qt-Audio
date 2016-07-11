#ifndef POSITIONTOINT_H
#define POSITIONTOINT_H

#include <type_traits>
#include <ratio>
#include <tuple>

enum Direction : int{
    Left,
    Right,
    Up,
    Down
};

class QPoint;

template<unsigned int forX = 1, unsigned int forY = 1>
class Counter{
private:
    double width;
    double height;
    double dBRange;
    double FreqRange;

    std::ratio<forX, 1> ratioX;
    std::ratio<forY, 1> ratioY;
    std::pair<double, double> result;

public:

    inline Counter(const double& wid, const double& hei, const double& dB, const double& Freq)
             :width(wid),height(hei),dBRange(dB),FreqRange(Freq){}

    ~Counter()=default;

    Counter(const Counter<forX, forY>& other) = delete;
    Counter& operator=(const Counter<forX, forY>& other) = delete;

    std::pair<int, int>& operator()(const QPoint& startPoint, const QPoint& endPoint, const std::pair<Direction, Direction>& directions)noexcept
    {
        double sx = startPoint.x();
        double sy = startPoint.y();
        double ex = endPoint.x();    //让我想起了EXclibur.
        double ey = endPoint.y();

        if(directions.first == Direction::Left){
            (this->result).first = ((ex - sx)/(this->width))*(this->FreqRange); //得到的是一个负值.

        }else{
            (this->result).first = ((sx - ex)/(this->width))*(this->FreqRange); //得到一个正值.
        }

        if(directions.second == Direction::Up){
            (this->result).second = ((ey - sy)/(this->height))*(this->dBRange); //得到一个正值.

        }else{
            (this->result).second = ((sy - ey)/(this->height))*(this->dBRange); //得到一个右值.
        }

        return (this->result);

    }
};

class JundgeDirection{
public:
    JundgeDirection()=default;
    ~JundgeDirection()=default;

    std::pair<Direction, Direction> operator()(const QPoint& startPoint, const QPoint& endPoint)noexcept
    {
        std::pair<Direction, Direction> dirPair;
        if(startPoint.x() < endPoint.x()){
            dirPair.first = Direction::Left;

        }else{
            dirPair.first = Direction::Right;
        }

        if(startPoint.y() < endPoint.y()){
            dirPair.second = Direction::Up;

        }else{
            dirPair.second = Direction::Down;
        }

        return dirPair;
    }
};

#endif // POSITIONTOINT_H
