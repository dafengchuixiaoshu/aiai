/**@file
Functions for computing transforms from image feature correspondences

Copyright (C) 2006-2010  Rob Hess <hess@eecs.oregonstate.edu>
@version 1.1.2-20100521
*/

/*
���ļ��а����ĺ��������У�
1��RANSAC�㷨����ƥ��ɸѡ
2������һЩ��RANSAC�㷨��غ���
*/


#ifndef XFORM_H
#define XFORM_H

#include <cxcore.h>


/********************************** Structures *******************************/

struct feature;

//RANSAC�㷨���õ��Ľṹ
//��RANSAC�㷨�����У����������ݻᱻ��ֵ��feature�ṹ��feature_data��Ա
/** holds feature data relevant to ransac */
struct ransac_data
{
    void* orig_feat_data; //������������feature_data�����ǰ��ֵ
    int sampled; //��ʶλ��ֵΪ1��ʶ���������Ƿ�ѡΪ����
};

//һЩ�궨��
/******************************* Defs and macros *****************************/

/*RANSAC�㷨���ݴ��
����ƥ����<pt,mpt>���Լ��任����H��
���pt��H�任��ĵ��mpt֮��ľ����ƽ��С��RANSAC_ERR_TOL����ɽ�����뵱ǰһ�¼�
*/
/* RANSAC error tolerance in pixels */
#define RANSAC_ERR_TOL 3

//�ڵ���Ŀռ��������Ŀ�İٷֱȵ���Сֵ
/** pessimistic estimate of fraction of inliers for RANSAC */
#define RANSAC_INLIER_FRAC_EST 0.25

//һ��ƥ����֧�ִ���ģ�͵ĸ��ʣ���֪���Ǹ�ʲô�õģ�
/** estimate of the probability that a correspondence supports a bad model */
#define RANSAC_PROB_BAD_SUPP 0.10

//������һ���������ĺ����꣬������ȡ����feat�е�feature_data��Ա��ת��Ϊransac_data��ʽ��ָ��
/* extracts a feature's RANSAC data */
#define feat_ransac_data( feat ) ( (struct ransac_data*) (feat)->feature_data )


/*������һ������ָ������ransac_xform_fn���䷵��ֵ��CvMat*���ͣ�����������
֮�������ransac_xform_fn�����庯��ָ��
�����͵ĺ���ָ�뱻����ransac_form()�����Ĳ�����
�����͵ĺ��������ƥ���Լ��ϼ����һ���任������Ϊ����ֵ
������
pts����ļ���
mpts����ļ��ϣ�pts[i]��mpts[i]�ǻ����Ӧ��ƥ���
n��pts��mpts�����е�ĸ�����pts��mpts�е�ĸ���������ͬ
����ֵ��һ���任���󣬽�pts�е�ÿһ����ת��Ϊmpts�еĶ�Ӧ�㣬����ֵΪ�ձ�ʾʧ��
*/
/**
Prototype for transformation functions passed to ransac_xform().  Functions
of this type should compute a transformation matrix given a set of point
correspondences.
@param pts array of points
@param mpts array of corresponding points; each \a pts[\a i], \a i=0..\a n-1,
	corresponds to \a mpts[\a i]
@param n number of points in both \a pts and \a mpts
@return Should return a transformation matrix that transforms each point in
	\a pts to the corresponding point in \a mpts or NULL on failure.
*/
typedef CvMat* (*ransac_xform_fn)( CvPoint2D64f* pts, CvPoint2D64f* mpts,int n );


/*������һ������ָ������ransac_err_fn���䷵��ֵ��double���ͣ�����������
֮�������ransac_err_fn�����庯��ָ��
�����͵ĺ���ָ�뱻����ransac_form()�����Ĳ�����
�����͵ĺ��������ƥ����(pt,mpt)�ͱ任����M�����һ��double���͵Ĵ������ֵ��Ϊ����ֵ
�˴������ֵ��������"��mpt"��"��pt��M����任��ĵ�"֮���Ƿ���һ��
������
pt��һ����
mpt����pt�Ķ�Ӧƥ���
M���任����
����ֵ��"��mpt"��"��pt��M����任��ĵ�"֮��Ĵ������ֵ
*/
/**
Prototype for error functions passed to ransac_xform().  For a given
point, its correspondence, and a transform, functions of this type should
compute a measure of error between the correspondence and the point after
the point has been transformed by the transform.
@param pt a point
@param mpt \a pt's correspondence
@param T a transform
@return Should return a measure of error between \a mpt and \a pt after
	\a pt has been transformed by the transform \a T.
*/
typedef double (*ransac_err_fn)( CvPoint2D64f pt, CvPoint2D64f mpt, CvMat* M );


