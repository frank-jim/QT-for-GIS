#include "filereader.h"
#include <gdal_priv.h>
#include <qjsonarray.h>
#include <qjsonobject.h>

FileReader::FileReader()
{

}

void FileReader::GeoJsonReader(QJsonDocument *geojson, SfsLayer *layer)
{
    double maxY = DBL_MIN,minY=DBL_MAX,maxX=DBL_MIN,minX=DBL_MAX;
    QString type = (*geojson)["type"].toString();
    //type ����Ϊ���� һ��Ϊ���ζ���һ��ΪҪ�ض���

    if(type=="Point")
    {
        double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
        SfsLayer *layer = new SfsLayer();
        SfsPoint *pt = new SfsPoint();
        QJsonArray array = (*geojson)["coordinates"].toArray();
        pt->x = array[0].toDouble();
        pt->y = array[1].toDouble();
        layer->geometries->append(pt);
        topY = pt->y>topY?pt->y:topY;
        buttomY = buttomY<pt->y?buttomY:pt->y;
        leftX = leftX<pt->x?leftX:pt->x;
        rightX = rightX>pt->x?rightX:pt->x;
        layer->bbox->setBoundary(topY,buttomY,leftX,rightX);
    }
    else if (type=="FeatureCollection")
    {
        QJsonArray Features =(*geojson)["features"].toArray();
        QJsonObject feature;
        QString Geometry;
        for(int i=0;i<Features.size();i++){
            feature = Features[i].toObject();
            QJsonObject geometry = feature["geometry"].toObject();
            Geometry = geometry["type"].toString();
            if(Geometry=="Point")
            {
                double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                SfsPoint *pt = new SfsPoint();
                QJsonArray crds = geometry["coordinates"].toArray();
                pt->x = crds[0].toDouble();
                pt->y = crds[1].toDouble();
                layer->geometries->append(pt);
                topY = pt->y>topY?pt->y:topY;
                buttomY = buttomY<pt->y?buttomY:pt->y;
                leftX = leftX<pt->x?leftX:pt->x;
                rightX = rightX>pt->x?rightX:pt->x;

                maxY = topY>maxY?topY:maxY;
                minY = buttomY<minY?buttomY:minY;
                maxX = rightX>maxX?rightX:maxX;
                minX = leftX<minX?leftX:minX;
            }
            if(Geometry=="Polygon"){
               double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
               QJsonArray boundary_array = geometry["coordinates"].toArray();//��������β�Σ�����Ϊ���ڵ�ģ������п׵Ķ����
               SfsPolygon *polygon = new SfsPolygon();
               SfsLineString *boundary = new SfsLineString();
               polygon->boundaries->append(boundary);
               for(int m1=0;m1<boundary_array.size();m1++){
                   QJsonArray points_array = boundary_array[m1].toArray();
                   for(int m2=0;m2<points_array.size();m2++)
                   {
                       //�����������
                       QJsonArray point_array = points_array[m2].toArray();
                       for (int m3=0;m3<point_array.size();m3++) {
                           //�������
                           SfsPoint * pt = new SfsPoint();
                           boundary->pts->append(pt);
                           pt->x = point_array[0].toDouble();
                           pt->y = point_array[1].toDouble();
                           topY = pt->y>topY?pt->y:topY;
                           buttomY = buttomY<pt->y?buttomY:pt->y;
                           leftX = leftX<pt->x?leftX:pt->x;
                           rightX = rightX>pt->x?rightX:pt->x;
                       }
                   }

               }
               polygon->bbox->setBoundary(topY,buttomY,leftX,rightX);
               maxY = topY>maxY?topY:maxY;
               minY = buttomY<minY?buttomY:minY;
               maxX = rightX>maxX?rightX:maxX;
               minX = leftX<minX?leftX:minX;
            }
            if(Geometry=="LineString"){
                double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                SfsLineString *lineString = new SfsLineString();
                layer->geometries->append(lineString);
                QJsonArray crds = geometry["coordinates"].toArray();
                for(int j=0;j<crds.size();j++){
                    QJsonArray crd = crds[j].toArray();
                    SfsPoint *pt = new SfsPoint();
                    lineString->pts->append(pt);
                    pt->x = crd[0].toDouble();
                    pt->y = crd[1].toDouble();
                    topY = pt->y>topY?pt->y:topY;
                    buttomY = buttomY<pt->y?buttomY:pt->y;
                    leftX = leftX<pt->x?leftX:pt->x;
                    rightX = rightX>pt->x?rightX:pt->x;
                }
                lineString->bbox->setBoundary(topY,buttomY,leftX,rightX);
                maxY = topY>maxY?topY:maxY;
                minY = buttomY<minY?buttomY:minY;
                maxX = rightX>maxX?rightX:maxX;
                minX = leftX<minX?leftX:minX;
            }
            if(Geometry=="MultiPoint"){
                double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                QJsonArray crds = geometry["coordinates"].toArray();
                for(int j=0;j<crds.size();j++){
                    QJsonArray crd = crds[j].toArray();
                    SfsPoint *pt = new SfsPoint();
                    //�޸ģ����ٴ洢multi������
                    layer->geometries->append(pt);
                    pt->x = crd[0].toDouble();
                    pt->y = crd[1].toDouble();
                    topY = pt->y>topY?pt->y:topY;
                    buttomY = buttomY<pt->y?buttomY:pt->y;
                    leftX = leftX<pt->x?leftX:pt->x;
                    rightX = rightX>pt->x?rightX:pt->x;
                }
                maxY = topY>maxY?topY:maxY;
                minY = buttomY<minY?buttomY:minY;
                maxX = rightX>maxX?rightX:maxX;
                minX = leftX<minX?leftX:minX;

            }
            if(Geometry=="MultiLineString"){

                QJsonArray array = geometry["coordinates"].toArray();
                for(int k = 0;k<array.size();k++){
                    double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                    SfsLineString *LineString = new SfsLineString();
                    layer->geometries->append(LineString);
                    QJsonArray crd_array = array[k].toArray();
                    for (int j=0;j<crd_array.size();j++) {
                        //ÿ�ζ������һ�������飨array��ʾ��
                        QJsonArray crds = crd_array[i].toArray();
                        SfsPoint * pt = new SfsPoint();
                        pt->x = crds[0].toDouble();
                        pt->y = crds[1].toDouble();
                        topY = pt->y>topY?pt->y:topY;
                        buttomY = buttomY<pt->y?buttomY:pt->y;
                        leftX = leftX<pt->x?leftX:pt->x;
                        rightX = rightX>pt->x?rightX:pt->x;
                        pt->bbox->setBoundary(topY,buttomY,leftX,rightX);
                        LineString->pts->append(pt);
                    }
                    LineString->bbox->setBoundary(topY,buttomY,leftX,rightX);
                    maxY = topY>maxY?topY:maxY;
                    minY = buttomY<minY?buttomY:minY;
                    maxX = rightX>maxX?rightX:maxX;
                    minX = leftX<minX?leftX:minX;
                }
            }
            if(Geometry=="MultiPolygon"){
                QJsonArray polygons_array = geometry["coordinates"].toArray();//�� �� ����β��
                for(int m1 =0;m1<polygons_array.size();m1++){
                   double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                   SfsPolygon *polygon = new SfsPolygon();
                   layer->geometries->append(polygon);
                   QJsonArray polygon_array = polygons_array[m1].toArray();//��������β��
                   for(int m2=0;m2<polygon_array.size();m2++){
                       //��������β�Σ�����Ϊ���ڵ�ģ������п׵Ķ����
                       SfsLineString *boundary = new SfsLineString();
                       polygon->boundaries->append(boundary);
                       QJsonArray points_array = polygon_array[m2].toArray();
                       for(int m3=0;m3<points_array.size();m3++)
                       {
                           //�����������
                           QJsonArray point_array = points_array[m3].toArray();
                           for (int m4=0;m4<point_array.size();m4++) {
                               //ÿ�ζ������һ�������飨array��ʾ��
                               SfsPoint * pt = new SfsPoint();
                               boundary->pts->append(pt);
                               pt->x = point_array[0].toDouble();
                               pt->y = point_array[1].toDouble();
                               buttomY = buttomY<pt->y?buttomY:pt->y;
                               leftX = leftX<pt->x?leftX:pt->x;
                               rightX = rightX>pt->x?rightX:pt->x;
                           }
                       }

                   }
                   polygon->bbox->setBoundary(topY,buttomY,leftX,rightX);
                   maxY = topY>maxY?topY:maxY;
                   minY = buttomY<minY?buttomY:minY;
                   maxX = rightX>maxX?rightX:maxX;
                   minX = leftX<minX?leftX:minX;
                }
            }
            //ͼ�㷶Χֵ
        }
        layer->bbox->setBoundary(maxY,minY,minX,minX);
    }
    else if (type=="GeometryCollection") {
        QJsonArray geometry_array = (*geojson)["geometries"].toArray();
        for(int j=0;j<geometry_array.size();j++){
            QJsonObject geometry = geometry_array[j].toObject();//����Ҫ�ؼ���
            QString Geometry = geometry["type"].toString();//����Ҫ�ص�����
            if(Geometry=="Point")
            {
                double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                SfsPoint *pt = new SfsPoint();
                QJsonArray crds = geometry["coordinates"].toArray();
                pt->x = crds[0].toDouble();
                pt->y = crds[1].toDouble();
                topY = pt->y>topY?pt->y:topY;
                buttomY = buttomY<pt->y?buttomY:pt->y;
                leftX = leftX<pt->x?leftX:pt->x;
                rightX = rightX>pt->x?rightX:pt->x;

                layer->geometries->append(pt);
                maxY = topY>maxY?topY:maxY;
                minY = buttomY<minY?buttomY:minY;
                maxX = rightX>maxX?rightX:maxX;
                minX = leftX<minX?leftX:minX;
            }
            if(Geometry=="Polygon"){
               double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
               QJsonArray polygon_array = geometry["coordinates"].toArray();//
               SfsPolygon *polygon = new SfsPolygon();
               layer->geometries->append(polygon);
                   for(int m1=0;m1<polygon_array.size();m1++){
                   //��ı߽��Σ��ڱ߽����߽�
                   SfsLineString *boundary = new SfsLineString();
                   polygon->boundaries->append(boundary);
                   QJsonArray points_array = polygon_array[m1].toArray();
                   for(int m2=0;m2<points_array.size();m2++)
                   {
                       //�����������
                       QJsonArray point_array = points_array[m2].toArray();
                       SfsPoint * pt = new SfsPoint();
                       pt->x = point_array[0].toDouble();
                       pt->y = point_array[1].toDouble();
                       boundary->pts->append(pt);
                       topY = pt->y>topY?pt->y:topY;
                       buttomY = buttomY<pt->y?buttomY:pt->y;
                       leftX = leftX<pt->x?leftX:pt->x;
                       rightX = rightX>pt->x?rightX:pt->x;

                   }
               }
               polygon->bbox->setBoundary(topY,buttomY,leftX,rightX);
               maxY = topY>maxY?topY:maxY;
               minY = buttomY<minY?buttomY:minY;
               maxX = rightX>maxX?rightX:maxX;
               minX = leftX<minX?leftX:minX;
            }
            if(Geometry=="LineString"){
                double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                SfsLineString *lineString = new SfsLineString();
                layer->geometries->append(lineString);
                QJsonArray crds = geometry["coordinates"].toArray();
                for(int j=0;j<crds.size();j++){
                    QJsonArray crd = crds[j].toArray();
                    SfsPoint *pt = new SfsPoint();
                    lineString->pts->append(pt);
                    pt->x = crd[0].toDouble();
                    pt->y = crd[1].toDouble();
                    topY = pt->y>topY?pt->y:topY;
                    buttomY = buttomY<pt->y?buttomY:pt->y;
                    leftX = leftX<pt->x?leftX:pt->x;
                    rightX = rightX>pt->x?rightX:pt->x;
                }
                lineString->bbox->setBoundary(topY,buttomY,leftX,rightX);
                maxY = topY>maxY?topY:maxY;
                minY = buttomY<minY?buttomY:minY;
                maxX = rightX>maxX?rightX:maxX;
                minX = leftX<minX?leftX:minX;
            }
            if(Geometry=="MultiPoint"){
                double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                QJsonArray crds = geometry["coordinates"].toArray();
                for(int j=0;j<crds.size();j++){
                    QJsonArray crd = crds[j].toArray();
                    SfsPoint *pt = new SfsPoint();
                    layer->geometries->append(pt);
                    pt->x = crd[0].toDouble();
                    pt->y = crd[1].toDouble();
                    topY = pt->y>topY?pt->y:topY;
                    buttomY = buttomY<pt->y?buttomY:pt->y;
                    leftX = leftX<pt->x?leftX:pt->x;
                    rightX = rightX>pt->x?rightX:pt->x;
                }
                maxY = topY>maxY?topY:maxY;
                minY = buttomY<minY?buttomY:minY;
                maxX = rightX>maxX?rightX:maxX;
                minX = leftX<minX?leftX:minX;
            }
            if(Geometry=="MultiLineString"){
                QJsonArray array = geometry["coordinates"].toArray();
                for(int k = 0;k<array.size();k++){
                    double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                    SfsLineString *lineString = new SfsLineString();
                    layer->geometries->append(lineString);
                    QJsonArray crd_array = array[k].toArray();
                    for (int j=0;j<crd_array.size();j++) {
                        //ÿ�ζ������һ�������飨array��ʾ��
                        QJsonArray crds = crd_array[j].toArray();
                        SfsPoint * pt = new SfsPoint();
                        pt->x = crds[0].toDouble();
                        pt->y = crds[1].toDouble();
                        topY = pt->y>topY?pt->y:topY;
                        buttomY = buttomY<pt->y?buttomY:pt->y;
                        leftX = leftX<pt->x?leftX:pt->x;
                        rightX = rightX>pt->x?rightX:pt->x;

                        lineString->pts->append(pt);
                    }
                    lineString->bbox->setBoundary(topY,buttomY,leftX,rightX);
                    maxY = topY>maxY?topY:maxY;
                    minY = buttomY<minY?buttomY:minY;
                    maxX = rightX>maxX?rightX:maxX;
                    minX = leftX<minX?leftX:minX;
                }
            }
            if(Geometry=="MultiPolygon"){
                QJsonArray polygons_array = geometry["coordinates"].toArray();//�� �� ����β��
                for(int m1 =0;m1<polygons_array.size();m1++){
                    double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                    SfsPolygon *polygon = new SfsPolygon();
                    layer->geometries->append(polygon);
                    QJsonArray polygon_array = polygons_array[m1].toArray();//��������β��
                    for(int m2=0;m2<polygon_array.size();m2++){
                        //��������β�Σ�����Ϊ���ڵ�ģ������п׵Ķ����
                        SfsLineString *boundary = new SfsLineString();
                        polygon->boundaries->append(boundary);
                        QJsonArray points_array = polygon_array[m2].toArray();
                        for(int m3=0;m3<points_array.size();m3++)
                        {
                            //�����������
                            QJsonArray point_array = points_array[m3].toArray();
                            for (int m4=0;m4<point_array.size();m4++) {
                                //ÿ�ζ������һ�������飨array��ʾ��
                                SfsPoint * pt = new SfsPoint();
                                boundary->pts->append(pt);
                                pt->x = point_array[0].toDouble();
                                pt->y = point_array[1].toDouble();
                                topY = pt->y>topY?pt->y:topY;
                                buttomY = buttomY<pt->y?buttomY:pt->y;
                                leftX = leftX<pt->x?leftX:pt->x;
                                rightX = rightX>pt->x?rightX:pt->x;
                            }
                        }                        
                    }
                    maxY = topY>maxY?topY:maxY;
                    minY = buttomY<minY?buttomY:minY;
                    maxX = rightX>maxX?rightX:maxX;
                    minX = leftX<minX?leftX:minX;
                    polygon->bbox->setBoundary(topY,buttomY,leftX,rightX);
                 }
            }
        }
        layer->bbox->setBoundary(maxY,minY,minX,minX);

    }
    else if (type=="Polygon") {
        double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
        SfsPolygon *polygon = new SfsPolygon();
        layer->geometries->append(polygon);
        QJsonArray boundary_array = (*geojson)["coordinates"].toArray();//��������β�Σ�����Ϊ���ڵ�ģ������п׵Ķ����
        for(int m1=0;m1<boundary_array.size();m1++){
            SfsLineString *boundary = new SfsLineString();
            polygon->boundaries->append(boundary);
            QJsonArray points_array = boundary_array[m1].toArray();
            for(int m2=0;m2<points_array.size();m2++)
            {
                //�����������
                QJsonArray point_array = points_array[m2].toArray();
                for (int m3=0;m3<point_array.size();m3++) {
                    //�������
                    SfsPoint * pt = new SfsPoint();
                    boundary->pts->append(pt);
                    pt->x = point_array[0].toDouble();
                    pt->y = point_array[1].toDouble();
                    topY = pt->y>topY?pt->y:topY;
                    buttomY = buttomY<pt->y?buttomY:pt->y;
                    leftX = leftX<pt->x?leftX:pt->x;
                    rightX = rightX>pt->x?rightX:pt->x;
                    pt->bbox->setBoundary(topY,buttomY,leftX,rightX);
                }
            }

        }
        polygon->bbox->setBoundary(topY,buttomY,leftX,rightX);
        layer->bbox->setBoundary(topY,buttomY,leftX,rightX);
    }
    else if (type=="LineString") {
        double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
        SfsLineString *lineString = new SfsLineString();
        layer->geometries->append(lineString);
        QJsonArray crds = (*geojson)["coordinates"].toArray();
        for(int j=0;j<crds.size();j++){
            QJsonArray crd = crds[j].toArray();
            SfsPoint *pt = new SfsPoint();
            lineString->pts->append(pt);
            pt->x = crd[0].toDouble();
            pt->y = crd[1].toDouble();
            topY = pt->y>topY?pt->y:topY;
            buttomY = buttomY<pt->y?buttomY:pt->y;
            leftX = leftX<pt->x?leftX:pt->x;
            rightX = rightX>pt->x?rightX:pt->x;
            pt->bbox->setBoundary(topY,buttomY,leftX,rightX);
        }
        lineString->bbox->setBoundary(topY,buttomY,leftX,rightX);
        layer->bbox->setBoundary(topY,buttomY,leftX,rightX);
    }
    else if (type=="MultiPoint") {
        double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
        QJsonArray crds = (*geojson)["coordinates"].toArray();
        for(int j=0;j<crds.size();j++){
            QJsonArray crd = crds[j].toArray();
            SfsPoint *pt = new SfsPoint();
            layer->geometries->append(pt);
            pt->x = crd[0].toDouble();
            pt->y = crd[1].toDouble();
            topY = pt->y>topY?pt->y:topY;
            buttomY = buttomY<pt->y?buttomY:pt->y;
            leftX = leftX<pt->x?leftX:pt->x;
            rightX = rightX>pt->x?rightX:pt->x;
        }
        layer->bbox->setBoundary(topY,buttomY,leftX,rightX);
    }
    else if (type=="MultiLineString") {
        QJsonArray array = (*geojson)["coordinates"].toArray();
        for(int k = 0;k<array.size();k++){
            double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
            SfsLineString *lineString = new SfsLineString();
            layer->geometries->append(lineString);
            QJsonArray crd_array = array[k].toArray();
            for (int j=0;j<crd_array.size();j++) {
                //ÿ�ζ������һ�������飨array��ʾ��
                QJsonArray crds = crd_array[j].toArray();
                SfsPoint * pt = new SfsPoint();
                pt->x = crds[0].toDouble();
                pt->y = crds[1].toDouble();
                topY = pt->y>topY?pt->y:topY;
                buttomY = buttomY<pt->y?buttomY:pt->y;
                leftX = leftX<pt->x?leftX:pt->x;
                rightX = rightX>pt->x?rightX:pt->x;
                pt->bbox->setBoundary(topY,buttomY,leftX,rightX);
                lineString->pts->append(pt);
            }
            lineString->bbox->setBoundary(topY,buttomY,leftX,rightX);
            maxY = topY>maxY?topY:maxY;
            minY = buttomY<minY?buttomY:minY;
            maxX = rightX>maxX?rightX:maxX;
            minX = leftX<minX?leftX:minX;
        }
        layer->bbox->setBoundary(maxY,minY,minX,minX);

    }
    else if (type=="MultiPolygon") {
        QJsonArray polygons_array = (*geojson)["coordinates"].toArray();//�� �� ����β��
        for(int m1 =0;m1<polygons_array.size();m1++){
            double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
            SfsPolygon *polygon = new SfsPolygon();
            layer->geometries->append(polygon);
            QJsonArray polygon_array = polygons_array[m1].toArray();//��������β��
            for(int m2=0;m2<polygon_array.size();m2++){
                //��������β�Σ�����Ϊ���ڵ�ģ������п׵Ķ����
                SfsLineString *boundary = new SfsLineString();
                polygon->boundaries->append(boundary);
                QJsonArray points_array = polygon_array[m2].toArray();
                for(int m3=0;m3<points_array.size();m3++)
                {
                    //�����������
                    QJsonArray point_array = points_array[m3].toArray();
                    for (int m4=0;m4<point_array.size();m4++) {
                        //ÿ�ζ������һ�������飨array��ʾ��
                        SfsPoint * pt = new SfsPoint();
                        boundary->pts->append(pt);
                        pt->x = point_array[0].toDouble();
                        pt->y = point_array[1].toDouble();
                        topY = pt->y>topY?pt->y:topY;
                        buttomY = buttomY<pt->y?buttomY:pt->y;
                        leftX = leftX<pt->x?leftX:pt->x;
                        rightX = rightX>pt->x?rightX:pt->x;
                        pt->bbox->setBoundary(topY,buttomY,leftX,rightX);
                    }
                }

            }
            maxY = topY>maxY?topY:maxY;
            minY = buttomY<minY?buttomY:minY;
            maxX = rightX>maxX?rightX:maxX;
            minX = leftX<minX?leftX:minX;
            polygon->bbox->setBoundary(topY,buttomY,leftX,rightX);
         }
        layer->bbox->setBoundary(maxY,minY,minX,minX);
    }

    qDebug()<<"Loading successed";
}

