/*
Functions for detecting SIFT image features.

For more information, refer to:

Lowe, D.  Distinctive image features from scale-invariant keypoints.
<EM>International Journal of Computer Vision, 60</EM>, 2 (2004),
pp.91--110.

Copyright (C) 2006-2010  Rob Hess <hess@eecs.oregonstate.edu>

Note: The SIFT algorithm is patented in the United States and cannot be
used in commercial products without a license from the University of
British Columbia.  For more information, refer to the file LICENSE.ubc
that accompanied this distribution.

@version 1.1.2-20100521
*/

/*
  ���ļ�����Ҫ
  ����SIFT���������ʵ��
*/

#include "sift.h"
#include "imgfeatures.h"
#include "utils.h"

#include <cxcore.h>
#include <cv.h>

/************************ δ��¶�ӿڵ�һЩ���غ��������� **************************/
/************************* Local Function Prototypes *************************/

//��ԭͼת��Ϊ32λ�Ҷ�ͼ����һ����Ȼ�����һ�θ�˹ƽ���������ݲ���img_dbl�����Ƿ�ͼ��ߴ�Ŵ�Ϊԭͼ��2��
static IplImage* create_init_img( IplImage*, int, double );
//������ͼ��ת��Ϊ32λ�Ҷ�ͼ,�����й�һ��
static IplImage* convert_to_gray32( IplImage* );
//�����������������˹������
static IplImage*** build_gauss_pyr( IplImage*, int, int, double );
//������ͼ�����²����������ķ�֮һ��С��ͼ��(ÿ��ά���ϼ���)��ʹ������ڲ�ֵ����
static IplImage* downsample( IplImage* );
//ͨ���Ը�˹��������ÿ��������ͼ�������������˹��ֽ�����
static IplImage*** build_dog_pyr( IplImage***, int, int );
//�ڳ߶ȿռ��м�⼫ֵ�㣬ͨ����ֵ��ȷ��λ��ȥ���ͶԱȶȵĵ㣬ȥ����Ե�㣬���ؼ�⵽������������
static CvSeq* scale_space_extrema( IplImage***, int, int, double, int, CvMemStorage*);
//ͨ���ڳ߶ȿռ��н�һ�����ص��ֵ������Χ3*3*3�����ڵĵ�Ƚ��������˵��Ƿ�ֵ��(����ֵ��С����)
static int is_extremum( IplImage***, int, int, int, int );
//ͨ�������ؼ���ֵ���м�ֵ�㾫ȷ��λ(������ֵ������)����ȥ���ͶԱȶȵļ�ֵ�㣬������������������feature�ṹ����
static struct feature* interp_extremum( IplImage***, int, int, int, int, int, double);
//����һ�μ�ֵ���ֵ������x��y���ҷ���(�㷽��)�ϵ�������ƫ����(����)
static void interp_step( IplImage***, int, int, int, int, double*, double*, double* );
//��DoG�������м���ĳ���x����y�����Լ��߶ȷ����ϵ�ƫ����
static CvMat* deriv_3D( IplImage***, int, int, int, int );
//��DoG�������м���ĳ���3*3��ɭ����
static CvMat* hessian_3D( IplImage***, int, int, int, int );
//���㱻��ֵ��ĶԱȶȣ�D + 0.5 * dD^T * X
static double interp_contr( IplImage***, int, int, int, int, double, double, double );
//Ϊһ��feature�ṹ����ռ䲢��ʼ��
static struct feature* new_feature( void );
//ȥ����Ե��Ӧ����ͨ�����������ʱ�ֵ�ж�ĳ���Ƿ��Ե��
static int is_too_edge_like( IplImage*, int, int, int );
//����������������ÿ��������ĳ߶�
static void calc_feature_scales( CvSeq*, double, int );
//��������������ÿ����������������(�������˽�ͼ��Ŵ�Ϊԭͼ��2��ʱ������������֮�����)
static void adjust_for_img_dbl( CvSeq* );
//����ÿ����������ݶ�ֱ��ͼ���ҳ�����������һ���������в�ֹһ�������򣬽����Ϊ����������
static void calc_feature_oris( CvSeq*, IplImage*** );
//����ָ�����ص���ݶȷ���ֱ��ͼ�����ش��ֱ��ͼ������
static double* ori_hist( IplImage*, int, int, int, int, double );
//����ָ������ݶȵķ�ֵmagnitude�ͷ���orientation
static int calc_grad_mag_ori( IplImage*, int, int, double*, double* );
//���ݶȷ���ֱ��ͼ���и�˹ƽ�����ֲ���û�з��䲻���Զ������������㲻�ȶ�������
static void smooth_ori_hist( double*, int );
//�����ݶ�ֱ��ͼ����������ݶȷ�ֵ��������ֱ��ͼ�����bin��ֵ
static double dominant_ori( double*, int );
//����ǰ�������ֱ��ͼ��ĳ��bin��ֵ���ڸ�������ֵ����������һ�������㲢��ӵ�����������ĩβ
static void add_good_ori_features( CvSeq*, double*, int, double, struct feature* );
//�������feature�ṹ����������������ؿ�¡���ɵ��������ָ��
static struct feature* clone_feature( struct feature* );
//����������������ÿ�����������������������
static void compute_descriptors( CvSeq*, IplImage***, int, int );
//���������㸽������ķ���ֱ��ͼ����ֱ��ͼ�ڼ���������������Ҫ�õ�������ֵ��һ��d*d*n����ά����
static double*** descr_hist( IplImage*, int, int, double, double, int, int );
static void interp_hist_entry( double***, double, double, double, double, int, int);
//��ĳ������ķ���ֱ��ͼת��Ϊ���������������������������ӹ�һ����������Ԫ��ת��Ϊ���ͣ�����ָ����������
static void hist_to_descr( double***, int, int, struct feature* );
//��һ������������������ӣ���������������������ÿ��Ԫ�س������������ӵ�ģ
static void normalize_descr( struct feature* );
//�ȽϺ������������㰴�߶ȵĽ������У���������������CvSeqSort��
static int feature_cmp( void*, void*, void* );
//�ͷż������������ӹ������õ��ķ���ֱ��ͼ���ڴ�ռ�
static void release_descr_hist( double****, int );
//�ͷŽ�����ͼ����Ĵ洢�ռ�
static void release_pyr( IplImage****, int, int );


/*********************** Functions prototyped in sift.h **********************/

/*ʹ��Ĭ�ϲ�����ͼ������ȡSIFT������
������
img��ͼ��ָ��
feat�������洢�������feature�����ָ��
      ��������ڴ潫�ڱ������б����䣬ʹ���������ڵ��ó��ͷţ�free(*feat)
����ֵ����ȡ�������������������-1������ȡʧ��
*/
/**
Finds SIFT features in an image using default parameter values.  All
detected features are stored in the array pointed to by \a feat.

@param img the image in which to detect features
@param feat a pointer to an array in which to store detected features

@return Returns the number of features stored in \a feat or -1 on failure
@see _sift_features()
*/
int sift_features( IplImage* img, struct feature** feat )
{
    //����_sift_features()����������������
	return _sift_features( img, feat, SIFT_INTVLS, SIFT_SIGMA, SIFT_CONTR_THR,
							SIFT_CURV_THR, SIFT_IMG_DBL, SIFT_DESCR_WIDTH,
							SIFT_DESCR_HIST_BINS );
}