/***************************** Function Prototypes ***************************/

/*����RANSAC�㷨����������ɸѡ����������ƥ��ı任����
������
features�����������飬ֻ�е�mtype���͵�ƥ������ʱ�ű��������е�Ӧ�Լ���
n�����������
mtype������ʹ��ÿ����������ĸ�ƥ������б任����ļ��㣬Ӧ����FEATURE_MDL_MATCH��
    FEATURE_BCK_MATCH��FEATURE_MDL_MATCH�е�һ��������FEATURE_MDL_MATCH��
    ��Ӧ��ƥ����������ÿ���������img_pt�����ƥ����mdl_pt��
    ���򣬶�Ӧ��ƥ������ÿ���������img_pt�����ƥ����img_pt��
xform_fn������ָ�룬ָ���������ĵ�Խ��б任�������ĺ�����һ�㴫��lsq_homog()����
m���ں���xform_fn�м���任������Ҫ����С������Ը���
p_badxform������Ĵ�����ʣ�������RANSAC�㷨������ı任�������ĸ��ʣ���ǰ�������ģ�͵Ĵ������С��p_badxformʱ����ֹͣ
err_fn������ָ�룬���ڸ����ı任���󣬼����ƶ���ƥ����֮��ı任��һ�㴫��homog_xfer_err()����
err_tol���ݴ�ȣ����ڸ����ı任�����ڴ˷�Χ�ڵĵ�Ա���Ϊ���ڵ�
inliers�����������ָ�����飬ָ�����������յ��ڵ㼯�ϣ���Ϊ�գ���ʾû���������Ҫ���һ�¼���
        ��������ڴ潫�ڱ������б����䣬ʹ���������ڵ��ó��ͷţ�free(*inliers)
n_in��������������ռ�������ڵ����Ŀ
����ֵ��RANSAC�㷨������ı任������Ϊ�գ���ʾ������޷�������ɽ��ܾ���
*/
/**
Calculates a best-fit image transform from image feature correspondences using RANSAC.

For more information refer to:
Fischler, M. A. and Bolles, R. C.  Random sample consensus: a paradigm for
model fitting with applications to image analysis and automated cartography.
<EM>Communications of the ACM, 24</EM>, 6 (1981), pp. 381--395.

@param features an array of features; only features with a non-NULL match
	of type \a mtype are used in homography computation
@param n number of features in \a feat
@param mtype determines which of each feature's match fields to use
	for transform computation; should be one of FEATURE_FWD_MATCH,
	FEATURE_BCK_MATCH, or FEATURE_MDL_MATCH; if this is FEATURE_MDL_MATCH,
	correspondences are assumed to be between a feature's img_pt field
	and its match's mdl_pt field, otherwise correspondences are assumed to
	be between the the feature's img_pt field and its match's img_pt field
@param xform_fn pointer to the function used to compute the desired
	transformation from feature correspondences
@param m minimum number of correspondences necessary to instantiate the
	transform computed by \a xform_fn
@param p_badxform desired probability that the final transformation
	returned by RANSAC is corrupted by outliers (i.e. the probability that
	no samples of all inliers were drawn)
@param err_fn pointer to the function used to compute a measure of error
	between putative correspondences for a given transform
@param err_tol correspondences within this distance of each other are
	considered as inliers for a given transform
@param inliers if not NULL, output as an array of pointers to the final
	set of inliers; memory for this array is allocated by this function and
	must be freed by the caller using free(*inliers)
@param n_in if not NULL, output as the final number of inliers

@return Returns a transformation matrix computed using RANSAC or NULL
	on error or if an acceptable transform could not be computed.
*/
extern CvMat* ransac_xform( struct feature* features, int n, int mtype,
						   ransac_xform_fn xform_fn, int m,
						   double p_badxform, ransac_err_fn err_fn,
						   double err_tol, struct feature*** inliers,
						   int* n_in );


