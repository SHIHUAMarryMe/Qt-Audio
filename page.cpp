#include "page.h"
#include "positiontoint.h"
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QRectF>
#include <QDebug>
#include <QLabel>
#include <iterator>
#include <algorithm>
#include <functional>
#include <QMouseEvent>
#include <QMimeData>
#include <QApplication>
#include <QDrag>
#include <QByteArray>
#include <QCheckBox>
#include <QCheckBox>
#include <QPushButton>

OutPage::OutPage(QWidget* parent)
         :QWidget(parent),
          hei(0),
          wid(0),
          horizontalLineNumber(9),
          verticalLineNumber(12),
          maxdB(15),
          labelSize(QSize(40, 30)),
          lineEditSize(QSize(60, 30)),
          //repaintFlag(false),
          FreqLayout(new QHBoxLayout),
          GainLayout(new QHBoxLayout),
          QvalLayout(new QHBoxLayout),
          TypeLayout(new QHBoxLayout),
          EQLabelLayout(new QHBoxLayout),
          BypsLayout(new QHBoxLayout),
          HPFAndLPFLabelLayout(new QVBoxLayout),
          HPFAndLPFLineEditLayout(new QVBoxLayout),
          BoxLayout(new QBoxLayout(QBoxLayout::TopToBottom)),
          HPFAndLPFBoxLayout(new QBoxLayout(QBoxLayout::LeftToRight)),
          totalLayout(new QBoxLayout(QBoxLayout::LeftToRight)),
          jundgePair(false, -1),
          configBoxLayout(new QHBoxLayout)
{
    this->setMinimumSize(QSize(800, 500));
    this->setMaximumWidth(500);
    this->wid = this->width()- 40;
    this->hei = this->height()- 250;

    this->rect = QRect(45, 30, this->wid-40, this->hei-30); //设置表格的范围.

    this->setAcceptDrops(true);      //设置接受拖拽事件.
    this->setMouseTracking(true);   //设置跟踪鼠标.

    this->getLinePoint();         //获得表格中各条线的两点.表中的可动圆圈.
    this->createLineEdit();
    this->initializedImage();   //加载图片.
    this->drawImageOnTable();
    this->addLineEditToLayout();

    this->createCheckBox();

    this->addConfigBlock();

    this->manageLayout();

}

OutPage::~OutPage()
{
    this->clearVector(this->Freq);
    this->clearVector(this->Gain);
    this->clearVector(this->Qval);
    this->clearVector(this->dB);
    this->clearVector(this->dBPoint);
    this->clearVector(this->labels);

    if(this->FreqLayout != nullptr){
        delete this->FreqLayout;
        this->FreqLayout = nullptr;
    }

    if(this->GainLayout != nullptr){
        delete this->GainLayout;
        this->GainLayout = nullptr;
    }

    if(this->QvalLayout != nullptr){
        delete this->QvalLayout;
        this->QvalLayout = nullptr;
    }

    if(this->HPFAndLPFLabelLayout != nullptr){
        delete this->HPFAndLPFLabelLayout;
        this->HPFAndLPFLabelLayout = nullptr;
    }
}

template<typename Ty>
void OutPage::clearVector(std::vector<Ty>& reVec)noexcept
{
    this->clear(reVec, std::is_pointer<Ty>());
}

template<typename Ty>
void OutPage::clear(std::vector<Ty>& reVec, std::true_type)noexcept
{
    if(!reVec.empty()){
        iter<Ty> begin = reVec.begin();
        for(; begin != reVec.end(); ++begin){
            delete *begin;
            *begin = nullptr;
        }

        //qDebug()<<"pointer";
        reVec.clear();
    }
}

template<typename Ty>
void OutPage::clear(std::vector<Ty>& reVec, std::false_type)noexcept
{
    if(!reVec.empty()){
        reVec.clear();
    }

    //qDebug()<<"class";
}



void OutPage::paintEvent(QPaintEvent* ev)
{
    QPainter paint(this);

        QPen pen(Qt::SolidLine);
        pen.setColor(Qt::darkRed);
        paint.setPen(pen);

        paint.drawRect(20, 20, this->wid, this->hei);
        paint.fillRect(20, 20, this->wid, this->hei, Qt::black);
        paint.drawRect(45, 30, this->wid-40, this->hei-30); //画一个黄色的矩形.

        paint.save();
        this->drawLines(paint); //画出表格中的各条线.
        paint.restore();

    paint.save();
    this->drawCurve(paint);
    paint.restore();

}

