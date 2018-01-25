/*
Functions and structures for maintaining a k-d tree database of image
features.

For more information, refer to:

Beis, J. S. and Lowe, D. G.  Shape indexing using approximate
nearest-neighbor search in high-dimensional spaces.  In <EM>Conference
on Computer Vision and Pattern Recognition (CVPR)</EM> (2003),
pp. 1000--1006.

Copyright (C) 2006-2010  Rob Hess <hess@eecs.oregonstate.edu>

@version 1.1.2-20100521
*/

/*
  ���ļ�����k-d���Ľ�����BBF���Һ�����ʵ��
*/

#include "kdtree.h"
#include "minpq.h"
#include "imgfeatures.h"
#include "utils.h"

#include <cxcore.h>

#include <stdio.h>

//BBF���õ��Ľṹ���ɴ洢��ǰ�㵽Ŀ���ľ���
//��kd�����������У����������ݻᱻ��ֵ��feature�ṹ��feature_data��Ա
struct bbf_data
{
    double d;  //�������㵽Ŀ����ŷʽ����ֵ
    void* old_data; //������������feature_data�����ǰ��ֵ
};

/************************ δ��¶�ӿڵ�һЩ���غ��������� **************************/
/************************* Local Function Prototypes *************************/

//�ø����������㼯��ʼ��k-d���ڵ�
static struct kd_node* kd_node_init( struct feature*, int );
//��չָ����k-d���ڵ㼰�����Һ���
static void expand_kd_node_subtree( struct kd_node* );
//ȷ������ڵ����������������ֵ
static void assign_part_key( struct kd_node* );
//�ҵ������������ֵ
static double median_select( double*, int );
//�ҵ����������е�rС����
static double rank_select( double*, int, int );
//�ò��뷨���������������������
static void insertion_sort( double*, int );
//���ݸ���������ֵ�ָ����飬ʹ����ǰ����С��pivot���󲿷ִ���pivot
static int partition_array( double*, int, double );
//��ָ����k-d���ڵ��ϻ��������㼯
static void partition_features( struct kd_node* );
//�Ӹ����������k-d��ֱ��Ҷ�ڵ㣬���������н�δ�����Ľڵ�������ȼ��������
static struct kd_node* explore_to_leaf( struct kd_node*, struct feature*,struct min_pq* );
//����һ�������㵽��������飬ʹ�����еĵ㰴��Ŀ���ľ�����������
static int insert_into_nbr_array( struct feature*, struct feature**, int, int );
//�жϸ������Ƿ���ĳ������
static int within_rect( CvPoint2D64f, CvRect );


/******************** ����kdtree.h�������ĺ��� **********************/
/******************** Functions prototyped in kdtree.h **********************/

/*���ݸ����������㼯�Ͻ���k-d��
������
features�����������飬ע�⣺�˺�������ı�features������Ԫ�ص�����˳��
n�����������
����ֵ�������õ�k-d��������ָ��
*/
/*
A function to build a k-d tree database from keypoints in an array.

@param features an array of features
@param n the number of features in features

@return Returns the root of a kd tree built from features or NULL on error.
*/
struct kd_node* kdtree_build( struct feature* features, int n )
{
	struct kd_node* kd_root;

    //����������
	if( ! features  ||  n <= 0 )
	{
		fprintf( stderr, "Warning: kdtree_build(): no features, %s, line %d\n",
				__FILE__, __LINE__ );
		return NULL;
	}

    //���ú������ø����������㼯��ʼ��k-d���ڵ㣬����ֵ��Ϊ����
	kd_root = kd_node_init( features, n );
    //���ú�������չ���ڵ�kd_root�������Һ���
	expand_kd_node_subtree( kd_root );

	return kd_root;
}