/*ʹ���û�ָ���Ĳ�����ͼ������ȡSIFT������
������
img������ͼ��
feat���洢������������ָ��
      ��������ڴ潫�ڱ������б����䣬ʹ���������ڵ��ó��ͷţ�free(*feat)
intvls��ÿ��Ĳ���
sigma����ʼ��˹ƽ��������
contr_thr���Աȶ���ֵ����Թ�һ�����ͼ������ȥ�����ȶ�����
curv_thr��ȥ����Ե����������������ֵ
img_dbl���Ƿ�ͼ��Ŵ�Ϊ֮ǰ������
descr_width���������������У����㷽��ֱ��ͼʱ���������㸽������Ϊdescr_width*descr_width������ÿ����������һ��ֱ��ͼ
descr_hist_bins���������������У�ÿ��ֱ��ͼ��bin�ĸ���
����ֵ����ȡ�������������������-1������ȡʧ��
*/
/**
Finds SIFT features in an image using user-specified parameter values.  All
detected features are stored in the array pointed to by \a feat.

@param img the image in which to detect features
@param feat a pointer to an array in which to store detected features
@param intvls the number of intervals sampled per octave of scale space
@param sigma the amount of Gaussian smoothing applied to each image level
	before building the scale space representation for an octave
@param cont_thr a threshold on the value of the scale space function
	\f$\left|D(\hat{x})\right|\f$, where \f$\hat{x}\f$ is a vector specifying
	feature location and scale, used to reject unstable features;  assumes
	pixel values in the range [0, 1]
@param curv_thr threshold on a feature's ratio of principle curvatures
	used to reject features that are too edge-like
@param img_dbl should be 1 if image doubling prior to scale space
	construction is desired or 0 if not
@param descr_width the width, \f$n\f$, of the \f$n \times n\f$ array of
	orientation histograms used to compute a feature's descriptor
@param descr_hist_bins the number of orientations in each of the
	histograms in the array used to compute a feature's descriptor

@return Returns the number of keypoints stored in \a feat or -1 on failure
@see sift_keypoints()
*/
int _sift_features( IplImage* img, struct feature** feat, int intvls,
				   double sigma, double contr_thr, int curv_thr,
				   int img_dbl, int descr_width, int descr_hist_bins )
{
    IplImage* init_img;//ԭͼ����ʼ�����ͼ�񣬹�һ����32λ�Ҷ�ͼ
    IplImage*** gauss_pyr, *** dog_pyr;//����ָ�룬��˹������ͼ���飬DoG������ͼ����
    CvMemStorage* storage;//�洢��
    CvSeq* features;//�洢����������У������д�ŵ���struct feature���͵�ָ��
	int octvs, i, n = 0;

    //����������
	/* check arguments */
	if( ! img )
		fatal_error( "NULL pointer error, %s, line %d",  __FILE__, __LINE__ );

	if( ! feat )
		fatal_error( "NULL pointer error, %s, line %d",  __FILE__, __LINE__ );

	/* build scale space pyramid; smallest dimension of top level is ~4 pixels */

    //��������һ�������������߶ȿռ䣬��������˹���(DoG)������dog_pyr
    //��ԭͼת��Ϊ32λ�Ҷ�ͼ����һ����Ȼ�����һ�θ�˹ƽ���������ݲ���img_dbl�����Ƿ�ͼ��ߴ�Ŵ�Ϊԭͼ��2��
	init_img = create_init_img( img, img_dbl, sigma );
    //�����˹������������octvs
    octvs = log( MIN( init_img->width, init_img->height ) ) / log(2) - 2;
    //Ϊ�˱�֤�����ԣ���ÿһ��Ķ�������ø�˹ģ������3��ͼ�����Ը�˹������ÿ����intvls+3�㣬DOG������ÿ����intvls+2��
    //������˹������gauss_pyr����һ��octvs*(intvls+3)��ͼ������
	gauss_pyr = build_gauss_pyr( init_img, octvs, intvls, sigma );
    //������˹���(DoG)������dog_pyr����һ��octvs*(intvls+2)��ͼ������
	dog_pyr = build_dog_pyr( gauss_pyr, octvs, intvls );

    //������������������ڳ߶ȿռ��м�⼫ֵ�㣬�����о�ȷ��λ��ɸѡ
    //����Ĭ�ϴ�С���ڴ�洢��
	storage = cvCreateMemStorage( 0 );
    //�ڳ߶ȿռ��м�⼫ֵ�㣬ͨ����ֵ��ȷ��λ��ȥ���ͶԱȶȵĵ㣬ȥ����Ե�㣬���ؼ�⵽������������
    features = scale_space_extrema( dog_pyr, octvs, intvls, contr_thr, curv_thr, storage );
    //��������������features��ÿ��������ĳ߶�
	calc_feature_scales( features, sigma, intvls );
    //�������˽�ͼ��Ŵ�Ϊԭͼ��2��
    if( img_dbl )//��������������ÿ����������������(�������˽�ͼ��Ŵ�Ϊԭͼ��2��ʱ������������֮�����)
		adjust_for_img_dbl( features );

    //�����������������������㷽��ֵ����ɴ˲����ÿ����������������Ϣ��λ�á��߶ȡ�����
    //����ÿ����������ݶ�ֱ��ͼ���ҳ�����������һ���������в�ֹһ�������򣬽����Ϊ����������
	calc_feature_oris( features, gauss_pyr );

    //���������ģ���������������������
    //����������������ÿ�����������������������
	compute_descriptors( features, gauss_pyr, descr_width, descr_hist_bins );

	/* sort features by decreasing scale and move from CvSeq to array */
    //��������߶ȵĽ�������������Ԫ�ص�˳��feature_cmp���Զ���ıȽϺ���
	cvSeqSort( features, (CvCmpFunc)feature_cmp, NULL );

    //��CvSeq���͵�����������featuresת��Ϊͨ�õ�struct feature���͵�����feat
    n = features->total;//���������
    *feat = calloc( n, sizeof(struct feature) );//����ؼ�
    //������features�е�Ԫ�ؿ���������feat�У���������ָ���feat
	*feat = cvCvtSeqToArray( features, *feat, CV_WHOLE_SEQ );

    //�ͷ�����������feat�������������feature_data��Ա����Ϊ�˳�Ա�е������ڼ������������û����
	for( i = 0; i < n; i++ )
	{
		free( (*feat)[i].feature_data );
		(*feat)[i].feature_data = NULL;
	}

    //�ͷŸ�����ʱ���ݵĴ洢�ռ�
    cvReleaseMemStorage( &storage );//�ͷ��ڴ�洢��
    cvReleaseImage( &init_img );//�ͷų�ʼ�����ͼ��
    release_pyr( &gauss_pyr, octvs, intvls + 3 );//�ͷŸ�˹������ͼ����
    release_pyr( &dog_pyr, octvs, intvls + 2 );//�ͷŸ�˹��ֽ�����ͼ����

    return n;//���ؼ�⵽��������ĸ���
}



/******************************* ���غ�����ʵ�� ********************************/
/************************ Functions prototyped here **************************/

/*��ԭͼת��Ϊ32λ�Ҷ�ͼ����һ����Ȼ�����һ�θ�˹ƽ���������ݲ���img_dbl�����Ƿ�ͼ��ߴ�Ŵ�Ϊԭͼ��2��
������
img�������ԭͼ��
img_dbl���Ƿ�ͼ��Ŵ�Ϊ֮ǰ������
sigma����ʼ��˹ƽ��������
����ֵ����ʼ����ɵ�ͼ��
*/
/*
Converts an image to 32-bit grayscale and Gaussian-smooths it.  The image is
optionally doubled in size prior to smoothing.
@param img input image
@param img_dbl if true, image is doubled in size prior to smoothing
@param sigma total std of Gaussian smoothing
*/
static IplImage* create_init_img( IplImage* img, int img_dbl, double sigma )
{
	IplImage* gray, * dbl;
	float sig_diff;

    //���ú�����������ͼ��ת��Ϊ32λ�Ҷ�ͼ������һ��
	gray = convert_to_gray32( img );

    //�������˽�ͼ��Ŵ�Ϊԭͼ��2��
	if( img_dbl )
	{
        //��ͼ�񳤿���չһ��ʱ�������˵�-1�㣬�ò�߶�Ϊ��
		sig_diff = sqrt( sigma * sigma - SIFT_INIT_SIGMA * SIFT_INIT_SIGMA * 4 );
        dbl = cvCreateImage( cvSize( img->width*2, img->height*2 ),IPL_DEPTH_32F, 1 );//�����Ŵ�ͼ��
        cvResize( gray, dbl, CV_INTER_CUBIC );//�Ŵ�ԭͼ�ĳߴ�
        //��˹ƽ������˹����x,y�����ϵı�׼���sig_diff
        cvSmooth( dbl, dbl, CV_GAUSSIAN, 0, 0, sig_diff, sig_diff );
		cvReleaseImage( &gray );
		return dbl;
	}
    else//���÷Ŵ�Ϊԭͼ��2��
	{
        //�����0��ĳ߶�
		sig_diff = sqrt( sigma * sigma - SIFT_INIT_SIGMA * SIFT_INIT_SIGMA );
        //��˹ƽ������˹����x,y�����ϵı�׼���sig_diff
		cvSmooth( gray, gray, CV_GAUSSIAN, 0, 0, sig_diff, sig_diff );
		return gray;
	}
}


/*������ͼ��ת��Ϊ32λ�Ҷ�ͼ,�����й�һ��
������
img������ͼ��3ͨ��8λ��ɫͼ(BGR)��8λ�Ҷ�ͼ
����ֵ��32λ�Ҷ�ͼ
*/
/*
Converts an image to 32-bit grayscale
@param img a 3-channel 8-bit color (BGR) or 8-bit gray image
@return Returns a 32-bit grayscale image
*/
static IplImage* convert_to_gray32( IplImage* img )
{
	IplImage* gray8, * gray32;

    gray32 = cvCreateImage( cvGetSize(img), IPL_DEPTH_32F, 1 );//����32λ��ͨ��ͼ��

    //���Ƚ�ԭͼת��Ϊ8λ��ͨ��ͼ��
    if( img->nChannels == 1 )//��ԭͼ������ǵ�ͨ����ֱ�ӿ�¡ԭͼ
		gray8 = cvClone( img );
    else//��ԭͼ��3ͨ��ͼ��
	{
        gray8 = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1 );//����8λ��ͨ��ͼ��
        cvCvtColor( img, gray8, CV_BGR2GRAY );//��ԭͼת��Ϊ8Ϊ��ͨ��ͼ��
	}

    //Ȼ��8Ϊ��ͨ��ͼ��gray8ת��Ϊ32λ��ͨ��ͼ�񣬲����й�һ������(����255)
	cvConvertScale( gray8, gray32, 1.0 / 255.0, 0 );

    cvReleaseImage( &gray8 );//�ͷ���ʱͼ��

    return gray32;//����32λ��ͨ��ͼ��
}