void OutPage::drawCurve(QPainter& p)noexcept
{
    QPen pen(Qt::red);
    pen.setStyle(Qt::DotLine);
    pen.setWidth(2);
    p.setPen(pen);

    imgCIter<QPoint, std::shared_ptr<QPixmap>, ComparePoint> itrOne;
    imgCIter<QPoint, std::shared_ptr<QPixmap>, ComparePoint> itrTwo;

        for(unsigned int i=1; i<8; ++i){
            itrOne = (this->imagesContainer)[i].cbegin();
            itrTwo = (this->imagesContainer)[i-1].cbegin();

            //下面之所以+12是因为我们获得的坐标总是topLeft的点.
            p.drawLine((itrOne->first).x() + 12 , (itrOne->first).y() + 12,
                        (itrTwo->first).x() + 12, (itrTwo->first).y() + 12);
        }

}


void OutPage::dragEnterEvent(QDragEnterEvent* dragEnterEvent)
{

    const QMimeData* mimeData = dragEnterEvent->mimeData();
    if(mimeData->hasImage()){

        if(dragEnterEvent->source() == this){
            qDebug()<<"dragEnterEvent";



            dragEnterEvent->setDropAction(Qt::MoveAction);
            dragEnterEvent->accept();
        }

    }else{
        qDebug()<< "ignore Enter Event";
        dragEnterEvent->ignore();
    }
}

void OutPage::dragMoveEvent(QDragMoveEvent* dragMoveEvent)
{
    dragMoveEvent->accept((this->rect));

    const QMimeData* mimeData = dragMoveEvent->mimeData();
    qDebug()<< "loop";
    if(mimeData->hasImage()){

        if(dragMoveEvent->source() == this){

            (this->dragMovePoint) = dragMoveEvent->pos(); //实时获取鼠标的所在的坐标.
            qDebug()<< (this->dragMovePoint);



            dragMoveEvent->setDropAction(Qt::MoveAction);
            dragMoveEvent->accept();

        }else{
            dragMoveEvent->acceptProposedAction();
        }

    }else{
        dragMoveEvent->ignore();
    }
}


void OutPage::dropEvent(QDropEvent* dropEvent)
{
    qDebug() << "dropEvent";
    const QMimeData* mimeData = dropEvent->mimeData();
    if(mimeData->hasImage()){

        qDebug() << "dropEvent1";
        if((this->jundgePair).second >= 0){

            this->dropPoint = dropEvent->pos(); //获得落点.

            QString pixmapName =
               QString(":images/%1.png").arg(QString::fromStdString(std::to_string((this->jundgePair).second)));

            (this->tempPixmap) =
               std::make_shared<QPixmap>(pixmapName);

            this->dropPoint = dropEvent->pos();

            if((this->dropPoint).x() < 45){
                dropPoint.setX(45);

            }else if((this->dropPoint).x() > static_cast<int>((this->wid + 5))){
                (this->dropPoint).setX(this->wid + 5);

            }

            if((this->dropPoint).y() < 30){
                dropPoint.setY(30);

            }else if((this->dropPoint).y() > static_cast<int>(this->hei)){
                (this->dropPoint).setY(this->hei);
            }

            (this->dropHotPoint) = /*(dropEvent->pos())*/ (this->dropPoint) - (this->dragHotPoint);

            //更新imagesContainer的坐标.
            std::pair<QPoint, std::shared_ptr<QPixmap>> tempPair((this->dropHotPoint), std::make_shared<QPixmap>(pixmapName));
            (this->imagesContainer)[(this->jundgePair).second].clear();
            (this->imagesContainer)[(this->jundgePair).second].insert(tempPair);

            (this->tempLabel) = new QLabel(this);
            (this->tempLabel)->move(this->dropHotPoint);
            (this->tempLabel)->setPixmap(*(this->tempPixmap));
            (this->tempLabel)->show();
            (this->tempLabel)->setAttribute(Qt::WA_DeleteOnClose);

            qDebug()<< "drop!";

            if(dropEvent->source() == this){
                (this->imgRectContainer)[(this->jundgePair).second] = (this->tempLabel); //更新位于imagesContainer中的QLabel.

                this->repaint(); //每次拖动图片的时候再放下的瞬间启动重绘事件.
                dropEvent->setDropAction(Qt::MoveAction);
                dropEvent->accept();

                qDebug()<< "complete to drop!";

            }else{
                dropEvent->acceptProposedAction();
            }
        }

    }else{
        dropEvent->ignore();
    }
}