/*��BBF�㷨��k-d���в���ָ���������k�������������
������
kd_root��ͼ��������k-d��������
feat��Ŀ��������
k�����ڸ���
nbrs��k�������������ָ�����飬����Ŀ��������ľ�����������
      ��������ڴ潫�ڱ������б����䣬ʹ���������ڵ��ó��ͷţ�free(*nbrs)
max_nn_chks����������������������ֵ��������
����ֵ���洢��nbrs�еĽ��ڸ���������-1��ʾʧ��
*/
/*
Finds an image feature's approximate k nearest neighbors in a kd tree using
Best Bin First search.
@param kd_root root of an image feature kd tree
@param feat image feature for whose neighbors to search
@param k number of neighbors to find
@param nbrs pointer to an array in which to store pointers to neighbors
	in order of increasing descriptor distance
@param max_nn_chks search is cut off after examining this many tree entries
@return Returns the number of neighbors found and stored in nbrs, or -1 on error.
*/
int kdtree_bbf_knn( struct kd_node* kd_root, struct feature* feat, int k,
					struct feature*** nbrs, int max_nn_chks )
{
    struct kd_node* expl; //expl�ǵ�ǰ�����ڵ�
    struct min_pq* min_pq; //���ȼ�����
    struct feature* tree_feat, ** _nbrs; //tree_feat�ǵ���SIFT������_nbrs�д���Ų��ҳ����Ľ��������ڵ�
    struct bbf_data* bbf_data; //bbf_data��һ�����������ʱ�������ݺ����������Ļ���ṹ
    int i, t = 0, n = 0; //t����������������n�ǵ�ǰ����������е�Ԫ�ظ���

    //����������
	if( ! nbrs  ||  ! feat  ||  ! kd_root )
	{
        fprintf( stderr, "Warning: NULL pointer error, %s, line %d\n", __FILE__, __LINE__ );
		return -1;
	}

    _nbrs = calloc( k, sizeof( struct feature* ) ); //�����ҽ��������Ӧ��С���ڴ�
    min_pq = minpq_init(); //min_pq���г�ʼ��������Ĭ�ϴ�С�Ŀռ�
    minpq_insert( min_pq, kd_root, 0 ); //�����ڵ��Ȳ��뵽min_pq���ȼ�������

    //min_pq����û�л�������δ�ﵽ����������
	while( min_pq->n > 0  &&  t < max_nn_chks )
	{
        //��min_pq����ȡ(���Ƴ�)���ȼ���ߵĽڵ㣬��ֵ����ǰ�ڵ�expl
		expl = (struct kd_node*)minpq_extract_min( min_pq );
		if( ! expl )
        {   //������
			fprintf( stderr, "Warning: PQ unexpectedly empty, %s line %d\n",__FILE__, __LINE__ );
			goto fail;
		}
        //�ӵ�ǰ�����ڵ�explһֱ������Ҷ�ӽڵ㣬���������н�δ�����Ľڵ�������ȼ�������У�����ֵΪҶ�ӽڵ�
		expl = explore_to_leaf( expl, feat, min_pq );
		if( ! expl )
        {   //������
			fprintf( stderr, "Warning: PQ unexpectedly empty, %s line %d\n",__FILE__, __LINE__ );
			goto fail;
		}

        //�Ƚϲ��������
		for( i = 0; i < expl->n; i++ )
		{
            tree_feat = &expl->features[i];//��i���������ָ��
            bbf_data = malloc( sizeof( struct bbf_data ) );//�½�bbf�ṹ
			if( ! bbf_data )
            {   //������
                fprintf( stderr, "Warning: unable to allocate memory," " %s line %d\n", __FILE__, __LINE__ );
				goto fail;
			}
            bbf_data->old_data = tree_feat->feature_data;//�����i���������feature_data����ǰ��ֵ
            bbf_data->d = descr_dist_sq(feat, tree_feat);//��ǰ�������Ŀ���֮���ŷ�Ͼ���
            tree_feat->feature_data = bbf_data;//��bbf�ṹ�������������feature_data��
            //�жϲ�������������������㵽���������_nbrs��,����ɹ�����1
            //�������������Ԫ�ظ����Ѵﵽkʱ����������Ԫ�ظ����������ӣ��������Ԫ�ص�ֵ
			n += insert_into_nbr_array( tree_feat, _nbrs, n, k );
		}
        t++;//��������
	}