void FileReader::ShpfileReader(GDALDataset *pDoc, SfsLayer *layer)
{

    //��ȡshp�ļ�
    double maxY = DBL_MIN,minY=DBL_MAX,maxX=DBL_MIN,minX=DBL_MAX;//����ͼ��ķ�Χ
    OGRLayer *ogrlayer;//shpͼ��
    OGRGeometry *poGeometry;//shp���ζ���

    for (int i=0;i<pDoc->GetLayers().size();i++) {
        qDebug()<<"Layer";
        //ͼ��ѭ��
        //����ͼ�㣬ÿ�ζ���һ��ͼ����½�һ��ͼ�����
        ogrlayer = pDoc->GetLayer(i);
        layer->setName(ogrlayer->GetName());
        OGRFeatureDefn *poFDefn = ogrlayer->GetLayerDefn();//ͼ��Ҫ�صĶ�������
        OGRFeature *ogrfeature;//ͼ���ڵ�Ҫ��
        int iGeomField;//����Ҫ�ص���������
        int proCount = poFDefn->GetFieldCount();//ÿ��ͼ���ڲ������������ݵĸ���
        int nGeomFieldCount;
        while ((ogrfeature=ogrlayer->GetNextFeature())!=nullptr) {
            QString name =ogrfeature->GetFieldAsString(8);
            Properties *properties = new Properties();
            for (int j=0;j<proCount;j++) {
                OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn(j);
                switch (poFieldDefn->GetType()){
                case OFTInteger:
                    {
                        properties->ProType->append(Int_PRO);
                        properties->ProName->append(poFDefn->GetFieldDefn(j)->GetNameRef());
                        int *value = new int;
                        (*value) = ogrfeature->GetFieldAsInteger(j);
                        properties->ProValue->append(value);
                        break;
                    }
                case OFTInteger64:
                    {
                        properties->ProType->append(Int_PRO);
                        properties->ProName->append(poFDefn->GetFieldDefn(j)->GetNameRef());
                        int *value = new int;
                        (*value) = ogrfeature->GetFieldAsInteger64(j);
                        properties->ProValue->append(value);
                        break;
                    }
                case OFTString:
                    {
                        properties->ProType->append(String_PRO);
                        properties->ProName->append(poFDefn->GetFieldDefn(j)->GetNameRef());
                        QString *value = new QString;
                        (*value) = ogrfeature->GetFieldAsString(j);
                        properties->ProValue->append(value);
                        break;
                    }
                case OFTReal:
                    {
                        properties->ProType->append(String_PRO);
                        properties->ProName->append(poFDefn->GetFieldDefn(j)->GetNameRef());
                        double *value = new double;
                        (*value) = ogrfeature->GetFieldAsDouble(j);
                        properties->ProValue->append(value);
                        break;
                    }
                case OFTWideString:
                    {
                        properties->ProType->append(String_PRO);
                        properties->ProName->append(poFDefn->GetFieldDefn(j)->GetNameRef());
                        QString *value = new QString;
                        (*value) = ogrfeature->GetFieldAsString(j);
                        properties->ProValue->append(value);
                        break;
                    }
                }
            }
            nGeomFieldCount = ogrfeature->GetGeomFieldCount();//�õ�feature�ļ��ζ���ĸ���
            //ѭ������
            for (iGeomField=0;iGeomField<nGeomFieldCount;iGeomField++) {
                poGeometry = ogrfeature->GetGeomFieldRef(iGeomField);
                //���ݼ����������� ����
                if(poGeometry->getGeometryType()==wkbPolygon)
                {
                    //����ǵ�һ����Ҫ��
                    double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                    SfsPolygon *polygon = new SfsPolygon();
                    layer->geometries->append(polygon);
                    OGRPolygon *ogrpolygon = poGeometry->toPolygon();   //��������β�Σ�����Ϊ���ڵ�ģ������п׵Ķ����
                    SfsLineString *boundaryOut = new SfsLineString();//��Ҫ�ض�����߽磬�ȴ洢��߽磬SfsPolygon Ĭ�ϵ�һ��������߽�
                    polygon->boundaries->append(boundaryOut);
                    OGRLineString *ogrlineString =  ogrpolygon->getExteriorRing();
                    for(int m2=0;m2<ogrlineString->getNumPoints();m2++)
                    {
                        //�����������
                        SfsPoint * pt = new SfsPoint();
                        boundaryOut->pts->append(pt);
                        pt->x = ogrlineString->getX(m2);
                        pt->y = ogrlineString->getY(m2);
                        topY = pt->y>topY?pt->y:topY;
                        buttomY = buttomY<pt->y?buttomY:pt->y;
                        leftX = leftX<pt->x?leftX:pt->x;
                        rightX = rightX>pt->x?rightX:pt->x;
                    }
                    //�ڶ����ѭ��
                    for (int m3=0;m3<ogrpolygon->getNumInteriorRings();m3++) {
                        SfsLineString *boundaryIn = new SfsLineString();
                        OGRLineString * linestring = ogrpolygon->getInteriorRing(m3);
                        polygon->boundaries->append(boundaryIn);
                        for(int m4=0;m4<linestring->getNumPoints();m4++)
                        {
                            //�����������
                            SfsPoint * pt = new SfsPoint();
                            boundaryIn->pts->append(pt);
                            pt->x = linestring->getX(m4);
                            pt->y = linestring->getY(m4);

                            topY = pt->y>topY?pt->y:topY;
                            buttomY = buttomY<pt->y?buttomY:pt->y;
                            leftX = leftX<pt->x?leftX:pt->x;
                            rightX = rightX>pt->x?rightX:pt->x;
                        }

                    }
                    polygon->properties = properties;
                    polygon->setName(name);
                    polygon->bbox->setBoundary(topY,buttomY,leftX,rightX);
                    maxY = topY>maxY?topY:maxY;
                    minY = buttomY<minY?buttomY:minY;
                    maxX = rightX>maxX?rightX:maxX;
                    minX = leftX<minX?leftX:minX;
                }
                else if (poGeometry->getGeometryType()==wkbPoint) {
                    //����ǵ�����
                    double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                    OGRPoint *point = poGeometry->toPoint();
                    SfsPoint *pt = new SfsPoint();
                    layer->geometries->append(pt);
                    pt->x = point->getX();
                    pt->y = point->getY();
                    //��������
                    pt->properties =  properties;

                    topY = pt->y>topY?pt->y:topY;
                    buttomY = buttomY<pt->y?buttomY:pt->y;
                    leftX = leftX<pt->x?leftX:pt->x;
                    rightX = rightX>pt->x?rightX:pt->x;
                    //��ȡͼ��ķ�Χ
                    maxY = topY>maxY?topY:maxY;
                    minY = buttomY<minY?buttomY:minY;
                    maxX = rightX>maxX?rightX:maxX;
                    minX = leftX<minX?leftX:minX;
                }
                else if (poGeometry->getGeometryType()==wkbLineString) {
                    double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                    SfsLineString *lineString = new SfsLineString();
                    layer->geometries->append(lineString);
                    OGRLineString *ogrLineString = poGeometry->toLineString();
                    for (int m1=0;m1<ogrLineString->getNumPoints();m1++) {
                        SfsPoint *pt = new SfsPoint();
                        lineString->pts->append(pt);
                        pt->x = ogrLineString->getX(m1);
                        pt->y = ogrLineString->getY(m1);

                        topY = pt->y>topY?pt->y:topY;
                        buttomY = buttomY<pt->y?buttomY:pt->y;
                        leftX = leftX<pt->x?leftX:pt->x;
                        rightX = rightX>pt->x?rightX:pt->x;
                    }
                    lineString->bbox->setBoundary(topY,buttomY,leftX,rightX);
                    //��ȡͼ��ķ�Χ
                    lineString->properties = properties;
                    maxY = topY>maxY?topY:maxY;
                    minY = buttomY<minY?buttomY:minY;
                    maxX = rightX>maxX?rightX:maxX;
                    minX = leftX<minX?leftX:minX;
                }
                else if (poGeometry->getGeometryType()==wkbMultiPoint) {
                    //����Ƕ������
                    double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                    OGRMultiPoint *ogrMultiPoint = poGeometry->toMultiPoint();
                    for (int m1=0;m1<ogrMultiPoint->getNumGeometries();m1++) {
                        SfsPoint *pt = new SfsPoint();
                        layer->geometries->append(pt);
                        pt->x = (*(ogrMultiPoint->begin()+m1))->getX();
                        pt->y = (*(ogrMultiPoint->begin()+m1))->getY();

                        topY = pt->y>topY?pt->y:topY;
                        buttomY = buttomY<pt->y?buttomY:pt->y;
                        leftX = leftX<pt->x?leftX:pt->x;
                        rightX = rightX>pt->x?rightX:pt->x;
                    }
                    //��ȡͼ��ķ�Χ
                    maxY = topY>maxY?topY:maxY;
                    minY = buttomY<minY?buttomY:minY;
                    maxX = rightX>maxX?rightX:maxX;
                    minX = leftX<minX?leftX:minX;
                }
                else if (poGeometry->getGeometryType()==wkbMultiLineString) {
                    //���Ϊ����
                    OGRMultiLineString *ogrMultilineString = poGeometry->toMultiLineString();
                    for (int m1=0;m1<ogrMultilineString->getNumGeometries();m1++) {
                        //���߲��
                        double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                        OGRLineString *ogrLineString = *(ogrMultilineString->begin()+m1);
                        SfsLineString *lineString = new SfsLineString();
                        layer->geometries->append(lineString);
                        for (int m2=0;m2<ogrLineString->getNumPoints();m2++) {
                            SfsPoint *pt = new SfsPoint();
                            lineString->pts->append(pt);
                            pt->x = ogrLineString->getX(m2);

                            topY = pt->y>topY?pt->y:topY;
                            buttomY = buttomY<pt->y?buttomY:pt->y;
                            leftX = leftX<pt->x?leftX:pt->x;
                            rightX = rightX>pt->x?rightX:pt->x;
                            pt->y = ogrLineString->getY(m2);
                        }
                        lineString->bbox->setBoundary(topY,buttomY,leftX,rightX);
                        maxY = topY>maxY?topY:maxY;
                        minY = buttomY<minY?buttomY:minY;
                        maxX = rightX>maxX?rightX:maxX;
                        minX = leftX<minX?leftX:minX;
                    }

                }
                else if (poGeometry->getGeometryType()==wkbMultiPolygon) {
                    //�� ����β��
                    OGRMultiPolygon *ogrMultiPolygon =  poGeometry->toMultiPolygon();
                    for (int m1=0;m1<ogrMultiPolygon->getNumGeometries();m1++) {
                        //������β��
                        double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                        SfsPolygon *polygon = new SfsPolygon();
                        layer->geometries->append(polygon);
                        OGRPolygon *ogrpolygon = *(ogrMultiPolygon->begin()+m1);
                        SfsLineString *boundaryOut = new SfsLineString();//��Ҫ�ض�����߽磬�ȴ洢��߽磬SfsPolygon Ĭ�ϵ�һ��������߽�
                        OGRLineString *ogrlineString =  ogrpolygon->getExteriorRing();
                        polygon->boundaries->append(boundaryOut);
                        for(int m2=0;m2<ogrlineString->getNumPoints();m2++)
                        {
                            //�����������
                            SfsPoint * pt = new SfsPoint();
                            boundaryOut->pts->append(pt);
                            pt->x = ogrlineString->getX(m2);
                            pt->y = ogrlineString->getY(m2);

                            topY = pt->y>topY?pt->y:topY;
                            buttomY = buttomY<pt->y?buttomY:pt->y;
                            leftX = leftX<pt->x?leftX:pt->x;
                            rightX = rightX>pt->x?rightX:pt->x;
                        }
                        //�ڶ����ѭ��
                        for (int m3=0;m3<ogrpolygon->getNumInteriorRings();m3++) {
                            SfsLineString *boundaryIn = new SfsLineString();
                            OGRLineString * linestring = ogrpolygon->getInteriorRing(m3);
                            polygon->boundaries->append(boundaryIn);
                            for(int m2=0;m2<linestring->getNumPoints();m2++)
                            {
                                //�����������
                                SfsPoint * pt = new SfsPoint();
                                boundaryIn->pts->append(pt);
                                pt->x = linestring->getX(m2);
                                pt->y = linestring->getY(m2);

                                topY = pt->y>topY?pt->y:topY;
                                buttomY = buttomY<pt->y?buttomY:pt->y;
                                leftX = leftX<pt->x?leftX:pt->x;
                                rightX = rightX>pt->x?rightX:pt->x;
                            }
                        }
                        maxY = topY>maxY?topY:maxY;
                        minY = buttomY<minY?buttomY:minY;
                        maxX = rightX>maxX?rightX:maxX;
                        minX = leftX<minX?leftX:minX;
                        polygon->bbox->setBoundary(topY,buttomY,leftX,rightX);
                        polygon->setName(name);
                    }
                }
            }
        }
        layer->bbox->setBoundary(maxY,minY,maxX,minX);
    }
}