/*�����������������˹������
������
base������ͼ����Ϊ��˹�������Ļ�ͼ��
octvs����˹������������
intvls��ÿ��Ĳ���
sigma����ʼ�߶�
����ֵ����˹����������һ��octvs*(intvls+3)��ͼ������
*/
/*
Builds Gaussian scale space pyramid from an image
@param base base image of the pyramid
@param octvs number of octaves of scale space
@param intvls number of intervals per octave
@param sigma amount of Gaussian smoothing per octave
@return Returns a Gaussian scale space pyramid as an octvs x (intvls + 3) array
*/
static IplImage*** build_gauss_pyr( IplImage* base, int octvs,
									int intvls, double sigma )
{
	IplImage*** gauss_pyr;
    //Ϊ�˱�֤�����ԣ���ÿһ��Ķ�������ø�˹ģ������3��ͼ�����Ը�˹������ÿ����intvls+3�㣬DOG������ÿ����intvls+2��
    double* sig = calloc( intvls + 3, sizeof(double));//ÿ���sigma��������
	double sig_total, sig_prev, k;
	int i, o;

    //Ϊ��˹������gauss_pyr����ռ䣬��octvs��Ԫ�أ�ÿ��Ԫ����һ��ͼ�����ָ��
    gauss_pyr = calloc( octvs, sizeof( IplImage** ) );
    //Ϊ��i��ͼ��gauss_pyr[i]����ռ䣬��intvls+3��Ԫ�أ�ÿ��Ԫ����һ��ͼ��ָ��
	for( i = 0; i < octvs; i++ )
		gauss_pyr[i] = calloc( intvls + 3, sizeof( IplImage* ) );

	/*	precompute Gaussian sigmas using the following formula:
        sigma_{total}^2 = sigma_{i}^2 + sigma_{i-1}^2   */
    //����ÿ�θ�˹ģ����sigma����
    sig[0] = sigma;//��ʼ�߶�
	k = pow( 2.0, 1.0 / intvls );
	for( i = 1; i < intvls + 3; i++ )
	{
        sig_prev = pow( k, i - 1 ) * sigma;//i-1��ĳ߶�
        sig_total = sig_prev * k;//i��ĳ߶�
        sig[i] = sqrt( sig_total * sig_total - sig_prev * sig_prev );//��������Ϊʲô��
	}

    //����������ɸ�˹������
    for( o = 0; o < octvs; o++ )//������
        for( i = 0; i < intvls + 3; i++ )//������
		{
            if( o == 0  &&  i == 0 )//��0�飬��0�㣬����ԭͼ��
				gauss_pyr[o][i] = cvCloneImage(base); 
            else if( i == 0 )//�µ�һ����ײ�ͼ��������һ�����һ��ͼ���²����õ�
				gauss_pyr[o][i] = downsample( gauss_pyr[o-1][intvls] );	
            else//����һ��ͼ����и�˹ƽ���õ���ǰ��ͼ��
            {   //����ͼ��
				gauss_pyr[o][i] = cvCreateImage( cvGetSize(gauss_pyr[o][i-1]),IPL_DEPTH_32F, 1 );
                //����һ��ͼ��gauss_pyr[o][i-1]���в���Ϊsig[i]�ĸ�˹ƽ�����õ���ǰ��ͼ��gauss_pyr[o][i]
                cvSmooth( gauss_pyr[o][i-1], gauss_pyr[o][i], CV_GAUSSIAN, 0, 0, sig[i], sig[i] );
			}
		}

    free( sig );//�ͷ�sigma��������

    return gauss_pyr;//���ظ�˹������
}


/*������ͼ�����²����������ķ�֮һ��С��ͼ��(ÿ��ά���ϼ���)��ʹ������ڲ�ֵ����
������
img������ͼ��
����ֵ���²������ͼ��
*/
/*
Downsamples an image to a quarter of its size (half in each dimension)
using nearest-neighbor interpolation
@param img an image
@return Returns an image whose dimensions are half those of img
*/
static IplImage* downsample( IplImage* img )
{
    //�²���ͼ��
    IplImage* smaller = cvCreateImage( cvSize(img->width / 2, img->height / 2), img->depth, img->nChannels );
    cvResize( img, smaller, CV_INTER_NN );//�ߴ�任

	return smaller;
}


/*ͨ���Ը�˹��������ÿ��������ͼ�������������˹��ֽ�����
������
gauss_pyr����˹������
octvs������
intvls��ÿ��Ĳ���
����ֵ����˹��ֽ���������һ��octvs*(intvls+2)��ͼ������
*/
/*
Builds a difference of Gaussians scale space pyramid by subtracting adjacent
intervals of a Gaussian pyramid
@param gauss_pyr Gaussian scale-space pyramid
@param octvs number of octaves of scale space
@param intvls number of intervals per octave
@return Returns a difference of Gaussians scale space pyramid as an octvs x (intvls + 2) array
*/
static IplImage*** build_dog_pyr( IplImage*** gauss_pyr, int octvs, int intvls )
{
	IplImage*** dog_pyr;
	int i, o;

    //Ϊ��˹��ֽ���������ռ䣬��octvs��Ԫ�أ�ÿ��Ԫ����һ��ͼ�����ָ��
	dog_pyr = calloc( octvs, sizeof( IplImage** ) );
    //Ϊ��i��ͼ��dog_pyr[i]����ռ䣬��(intvls+2)��Ԫ�أ�ÿ��Ԫ����һ��ͼ��ָ��
	for( i = 0; i < octvs; i++ )
		dog_pyr[i] = calloc( intvls + 2, sizeof(IplImage*) );

    //������������ͼ��
    for( o = 0; o < octvs; o++ )//������
        for( i = 0; i < intvls + 2; i++ )//������
        {   //����DoG�������ĵ�o���i��Ĳ��ͼ��
            dog_pyr[o][i] = cvCreateImage( cvGetSize(gauss_pyr[o][i]), IPL_DEPTH_32F, 1 );
            //����˹�������ĵ�o���i+1��ͼ��͵�i��ͼ��������õ�DoG�������ĵ�o���i��ͼ��
			cvSub( gauss_pyr[o][i+1], gauss_pyr[o][i], dog_pyr[o][i], NULL );
		}

    return dog_pyr;//���ظ�˹��ֽ�����
}


/*�ڳ߶ȿռ��м�⼫ֵ�㣬ͨ����ֵ��ȷ��λ��ȥ���ͶԱȶȵĵ㣬ȥ����Ե�㣬���ؼ�⵽������������
������
dog_pyr����˹��ֽ�����
octvs����˹��ֽ�����������
intvls��ÿ��Ĳ���
contr_thr���Աȶ���ֵ����Թ�һ�����ͼ������ȥ�����ȶ�����
cur_thr�������ʱ�ֵ����ֵ������ȥ����Ե����
storage���洢��
����ֵ�����ؼ�⵽�������������
*/
/*
Detects features at extrema in DoG scale space.  Bad features are discarded
based on contrast and ratio of principal curvatures.
@param dog_pyr DoG scale space pyramid
@param octvs octaves of scale space represented by dog_pyr
@param intvls intervals per octave
@param contr_thr low threshold on feature contrast
@param curv_thr high threshold on feature ratio of principal curvatures
@param storage memory storage in which to store detected features
@return Returns an array of detected features whose scales, orientations,
	and descriptors are yet to be determined.
*/
static CvSeq* scale_space_extrema( IplImage*** dog_pyr, int octvs, int intvls,
								   double contr_thr, int curv_thr, CvMemStorage* storage )
{
    CvSeq* features;//����������
    double prelim_contr_thr = 0.5 * contr_thr / intvls;//���صĶԱȶ���ֵ
	struct feature* feat;
	struct detection_data* ddata;
	int o, i, r, c;

    //�ڴ洢��storage�ϴ����洢��ֵ������У����д洢feature�ṹ���͵�����
	features = cvCreateSeq( 0, sizeof(CvSeq), sizeof(struct feature), storage );

    /*������˹��ֽ���������⼫ֵ��*/
    //SIFT_IMG_BORDERָ���߽��ȣ�ֻ���߽������ڵļ�ֵ��
    for( o = 0; o < octvs; o++ )//��o��
        for( i = 1; i <= intvls; i++ )//��i��
            for(r = SIFT_IMG_BORDER; r < dog_pyr[o][0]->height-SIFT_IMG_BORDER; r++)//��r��
                for(c = SIFT_IMG_BORDER; c < dog_pyr[o][0]->width-SIFT_IMG_BORDER; c++)//��c��
                    //���г����ĶԱȶȼ�飬ֻ�е���һ���������ֵ���ڶԱȶ���ֵprelim_contr_thrʱ�ż����������ص��Ƿ�����Ǽ�ֵ
                    //���ú���pixval32f��ȡͼ��dog_pyr[o][i]�ĵ�r�е�c�еĵ������ֵ��Ȼ�����ABS���������ֵ
					if( ABS( pixval32f( dog_pyr[o][i], r, c ) ) > prelim_contr_thr )
                        //ͨ���ڳ߶ȿռ��н�һ�����ص��ֵ������Χ3*3*3�����ڵĵ�Ƚ��������˵��Ƿ�ֵ��(����ֵ��С����)
                        if( is_extremum( dog_pyr, o, i, r, c ) )//���Ǽ�ֵ��
						{
                            //���ڼ�ֵ��ļ��������ɢ�ռ��н��еģ����Լ�⵽�ļ�ֵ�㲢��һ�������������ϵļ�ֵ��
                            //��Ϊ�����ļ�ֵ�����λ����������֮�䣬������ɢ�ռ���ֻ�ܾ�ȷ������㾫����
                            //ͨ�������ؼ���ֵ���м�ֵ�㾫ȷ��λ(������ֵ������)����ȥ���ͶԱȶȵļ�ֵ�㣬������������������feature�ṹ����
							feat = interp_extremum(dog_pyr, o, i, r, c, intvls, contr_thr);
                            //����ֵ�ǿգ������˵��ѱ��ɹ�����
                            if( feat )
							{
                                //���ú�feat_detection_data����ȡ����feat�е�feature_data��Ա��ת��Ϊdetection_data���͵�ָ��
								ddata = feat_detection_data( feat );
                                //ȥ����Ե��Ӧ����ͨ�����������ʱ�ֵ�ж�ĳ���Ƿ��Ե��,����ֵΪ0��ʾ���Ǳ�Ե�㣬����������
                                if( ! is_too_edge_like( dog_pyr[ddata->octv][ddata->intvl], ddata->r, ddata->c, curv_thr ) )
								{
                                    cvSeqPush( features, feat );//������������featuresĩβ�����¼�⵽��������feat
								}
								else
									free( ddata );
								free( feat );
							}
						}

    return features;//��������������
}