    minpq_release( &min_pq );//�ͷ����ȶ���

    //��������������е������㣬�ָ���feature_data���ֵ
	for( i = 0; i < n; i++ )
	{
		bbf_data = _nbrs[i]->feature_data;
        _nbrs[i]->feature_data = bbf_data->old_data;//��֮ǰ�����ݸ�ֵ��feature_data��
		free( bbf_data );
	}
	*nbrs = _nbrs;
	return n;

    //ʧ�ܴ���
fail:
	minpq_release( &min_pq );
    //��������������е������㣬�ָ���feature_data���ֵ
	for( i = 0; i < n; i++ )
	{
		bbf_data = _nbrs[i]->feature_data;
		_nbrs[i]->feature_data = bbf_data->old_data;
		free( bbf_data );
	}
	free( _nbrs );
	*nbrs = NULL;
	return -1;
}



/*
Finds an image feature's approximate k nearest neighbors within a specified
spatial region in a kd tree using Best Bin First search.

@param kd_root root of an image feature kd tree
@param feat image feature for whose neighbors to search
@param k number of neighbors to find
@param nbrs pointer to an array in which to store pointers to neighbors
	in order of increasing descriptor distance
@param max_nn_chks search is cut off after examining this many tree entries
@param rect rectangular region in which to search for neighbors
@param model if true, spatial search is based on kdtree features' model
	locations; otherwise it is based on their image locations

@return Returns the number of neighbors found and stored in \a nbrs
	(in case \a k neighbors could not be found before examining
	\a max_nn_checks keypoint entries).
*/
int kdtree_bbf_spatial_knn( struct kd_node* kd_root, struct feature* feat,
						   int k, struct feature*** nbrs, int max_nn_chks,
						   CvRect rect, int model )
{
	struct feature** all_nbrs, ** sp_nbrs;
	CvPoint2D64f pt;
	int i, n, t = 0;

	n = kdtree_bbf_knn( kd_root, feat, max_nn_chks, &all_nbrs, max_nn_chks );
	sp_nbrs = calloc( k, sizeof( struct feature* ) );
	for( i = 0; i < n; i++ )
	{
		if( model )
			pt = all_nbrs[i]->mdl_pt;
		else
			pt = all_nbrs[i]->img_pt;

		if( within_rect( pt, rect ) )
		{
			sp_nbrs[t++] = all_nbrs[i];
			if( t == k )
				goto end;
		}
	}
end:
	free( all_nbrs );
	*nbrs = sp_nbrs;
	return t;
}


/*�ͷ�k-d��ռ�õĴ洢�ռ�
*/
/*
De-allocates memory held by a kd tree

@param kd_root pointer to the root of a kd tree
*/
void kdtree_release( struct kd_node* kd_root )
{
	if( ! kd_root )
		return;
	kdtree_release( kd_root->kd_left );
	kdtree_release( kd_root->kd_right );
	free( kd_root );
}


/************************ δ��¶�ӿڵ�һЩ���غ��� **************************/
/************************ Functions prototyped here **************************/

/*�ø����������㼯��ʼ��k-d���ڵ�
������
features�������㼯
n�����������
����ֵ��k-d���ڵ�ָ��
*/
/*
Initializes a kd tree node with a set of features.  The node is not
expanded, and no ordering is imposed on the features.

@param features an array of image features
@param n number of features

@return Returns an unexpanded kd-tree node.
*/
static struct kd_node* kd_node_init( struct feature* features, int n )
{
	struct kd_node* kd_node;

    kd_node = malloc( sizeof( struct kd_node ) );//�����ڴ�
	memset( kd_node, 0, sizeof( struct kd_node ) );
    kd_node->ki = -1;//��������
    kd_node->features = features;//�ڵ��Ӧ�������㼯
    kd_node->n = n;//������ĸ���

	return kd_node;
}