void OutPage::mousePressEvent(QMouseEvent* mousePressEvent)
{
    std::function<std::pair<bool, int> (deqCIter<QLabel*>, deqCIter<QLabel*>, const QPoint&)> jundgeFunc =
            std::bind(FindImagePoint(), this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

    if(mousePressEvent->button() == Qt::LeftButton){

        //判断鼠标点到第几个图片上.
        (this->jundgePair) =
                jundgeFunc((this->imgRectContainer).cbegin(), (this->imgRectContainer).cend(), mousePressEvent->pos());

        qDebug()<< (this->jundgePair).first;
        if((this->jundgePair).first == true){

            qDebug()<< "2";
            imgCIter<QPoint, std::shared_ptr<QPixmap>, ComparePoint> itr =
                           (this->imagesContainer)[(this->jundgePair).second].cbegin();

            //这里设置的是拖拽时候的图标位于鼠标箭头下面的位置.
            (this->dragHotPoint) =
                (mousePressEvent->pos()) - (this->imgRectContainer)[(this->jundgePair).second]->pos();

            //获取鼠标按在图标上的时候，该图标的中点.
            (this->mousePressPoint) = (this->dragHotPoint);


            //下面获取拖拽时的是图标.
            (this->tempImage) =
                    std::make_shared<QImage>(QString(":images/%1.png").arg(QString::fromStdString(std::to_string((this->jundgePair).second))));


            QMimeData* dragData = new QMimeData;        //拖拽的图片临时存储到这里.
            dragData->setImageData(*(this->tempImage));

            QDrag* drag = new QDrag(this);
            drag->setMimeData(dragData);
            drag->setPixmap(*(itr->second));      //设置拖拽时候的图标.
            drag->setHotSpot(this->dragHotPoint);
                                                //这里设置的是鼠标箭头位于拖动的文件的图标的位置
                                               //上面的 mousePressEvent->pos() - (itr->first)获得的是中点.

            (this->imgRectContainer)[(this->jundgePair).second]->close();

            qDebug()<< "mousePress!";

            //需要注意的是这一步是会阻塞主线程的.
            if(drag->exec(Qt::MoveAction | Qt::CopyAction, Qt::MoveAction) == Qt::MoveAction){

                //在dropEvent()执行完毕之后才会执行这里.
                (this->tempLabel) = nullptr;
            }

        }

    }
}


/*
void OutPage::mouseMoveEvent(QMouseEvent* mouseMoveEvent)
{
    int x;
    int y;

    if(mouseMoveEvent->x() < 45){
        x = 45;
        (this->whetherBeyond) = true;

    }else if(mouseMoveEvent->x() > 45 + (this->wid + 5)){
        x = (this->wid + 5);
        (this->whetherBeyond) = true;
    }

    if(mouseMoveEvent->y() < 30){
        y = 30;
        (this->whetherBeyond) = true;

    }else if(mouseMoveEvent->y() > (this->hei)){
        y = this->hei;
        (this->whetherBeyond) = this->hei;
    }

    if(this->whetherBeyond == true){
        QCursor::setPos(x, y);
    }
}*/


/*
void OutPage::mouseMoveEvent(QMouseEvent* mouseMoveEvent)
{
    std::function<std::pair<bool, int> (deqCIter<std::shared_ptr<QRect>>, deqCIter<std::shared_ptr<QRect>>, const QPoint&)> jundgeFunc =
            std::bind(FindImagePoint(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

    if(mouseMoveEvent->buttons() & Qt::LeftButton){
        (this->dragEndPoint) = mouseMoveEvent->pos();
        int distance = ((this->dragStartPoint) - (mouseMoveEvent->pos())).manhattanLength();

        if(distance >= QApplication::startDragDistance()){
            (this->jundgePoint) = jundgeFunc((this->imgRectContainer).cbegin(), (this->imgRectContainer).cend(), std::ref(this->dragStartPoint));

            if((this->jundgePoint).first == true){

                imgCIter<QPoint, std::shared_ptr<QPixmap>, ComparePoint> itr = (this->imagesContainer)[(this->jundgePoint).second].cbegin();
                QImage image = (itr->second)->toImage();
                QVariant imageData = QVariant::fromValue(image);

                (this->dragData) = std::make_shared<QMimeData>();
                (this->drag) = std::make_shared<QDrag>(this);
                (this->dragData)->setImageData(imageData);
                (this->drag)->setPixmap(*(itr->second));

                qDebug()<< "mouseMoveEvent";
                (this->drag)->setMimeData((this->dragData).get());

                (this->drag)->exec(Qt::MoveAction);
        }
    }
}

}*/




//获得表格中各条线的两端坐标以及dB标注位置的坐标.
void OutPage::getLinePoint()noexcept
{
    unsigned int horizontalLineGap = (this->hei - 50) / 7; //横线间隙.
    unsigned int verticalLineGap = (this->wid - 40) / (this->verticalLineNumber);  //竖线间隙.


    unsigned int verticalLineStartPoint = 45;    //x点
    unsigned int horizontalLineStartPoint = 30; //y点

    (this->dBPoint).push_back(QPoint(20, 30)); //dB String的第一个点.
    (this->dB).push_back(QString("15"));      //dB的最大值.

    int tempMaxdB = (this->maxdB);

    std::function<std::string()> lambda =
       [&]()->std::string { return ((tempMaxdB -= 5) == 0) ? std::string("0dB") : std::to_string(tempMaxdB); };


    for(unsigned int i=0; i < this->verticalLineNumber; ++i){
        if(i < (this->horizontalLineNumber)-2){
            QPoint hLineSPoint(45, horizontalLineStartPoint += horizontalLineGap);
            QPoint hLineEPoint(5+(this->wid), horizontalLineStartPoint);
            (this->horizontalLinePoint)[i][hLineSPoint] = hLineEPoint;  //把横线的坐标放到map中.

            QPoint dBStringPoint(20, horizontalLineStartPoint);
            (this->dBPoint).push_back(dBStringPoint);
            (this->dB).push_back(QString("%1").arg(QString::fromStdString( lambda() )));
        }

        //qDebug()<< i ;
        QPoint vLineSPoint(verticalLineStartPoint += verticalLineGap, 30);
        QPoint vLineEPoint(verticalLineStartPoint, this->hei);
        (this->verticalLinePoint)[i][vLineSPoint] = vLineEPoint;    //把竖线的坐标放到map里.       
    }

    (this->dBPoint).push_back(QPoint(20, (this->hei)));
    (this->dB).push_back(QString("%1").arg(QString::fromStdString(std::to_string(tempMaxdB -= 5))));


    //mapCIter<QPoint> vIter;  //(this->verticalLinePoint)[0].cbegin();
    //mapCIter<QPoint> hIter = (this->horizontalLinePoint)[2].cbegin();

    //for(unsigned int i=0; i<8; ++i){
     //   vIter = (this->verticalLinePoint)[i].cbegin();
     //  QRect rect((vIter->first).x()-10, (hIter->first).y()-10, 20, 20);

     //QRect rect = QPainter::boundingRect((vIter->first).x(), (hIter->first).y(), 20, 20, Qt::AlignCenter, QString::fromStdString(std::to_string(i)));
    //(this->rectsContainer).push_back(rect);
    //}

    //(this->rectsContainer).shrink_to_fit();

}

void OutPage::drawLines(QPainter& p) noexcept //画出表格.
{
    mapCIter<QPoint> vBeginIter;
    mapCIter<QPoint> hBeginIter;
    citer<QPoint> dBPointIter = (this->dBPoint).cbegin();
    citer<QString> dBStringIter = (this->dB).cbegin();

    for(unsigned int i=0; i < this->verticalLineNumber; ++i){
        if(i < this->horizontalLineNumber-2){
            //qDebug() << i;
            hBeginIter = (this->horizontalLinePoint)[i].cbegin();
            p.drawLine(hBeginIter->first, hBeginIter->second); //画出表格横线.
        }

        vBeginIter = (this->verticalLinePoint)[i].cbegin();
        p.drawLine(vBeginIter->first, vBeginIter->second); //画出表格竖线.
    }

    QPen pen(Qt::yellow);
    p.setPen(pen);
    for(; dBPointIter != (this->dBPoint).cend() && dBStringIter != (this->dB).cend(); ++dBPointIter){
        p.drawText(*dBPointIter, *dBStringIter);  //标出dB.
        ++dBStringIter;
    }

}



void OutPage::createLineEdit()noexcept //把labels, lineEdits放到容器内.
{
    std::string EQstr("EQ");

    std::shared_ptr<QLabel> freqLabel = std::make_shared<QLabel>(tr("Freq:")); //Freq
    freqLabel->setTextFormat(Qt::RichText);
    freqLabel->setMaximumSize(this->labelSize);

    std::shared_ptr<QLabel> gainLabel = std::make_shared<QLabel>(tr("Gain:")); //Gain
    gainLabel->setTextFormat(Qt::RichText);
    gainLabel->setMaximumSize(this->labelSize);

    std::shared_ptr<QLabel> qvalLabel = std::make_shared<QLabel>(tr("Gval:")); //Gval
    qvalLabel->setTextFormat(Qt::RichText);
    qvalLabel->setMaximumSize(this->labelSize);

    std::shared_ptr<QLabel> typeLabel = std::make_shared<QLabel>(tr("Type:")); //Type
    typeLabel->setTextFormat(Qt::RichText);
    typeLabel->setMinimumSize(this->labelSize);


    //std::shared_ptr<QLabel> emptyLabel = std::make_shared<QLabel>(QString(" ")); //占位label.
    //emptyLabel->setMaximumSize(this->labelSize);


    std::shared_ptr<QLabel> emptyEQLabel = std::make_shared<QLabel>(QString(" ")); //占位label.
    emptyEQLabel->setMaximumSize(this->labelSize);
    (this->EQTitleLabels).push_back(emptyEQLabel);
    
    //用于QCheckBox的占位lable.
    std::shared_ptr<QLabel> emptyBypLabel = std::make_shared<QLabel>(QString(" ")); //占位label.
    emptyBypLabel->setMaximumSize(this->labelSize);
    


    for(unsigned int i = 0; i<8; ++i){
        QLineEdit* lineEdit1 = new QLineEdit;
        lineEdit1->setMaximumSize(this->lineEditSize);

        QLineEdit* lineEdit2 = new QLineEdit;
        lineEdit2->setMaximumSize(this->lineEditSize);

        QLineEdit* lineEdit3 = new QLineEdit;
        lineEdit3->setMaximumSize(this->lineEditSize);

        QComboBox* comboBox = new QComboBox;
        comboBox->setMaximumSize(this->lineEditSize);
        comboBox->addItem(tr("PEQ"));
        comboBox->addItem(tr("LS6"));
        comboBox->addItem(tr("LS12"));
        comboBox->addItem(tr("HS6"));
        comboBox->addItem(tr("HS12"));

        std::shared_ptr<QLabel> EQLabel = std::make_shared<QLabel>(QString(QString::fromStdString(EQstr + std::to_string(i))));
        EQLabel->setFrameShape(QFrame::Box);
        EQLabel->setFrameShadow(QFrame::Plain);
        EQLabel->setMaximumSize(this->lineEditSize);

        (this->Freq).push_back(lineEdit1);
        (this->Gain).push_back(lineEdit2);
        (this->Qval).push_back(lineEdit3);
        (this->Type).push_back(comboBox);
        (this->EQTitleLabels).push_back(EQLabel);

        lineEdit1 = nullptr;
        lineEdit2 = nullptr;
        lineEdit3 = nullptr;
        comboBox = nullptr;
    }

    (this->labels).push_back(freqLabel);
    (this->labels).push_back(gainLabel);
    (this->labels).push_back(qvalLabel);
    (this->labels).push_back(typeLabel);
    (this->labels).push_back(emptyBypLabel);

    //-------------------用于config----------

    std::shared_ptr<QLabel> volumeLabel = std::make_shared<QLabel>(tr("Volume: "));
    volumeLabel->setMaximumSize(this->lineEditSize);
    (this->labels).push_back(std::move(volumeLabel));

    std::shared_ptr<QLabel> valueLabel = std::make_shared<QLabel>(tr("-60dB"));
    valueLabel->setMaximumSize(this->lineEditSize);
    (this->labels).push_back(std::move(valueLabel));

    std::shared_ptr<QLabel> PolLabel = std::make_shared<QLabel>(tr("Pol: "));
    PolLabel->setMaximumSize(this->lineEditSize);
    (this->labels).push_back(std::move(PolLabel));

    //-------------------------------------

    (this->labels).shrink_to_fit();   //优化内存占用.
    (this->Freq).shrink_to_fit();
    (this->Gain).shrink_to_fit();
    (this->Type).shrink_to_fit();
    (this->EQTitleLabels).shrink_to_fit();

    this->createHPFAndTPF();
}


void OutPage::createHPFAndTPF()noexcept
{
    std::size_t index = 0;
    std::shared_ptr<QLineEdit> lineEditOne = std::make_shared<QLineEdit>(QString("19.7"));
    lineEditOne->setMinimumSize(this->lineEditSize);
    (this->HPFAndLPFLineEdit)[index] = lineEditOne;

    std::shared_ptr<QLineEdit> lineEditTwo = std::make_shared<QLineEdit>(QString("12db-Bworth"));
    lineEditTwo->setMinimumSize(this->lineEditSize);
    (this->HPFAndLPFLineEdit)[1+index] = lineEditTwo;

    std::shared_ptr<QLineEdit> lineEditThree = std::make_shared<QLineEdit>(QString("20100"));
    lineEditThree->setMinimumSize(this->lineEditSize);
    (this->HPFAndLPFLineEdit)[2+index] = lineEditThree;

    std::shared_ptr<QLineEdit> lineEditFour = std::make_shared<QLineEdit>(QString("12db-Bworth"));
    lineEditFour->setMinimumSize(this->lineEditSize);
    (this->HPFAndLPFLineEdit)[3+index] = lineEditFour;


    std::shared_ptr<QLabel> labelOne= std::make_shared<QLabel>(QString("HPF"));
    labelOne->setMaximumSize(this->lineEditSize);
    labelOne->setFrameShape(QFrame::Box);
    labelOne->setFrameShadow(QFrame::Plain);
    (this->HPFAndLPFLabel)[0] = labelOne;

    std::shared_ptr<QLabel> labelTwo = std::make_shared<QLabel>(QString("LPF"));
    labelTwo->setMaximumSize(this->lineEditSize);
    labelTwo->setFrameShape(QFrame::Box);
    labelTwo->setFrameShadow(QFrame::Plain);
    (this->HPFAndLPFLabel)[1] = labelTwo;

    
    //把用于显示HPF和TPF的lineEdit添加到layout中.
    for(; index < (this->HPFAndLPFLineEdit).max_size(); ++index){

        //竖直显示lineEdit用于显示HPF和LPT.
        (this->HPFAndLPFLineEditLayout)->addWidget((this->HPFAndLPFLineEdit)[index].get());
    }

    (this->HPFAndLPFLabelLayout)->addWidget((this->HPFAndLPFLabel)[0].get());
    (this->HPFAndLPFLabelLayout)->addWidget((this->HPFAndLPFLabel)[1].get());

}


void OutPage::addLineEditToLayout()noexcept
{
    //qDebug()<<"enter";
    iter<QLineEdit*> begin1 = (this->Freq).begin();
    iter<QLineEdit*> begin2 = (this->Gain).begin();
    iter<QLineEdit*> begin3 = (this->Qval).begin();
    iter<QComboBox*> begin4 = (this->Type).begin();

    (this->FreqLayout)->addWidget(labels[0].get());
    (this->GainLayout)->addWidget(labels[1].get());
    (this->QvalLayout)->addWidget(labels[2].get());
    (this->TypeLayout)->addWidget(labels[3].get());
    (this->EQLabelLayout)->addWidget(EQTitleLabels[0].get());

    for(unsigned int i=0; i<8; ++i){
        (this->FreqLayout)->addWidget(*begin1);
        (this->GainLayout)->addWidget(*begin2);
        (this->QvalLayout)->addWidget(*begin3);
        (this->TypeLayout)->addWidget(*begin4);
        (this->EQLabelLayout)->addWidget(EQTitleLabels[i+1].get());

        ++begin1;
        ++begin2;
        ++begin3;
        ++begin4;
    }

}

void OutPage::manageLayout()noexcept
{

    (this->BoxLayout)->addLayout(this->EQLabelLayout);
    (this->BoxLayout)->addLayout(this->TypeLayout);
    (this->BoxLayout)->addLayout(this->FreqLayout);
    (this->BoxLayout)->addLayout(this->GainLayout);
    (this->BoxLayout)->addLayout(this->QvalLayout);

    (this->BoxLayout)->addLayout(this->BypsLayout);

    (this->BoxLayout)->addLayout(this->configBoxLayout);

    (this->BoxLayout)->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

    (this->HPFAndLPFBoxLayout)->addLayout(this->HPFAndLPFLabelLayout);
    (this->HPFAndLPFBoxLayout)->addLayout(this->HPFAndLPFLineEditLayout);
    (this->HPFAndLPFBoxLayout)->setAlignment(Qt::AlignRight | Qt::AlignBottom);


    (this->totalLayout)->addLayout(this->BoxLayout);
    (this->totalLayout)->addLayout(this->HPFAndLPFBoxLayout);

    this->setLayout(this->totalLayout);
}



void OutPage::initializedImage()
{
    mapCIter<QPoint> vIter;
    mapCIter<QPoint> hIter = (this->horizontalLinePoint)[2].cbegin();

    for(unsigned int i=0; i<8; ++i){

        vIter = (this->verticalLinePoint)[i].cbegin();

        std::shared_ptr<QPixmap> img = std::make_shared<QPixmap>(QString(":/images/%1.png").arg(QString::fromStdString(std::to_string(i))));
        QLabel* imgLabel = new QLabel(this);
        imgLabel->setMaximumSize(img->size());
        imgLabel->setAttribute(Qt::WA_DeleteOnClose);
        QPoint point((vIter->first).x() - 5, (hIter->first).y() - 5);

        if(img->isNull()){
            //qDebug()<< i << "-------";
            throw std::runtime_error(std::to_string(i));

        }else{

            (this->imgRectContainer).push_back(imgLabel);
            (this->imagesContainer)[i][point] = img;
        }

        imgLabel = nullptr;
    }

    (this->imgRectContainer).shrink_to_fit(); //最优化内存占用.

}

void OutPage::drawImageOnTable()
{
    imgCIter<QPoint, std::shared_ptr<QPixmap>, ComparePoint> itr;
    deqCIter<QLabel*> labelItr = (this->imgRectContainer).cbegin();

    if( !(this->imgRectContainer).empty() && !(this->imagesContainer).empty() ){

        for(unsigned int i=0; i<8; ++i){

            itr = (this->imagesContainer)[i].cbegin();
            (*labelItr)->move(itr->first);
            (*labelItr)->setPixmap(*(itr->second));
            (*labelItr)->show();
            qDebug()<< (*labelItr)->size();
            ++labelItr;
        }

    }else{
        throw std::runtime_error(std::string("fail to initialize images"));
    }
}

void OutPage::createCheckBox()noexcept
{
    if((this->Byps).empty()){
        for(std::size_t index =0; index != 8; ++index){
            std::shared_ptr<QCheckBox> checkBox = std::make_shared<QCheckBox>(QString("Byps"));
            checkBox->setMaximumSize(this->lineEditSize);
            (this->Byps).push_back(checkBox);
        }
    }

    (this->Byps).shrink_to_fit();


    //把QCheckBox放到布局管理(horizontal方向)里面.
    (this->BypsLayout)->addWidget((this->labels)[4].get());

    if(!(this->Byps).empty()){
        for(std::size_t index =0; index < 8; ++index){
            (this->BypsLayout)->addWidget((this->Byps[index]).get());
        }
    }

}

void OutPage::addConfigBlock()noexcept
{
    (this->configCheckBox) = std::make_shared<QCheckBox>(tr("Reversed Phase"));
    (this->configCheckBox)->setMaximumSize(this->lineEditSize);

    (this->configSlider) = std::make_shared<QSlider>(Qt::Horizontal);
    //(this->configSlider)->setMaximumSize(this->lineEditSize);
    (this->configSlider)->setFixedWidth((this->lineEditSize).width() * 3);
    (this->configSlider)->setRange(0, 60);
    (this->configSlider)->setSingleStep(1);

    (this->configPushButton) = std::make_shared<QPushButton>(tr("Mute")); //静音按钮.
    (this->configPushButton)->setMaximumSize(this->lineEditSize);

    (this->configBoxLayout)->addWidget((this->labels)[5].get());
    (this->configBoxLayout)->addWidget((this->configSlider).get());
    (this->configBoxLayout)->addWidget((this->labels)[6].get());
    (this->configBoxLayout)->addWidget((this->configPushButton).get());

}



