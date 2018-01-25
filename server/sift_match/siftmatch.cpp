#include "siftmatch.h"
#include "ui_siftmatch.h"

#include <QtCore>
#include <QtGui>

//SIFT�㷨ͷ�ļ�
//��extern "C"�����߱�������C���Եķ�ʽ���������
extern "C"
{
#include "imgfeatures.h"
#include "kdtree.h"
#include "minpq.h"
#include "sift.h"
#include "utils.h"
#include "xform.h"
}

//��k-d���Ͻ���BBF������������
/* the maximum number of keypoint NN candidates to check during BBF search */
#define KDTREE_BBF_MAX_NN_CHKS 200

//Ŀ���������ںʹν��ڵľ���ı�ֵ����ֵ�������ڴ���ֵ�����޳���ƥ����
//ͨ����ֵȡ0.6��ֵԽС�ҵ���ƥ����Խ��ȷ����ƥ����ĿԽ��
/* threshold on squared ratio of distances between NN and 2nd NN */
#define NN_SQ_DIST_RATIO_THR 0.49

//�������ַ���
#define IMG1 "ͼ1"
#define IMG2 "ͼ2"
#define IMG1_FEAT "ͼ1������"
#define IMG2_FEAT "ͼ2������"
#define IMG_MATCH1 "�����ֵɸѡ���ƥ����"
#define IMG_MATCH2 "RANSACɸѡ���ƥ����"
#define IMG_MOSAIC_TEMP "��ʱƴ��ͼ��"
#define IMG_MOSAIC_SIMPLE "����ƴ��ͼ"
#define IMG_MOSAIC_BEFORE_FUSION "�ص������ں�ǰ"
#define IMG_MOSAIC_PROC "������ƴ��ͼ"


SiftMatch::SiftMatch(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SiftMatch)
{    
    open_image_number = 0;//��ͼƬ�ĸ���

    //�趨�����ŵĵ�ѡ��ť�Ĺ�ѡ״̬��Ĭ���Ǻ���

    img1 = NULL;
    img2 = NULL;
    img1_Feat = NULL;
    img2_Feat = NULL;
    stacked = NULL;
    stacked_ransac = NULL;
    H = NULL;
    xformed = NULL;

    verticalStackFlag = false;//��ʾƥ�����ĺϳ�ͼ��Ĭ���Ǻ�������

    ui->setupUi(this);

    ui->openButton->setText(tr("�򿪵�һ��ͼƬ"));

    //�������а�ť
    ui->detectButton->setEnabled(false);
    ui->radioButton_horizontal->setEnabled(false);
    ui->radioButton_vertical->setEnabled(false);
    ui->matchButton->setEnabled(false);
    ui->mosaicButton->setEnabled(false);
    ui->restartButton->setEnabled(false);
}

SiftMatch::~SiftMatch()
{
    delete ui;
}

//��ͼƬ
void SiftMatch::on_openButton_clicked()
{
    //ֻ�ܴ�2��ͼƬ
    if(open_image_number <= 1 )
    {
        //ע�⣺�ļ�Ŀ¼�в��������ģ�����ͼƬ���ļ������Ϊ��xxx1.jpg,xxx2.jpg
        //һ�������xxx1.jpgӦ����ͼ��xxx2.jpgӦ����ͼ����������ˣ�������Զ���Ӧ
        QString img_name = QFileDialog::getOpenFileName(this,"Open Image", QDir::currentPath(),tr("Image Files(*.png *.jpeg *.jpg *.bmp *.gif)"));
        if(!img_name.isNull())
        {
            open_image_number++;
            //�򿪵�1��ͼƬ
            if( 1 == open_image_number )
            {
                ui->openButton->setText(tr("�򿪵ڶ���ͼƬ"));//�ı䰴ť�ı�
                ui->restartButton->setEnabled(true);//������ѡ��ť

                name1 = img_name;//�����1��ͼƬ���ļ���
                //qDebug()<<name1.insert( name1.lastIndexOf(".",-1) , "_Feat");
                img1 = cvLoadImage( img_name.toAscii().data() );//��ͼ1��ǿ�ƶ�ȡΪ��ͨ��ͼ��
                cvNamedWindow(IMG1);//��������
                cvShowImage(IMG1,img1);//��ʾԭͼ1
            }
            //�򿪵�2��ͼƬ
            else if( 2 == open_image_number )
            {
                ui->openButton->setText(tr("ֻ��������ͼƬ"));//�ı䰴ť�ı�
                ui->openButton->setEnabled(false);//���ô򿪰�ť
                ui->detectButton->setEnabled(true);//����������ⰴť

                name2 = img_name;//�����2��ͼƬ���ļ���
                img2 = cvLoadImage( img_name.toAscii().data() );//��ͼ2��ǿ�ƶ�ȡΪ��ͨ��ͼ��
                cvNamedWindow(IMG2);//��������
                cvShowImage(IMG2,img2);//��ʾԭͼ2
            }
        }
    }
}