/*�ݹ����չָ����k-d���ڵ㼰�����Һ���
*/
/*
Recursively expands a specified kd tree node into a tree whose leaves
contain one entry each.

@param kd_node an unexpanded node in a kd tree
*/
static void expand_kd_node_subtree( struct kd_node* kd_node )
{
    //���������Ҷ�ӽڵ�
	/* base case: leaf node */
	if( kd_node->n == 1  ||  kd_node->n == 0 )
	{
        kd_node->leaf = 1;//Ҷ�ڵ��־λ��Ϊ1
		return;
	}

    //���ú�����ȷ���ڵ����������������ֵ
	assign_part_key( kd_node );
    //��ָ��k-d���ڵ��ϻ��������㼯(������ָ���ڵ��ki��kvֵ�����������㼯)
	partition_features( kd_node );

    //������չ���Һ���
	if( kd_node->kd_left )
		expand_kd_node_subtree( kd_node->kd_left );
	if( kd_node->kd_right )
		expand_kd_node_subtree( kd_node->kd_right );
}


/*ȷ������ڵ����������������ֵ
������kd_node�������k-d���ڵ�
����ִ����󽫸�kd_node��ki��kv��Ա����
*/
/*
Determines the descriptor index at which and the value with which to
partition a kd tree node's features.

@param kd_node a kd tree node
*/
static void assign_part_key( struct kd_node* kd_node )
{
	struct feature* features;
    //�����ֵkv����ֵmean������var���������ֵvar_max
	double kv, x, mean, var, var_max = 0;
	double* tmp;
    int d, n, i, j, ki = 0; //��������ki

	features = kd_node->features;
    n = kd_node->n;//������
    d = features[0].d;//����������ά��

    //���������ֵ���Ƿ���������һά��ά��,��n��128ά�����������У�����kά�ķ��������k��������(�ָ�λ��)
	/* partition key index is that along which descriptors have most variance */
	for( j = 0; j < d; j++ )
	{
		mean = var = 0;
        //���jά�ľ�ֵ
		for( i = 0; i < n; i++ )
			mean += features[i].descr[j];
		mean /= n;
        //���jά�ķ���
		for( i = 0; i < n; i++ )
		{
			x = features[i].descr[j] - mean;
			var += x * x;
		}
		var /= n;
        //�ҵ���󷽲��ά��
		if( var > var_max )
		{
            ki = j;//��󷽲��ά����������
			var_max = var;
		}
	}

    //�����ֵ������������������kiά����ֵ(��kiά������м���Ǹ�ֵ)
	/* partition key value is median of descriptor values at ki */
	tmp = calloc( n, sizeof( double ) );
	for( i = 0; i < n; i++ )
		tmp[i] = features[i].descr[ki];
    //���ú�������tmp�������ֵ
	kv = median_select( tmp, n );
	free( tmp );

    kd_node->ki = ki;//�����ά������
    kd_node->kv = kv;//�����ֵ
}


/*�ҵ������������ֵ
������
array���������飬Ԫ��˳�򽫻ᱻ�Ķ�
n��Ԫ�ظ���
����ֵ����ֵ
*/
/*
Finds the median value of an array.  The array's elements are re-ordered
by this function.

@param array an array; the order of its elelemts is reordered
@param n number of elements in array

@return Returns the median value of array.
*/
static double median_select( double* array, int n )
{
    //���ú�������array�����еĵ�(n-1)/2С����������ֵ
	return rank_select( array, n, (n - 1) / 2 );
}


