#ifndef SIFTMATCH_H
#define SIFTMATCH_H

#include <QDialog>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

namespace Ui {
class SiftMatch;
}

class SiftMatch : public QDialog
{
    Q_OBJECT
    
public:
    explicit SiftMatch(QWidget *parent = 0);
    ~SiftMatch();

    void CalcFourCorner();//����ͼ2���ĸ��Ǿ�����H�任�������
    
private slots:
    void on_openButton_clicked();

    void on_detectButton_clicked();

    void on_matchButton_clicked();

    void on_restartButton_clicked();

    void on_mosaicButton_clicked();

private:
    Ui::SiftMatch *ui;

    int open_image_number;//��ͼƬ����
    QString name1,name2;//����ͼƬ���ļ���
    IplImage *img1, *img2;//IplImage��ʽ��ԭͼ
    IplImage *img1_Feat, *img2_Feat;//����������֮���ͼ

    bool verticalStackFlag;//��ʾƥ�����ĺϳ�ͼ���У�����ͼ���������еı�־
    IplImage *stacked;//��ʾƥ�����ĺϳ�ͼ����ʾ�������ֵ��ɸѡ���ƥ����
    IplImage *stacked_ransac;//��ʾƥ�����ĺϳ�ͼ����ʾ��RANSAC�㷨ɸѡ���ƥ����

    struct feature *feat1, *feat2;//feat1��ͼ1�����������飬feat2��ͼ2������������
    int n1, n2;//n1:ͼ1�е������������n2��ͼ2�е����������
    struct feature *feat;//ÿ��������
    struct kd_node *kd_root;//k-d��������
    struct feature **nbrs;//��ǰ�����������ڵ�����

    CvMat * H;//RANSAC�㷨����ı任����
    struct feature **inliers;//��RANSACɸѡ����ڵ�����
    int n_inliers;//��RANSAC�㷨ɸѡ����ڵ����,��feat2�о��з���Ҫ���������ĸ���

    IplImage *xformed;//��ʱƴ��ͼ����ֻ��ͼ2�任���ͼ
    IplImage *xformed_simple;//����ƴ��ͼ
    IplImage *xformed_proc;//������ƴ��ͼ

//    int img1LeftBound;//ͼ1��ƥ�����Ӿ��ε���߽�
//    int img1RightBound;//ͼ1��ƥ�����Ӿ��ε��ұ߽�
//    int img2LeftBound;//ͼ2��ƥ�����Ӿ��ε���߽�
//    int img2RightBound;//ͼ2��ƥ�����Ӿ��ε��ұ߽�

    //ͼ2���ĸ��Ǿ�����H�任�������
    CvPoint leftTop,leftBottom,rightTop,rightBottom;


};

#endif // SIFTMATCH_H