/*ͨ���ڳ߶ȿռ��н�һ�����ص��ֵ������Χ3*3*3�����ڵĵ�Ƚ��������˵��Ƿ�ֵ��(����ֵ��С����)
������
dog_pyr����˹��ֽ�����
octv�����ص����ڵ���
intvl�����ص����ڵĲ�
r�����ص����ڵ���
c�����ص����ڵ���
����ֵ����ָ�������ص��Ǽ�ֵ��(����ֵ��Сֵ)������1�����򷵻�0
*/
/*
Determines whether a pixel is a scale-space extremum by comparing it to it's 3x3x3 pixel neighborhood.
@param dog_pyr DoG scale space pyramid
@param octv pixel's scale space octave
@param intvl pixel's within-octave interval
@param r pixel's image row
@param c pixel's image col
@return Returns 1 if the specified pixel is an extremum (max or min) among it's 3x3x3 pixel neighborhood.
*/
static int is_extremum( IplImage*** dog_pyr, int octv, int intvl, int r, int c )
{
    //���ú���pixval32f��ȡͼ��dog_pyr[octv][intvl]�ĵ�r�е�c�еĵ������ֵ
	float val = pixval32f( dog_pyr[octv][intvl], r, c );
	int i, j, k;

    //����Ƿ����ֵ
	if( val > 0 )
	{
        for( i = -1; i <= 1; i++ )//��
            for( j = -1; j <= 1; j++ )//��
                for( k = -1; k <= 1; k++ )//��
					if( val < pixval32f( dog_pyr[octv][intvl+i], r + j, c + k ) )
						return 0;
	}
    //����Ƿ���Сֵ
	else
	{
        for( i = -1; i <= 1; i++ )//��
            for( j = -1; j <= 1; j++ )//��
                for( k = -1; k <= 1; k++ )//��
					if( val > pixval32f( dog_pyr[octv][intvl+i], r + j, c + k ) )
						return 0;
	}

	return 1;
}


/*ͨ�������ؼ���ֵ���м�ֵ�㾫ȷ��λ(������ֵ������)����ȥ���ͶԱȶȵļ�ֵ�㣬������������������feature�ṹ����
������
dog_pyr����˹��ֽ�����
octv�����ص����ڵ���
intvl�����ص����ڵĲ�
r�����ص����ڵ���
c�����ص����ڵ���
intvls��ÿ��Ĳ���
contr_thr���Աȶ���ֵ����Թ�һ�����ͼ������ȥ�����ȶ�����
����ֵ�����ؾ���ֵ�������������(feature����)���������޴β�ֵ��Ȼ�޷���ȷ������������߸õ�Աȶȹ��ͣ�����NULL
*/
/*
Interpolates a scale-space extremum's location and scale to subpixel
accuracy to form an image feature.  Rejects features with low contrast.
Based on Section 4 of Lowe's paper.  
@param dog_pyr DoG scale space pyramid
@param octv feature's octave of scale space
@param intvl feature's within-octave interval
@param r feature's image row
@param c feature's image column
@param intvls total intervals per octave
@param contr_thr threshold on feature contrast
@return Returns the feature resulting from interpolation of the given
	parameters or NULL if the given location could not be interpolated or
	if contrast at the interpolated loation was too low.  If a feature is
	returned, its scale, orientation, and descriptor are yet to be determined.
*/
static struct feature* interp_extremum( IplImage*** dog_pyr, int octv, int intvl,
										int r, int c, int intvls, double contr_thr )
{
    struct feature* feat;//�������������
    struct detection_data* ddata;//����������йصĽṹ������feature�ṹ��feature_data��Ա��
    double xi, xr, xc, contr;//xi,xr,xc�ֱ�Ϊ�����ص�intvl(��),row(y),col(x)�����ϵ�����(ƫ����)
    int i = 0;//��ֵ����

    //SIFT_MAX_INTERP_STEPSָ���˹ؼ��������ֵ������������������ٴΣ�Ĭ����5
	while( i < SIFT_MAX_INTERP_STEPS )
	{
        //����һ�μ�ֵ���ֵ�������(�㷽��,intvl����)��y��x�����ϵ�������ƫ����(����)
		interp_step( dog_pyr, octv, intvl, r, c, &xi, &xr, &xc );
        //�������ⷽ���ϵ�ƫ��������0.5ʱ����ζ�Ų�ֵ�����Ѿ�ƫ�Ƶ������ٽ����ϣ����Ա���ı䵱ǰ�ؼ����λ������
        if( ABS( xi ) < 0.5  &&  ABS( xr ) < 0.5  &&  ABS( xc ) < 0.5 )//����������ƫ������С��0.5����ʾ�Ѿ�����ȷ�����ü�����ֵ
			break;

        //�����ؼ�������꣬x,y,���������ϵ�ԭ�������ƫ����ȡ��(��������)
        c += cvRound( xc );//x��������
        r += cvRound( xr );//y��������
        intvl += cvRound( xi );//�ҷ��򣬼��㷽��

        //�����������󳬳���Χ���������ֵ������NULL
        if( intvl < 1  ||           //�������֮��Խ��
			intvl > intvls  ||
            c < SIFT_IMG_BORDER  ||   //���������֮�󵽱߽�����
			r < SIFT_IMG_BORDER  ||
			c >= dog_pyr[octv][0]->width - SIFT_IMG_BORDER  ||
			r >= dog_pyr[octv][0]->height - SIFT_IMG_BORDER )
		{
			return NULL;
		}

		i++;
	}

    //������SIFT_MAX_INTERP_STEPS�β�ֵ��û������������ľ�ȷλ�ã��򷵻�NULL���������˼�ֵ��
	if( i >= SIFT_MAX_INTERP_STEPS )
		return NULL;

    //���㱻��ֵ��ĶԱȶȣ�D + 0.5 * dD^T * X
	contr = interp_contr( dog_pyr, octv, intvl, r, c, xi, xr, xc );
    if( ABS( contr ) < contr_thr / intvls )//���õ�Աȶȹ�С������������NULL
		return NULL;

    //Ϊһ��������feature�ṹ����ռ䲢��ʼ��������������ָ��
	feat = new_feature();
    //���ú�feat_detection_data����ȡ����feat�е�feature_data��Ա��ת��Ϊdetection_data���͵�ָ��
	ddata = feat_detection_data( feat );

    //������������긳ֵ��������feat
    //ԭͼ���������x���꣬��Ϊ��octv���е�ͼ�ĳߴ��ԭͼС2^octv������������ֵҪ����2^octv
    feat->img_pt.x = feat->x = ( c + xc ) * pow( 2.0, octv );
    //ԭͼ���������y���꣬��Ϊ��octv���е�ͼ�ĳߴ��ԭͼС2^octv������������ֵҪ����2^octv
	feat->img_pt.y = feat->y = ( r + xr ) * pow( 2.0, octv );

    ddata->r = r;//���������ڵ���
    ddata->c = c;//���������ڵ���
    ddata->octv = octv;//��˹��ֽ������У����������ڵ���
    ddata->intvl = intvl;//��˹��ֽ������У����������ڵ����еĲ�
    ddata->subintvl = xi;//�������ڲ㷽��(�ҷ���,intvl����)�ϵ�������ƫ����

    return feat;//����������ָ��
}


/*����һ�μ�ֵ���ֵ������x��y���ҷ���(�㷽��)�ϵ�������ƫ����(����)
������
dog_pyr����˹��ֽ�����
octv�����ص����ڵ���
intvl�����ص����ڵĲ�
r�����ص����ڵ���
c�����ص����ڵ���
xi������������㷽���ϵ�����������(ƫ��)
xr�����������y�����ϵ�����������(ƫ��)
xc�����������x�����ϵ�����������(ƫ��)
*/
/*
Performs one step of extremum interpolation.  Based on Eqn. (3) in Lowe's paper.
@param dog_pyr difference of Gaussians scale space pyramid
@param octv octave of scale space
@param intvl interval being interpolated
@param r row being interpolated
@param c column being interpolated
@param xi output as interpolated subpixel increment to interval
@param xr output as interpolated subpixel increment to row
@param xc output as interpolated subpixel increment to col
*/
static void interp_step( IplImage*** dog_pyr, int octv, int intvl, int r, int c,
						 double* xi, double* xr, double* xc )
{
	CvMat* dD, * H, * H_inv, X;
	double x[3] = { 0 };

    //��DoG�������м���ĳ���x����y�����Լ��߶ȷ����ϵ�ƫ��������������������dD��
	dD = deriv_3D( dog_pyr, octv, intvl, r, c );
    //��DoG�������м���ĳ���3*3��ɭ����
	H = hessian_3D( dog_pyr, octv, intvl, r, c );
    H_inv = cvCreateMat( 3, 3, CV_64FC1 );//��ɭ���������
	cvInvert( H, H_inv, CV_SVD );
	cvInitMatHeader( &X, 3, 1, CV_64FC1, x, CV_AUTOSTEP );
    //X = - H^(-1) * dD��H������Ԫ�طֱ���x,y,�ҷ����ϵ�ƫ����(�����SIFT�㷨˵��)
	cvGEMM( H_inv, dD, -1, NULL, 0, &X, 0 );

	cvReleaseMat( &dD );
	cvReleaseMat( &H );
	cvReleaseMat( &H_inv );

    *xi = x[2];//�ҷ���(�㷽��)ƫ����
    *xr = x[1];//y������ƫ����
    *xc = x[0];//x������ƫ����
}


/*��DoG�������м���ĳ���x����y�����Լ��߶ȷ����ϵ�ƫ����
������
dog_pyr����˹��ֽ�����
octv�����ص����ڵ���
intvl�����ص����ڵĲ�
r�����ص����ڵ���
c�����ص����ڵ���
����ֵ������3��ƫ������ɵ�������{ dI/dx, dI/dy, dI/ds }^T
*/
/*
Computes the partial derivatives in x, y, and scale of a pixel in the DoG scale space pyramid.
@param dog_pyr DoG scale space pyramid
@param octv pixel's octave in dog_pyr
@param intvl pixel's interval in octv
@param r pixel's image row
@param c pixel's image col
@return Returns the vector of partial derivatives for pixel I
	{ dI/dx, dI/dy, dI/ds }^T as a CvMat*
*/
static CvMat* deriv_3D( IplImage*** dog_pyr, int octv, int intvl, int r, int c )
{
	CvMat* dI;
	double dx, dy, ds;

    //����������ƫ�����������õĸ������ȡ��ֵ���ݶȼ��㷽��
    //��x�����ϵĲ�������ƴ���ƫ����
	dx = ( pixval32f( dog_pyr[octv][intvl], r, c+1 ) -
		pixval32f( dog_pyr[octv][intvl], r, c-1 ) ) / 2.0;
    //��y�����ϵĲ�������ƴ���ƫ����
	dy = ( pixval32f( dog_pyr[octv][intvl], r+1, c ) -
		pixval32f( dog_pyr[octv][intvl], r-1, c ) ) / 2.0;
    //����Ĳ�������ƴ���߶ȷ����ϵ�ƫ����
	ds = ( pixval32f( dog_pyr[octv][intvl+1], r, c ) -
		pixval32f( dog_pyr[octv][intvl-1], r, c ) ) / 2.0;

    //���������
	dI = cvCreateMat( 3, 1, CV_64FC1 );
	cvmSet( dI, 0, 0, dx );
	cvmSet( dI, 1, 0, dy );
	cvmSet( dI, 2, 0, ds );

	return dI;
}