/*�ҵ����������е�rС����
������
array���������飬Ԫ��˳�򽫻ᱻ�Ķ�
n��Ԫ�ظ���
r���ҵ�rСԪ��
����ֵ����rС��Ԫ��ֵ
*/
/*
Finds the element of a specified rank in an array using the linear time
median-of-medians algorithm by Blum, Floyd, Pratt, Rivest, and Tarjan.
The elements of the array are re-ordered by this function.
@param array an array; the order of its elelemts is reordered
@param n number of elements in array
@param r the zero-based rank of the element to be selected
@return Returns the element from array with zero-based rank r.
*/
static double rank_select( double* array, int n, int r )
{
	double* tmp, med;
	int gr_5, gr_tot, rem_elts, i, j;

	/* base case */
	if( n == 1 )
		return array[0];

    //������ֳ�5��һ�飬��gr_tot��
	/* divide array into groups of 5 and sort them */
    gr_5 = n / 5; //��ĸ���-1��n/5����ȡ��
    gr_tot = cvCeil( n / 5.0 ); //��ĸ�����n/5����ȡ��
    rem_elts = n % 5;//���һ���е�Ԫ�ظ���
	tmp = array;
    //��ÿ����в�������
	for( i = 0; i < gr_5; i++ )
	{
		insertion_sort( tmp, 5 );
		tmp += 5;
	}
    //���һ��
	insertion_sort( tmp, rem_elts );

    //����ֵ����ֵ
	/* recursively find the median of the medians of the groups of 5 */
	tmp = calloc( gr_tot, sizeof( double ) );
    //��ÿ��5Ԫ���е���ֵ(���±�Ϊ2,2+5,...��Ԫ��)���Ƶ�temp����
	for( i = 0, j = 2; i < gr_5; i++, j += 5 )
		tmp[i] = array[j];
    //���һ�����ֵ
	if( rem_elts )
		tmp[i++] = array[n - 1 - rem_elts/2];
    //��temp�е���ֵmed������ֵ����ֵ
	med = rank_select( tmp, i, ( i - 1 ) / 2 );
	free( tmp );

    //������ֵ����ֵ�������飬�����ֽ���Ƿ��ǵ�rС��������������ݹ����rank_select����ѡ��
	/* partition around median of medians and recursively select if necessary */
    j = partition_array( array, n, med );//�������飬����med���������е�����
    if( r == j )//����ǵ�rС����
		return med;
    else if( r < j )//��rС������ǰ�벿��
		return rank_select( array, j, r );
    else//��rС�����ں�벿��
	{
		array += j+1;
		return rank_select( array, ( n - j - 1 ), ( r - j - 1 ) );
	}
}


/*�ò��뷨���������������������
������
array����������
n��Ԫ�ظ���
*/
/*
Sorts an array in place into increasing order using insertion sort.

@param array an array
@param n number of elements
*/
static void insertion_sort( double* array, int n )
{
	double k;
	int i, j;

	for( i = 1; i < n; i++ )
	{
		k = array[i];
		j = i-1;
		while( j >= 0  &&  array[j] > k )
		{
			array[j+1] = array[j];
			j -= 1;
		}
		array[j+1] = k;
	}
}


/*���ݸ���������ֵ�ָ����飬ʹ����ǰ����С��pivot���󲿷ִ���pivot
������
array����������
n�������Ԫ�ظ���
pivot������ֵ
����ֵ���ָ��������±�
*/
/*
Partitions an array around a specified value.
@param array an array
@param n number of elements
@param pivot value around which to partition
@return Returns index of the pivot after partitioning
*/
static int partition_array( double* array, int n, double pivot )
{
	double tmp;
	int p, i, j;

	i = -1;
	for( j = 0; j < n; j++ )
		if( array[j] <= pivot )
		{
			tmp = array[++i];
			array[i] = array[j];
			array[j] = tmp;
			if( array[i] == pivot )
                p = i;//p����������±�
		}
    //����������һ��С����������Ի�
	array[p] = array[i];
	array[i] = pivot;

	return i;
}