/*�������Ա任�����е�ƥ�����ƽ�浥Ӧ��
������
pts����ļ���
mpts����ļ��ϣ�pts[i]��mpts[i]�ǻ����Ӧ��ƥ���
n��pts��mpts�����е�ĸ�����pts��mpts�е�ĸ���������ͬ�����ҵ�Ը�������Ϊ4
����ֵ���任���󣬿ɽ�pts�еĵ�任Ϊmpts�еĵ㣬�����������4�򷵻ؿ�
*/
/**
Calculates a planar homography from point correspondeces using the direct
linear transform.  Intended for use as a ransac_xform_fn.
@param pts array of points
@param mpts array of corresponding points; each \a pts[\a i], \a i=0..\a
	n-1, corresponds to \a mpts[\a i]
@param n number of points in both \a pts and \a mpts; must be at least 4
@return Returns the \f$3 \times 3\f$ planar homography matrix that
	transforms points in \a pts to their corresponding points in \a mpts
	or NULL if fewer than 4 correspondences were provided
*/
extern CvMat* dlt_homog( CvPoint2D64f* pts, CvPoint2D64f* mpts, int n );


/*����4������������С����ƽ�浥Ӧ�Ա任����
������
pts����ļ���
mpts����ļ��ϣ�pts[i]��mpts[i]�ǻ����Ӧ��ƥ���
n��pts��mpts�����е�ĸ�����pts��mpts�е�ĸ���������ͬ�����ҵ�Ը�������Ϊ4
����ֵ���任���󣬿ɽ�pts�еĵ�任Ϊmpts�еĵ㣬�����������4�򷵻ؿ�
*/
/**
Calculates a least-squares planar homography from point correspondeces.
Intended for use as a ransac_xform_fn.
@param pts array of points
@param mpts array of corresponding points; each \a pts[\a i], \a i=0..\a n-1,
	corresponds to \a mpts[\a i]
@param n number of points in both \a pts and \a mpts; must be at least 4
@return Returns the \f$3 \times 3\f$ least-squares planar homography
	matrix that transforms points in \a pts to their corresponding points
	in \a mpts or NULL if fewer than 4 correspondences were provided
*/
extern CvMat* lsq_homog( CvPoint2D64f* pts, CvPoint2D64f* mpts, int n );


/*���ڸ����ĵ�Ӧ�Ծ���H�����������pt��H�任��ĵ�����ƥ���mpt֮������
���磺������x�����Ӧ��x'����Ӧ�Ծ���H�������x'��Hx֮��ľ����ƽ����d(x', Hx)^2
������
pt��һ����
mpt��pt�Ķ�Ӧƥ���
H����Ӧ�Ծ���
����ֵ��ת�����
*/
/**
Calculates the transfer error between a point and its correspondence for
a given homography, i.e. for a point \f$x\f$, it's correspondence \f$x'\f$,
and homography \f$H\f$, computes \f$d(x', Hx)^2\f$.  Intended for use as a
ransac_err_fn.
@param pt a point
@param mpt \a pt's correspondence
@param H a homography matrix
@return Returns the transfer error between \a pt and \a mpt given \a H
*/
extern double homog_xfer_err( CvPoint2D64f pt, CvPoint2D64f mpt, CvMat* H );


/*�����pt��͸�ӱ任��ĵ㣬������һ��pt��͸�ӱ任����T������任��ĵ�
������(x,y)���任����M������[x',y',w']^T = M * [x,y,1]^T(^T��ʾת��)��
��任��ĵ���(u,v) = (x'/w', y'/w')
ע�⣺����任��͸�ӱ任������
������
pt��һ����ά��
T��͸�ӱ任����
����ֵ��pt��͸�ӱ任��ĵ�
*/
/**
Performs a perspective transformation on a single point.  That is, for a
point \f$(x, y)\f$ and a \f$3 \times 3\f$ matrix \f$T\f$ this function
returns the point \f$(u, v)\f$, where<BR>
\f$[x' \ y' \ w']^T = T \times [x \ y \ 1]^T\f$,<BR>
and<BR>
\f$(u, v) = (x'/w', y'/w')\f$.
Note that affine transforms are a subset of perspective transforms.
@param pt a 2D point
@param T a perspective transformation matrix
@return Returns the point \f$(u, v)\f$ as above.
*/
extern CvPoint2D64f persp_xform_pt( CvPoint2D64f pt, CvMat* T );


#endif