//��������
void SiftMatch::on_detectButton_clicked()
{
    img1_Feat = cvCloneImage(img1);//����ͼ1�������������������
    img2_Feat = cvCloneImage(img2);//����ͼ2�������������������

    //Ĭ����ȡ����LOWE��ʽ��SIFT������
    //��ȡ����ʾ��1��ͼƬ�ϵ�������
    n1 = sift_features( img1, &feat1 );//���ͼ1�е�SIFT������,n1��ͼ1�����������
    export_features("feature1.txt",feat1,n1);//��������������д�뵽�ļ�
    draw_features( img1_Feat, feat1, n1 );//����������
    cvNamedWindow(IMG1_FEAT);//��������
    cvShowImage(IMG1_FEAT,img1_Feat);//��ʾ
    QString name1_Feat = name1;//�ļ�����ԭ�ļ�����"_Feat"
    cvSaveImage(name1_Feat.insert( name1_Feat.lastIndexOf(".",-1) , "_Feat").toAscii().data(),img1_Feat);//����ͼƬ

    //��ȡ����ʾ��2��ͼƬ�ϵ�������
    n2 = sift_features( img2, &feat2 );//���ͼ2�е�SIFT�����㣬n2��ͼ2�����������
    export_features("feature2.txt",feat2,n2);//��������������д�뵽�ļ�
    draw_features( img2_Feat, feat2, n2 );//����������
    cvNamedWindow(IMG2_FEAT);//��������
    cvShowImage(IMG2_FEAT,img2_Feat);//��ʾ
    QString name2_Feat = name2;//�ļ�����ԭ�ļ�����"_Feat"
    cvSaveImage(name2_Feat.insert( name2_Feat.lastIndexOf(".",-1) , "_Feat").toAscii().data(),img2_Feat);//����ͼƬ

    ui->detectButton->setEnabled(false);//����������ⰴť
    ui->radioButton_horizontal->setEnabled(true);//�������з���ѡ��ť
    ui->radioButton_vertical->setEnabled(true);
    ui->matchButton->setEnabled(true);//��������ƥ�䰴ť
}

