#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <map>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>

const int MAXN = 10000000;

Graph grap;
int n = 54;
//std::vector<std::vector<int>> prev(n, std::vector<int>(n, -1));
std::map<QString,coordinatesStr>mp;
std::map<QString,coordinates>doubleMap;
std::vector<QString>commonNode;
std::map<QString,QString>mapVillage;
std::map<QString,std::vector<QString>>mapWay;
std::map<QString,int>villageToInt;
std::map<int,QString>villageToString;
std::vector<QString>nodeVillage;

//ui->graphicsView->setScene(scene);

int dist[54][54];  // 保存点与点之间的距离
int prev[100][100];  // 保存路径上的前一个点
std::vector<int> path;  // 存储最短路径上的点
int Matrix[54][54] = {0};
resolve_xml Xml;

double resolve_xml::degreesToRadians(double degrees) {
    return degrees * M_PI / 180;
}
double resolve_xml::radiansToDegrees(double radians) {
    return radians * 180 / M_PI;
}
double resolve_xml::calculateDistance(coordinates node1, coordinates node2) { //经纬度坐标
    double dLat = degreesToRadians(node2.lat - node1.lat);   //Haversine公式
    double dLon = degreesToRadians(node2.lon - node1.lon);
    double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
               std::cos(degreesToRadians(node1.lat)) * std::cos(degreesToRadians(node2.lat)) *
               std::sin(dLon / 2) * std::sin(dLon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    double distance = 6378137 * c;
    return distance;
}

void resolve_xml::strChangeDouble(coordinatesStr node)
{
    coordinates coor;
    coor.id = node.id.toInt();
    coor.lat = node.lat.toDouble();
    coor.lon = node.lon.toDouble();
    doubleMap[node.id] = coor;
}

void resolve_xml::findCommonNode() //查找公共交点
{
    QFile file(":/qrc/map.osm");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
            qDebug() << "无法打开XML文件.";
            //exit(0);
    }
    QXmlStreamReader xml(&file);

    std::set<QString> wayNodes;

    int wayCount = 0;
    bool flag = false;

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartElement) {
            if (xml.name() == "way") {
                flag = true;
            }
            else if (xml.name() == "nd" && flag == true) {
                QString ndRef = xml.attributes().value("ref").toString();
                if (wayNodes.find(ndRef) != wayNodes.end()) {
                    commonNode.push_back(ndRef);
                }
                else {
                    wayNodes.insert(ndRef);
                }
            }
            else if(xml.name() == "relation"){
                flag = false;
            }
            else if(xml.name() == "member"){
                flag = false;
            }
        }
    }
    file.close();
}