/*��DoG�������м���ĳ���3*3��ɭ����
    / Ixx  Ixy  Ixs \
    | Ixy  Iyy  Iys |
    \ Ixs  Iys  Iss /
������
dog_pyr����˹��ֽ�����
octv�����ص����ڵ���
intvl�����ص����ڵĲ�
r�����ص����ڵ���
c�����ص����ڵ���
����ֵ������3*3�ĺ�ɭ����
*/
/*
Computes the 3D Hessian matrix for a pixel in the DoG scale space pyramid.
@param dog_pyr DoG scale space pyramid
@param octv pixel's octave in dog_pyr
@param intvl pixel's interval in octv
@param r pixel's image row
@param c pixel's image col
@return Returns the Hessian matrix (below) for pixel I as a CvMat*

    / Ixx  Ixy  Ixs \
    | Ixy  Iyy  Iys |
	\ Ixs  Iys  Iss /
*/
static CvMat* hessian_3D( IplImage*** dog_pyr, int octv, int intvl, int r, int c )
{
	CvMat* H;
	double v, dxx, dyy, dss, dxy, dxs, dys;

    v = pixval32f( dog_pyr[octv][intvl], r, c );//�õ������ֵ

    //�ò�ֽ��ƴ��浹��(���幫ʽ�������ݶȵ���)
    //dxx = f(i+1,j) - 2f(i,j) + f(i-1,j)
    //dyy = f(i,j+1) - 2f(i,j) + f(i,j-1)
	dxx = ( pixval32f( dog_pyr[octv][intvl], r, c+1 ) + 
			pixval32f( dog_pyr[octv][intvl], r, c-1 ) - 2 * v );
	dyy = ( pixval32f( dog_pyr[octv][intvl], r+1, c ) +
			pixval32f( dog_pyr[octv][intvl], r-1, c ) - 2 * v );
	dss = ( pixval32f( dog_pyr[octv][intvl+1], r, c ) +
			pixval32f( dog_pyr[octv][intvl-1], r, c ) - 2 * v );
	dxy = ( pixval32f( dog_pyr[octv][intvl], r+1, c+1 ) -
			pixval32f( dog_pyr[octv][intvl], r+1, c-1 ) -
			pixval32f( dog_pyr[octv][intvl], r-1, c+1 ) +
			pixval32f( dog_pyr[octv][intvl], r-1, c-1 ) ) / 4.0;
	dxs = ( pixval32f( dog_pyr[octv][intvl+1], r, c+1 ) -
			pixval32f( dog_pyr[octv][intvl+1], r, c-1 ) -
			pixval32f( dog_pyr[octv][intvl-1], r, c+1 ) +
			pixval32f( dog_pyr[octv][intvl-1], r, c-1 ) ) / 4.0;
	dys = ( pixval32f( dog_pyr[octv][intvl+1], r+1, c ) -
			pixval32f( dog_pyr[octv][intvl+1], r-1, c ) -
			pixval32f( dog_pyr[octv][intvl-1], r+1, c ) +
			pixval32f( dog_pyr[octv][intvl-1], r-1, c ) ) / 4.0;

    //��ɺ�ɭ����
	H = cvCreateMat( 3, 3, CV_64FC1 );
	cvmSet( H, 0, 0, dxx );
	cvmSet( H, 0, 1, dxy );
	cvmSet( H, 0, 2, dxs );
	cvmSet( H, 1, 0, dxy );
	cvmSet( H, 1, 1, dyy );
	cvmSet( H, 1, 2, dys );
	cvmSet( H, 2, 0, dxs );
	cvmSet( H, 2, 1, dys );
	cvmSet( H, 2, 2, dss );

	return H;
}


/*���㱻��ֵ��ĶԱȶȣ�D + 0.5 * dD^T * X
������
dog_pyr����˹��ֽ�����
octv�����ص����ڵ���
intvl�����ص����ڵĲ�
r�����ص����ڵ���
c�����ص����ڵ���
xi���㷽���ϵ�����������
xr��y�����ϵ�����������
xc��x�����ϵ�����������
����ֵ����ֵ��ĶԱȶ�
*/
/*
Calculates interpolated pixel contrast.  Based on Eqn. (3) in Lowe's paper.
@param dog_pyr difference of Gaussians scale space pyramid
@param octv octave of scale space
@param intvl within-octave interval
@param r pixel row
@param c pixel column
@param xi interpolated subpixel increment to interval
@param xr interpolated subpixel increment to row
@param xc interpolated subpixel increment to col
@param Returns interpolated contrast.
*/
static double interp_contr( IplImage*** dog_pyr, int octv, int intvl, int r,
							int c, double xi, double xr, double xc )
{
	CvMat* dD, X, T;
	double t[1], x[3] = { xc, xr, xi };

    //ƫ������ɵ�������X��������x��y�����������ϵ�ƫ����
	cvInitMatHeader( &X, 3, 1, CV_64FC1, x, CV_AUTOSTEP );
    //����˷��Ľ��T����һ����ֵ
	cvInitMatHeader( &T, 1, 1, CV_64FC1, t, CV_AUTOSTEP );
    //��DoG�������м���ĳ���x����y�����Լ��߶ȷ����ϵ�ƫ��������������������dD��
	dD = deriv_3D( dog_pyr, octv, intvl, r, c );
    //����˷���T = dD^T * X
	cvGEMM( dD, &X, 1, NULL, 0, &T,  CV_GEMM_A_T );
	cvReleaseMat( &dD );

    //���ؼ�����ĶԱȶ�ֵ��D + 0.5 * dD^T * X (���幫ʽ�Ƶ���SIFT�㷨˵��)
	return pixval32f( dog_pyr[octv][intvl], r, c ) + t[0] * 0.5;
}


/*Ϊһ��feature�ṹ����ռ䲢��ʼ��
����ֵ����ʼ����ɵ�feature�ṹ��ָ��
*/
/*
Allocates and initializes a new feature
@return Returns a pointer to the new feature
*/
static struct feature* new_feature( void )
{
    struct feature* feat;//������ָ��
    struct detection_data* ddata;//�����������صĽṹ

    feat = malloc( sizeof( struct feature ) );//����ռ�
    memset( feat, 0, sizeof( struct feature ) );//����
	ddata = malloc( sizeof( struct detection_data ) );
	memset( ddata, 0, sizeof( struct detection_data ) );
    feat->feature_data = ddata;//�����������صĽṹָ�븳ֵ���������feature_data��Ա
    feat->type = FEATURE_LOWE;//Ĭ����LOWE���͵�������

	return feat;
}


/*ȥ����Ե��Ӧ����ͨ�����������ʱ�ֵ�ж�ĳ���Ƿ��Ե��
������
dog_img�������������ڵ�DoGͼ��
r�����������ڵ���
c�����������ڵ���
cur_thr�������ʱ�ֵ����ֵ������ȥ����Ե����
����ֵ��0���˵��ǷǱ�Ե�㣻1���˵��Ǳ�Ե��
*/
/*
Determines whether a feature is too edge like to be stable by computing the
ratio of principal curvatures at that feature.  Based on Section 4.1 of Lowe's paper.
@param dog_img image from the DoG pyramid in which feature was detected
@param r feature row
@param c feature col
@param curv_thr high threshold on ratio of principal curvatures
@return Returns 0 if the feature at (r,c) in dog_img is sufficiently corner-like or 1 otherwise.
*/
static int is_too_edge_like( IplImage* dog_img, int r, int c, int curv_thr )
{
	double d, dxx, dyy, dxy, tr, det;

    /*ĳ������������亣ɭ���������ֵ�����ȣ�Ϊ�˱���ֱ�Ӽ�������ֵ������ֻ��������ֵ�ı�ֵ
      ��ͨ�����㺣ɭ����ļ�tr(H)������ʽdet(H)����������ֵ�ı�ֵ
      ��a�Ǻ�ɭ����Ľϴ�����ֵ��b�ǽ�С������ֵ����a = r*b��r�Ǵ�С����ֵ�ı�ֵ
      tr(H) = a + b; det(H) = a*b;
      tr(H)^2 / det(H) = (a+b)^2 / ab = (r+1)^2/r
      rԽ��Խ�����Ǳ�Ե�㣻����r������(r+1)^2/r ��ֵҲ�������Կ�ͨ��(r+1)^2/r �ж������ʱ�ֵ�Ƿ���������*/
	/* principal curvatures are computed using the trace and det of Hessian */
    d = pixval32f(dog_img, r, c);//���ú���pixval32f��ȡͼ��dog_img�ĵ�r�е�c�еĵ������ֵ

    //�ò�ֽ��ƴ���ƫ���������ɭ����ļ���Ԫ��ֵ
    /*  / dxx  dxy \
        \ dxy  dyy /   */
	dxx = pixval32f( dog_img, r, c+1 ) + pixval32f( dog_img, r, c-1 ) - 2 * d;
	dyy = pixval32f( dog_img, r+1, c ) + pixval32f( dog_img, r-1, c ) - 2 * d;
	dxy = ( pixval32f(dog_img, r+1, c+1) - pixval32f(dog_img, r+1, c-1) -
			pixval32f(dog_img, r-1, c+1) + pixval32f(dog_img, r-1, c-1) ) / 4.0;
    tr = dxx + dyy;//��ɭ����ļ�
    det = dxx * dyy - dxy * dxy;//��ɭ���������ʽ

    //������ʽΪ�������������в�ͬ�ķ��ţ�ȥ���˵�
	/* negative determinant -> curvatures have different signs; reject feature */
	if( det <= 0 )
        return 1;//����1�����˵��Ǳ�Ե��

    //ͨ��ʽ�ӣ�(r+1)^2/r �ж������ʵı�ֵ�Ƿ�������������С����ֵ���������Ǳ�Ե��
	if( tr * tr / det < ( curv_thr + 1.0 )*( curv_thr + 1.0 ) / curv_thr )
        return 0;//���Ǳ�Ե��
    return 1;//�Ǳ�Ե��
}


