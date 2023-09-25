#ifndef RESOLVE_XML_H
#define RESOLVE_XML_H

#include <QXmlStreamReader>
#include <QFile>
#include <unordered_map>
#include <QDebug>
#include <cmath>
#include <vector>
#include <QString>
#include <set>

struct coordinates  //对应的经纬度坐标(double)
{
    int id;
    double lon;
    double lat;
};
struct coordinatesStr  //对应的经纬度坐标(QString)
{
    QString id;
    QString lon;
    QString lat;
};
struct coordinatesInt //对应的经纬度坐标(int)
{
    QString id;
    int x;
    int y;
};

class resolve_xml
{
public:
    double degreesToRadians(double degrees);
    double radiansToDegrees(double radians);
    double calculateDistance(coordinates node1, coordinates node2);
    void saveNode(int id,coordinates node);
    void strChangeDouble(coordinatesStr node);
    void findCommonNode();
    void changeCoor(QString strId);
    std::vector<coordinatesInt>doubleCoor;
    QString minLat;     //最小纬度
    QString maxLat;     //最大经度
    QString minLon;
    QString maxLon;
};




#endif // RESOLVE_XML_H