void resolve_xml::changeCoor(QString strId)
{
    coordinates coo = doubleMap[strId];
    coordinatesInt cooint ;
    int width = (maxLon - minLon) * 10000;
    int length = (maxLat - minLat) * 10000;
    int x = (maxLat - coo.lat ) * 10000 ;
    int y = (coo.lon - minLon) * 10000 ;
    cooint.id = strId;
    cooint.x = y;
    cooint.y = x;
    Xml.doubleCoor.push_back(cooint);
    Xml.doubleCoorMap[strId] = cooint;
}
/*void resolve_xml::saveNode(int id,coordinates node)
{
    mp[id] = node;
}*/
int resolve_xml::  imortant()
{

    std::vector<QString>strNode;
    std::vector<QString>strWay;
    coordinatesStr coorStr;   //QString类型经纬度的结构体
    QString wayId;      //way的Id
    QString tagKey;
    QString tagValue;
    QString nodeId;     //node的ID
    QString nodeLon;    //node的经度
    QString nodeLat;    //node的纬度
    QFile fileName(":/qrc/map.osm");
    if (!fileName.open(QIODevice::ReadOnly | QIODevice::Text))
    {
            qDebug() << "无法打开XML文件.";
            return 1;
    }
    QXmlStreamReader xml(&fileName);
        while (!xml.atEnd() && !xml.hasError()) {
            QXmlStreamReader::TokenType token = xml.readNext();

            if (token == QXmlStreamReader::StartElement) {
                if(xml.name() == "node"){
                    nodeId = xml.attributes().value("id").toString();
                    nodeLon = xml.attributes().value("lon").toString();
                    nodeLat = xml.attributes().value("lat").toString();
                    coorStr.id = nodeId;
                    coorStr.lat = nodeLat;
                    coorStr.lon = nodeLon;
                    mp[nodeId] = coorStr;
                    strNode.push_back(nodeId);
                }
                else if (xml.name() == "way") {
                    // 提取way元素的id属性值
                    strWay.clear();
                    wayId = xml.attributes().value("id").toString();
                    //qDebug() << "Way ID:" << wayId;
                } else if (xml.name() == "nd") {
                    // 提取nd元素的ref属性值
                    QString ndRef = xml.attributes().value("ref").toString();
                    strWay.push_back(ndRef);
                    mapWay[wayId] = strWay;
                    //qDebug() << "ND Ref:" << ndRef;
                } else if (xml.name() == "tag") {
                    // 提取tag元素的k和v属性值
                    QString str1 = tagKey;
                    QString str2 = tagValue;
                    tagKey = xml.attributes().value("k").toString();
                    tagValue = xml.attributes().value("v").toString();
                    if(tagValue == "village")
                    {
                        nodeVillage.push_back(str2);
                        mapVillage[str2] = nodeId;
                    }
                    //qDebug() << "Tag Key:" << tagKey << ", Value:" << tagValue;
                }
                else if(xml.name() == "bounds")
                {
                    //提取数据中的最大最小经纬度
                    Xml.minLat = xml.attributes().value("minlat").toString().toDouble();
                    Xml.minLon = xml.attributes().value("minlon").toString().toDouble();
                    Xml.maxLat = xml.attributes().value("maxlat").toString().toDouble();
                    Xml.maxLon = xml.attributes().value("maxlon").toString().toDouble();
                }
            }
        }
       if (xml.hasError())
       {
           qDebug() << "XML解析错误: " << xml.errorString();
           return 1;
       }
      for(auto it = mp.begin();it != mp.end();it++) //将QString的经纬度转化成double类型的经纬度
      {
          QString Id = it->first;
          coordinatesStr jw = it->second;
          Xml.strChangeDouble(jw);
          Xml.changeCoor(Id);
          //qDebug()<<Id;
          //qDebug()<<jw.lat<<" "<<jw.lon;
      }
      for(auto it = mapWay.begin();it != mapWay.end();it++)
      {
          std::vector<QString> Strway = it->second;
          for(int i = 0;i < Strway.size();i++)
          {
                QString Qstr = Strway[i];
                double x = doubleMap[Qstr].lat;
                double y = doubleMap[Qstr].lon;
                minLat = std::min(minLat,x);
                minLon = std::min(minLon,y);
                maxLat = std::max(maxLat,x);
                maxLon = std::max(maxLon,y);
          }
      }
     // qDebug()<<"xxxx"<<minLat<<minLon<<maxLat<<maxLon; //输出最大最小经纬度
      for(auto it = mapWay.begin();it != mapWay.end();it++) //保存所有way的点
      {
          QString way = it->first;
          std::vector<QString> Strway = it->second;
          for(int i = 0;i < Strway.size();i++)
          {
                QString Qstr = Strway[i];
                //Xml.changeCoor(Qstr);
          }
      }
      for(auto it = mapWay.begin();it != mapWay.end();it++) //根据经纬度计算点与点之间距离，连接成路
      {
          QString way = it->first;
          //qDebug()<<way;
          std::vector<QString> Strway = it->second;
          //qDebug()<<"way nodeNumber:"<<Strway.size();
          int distanceWay = 0;
          for(int i = 1;i < Strway.size();i++)
          {
                QString Qstr1 = Strway[i-1];
                QString Qstr2 = Strway[i];
                coordinates node1 = doubleMap[Qstr1];
                coordinates node2 = doubleMap[Qstr2];
                distanceWay += (int)Xml.calculateDistance(node1,node2);
          }
          //qDebug()<<"way distance:"<<distanceWay;
      }
      for(auto it = mapVillage.begin();it != mapVillage.end();it++)
      {
          //qDebug()<<"village: "<<it->first<<"village nodeId: "<<it->second;
      }
      for(int i = 0;i < nodeVillage.size();i++)
      {
          QString sst = nodeVillage[i];
          villageToInt[sst] = i;
          villageToString[i] = sst;
          //qDebug()<<sst<<i;
      }
      /*QString str = "121.1458065";
      bool ok;
      double value = str.toDouble(&ok);
      if (ok) {
          qDebug() << "Converted value:" << value;
      } else {
          qDebug() << "Conversion failed.";
      }*/
      Xml.findCommonNode();
      for(int i = 0;i < commonNode.size();i++)
      {
          //qDebug()<<"commonNode"<<commonNode[i];
      }
      //qDebug()<<"commonNode size"<<commonNode.size()<<"way size:"<<mapWay.size()<<"village size"<<mapVillage.size();
      //qDebug()<<"---"<<Xml.doubleCoor.size();
      fileName.close();
      return 0;
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showComBox()
{
    for(auto it = mapVillage.begin();it != mapVillage.end();it++)
    {
        QString stt = it->first;
        ui->startBox->addItem(stt);
    }
    for(auto it = mapVillage.begin();it != mapVillage.end();it++)
    {
        QString stt = it->first;
        ui->endBox->addItem(stt);
    }
    qDebug()<<mapVillage.size();
}

void MainWindow::zoomInOut()
{
    // 连接放大按钮
    connect(ui->InButton, &QPushButton::clicked, this, &MainWindow::InButton_clicked);

    // 连接缩小按钮
    connect(ui->outButton, &QPushButton::clicked, this, &MainWindow::OutButton_clicked);

    //连接最短路径按钮
    connect(ui->shortBtn, &QPushButton::clicked, this, &MainWindow::shortBtnclicked);
}

void MainWindow::InButton_clicked()
{
    qDebug() << "InButton was clicked or triggered!";
    double scaleFactor = 1.15;
    ui->graphicsView->scale(scaleFactor, scaleFactor);
}

void MainWindow::OutButton_clicked()
{
    double scaleFactor = 1.15;
    ui->graphicsView->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
}

void MainWindow::showShortPath()
{

    QGraphicsScene *scene = ui->graphicsView->scene();
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);  //抗锯齿功能，画的线更润换
   // ui->graphicsView->setScene(scene);
    std::vector<QPointF>shortPath;
    QPainterPath pathVillage;
    for(int i = 0;i < path.size();i++)
    {
        int  intId = path[i];
        QString StrID = villageToString[intId];
        coordinatesInt coorvillage = Xml.doubleCoorMap[mapVillage[StrID]];
        QPointF points;
        points.setX(coorvillage.x);
        points.setY(coorvillage.y);
        shortPath.push_back(points);
    }
    qDebug()<<shortPath.size();
    int distance;
    for(int i = 1;i < path.size();i++)
    {
        int  intId1 = path[i-1];
        int  intId2 = path[i];
        QString StrID1 = villageToString[intId1];
        QString StrID2 = villageToString[intId2];
        coordinates node1 = doubleMap[mapVillage[StrID1]];
        coordinates node2 = doubleMap[mapVillage[StrID2]];
        distance = Xml.calculateDistance(node1,node2);
    }
    QString setdist = QString::number(distance);
    setdist += " meter";
    ui->showDist->setText(setdist);
    /*if(shortPath.size() != 0)
    {

        pathVillage.moveTo(shortPath[0]);
        for(int i = 1;i <shortPath.size();i++)
        {
            pathVillage.lineTo(shortPath[i]);
        }
        QGraphicsPathItem *pathItems = new QGraphicsPathItem(pathVillage);
        scene->addItem(pathItems);

        // 可选：设置道路的样式
        QPen roadPenVillage(Qt::green, 2);  // 示例：绿色，线宽为3
        pathItems->setPen(roadPenVillage);
    }*/
    if(!scene) {
        scene = new QGraphicsScene(this);
        ui->graphicsView->setScene(scene);
    }
    for( int i = 1; i < shortPath.size();i++)
    {
        QPen pen(Qt::red);
        pen.setWidth(3);
        QPointF point1 = shortPath[i-1];
        QPointF point2 = shortPath[i];
        scene->addLine(point1.x(), point1.y(), point2.x(), point2.y(), pen);  // 这里使用红色画笔
    }
}