/*����������������ÿ��������ĳ߶�
������
features������������
sigma����ʼ��˹ƽ������������ʼ�߶�
intvls���߶ȿռ���ÿ��Ĳ���
*/
/*
Calculates characteristic scale for each feature in an array.
@param features array of features
@param sigma amount of Gaussian smoothing per octave of scale space
@param intvls intervals per octave of scale space
*/
static void calc_feature_scales( CvSeq* features, double sigma, int intvls )
{
	struct feature* feat;
	struct detection_data* ddata;
	double intvl;
	int i, n;

    n = features->total;//�ܵ����������

    //����������
	for( i = 0; i < n; i++ )
	{
        //���ú꣬��ȡ����features�еĵ�i��Ԫ�أ���ǿ��ת��Ϊstruct feature����
        feat = CV_GET_SEQ_ELEM( struct feature, features, i );
        //���ú�feat_detection_data����ȡ����feat�е�feature_data��Ա��ת��Ϊdetection_data���͵�ָ��
		ddata = feat_detection_data( feat );
        //���������ڵĲ���ddata->intvl�����������ڲ㷽���ϵ�������ƫ�������õ�������Ľ�Ϊ��ȷ�Ĳ���
        intvl = ddata->intvl + ddata->subintvl;
        //����������ĳ߶�(��ʽ��SIFT�㷨˵��)������ֵ��scl��Ա
		feat->scl = sigma * pow( 2.0, ddata->octv + intvl / intvls );
        //�������������ڵ���ĳ߶ȣ���detection_data��scl_octv��Ա��ֵ
		ddata->scl_octv = sigma * pow( 2.0, intvl / intvls );
	}
}


/*��������������ÿ����������������(�������˽�ͼ��Ŵ�Ϊԭͼ��2��ʱ������������֮�����)
������
features������������
*/
/*
Halves feature coordinates and scale in case the input image was doubled
prior to scale space construction.
@param features array of features
*/
static void adjust_for_img_dbl( CvSeq* features )
{
	struct feature* feat;
	int i, n;

    n = features->total;//�ܵ����������

    //����������
	for( i = 0; i < n; i++ )
	{
        //���ú꣬��ȡ����features�еĵ�i��Ԫ�أ���ǿ��ת��Ϊstruct feature����
		feat = CV_GET_SEQ_ELEM( struct feature, features, i );
        //���������x,y����ͳ߶ȶ�����
		feat->x /= 2.0;
		feat->y /= 2.0;
		feat->scl /= 2.0;
		feat->img_pt.x /= 2.0;
		feat->img_pt.y /= 2.0;
	}
}


/*����ÿ����������ݶ�ֱ��ͼ���ҳ�����������һ���������в�ֹһ�������򣬽����Ϊ����������
������
features������������
gauss_pyr����˹������
*/
/*
Computes a canonical orientation for each image feature in an array.  Based
on Section 5 of Lowe's paper.  This function adds features to the array when
there is more than one dominant orientation at a given feature location.
@param features an array of image features
@param gauss_pyr Gaussian scale space pyramid
*/
static void calc_feature_oris( CvSeq* features, IplImage*** gauss_pyr )
{
	struct feature* feat;
	struct detection_data* ddata;
    double* hist;//����ݶ�ֱ��ͼ������
	double omax;
    int i, j, n = features->total;//���������

    //��������������
	for( i = 0; i < n; i++ )
	{
        //��ÿ�����������feature�ṹ��С���ڴ�
		feat = malloc( sizeof( struct feature ) );
        //�Ƴ�����Ԫ�أ��ŵ�feat��
		cvSeqPopFront( features, feat );
        //���ú�feat_detection_data����ȡ����feat�е�feature_data��Ա��ת��Ϊdetection_data���͵�ָ��
        //detection_data�����д���д����������������ͳ߶ȣ��Լ����ڵĲ����
		ddata = feat_detection_data( feat );

        //����ָ�����ص���ݶȷ���ֱ��ͼ�����ش��ֱ��ͼ�������hist
        hist = ori_hist( gauss_pyr[ddata->octv][ddata->intvl],       //���������ڵ�ͼ��
                        ddata->r, ddata->c,                          //���������������
                        SIFT_ORI_HIST_BINS,                          //Ĭ�ϵ��ݶ�ֱ��ͼ��bin(����)����
                        cvRound( SIFT_ORI_RADIUS * ddata->scl_octv ),//�����㷽��ֵ�����У���������İ뾶Ϊ��3 * 1.5 * ��
                        SIFT_ORI_SIG_FCTR * ddata->scl_octv );       //����ֱ��ͼʱ�ݶȷ�ֵ�ĸ�˹Ȩ�صĳ�ʼֵ

        //���ݶ�ֱ��ͼ���и�˹ƽ�����ֲ���û�з��䲻���Զ������������㲻�ȶ�������,SIFT_ORI_SMOOTH_PASSESָ����ƽ������
		for( j = 0; j < SIFT_ORI_SMOOTH_PASSES; j++ )
			smooth_ori_hist( hist, SIFT_ORI_HIST_BINS );

        //�����ݶ�ֱ��ͼ����������ݶȷ�ֵ��������ֱ��ͼ�����bin��ֵ,���ظ�omax
		omax = dominant_ori( hist, SIFT_ORI_HIST_BINS );
        /*����ǰ�������ֱ��ͼ��ĳ��bin��ֵ���ڸ�������ֵ����������һ�������㲢��ӵ�����������ĩβ
          �����������ָ��feat���Ѿ�������������features���Ƴ��ģ����Լ�ʹ��������û�и�����(�ڶ������ڷ�ֵ��ֵ�ķ���)
          �ں���add_good_ori_features��Ҳ��ִ��һ�ο�¡feat�����䷽����в�ֵ���������������������еĲ���
          ��ֵ��ֵһ������Ϊ��ǰ��������ݶ�ֱ��ͼ�����binֵ��80%                   */
		add_good_ori_features( features, hist, SIFT_ORI_HIST_BINS,
								omax * SIFT_ORI_PEAK_RATIO, feat );
        //�ͷ��ڴ�
		free( ddata );
		free( feat );
		free( hist );
	}
}


/*����ָ�����ص���ݶȷ���ֱ��ͼ�����ش��ֱ��ͼ������
������
img��ͼ��ָ��
r�����������ڵ���
c�����������ڵ���
n��ֱ��ͼ����(bin)�ĸ�����Ĭ����36
rad������뾶���ڴ������м����ݶȷ���ֱ��ͼ
sigma������ֱ��ͼʱ�ݶȷ�ֵ�ĸ�˹Ȩ�صĳ�ʼֵ
����ֵ������һ��nԪ���飬�����Ƿ���ֱ��ͼ��ͳ������
*/
/*
Computes a gradient orientation histogram at a specified pixel.
@param img image
@param r pixel row
@param c pixel col
@param n number of histogram bins
@param rad radius of region over which histogram is computed
@param sigma std for Gaussian weighting of histogram entries
@return Returns an n-element array containing an orientation histogram
	representing orientations between 0 and 2 PI.
*/
static double* ori_hist( IplImage* img, int r, int c, int n, int rad, double sigma)
{
    double* hist;//ֱ��ͼ����
	double mag, ori, w, exp_denom, PI2 = CV_PI * 2.0;
	int bin, i, j;

    //Ϊֱ��ͼ�������ռ䣬��n��Ԫ�أ�n�����ĸ���
	hist = calloc( n, sizeof( double ) );
	exp_denom = 2.0 * sigma * sigma;

    //������ָ����Ϊ���ĵ���������
	for( i = -rad; i <= rad; i++ )
		for( j = -rad; j <= rad; j++ )
            //����ָ������ݶȵķ�ֵmag�ͷ���ori������ֵΪ1��ʾ����ɹ�
			if( calc_grad_mag_ori( img, r + i, c + j, &mag, &ori ) )
			{
                w = exp( -( i*i + j*j ) / exp_denom );//�õ���ݶȷ�ֵȨ��
                bin = cvRound( n * ( ori + CV_PI ) / PI2 );//�����ݶȵķ����Ӧ��ֱ��ͼ�е�bin�±�
				bin = ( bin < n )? bin : 0;
                hist[bin] += w * mag;//��ֱ��ͼ��ĳ��bin���ۼӼ�Ȩ��ķ�ֵ
			}

    return hist;//����ֱ��ͼ����
}


/*����ָ������ݶȵķ�ֵmagnitude�ͷ���orientation
������
img��ͼ��ָ��
r�����������ڵ���
c�����������ڵ���
img������������˵���ݶȷ�ֵ
ori������������˵���ݶȷ���
����ֵ�����ָ���ĵ��ǺϷ��㲢�Ѽ������ֵ�ͷ��򣬷���1�����򷵻�0
*/
/*
Calculates the gradient magnitude and orientation at a given pixel.
@param img image
@param r pixel row
@param c pixel col
@param mag output as gradient magnitude at pixel (r,c)
@param ori output as gradient orientation at pixel (r,c)
@return Returns 1 if the specified pixel is a valid one and sets mag and
	ori accordingly; otherwise returns 0
*/
static int calc_grad_mag_ori( IplImage* img, int r, int c, double* mag, double* ori )
{
	double dx, dy;

    //�����������ֵ���м��
	if( r > 0  &&  r < img->height - 1  &&  c > 0  &&  c < img->width - 1 )
	{
        //�ò�ֽ��ƴ���ƫ���������ݶȵķ�ֵ�ͷ���
        dx = pixval32f( img, r, c+1 ) - pixval32f( img, r, c-1 );//x����ƫ��
        dy = pixval32f( img, r-1, c ) - pixval32f( img, r+1, c );//y����ƫ��
        *mag = sqrt( dx*dx + dy*dy );//�ݶȵķ�ֵ�����ݶȵ�ģ
        *ori = atan2( dy, dx );//�ݶȵķ���
		return 1;
	}
    //��������ֵ���Ϸ�������0
	else
		return 0;
}