void FileReader::LoadPostGIS(GDALDataset *pDoc, SfsLayer *layer,QString layerName)
{
    //���ݿ�ͼ���ļ�
    OGRLayer *ogrlayer;//shpͼ��
    OGRGeometry *poGeometry;//shp���ζ���
    ogrlayer = pDoc->GetLayerByName(layerName.toStdString().c_str());
    if(ogrlayer==nullptr)
        LayerNone();
    else{
        OGRFeatureDefn *poFDefn = ogrlayer->GetLayerDefn();//ͼ��Ҫ�صĶ�������
        OGRFeature *ogrfeature;//ͼ���ڵ�Ҫ��

        int iGeomField;//����Ҫ�ص�λ��
        int nGeomFieldCount;//ÿ��feature����Ҫ�صĸ���
        while ((ogrfeature=ogrlayer->GetNextFeature())!=nullptr) {
            nGeomFieldCount = ogrfeature->GetGeomFieldCount();//�õ�feature�ļ��ζ���ĸ���
            //ѭ������
            for (iGeomField=0;iGeomField<nGeomFieldCount;iGeomField++) {
                poGeometry = ogrfeature->GetGeomFieldRef(iGeomField);
                //���ݼ����������� ����
                if(poGeometry->getGeometryType()==wkbPolygon)
                {
                    //����ǵ�һ����Ҫ��
                    SfsPolygon *polygon = new SfsPolygon();
                    layer->geometries->append(polygon);
                    OGRPolygon *ogrpolygon = poGeometry->toPolygon();   //��������β�Σ�����Ϊ���ڵ�ģ������п׵Ķ����
                    SfsLineString *boundaryOut = new SfsLineString();//��Ҫ�ض�����߽磬�ȴ洢��߽磬SfsPolygon Ĭ�ϵ�һ��������߽�
                    OGRLineString *ogrlineString =  ogrpolygon->getExteriorRing();
                    for(int m2=0;m2<ogrlineString->getNumPoints();m2++)
                    {
                        //�����������
                        SfsPoint * pt = new SfsPoint();
                        boundaryOut->pts->append(pt);
                        pt->x = ogrlineString->getX(m2);
                        pt->y = ogrlineString->getY(m2);
                    }
                    //�ڶ����ѭ��
                    for (int m3=0;m3<ogrpolygon->getNumInteriorRings();m3++) {
                        SfsLineString *boundaryIn = new SfsLineString();
                        OGRLineString * linestring = ogrpolygon->getInteriorRing(m3);
                        polygon->boundaries->append(boundaryIn);
                        for(int m2=0;m2<linestring->getNumPoints();m2++)
                        {
                            //�����������
                            SfsPoint * pt = new SfsPoint();
                            boundaryIn->pts->append(pt);
                            pt->x = linestring->getX(m2);
                            pt->y = linestring->getY(m2);
                        }

                    }

                }
                else if (poGeometry->getGeometryType()==wkbPoint) {
                    //����ǵ�����
                    OGRPoint *point = poGeometry->toPoint();
                    SfsPoint *pt = new SfsPoint();
                    layer->geometries->append(pt);
                    pt->x = point->getX();
                    pt->y = point->getY();
                }
                else if (poGeometry->getGeometryType()==wkbLineString) {
                    SfsLineString *lineString = new SfsLineString();
                    layer->geometries->append(lineString);
                    OGRLineString *ogrLineString = poGeometry->toLineString();
                    for (int m1=0;m1<ogrLineString->getNumPoints();m1++) {
                        SfsPoint *pt = new SfsPoint();
                        lineString->pts->append(pt);
                        pt->x = ogrLineString->getX(m1);
                        pt->y = ogrLineString->getY(m1);
                    }
                }
                else if (poGeometry->getGeometryType()==wkbMultiPoint) {
                    //����Ƕ������
                    SfsMultiPoint *MultiPoint = new SfsMultiPoint();
                    layer->geometries->append(MultiPoint);
                    OGRMultiPoint *ogrMultiPoint = poGeometry->toMultiPoint();
                    for (int m1=0;m1<ogrMultiPoint->getNumGeometries();m1++) {
                        SfsPoint *pt = new SfsPoint();
                        layer->geometries->append(pt);
                        pt->x = (*(ogrMultiPoint->begin()+m1))->getX();
                        pt->y = (*(ogrMultiPoint->begin()+m1))->getY();

                    }
                }
                else if (poGeometry->getGeometryType()==wkbMultiLineString) {
                    //���Ϊ����
                    SfsMultiLineString *MultiLineString = new SfsMultiLineString();
                    layer->geometries->append(MultiLineString);
                    OGRMultiLineString *ogrMultilineString = poGeometry->toMultiLineString();
                    for (int m1=0;m1<ogrMultilineString->getNumGeometries();m1++) {
                        //���߲��
                        OGRLineString *ogrLineString = *(ogrMultilineString->begin()+m1);
                        SfsLineString *lineString = new SfsLineString();
                        MultiLineString->lineStrings->append(lineString);
                        for (int m2=0;m2<ogrLineString->getNumPoints();m2++) {
                            SfsPoint *pt = new SfsPoint();
                            lineString->pts->append(pt);
                            pt->x = ogrLineString->getX(m2);
                            pt->y = ogrLineString->getY(m2);
                        }
                    }

                }
                else if (poGeometry->getGeometryType()==wkbMultiPolygon) {
                    //�� ����β��
                    SfsMultiPolygon *MultiPolygon = new SfsMultiPolygon();
                    layer->geometries->append(MultiPolygon);
                    OGRMultiPolygon *ogrMultiPolygon =  poGeometry->toMultiPolygon();
                    for (int m1=0;m1<ogrMultiPolygon->getNumGeometries();m1++) {

                        //������β��
                        SfsPolygon *polygon = new SfsPolygon();
                        MultiPolygon->polygons->append(polygon);
                        OGRPolygon *ogrpolygon = *(ogrMultiPolygon->begin()+m1);
                        SfsLineString *boundaryOut = new SfsLineString();//��Ҫ�ض�����߽磬�ȴ洢��߽磬SfsPolygon Ĭ�ϵ�һ��������߽�
                        OGRLineString *ogrlineString =  ogrpolygon->getExteriorRing();
                        polygon->boundaries->append(boundaryOut);
                        for(int m2=0;m2<ogrlineString->getNumPoints();m2++)
                        {
                            //�����������
                            SfsPoint * pt = new SfsPoint();
                            boundaryOut->pts->append(pt);
                            pt->x = ogrlineString->getX(m2);
                            pt->y = ogrlineString->getY(m2);
                        }
                        //�ڶ����ѭ��
                        for (int m3=0;m3<ogrpolygon->getNumInteriorRings();m3++) {
                            SfsLineString *boundaryIn = new SfsLineString();
                            OGRLineString * linestring = ogrpolygon->getInteriorRing(m3);
                            polygon->boundaries->append(boundaryIn);
                            for(int m2=0;m2<linestring->getNumPoints();m2++)
                            {
                                //�����������
                                SfsPoint * pt = new SfsPoint();
                                boundaryIn->pts->append(pt);
                                pt->x = linestring->getX(m2);
                                pt->y = linestring->getY(m2);
                            }

                        }

                    }
                }
            }
        }

    }

}