//����ƥ��
void SiftMatch::on_matchButton_clicked()
{
    //���û���ѡ��ˮƽ���а�ť
    if(ui->radioButton_horizontal->isChecked())
    {
        //��2��ͼƬ�ϳ�1��ͼƬ,img1����img2����
        stacked = stack_imgs_horizontal(img1, img2);//�ϳ�ͼ����ʾ�������ֵ��ɸѡ���ƥ����
    }
    else//�û���ѡ�˴�ֱ���а�ť
    {
        verticalStackFlag = true;//��ֱ���б�ʶ��Ϊtrue
        //��2��ͼƬ�ϳ�1��ͼƬ,img1���ϣ�img2����
        stacked = stack_imgs( img1, img2 );//�ϳ�ͼ����ʾ�������ֵ��ɸѡ���ƥ����
    }

    //����ͼ1�������㼯feat1����k-d��������k-d������kd_root
    kd_root = kdtree_build( feat1, n1 );

    Point pt1,pt2;//���ߵ������˵�
    double d0,d1;//feat2��ÿ�������㵽����ںʹν��ڵľ���
    int matchNum = 0;//�������ֵ��ɸѡ���ƥ���Եĸ���

    //���������㼯feat2�����feat2��ÿ��������feat��ѡȡ���Ͼ����ֵ������ƥ��㣬�ŵ�feat��fwd_match����
    for(int i = 0; i < n2; i++ )
    {
        feat = feat2+i;//��i���������ָ��
        //��kd_root������Ŀ���feat��2������ڵ㣬�����nbrs�У�����ʵ���ҵ��Ľ��ڵ����
        int k = kdtree_bbf_knn( kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS );
        if( k == 2 )
        {
            d0 = descr_dist_sq( feat, nbrs[0] );//feat������ڵ�ľ����ƽ��
            d1 = descr_dist_sq( feat, nbrs[1] );//feat��ν��ڵ�ľ����ƽ��
            //��d0��d1�ı�ֵС����ֵNN_SQ_DIST_RATIO_THR������ܴ�ƥ�䣬�����޳�
            if( d0 < d1 * NN_SQ_DIST_RATIO_THR )
            {   //��Ŀ���feat������ڵ���Ϊƥ����
                pt2 = Point( cvRound( feat->x ), cvRound( feat->y ) );//ͼ2�е������
                pt1 = Point( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );//ͼ1�е������(feat������ڵ�)
                if(verticalStackFlag)//��ֱ����
                    pt2.y += img1->height;//��������ͼ���������еģ�pt2�����������ͼ1�ĸ߶ȣ���Ϊ���ߵ��յ�
                else
                    pt2.x += img1->width;//��������ͼ���������еģ�pt2�ĺ��������ͼ1�Ŀ�ȣ���Ϊ���ߵ��յ�
                cvLine( stacked, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );//��������
                matchNum++;//ͳ��ƥ���Եĸ���
                feat2[i].fwd_match = nbrs[0];//ʹ��feat��fwd_match��ָ�����Ӧ��ƥ���
            }
        }
        free( nbrs );//�ͷŽ�������
    }
    qDebug()<<tr("�������ֵ��ɸѡ���ƥ���Ը�����")<<matchNum<<endl;
    //��ʾ�����澭�����ֵ��ɸѡ���ƥ��ͼ
    cvNamedWindow(IMG_MATCH1);//��������
    cvShowImage(IMG_MATCH1,stacked);//��ʾ
    //����ƥ��ͼ
    QString name_match_DistRatio = name1;//�ļ�����ԭ�ļ���ȥ����ź��"_match_DistRatio"
    cvSaveImage(name_match_DistRatio.replace( name_match_DistRatio.lastIndexOf(".",-1)-1 , 1 , "_match_DistRatio").toAscii().data(),stacked);


    //����RANSAC�㷨ɸѡƥ���,����任����H��
    //����img1��img2������˳��H��Զ�ǽ�feat2�е�������任Ϊ��ƥ��㣬����img2�еĵ�任Ϊimg1�еĶ�Ӧ��
    H = ransac_xform(feat2,n2,FEATURE_FWD_MATCH,lsq_homog,4,0.01,homog_xfer_err,3.0,&inliers,&n_inliers);

    //���ܳɹ�������任���󣬼�����ͼ���й�ͬ����
    if( H )
    {
        qDebug()<<tr("��RANSAC�㷨ɸѡ���ƥ���Ը�����")<<n_inliers<<endl;

//        //���H����
//        for(int i=0;i<3;i++)
//            qDebug()<<cvmGet(H,i,0)<<cvmGet(H,i,1)<<cvmGet(H,i,2);

        if(verticalStackFlag)//��2��ͼƬ�ϳ�1��ͼƬ,img1���ϣ�img2����
            stacked_ransac = stack_imgs( img1, img2 );//�ϳ�ͼ����ʾ��RANSAC�㷨ɸѡ���ƥ����
        else//��2��ͼƬ�ϳ�1��ͼƬ,img1����img2����
            stacked_ransac = stack_imgs_horizontal(img1, img2);//�ϳ�ͼ����ʾ��RANSAC�㷨ɸѡ���ƥ����

        //img1LeftBound = inliers[0]->fwd_match->x;//ͼ1��ƥ�����Ӿ��ε���߽�
        //img1RightBound = img1LeftBound;//ͼ1��ƥ�����Ӿ��ε��ұ߽�
        //img2LeftBound = inliers[0]->x;//ͼ2��ƥ�����Ӿ��ε���߽�
        //img2RightBound = img2LeftBound;//ͼ2��ƥ�����Ӿ��ε��ұ߽�

        int invertNum = 0;//ͳ��pt2.x > pt1.x��ƥ���Եĸ��������ж�img1���Ƿ���ͼ

        //������RANSAC�㷨ɸѡ��������㼯��inliers���ҵ�ÿ���������ƥ��㣬��������
        for(int i=0; i<n_inliers; i++)
        {
            feat = inliers[i];//��i��������
            pt2 = Point(cvRound(feat->x), cvRound(feat->y));//ͼ2�е������
            pt1 = Point(cvRound(feat->fwd_match->x), cvRound(feat->fwd_match->y));//ͼ1�е������(feat��ƥ���)
            //qDebug()<<"pt2:("<<pt2.x<<","<<pt2.y<<")--->pt1:("<<pt1.x<<","<<pt1.y<<")";//�����Ӧ���

            /*��ƥ�������ı߽�
            if(pt1.x < img1LeftBound) img1LeftBound = pt1.x;
            if(pt1.x > img1RightBound) img1RightBound = pt1.x;
            if(pt2.x < img2LeftBound) img2LeftBound = pt2.x;
            if(pt2.x > img2RightBound) img2RightBound = pt2.x;//*/

            //ͳ��ƥ��������λ�ù�ϵ�����ж�ͼ1��ͼ2������λ�ù�ϵ
            if(pt2.x > pt1.x)
                invertNum++;

            if(verticalStackFlag)//��ֱ����
                pt2.y += img1->height;//��������ͼ���������еģ�pt2�����������ͼ1�ĸ߶ȣ���Ϊ���ߵ��յ�
            else//ˮƽ����
                pt2.x += img1->width;//��������ͼ���������еģ�pt2�ĺ��������ͼ1�Ŀ�ȣ���Ϊ���ߵ��յ�
            cvLine(stacked_ransac,pt1,pt2,CV_RGB(255,0,255),1,8,0);//��ƥ��ͼ�ϻ�������
        }

        //����ͼ1�а�Χƥ���ľ���
        //cvRectangle(stacked_ransac,cvPoint(img1LeftBound,0),cvPoint(img1RightBound,img1->height),CV_RGB(0,255,0),2);
        //����ͼ2�а�Χƥ���ľ���
        //cvRectangle(stacked_ransac,cvPoint(img1->width+img2LeftBound,0),cvPoint(img1->width+img2RightBound,img2->height),CV_RGB(0,0,255),2);

        cvNamedWindow(IMG_MATCH2);//��������
        cvShowImage(IMG_MATCH2,stacked_ransac);//��ʾ��RANSAC�㷨ɸѡ���ƥ��ͼ
        //����ƥ��ͼ
        QString name_match_RANSAC = name1;//�ļ�����ԭ�ļ���ȥ����ź��"_match_RANSAC"
        cvSaveImage(name_match_RANSAC.replace( name_match_RANSAC.lastIndexOf(".",-1)-1 , 1 , "_match_RANSAC").toAscii().data(),stacked_ransac);


        /*�����м�����ı任����H������img2�еĵ�任Ϊimg1�еĵ㣬���������img1Ӧ������ͼ��img2Ӧ������ͼ��
          ��ʱimg2�еĵ�pt2��img1�еĶ�Ӧ��pt1��x����Ĺ�ϵ�������ǣ�pt2.x < pt1.x
          ���û��򿪵�img1����ͼ��img2����ͼ����img2�еĵ�pt2��img1�еĶ�Ӧ��pt1��x����Ĺ�ϵ�������ǣ�pt2.x > pt1.x
          ����ͨ��ͳ�ƶ�Ӧ��任ǰ��x�����С��ϵ������֪��img1�ǲ�����ͼ��
          ���img1����ͼ����img1�е�ƥ��㾭H������H_IVT�任��ɵõ�img2�е�ƥ���*/

        //��pt2.x > pt1.x�ĵ�ĸ��������ڵ������80%�����϶�img1������ͼ
        if(invertNum > n_inliers * 0.8)
        {
            qDebug()<<tr("img1������ͼ");
            CvMat * H_IVT = cvCreateMat(3, 3, CV_64FC1);//�任����������
            //��H������H_IVTʱ�����ɹ���������ط���ֵ
            if( cvInvert(H,H_IVT) )
            {
//                //���H_IVT
//                for(int i=0;i<3;i++)
//                    qDebug()<<cvmGet(H_IVT,i,0)<<cvmGet(H_IVT,i,1)<<cvmGet(H_IVT,i,2);
                cvReleaseMat(&H);//�ͷű任����H����Ϊ�ò�����
                H = cvCloneMat(H_IVT);//��H������H_IVT�е����ݿ�����H��
                cvReleaseMat(&H_IVT);//�ͷ�����H_IVT
                //��img1��img2�Ե�
                IplImage * temp = img2;
                img2 = img1;
                img1 = temp;
                //cvShowImage(IMG1,img1);
                //cvShowImage(IMG2,img2);
                ui->mosaicButton->setEnabled(true);//����ȫ��ƴ�Ӱ�ť
            }
            else//H������ʱ������0
            {
                cvReleaseMat(&H_IVT);//�ͷ�����H_IVT
                QMessageBox::warning(this,tr("����"),tr("�任����H������"));
            }
        }
        else
            ui->mosaicButton->setEnabled(true);//����ȫ��ƴ�Ӱ�ť
    }
    else //�޷�������任���󣬼�����ͼ��û���غ�����
    {
        QMessageBox::warning(this,tr("����"),tr("��ͼ���޹�������"));
    }

    ui->radioButton_horizontal->setEnabled(false);//�������з���ѡ��ť
    ui->radioButton_vertical->setEnabled(false);
    ui->matchButton->setEnabled(false);//��������ƥ�䰴ť
}