/*���ݶȷ���ֱ��ͼ���и�˹ƽ�����ֲ���û�з��䲻���Զ������������㲻�ȶ�������
������
hist������ݶ�ֱ��ͼ������
n���ݶ�ֱ��ͼ��bin�ĸ���
*/
/*
Gaussian smooths an orientation histogram.
@param hist an orientation histogram
@param n number of bins
*/
static void smooth_ori_hist( double* hist, int n )
{
	double prev, tmp, h0 = hist[0];
	int i;

	prev = hist[n-1];
    //���ƾ�ֵƯ�Ƶ�һ������ƽ��������ͻ���Ӱ��
	for( i = 0; i < n; i++ )
	{
		tmp = hist[i];
		hist[i] = 0.25 * prev + 0.5 * hist[i] + 
			0.25 * ( ( i+1 == n )? h0 : hist[i+1] );
		prev = tmp;
	}
}


/*�����ݶ�ֱ��ͼ����������ݶȷ�ֵ��������ֱ��ͼ�����bin��ֵ
������
hist�����ֱ��ͼ������
n��ֱ��ͼ��bin�ĸ���
����ֵ������ֱ��ͼ������bin��ֵ
*/
/*
Finds the magnitude of the dominant orientation in a histogram
@param hist an orientation histogram
@param n number of bins
@return Returns the value of the largest bin in hist
*/
static double dominant_ori( double* hist, int n )
{
	double omax;
	int maxbin, i;

	omax = hist[0];
	maxbin = 0;

    //����ֱ��ͼ���ҵ�����bin
	for( i = 1; i < n; i++ )
		if( hist[i] > omax )
		{
			omax = hist[i];
			maxbin = i;
		}
    return omax;//��������bin��ֵ
}


//�������С�������bin��ֵ�Ե�ǰbin����ֱ��ͼ��ֵ������ȡ����ȷ�ķ���Ƕ�ֵ
/*
Interpolates a histogram peak from left, center, and right values
*/
#define interp_hist_peak( l, c, r ) ( 0.5 * ((l)-(r)) / ((l) - 2.0*(c) + (r)) )


/*����ǰ�������ֱ��ͼ��ĳ��bin��ֵ���ڸ�������ֵ����������һ�������㲢��ӵ�����������ĩβ
  �����������ָ��feat���Ѿ�������������features���Ƴ��ģ����Լ�ʹ��������û�и�����(�ڶ������ڷ�ֵ��ֵ�ķ���)
  Ҳ��ִ��һ�ο�¡feat�����䷽����в�ֵ���������������������еĲ���
������
features������������
hist���ݶ�ֱ��ͼ
n��ֱ��ͼ��bin�ĸ���
mag_thr����ֵ��ֵ����ֱ��ͼ����bin��ֵ���ڴ���ֵ����������������
feat��һ��������ָ�룬�µ��������¡��feat��������ͬ
*/
/*
Adds features to an array for every orientation in a histogram greater than a specified threshold.
@param features new features are added to the end of this array
@param hist orientation histogram
@param n number of bins in hist
@param mag_thr new features are added for entries in hist greater than this
@param feat new features are clones of this with different orientations
*/
static void add_good_ori_features( CvSeq* features, double* hist, int n,
								   double mag_thr, struct feature* feat )
{
	struct feature* new_feat;
	double bin, PI2 = CV_PI * 2.0;
	int l, r, i;

    //����ֱ��ͼ
	for( i = 0; i < n; i++ )
	{
        l = ( i == 0 )? n - 1 : i-1;//ǰһ��(��ߵ�)bin���±�
        r = ( i + 1 ) % n;//��һ��(�ұߵ�)bin���±�

        //����ǰ��bin�Ǿֲ���ֵ(��ǰһ���ͺ�һ��bin����)������ֵ���ڸ����ķ�ֵ��ֵ����������һ�������㲢��ӵ�����������ĩβ
		if( hist[i] > hist[l]  &&  hist[i] > hist[r]  &&  hist[i] >= mag_thr )
		{
            //�������С�������bin��ֵ�Ե�ǰbin����ֱ��ͼ��ֵ
			bin = i + interp_hist_peak( hist[l], hist[i], hist[r] );
            bin = ( bin < 0 )? n + bin : ( bin >= n )? bin - n : bin;//����ֵ����淶��[0,n]��
            new_feat = clone_feature( feat );//��¡��ǰ������Ϊ��������
            new_feat->ori = ( ( PI2 * bin ) / n ) - CV_PI;//��������ķ���
            cvSeqPush( features, new_feat );//���뵽����������ĩβ
			free( new_feat );
		}
	}
}


/*�������feature�ṹ����������������ؿ�¡���ɵ��������ָ��
������
feat����Ҫ����¡���������ָ��
����ֵ���������ɵ��������ָ��
*/
/*
Makes a deep copy of a feature
@param feat feature to be cloned
@return Returns a deep copy of feat
*/
static struct feature* clone_feature( struct feature* feat )
{
	struct feature* new_feat;
	struct detection_data* ddata;

    //Ϊһ��feature�ṹ����ռ䲢��ʼ��
    new_feat = new_feature();
    //���ú�feat_detection_data����ȡ����feat�е�feature_data��Ա��ת��Ϊdetection_data���͵�ָ��
	ddata = feat_detection_data( new_feat );
    //���ڴ�ռ���и�ֵ
	memcpy( new_feat, feat, sizeof( struct feature ) );
	memcpy( ddata, feat_detection_data(feat), sizeof( struct detection_data ) );
	new_feat->feature_data = ddata;

    return new_feat;//���ؿ�¡���ɵ��������ָ��
}


/*����������������ÿ�����������������������
������
features������������
gauss_pyr����˹������ͼ����
d�����㷽��ֱ��ͼʱ���������㸽������Ϊd*d������ÿ����������һ��ֱ��ͼ
n��ÿ������ֱ��ͼ��bin����
*/
/*
Computes feature descriptors for features in an array.  Based on Section 6 of Lowe's paper.
@param features array of features
@param gauss_pyr Gaussian scale space pyramid
@param d width of 2D array of orientation histograms
@param n number of bins per orientation histogram
*/
static void compute_descriptors( CvSeq* features, IplImage*** gauss_pyr, int d, int n)
{
	struct feature* feat;
	struct detection_data* ddata;
    double*** hist;//d*d*n����άֱ��ͼ����
    int i, k = features->total;//������ĸ���

    //���������������е�������
	for( i = 0; i < k; i++ )
	{
        //���ú꣬��ȡ����features�еĵ�i��Ԫ�أ���ǿ��ת��Ϊstruct feature����
		feat = CV_GET_SEQ_ELEM( struct feature, features, i );
        //���ú�feat_detection_data����ȡ����feat�е�feature_data��Ա��ת��Ϊdetection_data���͵�ָ��
		ddata = feat_detection_data( feat );
        //���������㸽������ķ���ֱ��ͼ����ֱ��ͼ�ڼ���������������Ҫ�õ�������ֵ��һ��d*d*n����ά����
		hist = descr_hist( gauss_pyr[ddata->octv][ddata->intvl], ddata->r,
			ddata->c, feat->ori, ddata->scl_octv, d, n );
        //��ĳ������ķ���ֱ��ͼת��Ϊ���������������������������ӹ�һ����������Ԫ��ת��Ϊ���ͣ�����������feat��
		hist_to_descr( hist, d, n, feat );
        //�ͷ�������ķ���ֱ��ͼ
		release_descr_hist( &hist, d );
	}
}


