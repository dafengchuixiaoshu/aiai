/**@file
Functions and structures for dealing with image features

Copyright (C) 2006-2010  Rob Hess <hess@eecs.oregonstate.edu>

@version 1.1.2-20100521
*/

/*
  ���ļ��ж����˴洢������Ľṹ��feature���Լ���������ԭ�͵�������
1��������ĵ��뵼��
2�����������
*/


#ifndef IMGFEATURES_H
#define IMGFEATURES_H

#include "cxcore.h"

/*����������ͣ�
FEATURE_OXFD��ʾ��ţ���ѧVGG�ṩ��Դ���е��������ʽ��
FEATURE_LOWE��ʾ��David.Lowe�ṩ��Դ���е��������ʽ
*/
/** FEATURE_OXFD <BR> FEATURE_LOWE */
enum feature_type
{
	FEATURE_OXFD,
	FEATURE_LOWE,
};

/*������ƥ�����ͣ�
FEATURE_FWD_MATCH������feature�ṹ�е�fwd_match���Ƕ�Ӧ��ƥ���
FEATURE_BCK_MATCH������feature�ṹ�е�bck_match���Ƕ�Ӧ��ƥ���
FEATURE_MDL_MATCH������feature�ṹ�е�mdl_match���Ƕ�Ӧ��ƥ���
*/
/** FEATURE_FWD_MATCH <BR> FEATURE_BCK_MATCH <BR> FEATURE_MDL_MATCH */
enum feature_match_type
{
	FEATURE_FWD_MATCH,
	FEATURE_BCK_MATCH,
	FEATURE_MDL_MATCH,
};

/*���������������ɫ*/
/* colors in which to display different feature types */
#define FEATURE_OXFD_COLOR CV_RGB(255,255,0)
#define FEATURE_LOWE_COLOR CV_RGB(255,0,255)

/*������������ӳ��ȣ���Ϊ128*/
/** max feature descriptor length */
#define FEATURE_MAX_D 128

/*������ṹ��
�˽ṹ��ɴ洢2�����͵������㣺
FEATURE_OXFD��ʾ��ţ���ѧVGG�ṩ��Դ���е��������ʽ��
FEATURE_LOWE��ʾ��David.Lowe�ṩ��Դ���е��������ʽ��
�����OXFD���͵������㣬�ṹ���е�a,b,c��Ա��������������Χ�ķ�������(��Բ�Ĳ���)��������
�����LOWE���͵������㣬�ṹ���е�scl��ori��Ա������������Ĵ�С�ͷ���
fwd_match��bck_match��mdl_matchһ��ͬʱֻ��һ�������ã�����ָ�����������Ӧ��ƥ���
*/
/**
Structure to represent an affine invariant image feature.  The fields
x, y, a, b, c represent the affine region around the feature:
a(x-u)(x-u) + 2b(x-u)(y-v) + c(y-v)(y-v) = 1
*/
struct feature
{
    double x;                      /**< x coord */ //�������x����
    double y;                      /**< y coord */ //�������y����
    double a;                      /**< Oxford-type affine region parameter */ //OXFD����������Բ�Ĳ���
    double b;                      /**< Oxford-type affine region parameter */ //OXFD����������Բ�Ĳ���
    double c;                      /**< Oxford-type affine region parameter */ //OXFD����������Բ�Ĳ���
    double scl;                    /**< scale of a Lowe-style feature *///LOWE������ĳ߶�
    double ori;                    /**< orientation of a Lowe-style feature */ //LOWE������ķ���
    int d;                         /**< descriptor length */ //���������ӵĳ��ȣ���ά����һ����128
    double descr[FEATURE_MAX_D];   /**< descriptor */ //128ά�����������ӣ���һ��double����
    int type;                      /**< feature type, OXFD or LOWE */ //����������
	int category;                  /**< all-purpose feature category */
    struct feature* fwd_match;     /**< matching feature from forward image */   //ָ�����������Ӧ��ƥ���
    struct feature* bck_match;     /**< matching feature from backmward image */ //ָ�����������Ӧ��ƥ���
    struct feature* mdl_match;     /**< matching feature from model */           //ָ�����������Ӧ��ƥ���
    CvPoint2D64f img_pt;           /**< location in image */ //�����������,����(x,y)
    CvPoint2D64f mdl_pt;           /**< location in model */ //��ƥ��������mdl_matchʱ�õ�
    void* feature_data;            /**< user-definable data */ //�û����������:
                                                               //��SIFT��ֵ�����У���detection_data�ṹ��ָ��
                                                               //��k-d�������У���bbf_data�ṹ��ָ��
                                                               //��RANSAC�㷨�У���ransac_data�ṹ��ָ��
};


/*���ļ��ж���ͼ������
�ļ��е��������ʽ������FEATURE_OXFD��FEATURE_LOWE��ʽ
������
filename���ļ���
type������������
feat�������洢�������feature�����ָ��
����ֵ����������������
*/
/**
Reads image features from file.  The file should be formatted as from
the code provided by the Visual Geometry Group at Oxford or from the
code provided by David Lowe.
@param filename location of a file containing image features
@param type determines how features are input.  If \a type is FEATURE_OXFD,
	the input file is treated as if it is from the code provided by the VGG
	at Oxford: http://www.robots.ox.ac.uk:5000/~vgg/research/affine/index.html
	<BR><BR>
	If \a type is FEATURE_LOWE, the input file is treated as if it is from
	David Lowe's SIFT code: http://www.cs.ubc.ca/~lowe/keypoints  
@param feat pointer to an array in which to store imported features; memory for
    this array is allocated by this function and must be freed by the caller using free(*feat)
@return Returns the number of features imported from filename or -1 on error
*/
extern int import_features( char* filename, int type, struct feature** feat );


/*����feature���鵽�ļ�
������
filename���ļ���
feat����������
n�����������
����ֵ��0���ɹ���1��ʧ��
*/
/**
Exports a feature set to a file formatted depending on the type of
features, as specified in the feature struct's type field.
@param filename name of file to which to export features
@param feat feature array
@param n number of features 
@return Returns 0 on success or 1 on error
*/
extern int export_features( char* filename, struct feature* feat, int n );


/*��ͼƬ�ϻ���������
������
img��ͼ��
feat������������
n�����������
*/
/**
Displays a set of features on an image
@param img image on which to display features
@param feat array of Oxford-type features
@param n number of features
*/
extern void draw_features( IplImage* img, struct feature* feat, int n );


/*�����������������Ӽ��ŷ�Ͼ����ƽ��
������
f1:��һ��������
f2:�ڶ���������
����ֵ��ŷ�Ͼ����ƽ��
*/
/**
Calculates the squared Euclidian distance between two feature descriptors.
@param f1 first feature
@param f2 second feature
@return Returns the squared Euclidian distance between the descriptors of
\a f1 and \a f2.
*/
extern double descr_dist_sq( struct feature* f1, struct feature* f2 );


#endif