/*��ָ����k-d���ڵ��ϻ��������㼯
ʹ�������㼯��ǰ�벿���ǵ�kiάС������ĵ㣬��벿���ǵ�kiά��������ĵ�
*/
/*
Partitions the features at a specified kd tree node to create its two
children.

@param kd_node a kd tree node whose partition key is set
*/
static void partition_features( struct kd_node* kd_node )
{
	struct feature* features, tmp;
	double kv;
	int n, ki, p, i, j = -1;

    features = kd_node->features;//����������
    n = kd_node->n;//���������
    //printf("%d\n",n);
    ki = kd_node->ki;//�����ά������(��һά������)
    kv = kd_node->kv;//�����ֵ
	for( i = 0; i < n; i++ )
    {
        //����i������������������ĵ�kiά��ֵС��kv
		if( features[i].descr[ki] <= kv )
		{
			tmp = features[++j];
			features[j] = features[i];
			features[i] = tmp;
			if( features[j].descr[ki] == kv )
                p = j;//p�����������ڵ�λ��
		}
    }
    //������features[p]�����һ��С������ĵ�features[j]�Ի�
	tmp = features[p];
	features[p] = features[j];
	features[j] = tmp;
    //�˺������λ���±�Ϊj

    //����������������ͬһ�࣬��˽ڵ��ΪҶ�ڵ�
	/* if all records fall on same side of partition, make node a leaf */
	if( j == n - 1 )
	{
		kd_node->leaf = 1;
		return;
	}

    //��ʼ�����ӵĸ��ڵ㣬���ӹ�j+1��������
	kd_node->kd_left = kd_node_init( features, j + 1 );
    //��ʼ���Һ��ӵĸ��ڵ㣬�Һ��ӹ�n-j-1��������
	kd_node->kd_right = kd_node_init( features + ( j + 1 ), ( n - j - 1 ) );
}


/*�Ӹ����������k-d��ֱ��Ҷ�ڵ㣬���������н�δ�����Ľڵ�������ȼ��������
���ȼ����к�����·����ͬʱ���ɵģ���Ҳ��BBF�㷨�ľ������ڣ��ڶ���������ʱ
������·����һ��ķ�֧���뵽���ȼ������У�������ʱ���ҡ������ȼ����е���
����Ǹ���Ŀ��������ָƽ��ľ���ABS(kv - feat->descr[ki])
������
kd_node��Ҫ����������������
feat��Ŀ��������
min_pq�����ȼ�����
����ֵ��Ҷ�ӽڵ��ָ��
*/
/*
Explores a kd tree from a given node to a leaf.  Branching decisions are
made at each node based on the descriptor of a given feature.  Each node
examined but not explored is put into a priority queue to be explored
later, keyed based on the distance from its partition key value to the
given feature's desctiptor.
@param kd_node root of the subtree to be explored
@param feat feature upon which branching decisions are based
@param min_pq a minimizing priority queue into which tree nodes are placed as described above
@return Returns a pointer to the leaf node at which exploration ends or
	NULL on error.
*/
static struct kd_node* explore_to_leaf( struct kd_node* kd_node, struct feature* feat,
										struct min_pq* min_pq )
{
    //unexpl�д�������ȼ����еĺ�ѡ�ڵ�(��δ�����Ľڵ�)��explΪ��ǰ�����ڵ�
	struct kd_node* unexpl, * expl = kd_node;
    double kv;//�ָ������ֵ
    int ki;//�ָ������ά������

    //һֱ������Ҷ�ӽڵ㣬���������н�δ�����Ľڵ�������ȼ��������
	while( expl  &&  ! expl->leaf )
	{
		ki = expl->ki;
		kv = expl->kv;

        //�����ά�����������������ά��������
		if( ki >= feat->d )
		{
            fprintf( stderr, "Warning: comparing imcompatible descriptors, %s" " line %d\n", __FILE__, __LINE__ );
			return NULL;
		}
        //Ŀ��������kiά������С�ڵ���kv����������������
		if( feat->descr[ki] <= kv )
		{
            unexpl = expl->kd_right;//��ѡ�����ڵ���expl���������ĸ�
            expl = expl->kd_left;//��ǰ�����ڵ���expl���������ĸ�
		}
        else//Ŀ��������kiά�����ݴ���kv����������������
		{
            unexpl = expl->kd_left;//��ѡ�����ڵ���expl��������
            expl = expl->kd_right;//��ǰ�����ڵ���expl��������
		}

        //����ѡ�ڵ�unexpl����Ŀ��������kiά���丸�ڵ�ľ�����뵽���ȶ����У�����ԽС�����ȼ�Խ��
		if( minpq_insert( min_pq, unexpl, ABS( kv - feat->descr[ki] ) ) )
		{
			fprintf( stderr, "Warning: unable to insert into PQ, %s, line %d\n",__FILE__, __LINE__ );
			return NULL;
		}
	}