//����ͼ2���ĸ��Ǿ�����H�任�������
void SiftMatch::CalcFourCorner()
{
    //����ͼ2���ĸ��Ǿ�����H�任�������
    double v2[]={0,0,1};//���Ͻ�
    double v1[3];//�任�������ֵ
    CvMat V2 = cvMat(3,1,CV_64FC1,v2);
    CvMat V1 = cvMat(3,1,CV_64FC1,v1);
    cvGEMM(H,&V2,1,0,1,&V1);//����˷�
    leftTop.x = cvRound(v1[0]/v1[2]);
    leftTop.y = cvRound(v1[1]/v1[2]);
    //cvCircle(xformed,leftTop,7,CV_RGB(255,0,0),2);

    //��v2��������Ϊ���½�����
    v2[0] = 0;
    v2[1] = img2->height;
    V2 = cvMat(3,1,CV_64FC1,v2);
    V1 = cvMat(3,1,CV_64FC1,v1);
    cvGEMM(H,&V2,1,0,1,&V1);
    leftBottom.x = cvRound(v1[0]/v1[2]);
    leftBottom.y = cvRound(v1[1]/v1[2]);
    //cvCircle(xformed,leftBottom,7,CV_RGB(255,0,0),2);

    //��v2��������Ϊ���Ͻ�����
    v2[0] = img2->width;
    v2[1] = 0;
    V2 = cvMat(3,1,CV_64FC1,v2);
    V1 = cvMat(3,1,CV_64FC1,v1);
    cvGEMM(H,&V2,1,0,1,&V1);
    rightTop.x = cvRound(v1[0]/v1[2]);
    rightTop.y = cvRound(v1[1]/v1[2]);
    //cvCircle(xformed,rightTop,7,CV_RGB(255,0,0),2);

    //��v2��������Ϊ���½�����
    v2[0] = img2->width;
    v2[1] = img2->height;
    V2 = cvMat(3,1,CV_64FC1,v2);
    V1 = cvMat(3,1,CV_64FC1,v1);
    cvGEMM(H,&V2,1,0,1,&V1);
    rightBottom.x = cvRound(v1[0]/v1[2]);
    rightBottom.y = cvRound(v1[1]/v1[2]);
    //cvCircle(xformed,rightBottom,7,CV_RGB(255,0,0),2);

}

