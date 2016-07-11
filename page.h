#ifndef OUTPAGE_H
#define OUTPAGE_H

#include <QWidget>
#include <memory>
#include <vector>
#include <map>
#include <QHBoxLayout>
#include <typeinfo>
#include <QLineEdit>
#include <QComboBox>
#include <deque>
#include <QDebug>

class QPainter;
class QBrush;
class QPen;
class QRectF;
class QHBoxLayout;
class QLineEdit;
class QLabel;
class QLineEdit;
class QComboBox;
class QPixmap;
class QDrag;
class QByteArray;
class QCheckBox;
class QSlider;
class QPushButton;


class OutPage : public QWidget
{
    Q_OBJECT

public:

    class ComparePoint{
    public:
        bool operator()(const QPoint& first, const QPoint& second)
        {
            return (first.y() < second.y()) ? true : false;
        }
    };

    class FindImagePoint{
    public:
        template<typename inputIter>
        std::pair<bool, int> operator()(OutPage* page, inputIter begin, inputIter end, const QPoint& point)
        {
            inputIter tempIter = begin;

            if(begin != end){

                for(; begin != end; ++begin){
                    QLabel* label = static_cast<QLabel*>(page->childAt(point));

                    if(label == *begin){
                        //qDebug()<< "You click on the image!";
                        return std::pair<bool, int>(true, begin - tempIter);

                    }

                    label = nullptr;
                }

            }

            return std::pair<bool, int>(false, 1);
        }
    };

    using lineIndex = unsigned int;

    template<typename Ty>
    using iter = typename std::vector<Ty>::iterator;

    template<typename Ty>
    using citer = typename std::vector<Ty>::const_iterator;

    template<typename Ty>
    using mapCIter = typename std::map<Ty, Ty>::const_iterator;

    template<typename X, typename Y, typename Z>
    using imgCIter = typename std::map<X, Y, Z>::const_iterator;

    template<typename Ty>
    using deqCIter = typename std::deque<Ty>::const_iterator;

    OutPage(QWidget* parent = nullptr);
    ~OutPage();

protected:
    virtual void paintEvent(QPaintEvent* ev)override;

    virtual void mousePressEvent(QMouseEvent* mousePressEvent)override;
      //virtual void mouseMoveEvent(QMouseEvent* mouseMoveEvent)override;
      //virtual void mouseReleaseEvent(QMouseEvent* mouseReleaseEvent)override;

    virtual void dragMoveEvent(QDragMoveEvent* dragEvent)override;
    virtual void dragEnterEvent(QDragEnterEvent* dragEnterEvent)override;
    virtual void dropEvent(QDropEvent* dropEvent)override;

private:
    unsigned int hei; //当前窗口的高。
    unsigned int wid; //当前窗口的宽。
    unsigned int horizontalLineNumber; //表格中横线的条数。
    unsigned int verticalLineNumber;   //表格中竖线的条数。
    int maxdB;
    int maxHz;
    QSize labelSize;
    QSize lineEditSize;
    QRect rect;          //用于判断是否超出了给定的区域.
    bool whetherBeyond;  //如果超过了指定区域设置为true.

    std::map<lineIndex, std::map<QPoint, QPoint, ComparePoint>> horizontalLinePoint{}; //表格横线的左右两点坐标.
    std::map<lineIndex, std::map<QPoint, QPoint, ComparePoint>> verticalLinePoint{};  //表格内竖线上下两点坐标.
    std::map<lineIndex, std::map<QPoint, std::shared_ptr<QPixmap>, ComparePoint>> imagesContainer{}; //存储图片和它的坐标.

    std::vector<QString> dB{};
    //std::vector<QString> Hz{};
    std::vector<QPoint> dBPoint{};
    //std::vector<QPoint> HzPoint{};
    std::deque<QLabel*> imgRectContainer{}; //不要学我这么写！！！

    std::vector<QLineEdit*> Freq{};
    std::vector<QLineEdit*> Gain{};
    std::vector<QComboBox*> Type{};
    std::vector<QLineEdit*> Qval{};
    std::vector<std::shared_ptr<QLabel>> labels{};  //存放Freq, Gain, Qval, Type的标签.
    std::vector<std::shared_ptr<QLabel>> EQTitleLabels{};

    std::array<std::shared_ptr<QLineEdit>, 4> HPFAndLPFLineEdit; //4个lineEidt用于显示最高频率最低频率.
    std::array<std::shared_ptr<QLabel>, 2> HPFAndLPFLabel; //存放HPF和LPF标签.
    std::deque<std::shared_ptr<QCheckBox>> Byps{};        //存放8个Byps的checkBox.
    

    QHBoxLayout* FreqLayout;
    QHBoxLayout* GainLayout;
    QHBoxLayout* QvalLayout;
    QHBoxLayout* TypeLayout;
    QHBoxLayout* EQLabelLayout;
    QHBoxLayout* BypsLayout;

    QVBoxLayout* HPFAndLPFLabelLayout;
    QVBoxLayout* HPFAndLPFLineEditLayout;

    QBoxLayout* BoxLayout;
    QBoxLayout* HPFAndLPFBoxLayout; //用于显示HPF和TPF标签和lineEidt.
    QBoxLayout* totalLayout; //总的布局管理.


    QPoint dragHotPoint;    //拖拽的开始的时候鼠标位于拖拽标志的中点.
    QPoint dropHotPoint;   //放下拖拽时候的拖在图标的中点.
    QPoint dropPoint;     //放下拖拽图标时候的鼠标的点.
    QPoint mousePressPoint; //鼠标刚刚按下时候的点.

    std::pair<bool, int> jundgePair; //判断拖拽的点是否位于我们加载的图片上.
    std::shared_ptr<QPixmap> tempPixmap;
    std::shared_ptr<QImage>  tempImage;
    QLabel*  tempLabel;
    std::shared_ptr<QLabel> checkBoxLineTitle; //这是一个占位label,显示一个指定大小的label
                                               //在QCheckBox之前.

    //------------------用于config块----------
    QHBoxLayout* configBoxLayout;
    std::shared_ptr<QSlider> configSlider;   //滑块.
    std::shared_ptr<QCheckBox> configCheckBox;
    std::shared_ptr<QPushButton> configPushButton;
    //---------------------------------------


    QPoint dragMovePoint;      //用于实时更新图标的坐标.
    std::pair<double, double> value;


    void getLinePoint()noexcept;
    void drawLines(QPainter& p)noexcept;
    void drawCurve(QPainter& p)noexcept;
    void createLineEdit()noexcept;
    void addLineEditToLayout()noexcept;
    void initializedImage();
    void drawImageOnTable();
    void createHPFAndTPF()noexcept;
    void layoutHPFAndTPF()noexcept;
    void manageLayout()noexcept;
    void createCheckBox()noexcept;
    void addConfigBlock()noexcept;
    //void outLineBetweenWidget()noexcept;

    template<typename Ty>
    void clearVector(std::vector<Ty>& reVec)noexcept;

    template<typename Ty>
    void clear(std::vector<Ty>& reVec, std::true_type)noexcept;

    template<typename Ty>
    void clear(std::vector<Ty>& reVec, std::false_type)noexcept;

};

#endif // OUTPAGE_H