    return expl;//����Ҷ�ӽڵ��ָ��
}


/*����һ�������㵽��������飬ʹ�����еĵ㰴��Ŀ���ľ�����������
������
feat��Ҫ����������㣬��feature_data��Ӧ��ָ��bbf_data�ṹ��ָ�룬���е�dֵʱfeat��Ŀ���ľ����ƽ��
nbrs�����������
n����������������е�Ԫ�ظ���
k�����������Ԫ�ظ��������ֵ
����ֵ����feat�ɹ����룬����1�����򷵻�0
*/
/*
Inserts a feature into the nearest-neighbor array so that the array remains
in order of increasing descriptor distance from the search feature.

@param feat feature to be inderted into the array; it's feature_data field
	should be a pointer to a bbf_data with d equal to the squared descriptor
	distance between feat and the search feature
@param nbrs array of nearest neighbors neighbors
@param n number of elements already in nbrs and
@param k maximum number of elements in nbrs

@return If feat was successfully inserted into nbrs, returns 1; otherwise
	returns 0.
*/
static int insert_into_nbr_array( struct feature* feat, struct feature** nbrs, int n, int k )
{
    struct bbf_data* fdata, * ndata;//fdata��Ҫ����ĵ��bbf�ṹ��ndata������������еĵ��bbf�ṹ
    double dn, df; //dn��������������������bbf�ṹ�еľ���ֵ��df��Ҫ������������bbf�ṹ�еľ���ֵ
	int i, ret = 0;

    //ԭ���������Ϊ��
	if( n == 0 )
	{
		nbrs[0] = feat;
        return 1;//����ɹ�������1
	}

	/* check at end of array */
    fdata = (struct bbf_data*)feat->feature_data;//Ҫ������bbf�ṹ
    df = fdata->d;//Ҫ������������bbf�ṹ�еľ���ֵ
    ndata = (struct bbf_data*)nbrs[n-1]->feature_data;//����������еĵ��bbf�ṹ
    dn = ndata->d;//��������������һ���������bbf�ṹ�еľ���ֵ

    //df>=dn��˵��Ҫ����������������ĩβ
	if( df >= dn )
	{
        //�����������Ԫ�ظ����Ѵﵽ���ֵ���޷�����
		if( n == k )
		{
            feat->feature_data = fdata->old_data;//�����������Ǹ�ʲô��
			free( fdata );
            return 0;//����ʧ�ܣ�����0
		}
        nbrs[n] = feat;//���뵽ĩβ
        return 1;//����ɹ�������1
	}

    //���е��˴�˵������λ�ò�������ĩβ
	/* find the right place in the array */
    if( n < k )//�����������Ԫ�ظ���С�����ֵ���ɲ���
	{
        nbrs[n] = nbrs[n-1];//ԭ�������һ�������
        ret = 1;//��������Ϊ1
	}
    else//�����������Ԫ�ظ������ڻ�������ֵ���޷����룬������ret����0
    {//��ʵҲ�����޷����룬���������������Ԫ�ظ������䣬��ֵ�����
		nbrs[n-1]->feature_data = ndata->old_data;
		free( ndata );
	}
	i = n-2;
    //������������в���Ҫ�����λ��
	while( i >= 0 )
	{
		ndata = (struct bbf_data*)nbrs[i]->feature_data;
		dn = ndata->d;
        if( dn <= df )//�ҵ������
			break;
        nbrs[i+1] = nbrs[i];//һ�κ���
		i--;
	}
	i++;
    nbrs[i] = feat;//����

    return ret;//���ؽ��
}


/*�жϸ������Ƿ���ĳ������
*/
/*
Determines whether a given point lies within a specified rectangular region
@param pt point
@param rect rectangular region
@return Returns 1 if pt is inside rect or 0 otherwise
*/
static int within_rect( CvPoint2D64f pt, CvRect rect )
{
	if( pt.x < rect.x  ||  pt.y < rect.y )
		return 0;
	if( pt.x > rect.x + rect.width  ||  pt.y > rect.y + rect.height )
		return 0;
	return 1;
}
