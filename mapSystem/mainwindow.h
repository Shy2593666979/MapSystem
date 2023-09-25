#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QXmlStreamReader>
#include <QFile>
#include <unordered_map>
#include <QDebug>
#include <cmath>
#include <vector>
#include <QString>
#include <set>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>

const int INF = 10000000;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    //void on_InButton_clicked();
    //void on_OutButton_clicked();
    void showComBox();
    void zoomInOut();
    void floyd(int n);
    void initDist();
    void getPath(int start, int end);
    void initMap();
private slots:
    void InButton_clicked();
    void OutButton_clicked();
    void showShortPath();
    void shortBtnclicked();

protected:
    void paintEvent(QPaintEvent *event) override;
private:
    Ui::MainWindow *ui;

};
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
    int imortant();
    std::map<QString,coordinatesInt>doubleCoorMap;
    std::vector<coordinatesInt>doubleCoor;
    std::vector<coordinatesInt>doubleWay;
    double minLat;     //最小纬度
    double maxLat;     //最大经度
    double minLon;
    double maxLon;
};
class Graph {
public:
    int vertices = 54;
    std::vector<int>result;
    std::vector<std::vector<int>> distance;
    std::vector<std::vector<int>> next;
    void addEdge(int source, int destination, int weight) {
        distance[source][destination] = weight;
        next[source][destination] = destination;
    }

    void floydWarshall() {
    //distance.assign(54, std::vector<int>(54, INF));
    next.assign(54, std::vector<int>(54, -1));
        for (int k = 0; k < vertices; ++k) {
            for (int i = 0; i < vertices; ++i) {
                for (int j = 0; j < vertices; ++j) {
                    if (distance[i][k] != INF && distance[k][j] != INF && distance[i][k] + distance[k][j] < distance[i][j]) {
                        distance[i][j] = distance[i][k] + distance[k][j];
                        next[i][j] = next[i][k];
                    }
                }
            }
        }
    }

    void printShortestPath(int start, int end) {
        if (distance[start][end] == INF) {
            //cout << "No path exists from Node " << start << " to Node " << end << endl;
            return;
        }

        //cout << "Shortest Path from Node " << start << " to Node " << end << ": ";
        result.push_back(start);

        while (start != end) {
            start = next[start][end];
            //cout << " -> " << start;
            result.push_back(start);
        }

    }
};
#endif // MAINWINDOW_H