void MainWindow::floyd(int n)
{
    for(int k = 0; k < n; k++) {
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n; j++) {
                if(dist[i][k] != INT_MAX && dist[k][j] != INT_MAX && dist[i][j] > dist[i][k] + dist[k][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    prev[i][j] = k;
                }
            }
        }
    }
}

void MainWindow::initDist()
{
    n = 54;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            if(i == j) dist[i][j] = 0;
            else  if ( j % 2 == 0)
            {
                QString s1 = villageToString[i];
                QString s2 = villageToString[j];
                coordinates node1 = doubleMap[mapVillage[s1]];
                coordinates node2 = doubleMap[mapVillage[s2]];
                int distancesss = Xml.calculateDistance(node1,node2);
                qDebug()<<"s="<<distancesss;
                dist[i][j] = distancesss;
            }
            /*else if (Matrix[i][j] == 0)
            {
                dist[i][j] = INT_MAX;
            }*/
            prev[i][j] = i;
        }
    }
}

void MainWindow::getPath(int start, int end)
{
    //if(start != end) getPath(start, prev[start][end]);
    //path.push_back(end);
    //path.clear();
    /*QString cur1 = ui->startBox->currentText();
    QString cur2 = ui->endBox->currentText();
    int current1 = villageToInt[cur1];
    int current2 = villageToInt[cur2];
    path.push_back(current1);
    while (current1 != current2) {
        current1 = prev[current1][current2];
        path.push_back(current1);
    }*//*
    if (prev[start][end] == -1) {
            return ;
        }

        path.push_back(start);
        while (start != end) {
            start = prev[start][end];
            path.push_back(start);
        }*/
    /*if (start == end || prev[start][end] == -1) {
            // 如果已经到达终点或者无法到达，返回包含起点的路径
            return {start};
        } else {
            // 递归调用，将路径从起点到 next[start][end] 和从 next[start][end] 到终点连接起来
            std::vector<int> path1 = getPath(start, prev[start][end]);
            std::vector<int> path2 = getPath(prev[start][end], end);

            // 合并两个路径
            path1.insert(path1.end(), path2.begin() + 1, path2.end());
            return path1;
        }*/
    if(start != end) getPath(start, prev[start][end]);
        path.push_back(end);
}