//ȫ��ƴ��
void SiftMatch::on_mosaicButton_clicked()
{
    //���ܳɹ�������任���󣬼�����ͼ���й�ͬ���򣬲ſ��Խ���ȫ��ƴ��
    if(H)
    {
        //ƴ��ͼ��img1����ͼ��img2����ͼ
        CalcFourCorner();//����ͼ2���ĸ��Ǿ��任�������
        //Ϊƴ�ӽ��ͼxformed����ռ�,�߶�Ϊͼ1ͼ2�߶ȵĽ�С�ߣ�����ͼ2���ϽǺ����½Ǳ任��ĵ��λ�þ���ƴ��ͼ�Ŀ��
        xformed = cvCreateImage(cvSize(MIN(rightTop.x,rightBottom.x),MIN(img1->height,img2->height)),IPL_DEPTH_8U,3);
        //�ñ任����H����ͼimg2��ͶӰ�任(�任�������������)������ŵ�xformed��
        cvWarpPerspective(img2,xformed,H,CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS,cvScalarAll(0));
        cvNamedWindow(IMG_MOSAIC_TEMP); //��ʾ��ʱͼ,��ֻ��ͼ2�任���ͼ
        cvShowImage(IMG_MOSAIC_TEMP,xformed);

        //����ƴ�ӷ���ֱ�ӽ�����ͼimg1���ӵ�xformed�����
        xformed_simple = cvCloneImage(xformed);//����ƴ��ͼ��������xformed
        cvSetImageROI(xformed_simple,cvRect(0,0,img1->width,img1->height));
        cvAddWeighted(img1,1,xformed_simple,0,0,xformed_simple);
        cvResetImageROI(xformed_simple);
        cvNamedWindow(IMG_MOSAIC_SIMPLE);//��������
        cvShowImage(IMG_MOSAIC_SIMPLE,xformed_simple);//��ʾ����ƴ��ͼ

        //������ƴ��ͼ����¡��xformed
        xformed_proc = cvCloneImage(xformed);

        //�ص�������ߵĲ�����ȫȡ��ͼ1
        cvSetImageROI(img1,cvRect(0,0,MIN(leftTop.x,leftBottom.x),xformed_proc->height));
        cvSetImageROI(xformed,cvRect(0,0,MIN(leftTop.x,leftBottom.x),xformed_proc->height));
        cvSetImageROI(xformed_proc,cvRect(0,0,MIN(leftTop.x,leftBottom.x),xformed_proc->height));
        cvAddWeighted(img1,1,xformed,0,0,xformed_proc);
        cvResetImageROI(img1);
        cvResetImageROI(xformed);
        cvResetImageROI(xformed_proc);
        cvNamedWindow(IMG_MOSAIC_BEFORE_FUSION);
        cvShowImage(IMG_MOSAIC_BEFORE_FUSION,xformed_proc);//��ʾ�ں�֮ǰ��ƴ��ͼ

        //���ü�Ȩƽ���ķ����ں��ص�����
        int start = MIN(leftTop.x,leftBottom.x) ;//��ʼλ�ã����ص��������߽�
        double processWidth = img1->width - start;//�ص�����Ŀ��
        double alpha = 1;//img1�����ص�Ȩ��
        for(int i=0; i<xformed_proc->height; i++)//������
        {
            const uchar * pixel_img1 = ((uchar *)(img1->imageData + img1->widthStep * i));//img1�е�i�����ݵ�ָ��
            const uchar * pixel_xformed = ((uchar *)(xformed->imageData + xformed->widthStep * i));//xformed�е�i�����ݵ�ָ��
            uchar * pixel_xformed_proc = ((uchar *)(xformed_proc->imageData + xformed_proc->widthStep * i));//xformed_proc�е�i�����ݵ�ָ��
            for(int j=start; j<img1->width; j++)//�����ص��������
            {
                //�������ͼ��xformed�������صĺڵ㣬����ȫ����ͼ1�е�����
                if(pixel_xformed[j*3] < 50 && pixel_xformed[j*3+1] < 50 && pixel_xformed[j*3+2] < 50 )
                {
                    alpha = 1;
                }
                else
                {   //img1�����ص�Ȩ�أ��뵱ǰ�������ص�������߽�ľ��������
                    alpha = (processWidth-(j-start)) / processWidth ;
                }
                pixel_xformed_proc[j*3] = pixel_img1[j*3] * alpha + pixel_xformed[j*3] * (1-alpha);//Bͨ��
                pixel_xformed_proc[j*3+1] = pixel_img1[j*3+1] * alpha + pixel_xformed[j*3+1] * (1-alpha);//Gͨ��
                pixel_xformed_proc[j*3+2] = pixel_img1[j*3+2] * alpha + pixel_xformed[j*3+2] * (1-alpha);//Rͨ��
            }
        }
        cvNamedWindow(IMG_MOSAIC_PROC);//��������
        cvShowImage(IMG_MOSAIC_PROC,xformed_proc);//��ʾ������ƴ��ͼ

        //*�ص�����ȡ����ͼ���ƽ��ֵ��Ч������
        //����ROI���ǰ����ص�����ľ���
        cvSetImageROI(xformed_proc,cvRect(MIN(leftTop.x,leftBottom.x),0,img1->width-MIN(leftTop.x,leftBottom.x),xformed_proc->height));
        cvSetImageROI(img1,cvRect(MIN(leftTop.x,leftBottom.x),0,img1->width-MIN(leftTop.x,leftBottom.x),xformed_proc->height));
        cvSetImageROI(xformed,cvRect(MIN(leftTop.x,leftBottom.x),0,img1->width-MIN(leftTop.x,leftBottom.x),xformed_proc->height));
        cvAddWeighted(img1,0.5,xformed,0.5,0,xformed_proc);
        cvResetImageROI(xformed_proc);
        cvResetImageROI(img1);
        cvResetImageROI(xformed); //*/

        /*��ƴ�ӷ���Χ��������˲�������ƴ�ӷ죬Ч������
        //�ڴ���ǰ���ͼ�Ϸֱ����ú��ƴ�ӷ�ľ���ROI
        cvSetImageROI(xformed_proc,cvRect(img1->width-10,0,img1->width+10,xformed->height));
        cvSetImageROI(xformed,cvRect(img1->width-10,0,img1->width+10,xformed->height));
        cvSmooth(xformed,xformed_proc,CV_MEDIAN,5);//��ƴ�ӷ���Χ���������ֵ�˲�
        cvResetImageROI(xformed);
        cvResetImageROI(xformed_proc);
        cvShowImage(IMG_MOSAIC_PROC,xformed_proc);//��ʾ������ƴ��ͼ */

        /*��ͨ���񻯽���任���ͼ��ʧ������⣬����Ť�������ͼ��Ч������
        double a[]={  0, -1,  0, -1,  5, -1, 0, -1,  0  };//������˹�˲��˵�����
        CvMat kernel = cvMat(3,3,CV_64FC1,a);//������˹�˲���
        cvFilter2D(xformed_proc,xformed_proc,&kernel);//�˲�
        cvShowImage(IMG_MOSAIC_PROC,xformed_proc);//��ʾ������ƴ��ͼ*/

        //����ƴ��ͼ
        QString name_xformed = name1;//�ļ�����ԭ�ļ���ȥ����ź��"_Mosaic"
        cvSaveImage(name_xformed.replace( name_xformed.lastIndexOf(".",-1)-1 , 1 , "_Mosaic").toAscii().data() , xformed_simple);//�������ƴ��ͼ
        cvSaveImage(name_xformed.insert( name_xformed.lastIndexOf(".",-1) , "_Proc").toAscii().data() , xformed_proc);//���洦����ƴ��ͼ
        ui->mosaicButton->setEnabled(false);//����ȫ��ƴ�Ӱ�ť
    }
}