/*���������㸽������ķ���ֱ��ͼ����ֱ��ͼ�ڼ���������������Ҫ�õ�������ֵ��һ��d*d*n����ά����
������
img��ͼ��ָ��
r�����������ڵ���
c�����������ڵ���
ori���������������
scl��������ĳ߶�
d�����㷽��ֱ��ͼʱ���������㸽������Ϊd*d������ÿ����������һ��ֱ��ͼ��Ĭ��dΪ4
n��ÿ��ֱ��ͼ��bin�ĸ���
����ֵ��double���͵���ά���飬��һ��d*d�Ķ�ά���飬������ÿ��Ԫ����һ����n��bin��ֱ��ͼ����
*/
/*
Computes the 2D array of orientation histograms that form the feature
descriptor.  Based on Section 6.1 of Lowe's paper.
@param img image used in descriptor computation
@param r row coord of center of orientation histogram array
@param c column coord of center of orientation histogram array
@param ori canonical orientation of feature whose descr is being computed
@param scl scale relative to img of feature whose descr is being computed
@param d width of 2d array of orientation histograms
@param n bins per orientation histogram
@return Returns a d x d array of n-bin orientation histograms.
*/
static double*** descr_hist( IplImage* img, int r, int c, double ori,
							 double scl, int d, int n )
{
    double*** hist;//d*d*n����άֱ��ͼ����
	double cos_t, sin_t, hist_width, exp_denom, r_rot, c_rot, grad_mag,
		grad_ori, w, rbin, cbin, obin, bins_per_rad, PI2 = 2.0 * CV_PI;
	int radius, i, j;

    //Ϊֱ��ͼ�������ռ�
    hist = calloc( d, sizeof( double** ) );//Ϊ��һά����ռ�
	for( i = 0; i < d; i++ )
	{
        hist[i] = calloc( d, sizeof( double* ) );//Ϊ�ڶ�ά����ռ�
		for( j = 0; j < d; j++ )
            hist[i][j] = calloc( n, sizeof( double ) );//Ϊ����ά����ռ�
	}

    //Ϊ�˱�֤���������Ӿ�����ת�����ԣ�Ҫ��������Ϊ���ģ��ڸ�����������ת�Ƚǣ�����תΪ������ķ���
	cos_t = cos( ori );
	sin_t = sin( ori );

	bins_per_rad = n / PI2;
	exp_denom = d * d * 0.5;
    //�������������ӹ����У���������Χ��d*d�������У�ÿ������Ŀ��Ϊm*�Ҹ����أ�SIFT_DESCR_SCL_FCTR��m��Ĭ��ֵ����Ϊ������ĳ߶�
	hist_width = SIFT_DESCR_SCL_FCTR * scl;
    //���ǵ�Ҫ����˫���Բ�ֵ��ÿ������Ŀ��ӦΪ:SIFT_DESCR_SCL_FCTR * scl * ( d + 1.0 )
    //�ڿ��ǵ���ת���أ�ÿ������Ŀ��ӦΪ��SIFT_DESCR_SCL_FCTR * scl * ( d + 1.0 ) * sqrt(2)
    //���������İ뾶�ǣ�SIFT_DESCR_SCL_FCTR * scl * ( d + 1.0 ) * sqrt(2) / 2
	radius = hist_width * sqrt(2) * ( d + 1.0 ) * 0.5 + 0.5;

    //����ÿ�����������
	for( i = -radius; i <= radius; i++ )
		for( j = -radius; j <= radius; j++ )
		{
			/*
			Calculate sample's histogram array coords rotated relative to ori.
			Subtract 0.5 so samples that fall e.g. in the center of row 1 (i.e.
			r_rot = 1.5) have full weight placed in row 1 after interpolation.
			*/
            //������תΪ������
            //���濴������
			c_rot = ( j * cos_t - i * sin_t ) / hist_width;
			r_rot = ( j * sin_t + i * cos_t ) / hist_width;
			rbin = r_rot + d / 2 - 0.5;
			cbin = c_rot + d / 2 - 0.5;

			if( rbin > -1.0  &&  rbin < d  &&  cbin > -1.0  &&  cbin < d )
				if( calc_grad_mag_ori( img, r + i, c + j, &grad_mag, &grad_ori ))
				{
					grad_ori -= ori;
					while( grad_ori < 0.0 )
						grad_ori += PI2;
					while( grad_ori >= PI2 )
						grad_ori -= PI2;

					obin = grad_ori * bins_per_rad;
					w = exp( -(c_rot * c_rot + r_rot * r_rot) / exp_denom );
					interp_hist_entry( hist, rbin, cbin, obin, grad_mag * w, d, n );
				}
		}

	return hist;
}


//������˫���Բ�ֵ������Ŀ�����
/*
Interpolates an entry into the array of orientation histograms that form the feature descriptor.
@param hist 2D array of orientation histograms
@param rbin sub-bin row coordinate of entry
@param cbin sub-bin column coordinate of entry
@param obin sub-bin orientation coordinate of entry
@param mag size of entry
@param d width of 2D array of orientation histograms
@param n number of bins per orientation histogram
*/
static void interp_hist_entry( double*** hist, double rbin, double cbin,
							   double obin, double mag, int d, int n )
{
	double d_r, d_c, d_o, v_r, v_c, v_o;
	double** row, * h;
	int r0, c0, o0, rb, cb, ob, r, c, o;

	r0 = cvFloor( rbin );
	c0 = cvFloor( cbin );
	o0 = cvFloor( obin );
	d_r = rbin - r0;
	d_c = cbin - c0;
	d_o = obin - o0;

	/*
	The entry is distributed into up to 8 bins.  Each entry into a bin
	is multiplied by a weight of 1 - d for each dimension, where d is the
	distance from the center value of the bin measured in bin units.
	*/
	for( r = 0; r <= 1; r++ )
	{
		rb = r0 + r;
		if( rb >= 0  &&  rb < d )
		{
			v_r = mag * ( ( r == 0 )? 1.0 - d_r : d_r );
			row = hist[rb];
			for( c = 0; c <= 1; c++ )
			{
				cb = c0 + c;
				if( cb >= 0  &&  cb < d )
				{
					v_c = v_r * ( ( c == 0 )? 1.0 - d_c : d_c );
					h = row[cb];
					for( o = 0; o <= 1; o++ )
					{
						ob = ( o0 + o ) % n;
						v_o = v_c * ( ( o == 0 )? 1.0 - d_o : d_o );
						h[ob] += v_o;
					}
				}
			}
		}
	}
}


/*��ĳ������ķ���ֱ��ͼת��Ϊ���������������������������ӹ�һ����������Ԫ��ת��Ϊ���ͣ�����ָ����������
������
hist��d*d*n����άֱ��ͼ����
d�����㷽��ֱ��ͼʱ���������㸽������Ϊd*d������ÿ����������һ��ֱ��ͼ
n��ÿ��ֱ��ͼ��bin����
feat��������ָ�룬������õ����������Ӵ�������
*/
/*
Converts the 2D array of orientation histograms into a feature's descriptor vector.
@param hist 2D array of orientation histograms
@param d width of hist
@param n bins per histogram
@param feat feature into which to store descriptor
*/
static void hist_to_descr( double*** hist, int d, int n, struct feature* feat )
{
	int int_val, i, r, c, o, k = 0;

    //����d*d*n����άֱ��ͼ���飬�����е���������(һ����128��)������feat�ṹ��descr��Ա��
	for( r = 0; r < d; r++ )
		for( c = 0; c < d; c++ )
			for( o = 0; o < n; o++ )
				feat->descr[k++] = hist[r][c][o];

    feat->d = k;//���������ӵ�ά����һ����128
    //��һ������������������ӣ���������������������ÿ��Ԫ�س������������ӵ�ģ
	normalize_descr( feat );

    //����������������������������ֵSIFT_DESCR_MAG_THR��Ԫ��ǿ�и�ֵΪSIFT_DESCR_MAG_THR
	for( i = 0; i < k; i++ )
		if( feat->descr[i] > SIFT_DESCR_MAG_THR )
			feat->descr[i] = SIFT_DESCR_MAG_THR;
    //�ٴι�һ����������������
	normalize_descr( feat );

	/* convert floating-point descriptor to integer valued descriptor */
    //��������������������ÿ��Ԫ�س���ϵ��SIFT_INT_DESCR_FCTR����Ϊ���ͣ��������ֵ���ܳ���255
	for( i = 0; i < k; i++ )
	{
		int_val = SIFT_INT_DESCR_FCTR * feat->descr[i];
		feat->descr[i] = MIN( 255, int_val );
	}
}


/*��һ������������������ӣ���������������������ÿ��Ԫ�س������������ӵ�ģ
*/
/*
Normalizes a feature's descriptor vector to unitl length
@param feat feature
*/
static void normalize_descr( struct feature* feat )
{
	double cur, len_inv, len_sq = 0.0;
    int i, d = feat->d;//���������ӵ�ά��

    //�����������ӵ�ģ
	for( i = 0; i < d; i++ )
	{
		cur = feat->descr[i];
		len_sq += cur*cur;
	}
	len_inv = 1.0 / sqrt( len_sq );
    //������������ÿ��Ԫ�س������������ӵ�ģ����ɹ�һ��
	for( i = 0; i < d; i++ )
		feat->descr[i] *= len_inv;
}


/*�ȽϺ������������㰴�߶ȵĽ������У���������������CvSeqSort��
������
feat1����һ���������ָ��
feat2���ڶ����������ָ��
param���û��Զ�����������ﲻʹ��
����ֵ�����feat1�ĳ߶ȴ���feat2�ĳ߶ȣ�����1�����򷵻�-1������ȷ���0(������)
*/
/*
Compares features for a decreasing-scale ordering.  Intended for use with CvSeqSort
@param feat1 first feature
@param feat2 second feature
@param param unused
@return Returns 1 if feat1's scale is greater than feat2's, -1 if vice versa,
and 0 if their scales are equal
*/
static int feature_cmp( void* feat1, void* feat2, void* param )
{
    //������Ĳ���ǿ��ת��Ϊstruct feature���͵�ָ��
	struct feature* f1 = (struct feature*) feat1;
	struct feature* f2 = (struct feature*) feat2;

    //�Ƚ�����������ĳ߶�ֵ
	if( f1->scl < f2->scl )
		return 1;
	if( f1->scl > f2->scl )
		return -1;
	return 0;
}


/*�ͷż������������ӹ������õ��ķ���ֱ��ͼ���ڴ�ռ�
������
hist������ֱ��ͼ��ָ�룬��һ��d*d*n����άֱ��ͼ����
d��ֱ��ͼ����ǰ��ά��ά��
  */
/*
De-allocates memory held by a descriptor histogram
@param hist pointer to a 2D array of orientation histograms
@param d width of hist
*/
static void release_descr_hist( double**** hist, int d )
{
	int i, j;

	for( i = 0; i < d; i++)
	{
		for( j = 0; j < d; j++ )
            free( (*hist)[i][j] );//�ͷŵ���ά���ڴ�
        free( (*hist)[i] );//�ͷŵڶ�ά���ڴ�
	}
    free( *hist );//�ͷŵ�һά���ڴ�
	*hist = NULL;
}

/*�ͷŽ�����ͼ����Ĵ洢�ռ�
����:
pyr��������ͼ�����ָ��
octvs��������������
n��ÿһ���ͼ����
*/
/*
De-allocates memory held by a scale space pyramid
@param pyr scale space pyramid
@param octvs number of octaves of scale space
@param n number of images per octave
*/
static void release_pyr( IplImage**** pyr, int octvs, int n )
{
	int i, j;
	for( i = 0; i < octvs; i++ )
	{
		for( j = 0; j < n; j++ )
            cvReleaseImage( &(*pyr)[i][j] );//�ͷ�ÿ��ͼ��
        free( (*pyr)[i] );//�ͷ�ÿ����
	}
    free( *pyr );//�ͷŽ�����
	*pyr = NULL;
}