void MainWindow::initMap()
{
    Matrix[0][32] = Matrix[32][0] = 1;
    Matrix[0][53] = Matrix[53][0] = 1;
    Matrix[0][48] = Matrix[48][0] = 1;
    Matrix[0][47] = Matrix[47][0] = 1;
    Matrix[0][49] = Matrix[49][0] = 1;
    Matrix[0][50] = Matrix[50][0] = 1;
    Matrix[0][51] = Matrix[51][0] = 1;
    Matrix[0][36] = Matrix[36][0] = 1;
    Matrix[0][35] = Matrix[35][0] = 1;
    Matrix[0][34] = Matrix[34][0] = 1;
    Matrix[0][37] = Matrix[37][0] = 1;
    Matrix[0][40] = Matrix[40][0] = 1;
    Matrix[0][50] = Matrix[50][0] = 1;
    Matrix[2][33] = Matrix[33][2] = 1;
    Matrix[2][3] = Matrix[3][2] = 1;
    Matrix[2][4] = Matrix[4][2] = 1;
    Matrix[4][3] = Matrix[3][4] = 1;
    Matrix[4][33] = Matrix[33][4] = 1;
    Matrix[5][8] = Matrix[8][5] = 1;
    Matrix[5][9] = Matrix[9][5] = 1;
    Matrix[5][10] = Matrix[10][5] = 1;
    Matrix[7][6] = Matrix[6][7] = 1;
    Matrix[7][4] = Matrix[4][7] = 1;
    Matrix[11][43] = Matrix[43][11] = 1;
    Matrix[11][45] = Matrix[45][11] = 1;
    Matrix[11][42] = Matrix[42][11] = 1;
    Matrix[12][13] = Matrix[13][12] = 1;
    Matrix[12][14] = Matrix[14][12] = 1;
    Matrix[12][15] = Matrix[15][12] = 1;
    Matrix[16][20] = Matrix[20][16] = 1;
    Matrix[17][19] = Matrix[19][17] = 1;
    Matrix[17][18] = Matrix[18][17] = 1;
    Matrix[20][17] = Matrix[17][20] = 1;
    Matrix[20][18] = Matrix[18][20] = 1;
    Matrix[20][19] = Matrix[19][20] = 1;
    Matrix[21][22] = Matrix[22][21] = 1;
    Matrix[21][23] = Matrix[23][21] = 1;
    Matrix[21][26] = Matrix[26][21] = 1;
    Matrix[26][27] = Matrix[27][26] = 1;
    Matrix[26][24] = Matrix[24][26] = 1;
    Matrix[24][25] = Matrix[25][24] = 1;
    Matrix[25][31] = Matrix[31][25] = 1;
    Matrix[28][29] = Matrix[29][28] = 1;
    Matrix[28][30] = Matrix[30][28] = 1;
    Matrix[34][36] = Matrix[36][34] = 1;
    Matrix[35][36] = Matrix[36][35] = 1;
    Matrix[40][34] = Matrix[34][40] = 1;
    Matrix[40][37] = Matrix[37][40] = 1;
    Matrix[41][37] = Matrix[37][41] = 1;
    Matrix[41][34] = Matrix[34][41] = 1;

    Matrix[44][39] = Matrix[39][44] = 1;

    Matrix[39][38] = Matrix[38][39] = 1;
}
void MainWindow::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);  // 调用父类的 paintEvent 以确保其他部分绘制正确
    /*QPainter painter(this);
    QPen pen;
    pen.setWidth(2);
    painter.setPen(pen);
    qDebug()<<"   "<<Xml.doubleCoor.size();
    for(int i = 0;i <Xml.doubleCoor.size();i++)
    {
        coordinatesInt it = Xml.doubleCoor[i];
        pen.setColor(Qt::red); // 设置画笔颜色
        painter.drawPoint(it.x, it.y);     // 在坐标 (50, 50) 上绘制一个点
    }*/

    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing); //抗锯齿

    // 示例点位和名字
    /*int len = nodeVillage.size();
    QList<QPointF> points;
    for(int i = 0;i < len;i++)
    {
        QString villageNode = mapVillage[nodeVillage[i]];
        QPoint ppp;
        coordinatesInt coo = Xml.doubleCoorMap[villageNode];
        ppp.setX(coo.x);
        ppp.setY(coo.y);
        points.push_back(ppp);
        //qDebug()<<points.size();
    }

    for(int i = 0; i < nodeVillage.size(); ++i)
    {
        // 为每个点位创建一个小的椭圆
        QGraphicsEllipseItem *ellipse = scene->addEllipse(points[i].x() - 5, points[i].y() - 5, 10, 10, QPen(Qt::black), QBrush(Qt::red));

        // 创建一个文本项以显示名字
        QGraphicsTextItem *text = scene->addText(nodeVillage[i]);
        text->setPos(points[i].x() + 10, points[i].y() - text->boundingRect().height() / 2);
    }*/

    std::vector<std::vector<QPointF>>roads;
    for(auto it = mapWay.begin();it != mapWay.end();it++)
    {
        std::vector<QString> vectorStr = it->second;
        std::vector<QPointF>roadWay;
        for(int i = 0;i < vectorStr.size();i++)
        {
            QString wayStr = vectorStr[i];
            coordinatesInt coorWay;
            coorWay = Xml.doubleCoorMap[wayStr];
            QPoint pointWay;
            pointWay.setX(coorWay.x);
            pointWay.setY(coorWay.y);
            roadWay.push_back(pointWay);
        }
        roads.push_back(roadWay);
        roadWay.clear();
    }

    for (const std::vector<QPointF> &road : roads) {
        QPainterPath pathsss;
        pathsss.moveTo(road[0]);
        for (int i = 1; i < road.size(); ++i)
        {
            pathsss.lineTo(road[i]);
        }
        QGraphicsPathItem *pathItem = new QGraphicsPathItem(pathsss);
        scene->addItem(pathItem);

        // 可选：设置道路的样式
        QPen roadPen(Qt::blue, 2);  // 示例：蓝色，线宽为2
        pathItem->setPen(roadPen);
    }

    // 示例点位和名字
    int len = nodeVillage.size();
    QList<QPointF> points;
    for(int i = 0;i < len;i++)
    {
        QString villageNode = mapVillage[nodeVillage[i]];
        QPoint ppp;
        coordinatesInt coo = Xml.doubleCoorMap[villageNode];
        ppp.setX(coo.x);
        ppp.setY(coo.y);
        points.push_back(ppp);
        //qDebug()<<points.size();
    }

    for(int i = 0; i < nodeVillage.size(); ++i)
    {
        // 为每个点位创建一个小的椭圆
        QGraphicsEllipseItem *ellipse = scene->addEllipse(points[i].x() - 5, points[i].y() - 5, 10, 10, QPen(Qt::black), QBrush(Qt::red));

        // 创建一个文本项以显示名字
        QGraphicsTextItem *text = scene->addText(nodeVillage[i]);
        text->setPos(points[i].x() + 10, points[i].y() - text->boundingRect().height() / 2);
    }
}
class CustomGraphicsView : public QGraphicsView //放大缩小只能点击，不能使用滚轮
{
    Q_OBJECT
public:
    explicit CustomGraphicsView(QWidget* parent = nullptr)
        : QGraphicsView(parent) {}

protected:
    void wheelEvent(QWheelEvent* event) override
    {

    }
};


