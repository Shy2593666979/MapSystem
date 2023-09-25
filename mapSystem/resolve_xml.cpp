#include "mainwindow.h"
#include <map>

#define Max_Size 10000

std::map<QString,coordinatesStr>mp;
std::map<QString,coordinates>doubleMap;
std::vector<QString>commonNode;
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
    int width = (maxLon.toDouble() - minLon.toDouble()) * 10000;
    int length = (maxLat.toDouble() - minLat.toDouble()) * 10000;
    int x = (coo.lat - minLat.toDouble()) * 10000 ;
    int y = (coo.lon - minLon.toDouble()) * 10000 ;
    cooint.id = strId;
    cooint.x = x;
    cooint.y = y;
    doubleCoor.push_back(cooint);
}
/*void resolve_xml::saveNode(int id,coordinates node)
{
    mp[id] = node;
}*/
int main()
{
    std::vector<QString>strNode;
    std::vector<QString>strWay;
    std::map<QString,std::vector<QString>>mapWay;
    std::map<QString,QString>mapVillage;
    std::vector<QString>nodeVillage;
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
                    Xml.minLat = xml.attributes().value("minlat").toString();
                    Xml.minLon = xml.attributes().value("minlon").toString();
                    Xml.maxLat = xml.attributes().value("maxlat").toString();
                    Xml.maxLon = xml.attributes().value("maxlon").toString();
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
         // qDebug()<<"village: "<<it->first<<"village nodeId: "<<it->second;
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
      fileName.close();
      return 0;
}