// ����ѡ��
void SiftMatch::on_restartButton_clicked()
{
    //�ͷŲ��ر�ԭͼ1
    if(img1)
    {
        cvReleaseImage(&img1);
        cvDestroyWindow(IMG1);
    }
    //�ͷŲ��ر�ԭͼ2
    if(img2)
    {
        cvReleaseImage(&img2);
        cvDestroyWindow(IMG2);
    }
    //�ͷ�������ͼ1
    if(img1_Feat)
    {
        cvReleaseImage(&img1_Feat);
        cvDestroyWindow(IMG1_FEAT);
        free(feat1);//�ͷ�����������
    }
    //�ͷ�������ͼ2
    if(img2_Feat)
    {
        cvReleaseImage(&img2_Feat);
        cvDestroyWindow(IMG2_FEAT);
        free(feat2);//�ͷ�����������
    }
    //�ͷž����ֵɸѡ���ƥ��ͼ��kd��
    if(stacked)
    {
        cvReleaseImage(&stacked);
        cvDestroyWindow(IMG_MATCH1);
        kdtree_release(kd_root);//�ͷ�kd��
    }
    //ֻ����RANSAC�㷨�ɹ�����任����ʱ������Ҫ��һ���ͷ�������ڴ�ռ�
    if( H )
    {
        cvReleaseMat(&H);//�ͷű任����H
        free(inliers);//�ͷ��ڵ�����

        //�ͷ�RANSAC�㷨ɸѡ���ƥ��ͼ
        cvReleaseImage(&stacked_ransac);
        cvDestroyWindow(IMG_MATCH2);

        //�ͷ�ȫ��ƴ��ͼ��
        if(xformed)
        {
            cvReleaseImage(&xformed);
            cvReleaseImage(&xformed_simple);
            cvReleaseImage(&xformed_proc);
            cvDestroyWindow(IMG_MOSAIC_TEMP);
            cvDestroyWindow(IMG_MOSAIC_SIMPLE);
            cvDestroyWindow(IMG_MOSAIC_BEFORE_FUSION);
            cvDestroyWindow(IMG_MOSAIC_PROC);
        }
    }

    open_image_number = 0;//��ͼƬ��������
    verticalStackFlag = false;//��ʾƥ�����ĺϳ�ͼƬ�����з����ʶ��λ

    ui->openButton->setEnabled(true);//����򿪰�ť
    ui->openButton->setText(tr("�򿪵�һ��ͼƬ"));

    //�������а�ť
    ui->detectButton->setEnabled(false);
    ui->radioButton_horizontal->setEnabled(false);
    ui->radioButton_vertical->setEnabled(false);
    ui->matchButton->setEnabled(false);
    ui->mosaicButton->setEnabled(false);
    ui->restartButton->setEnabled(false);
}