void MainWindow::shortBtnclicked()
{
    //initMap();
    /*for(int i = 0;i < 54;i++)
    {
        for(int j = 0;j < 54;j++)
        {
            if(i == j)
            {
                grap.addEdge(i,j,0);
            }
            else
            {
                if(Matrix[i][j] == 1)
                {
                    QString s1 = villageToString[i];
                    QString s2 = villageToString[j];
                    coordinates node1 = doubleMap[mapVillage[s1]];
                    coordinates node2 = doubleMap[mapVillage[s2]];
                    int distancesss = Xml.calculateDistance(node1,node2);
                    qDebug()<<"s="<<distancesss;
                    //dist[i][j] = distancesss;
                    grap.addEdge(i,j,distancesss);
                }
                else
                {
                    grap.addEdge(i,j,MAXN);
                }
            }
        }
    }
    grap.floydWarshall();
    qDebug()<<grap.result.size();
    path = grap.result;*/
    initDist();
    floyd(54);
    QString textBox1 = ui->startBox->currentText();
    QString textBox2 = ui->endBox->currentText();
    int idx1 = villageToInt[textBox1];
    int idx2 = villageToInt[textBox2];
    //grap.printShortestPath(idx1,idx2);
    getPath(idx1,idx2);
    showShortPath();
    //path.clear();
}
