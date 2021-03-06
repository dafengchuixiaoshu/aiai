

/// ????????????????????????
/// ??????????????????????


/*-------------------------------------------------------------------------------------
wchar_t (UNICODE 2bit)->char (UTF-8)(multi bit )
???????????????????????.

??????????????.
dest_str:
????????????UTF-8?????????????.
src_wchar:???????????????.
?????:
??????????????????????. ???????????????????????, ???-1.

???! ??????????????????????????UTF-8????????.
--------------------------------------------------------------------------------------
*/

size_t g_f_wctou8(char * dest_str, const wchar_t src_wchar)
{
	int count_bytes = 0;
	wchar_t byte_one = 0, byte_other = 0x3f; // ?????????????????? 0x3f--->00111111
	unsigned char utf_one = 0, utf_other = 0x80; // ????"???"?�?UTF-8???? 0x80--->1000000
	wchar_t tmp_wchar = L'0'; // ???????????????????(????6?)
	unsigned char tmp_char = '0';

	if (!src_wchar)//
		return (size_t)-1;

	for (;;) // ?????????????
	{
		if (src_wchar <= 0x7f){ // <=01111111
			count_bytes = 1; // ASCII???: 0xxxxxxx( ~ 01111111)
			byte_one = 0x7f; // ???????????, ????????, ???
			utf_one = 0x0;
			break;
		}
		if ( (src_wchar > 0x7f) && (src_wchar <= 0x7ff) ){ // <=0111,11111111
			count_bytes = 2; // 110xxxxx 10xxxxxx[1](???11??1?, ????11*1)
			byte_one = 0x1f; // 00011111, ??????(1??????????)
			utf_one = 0xc0; // 11000000
			break;
		}
		if ( (src_wchar > 0x7ff) && (src_wchar <= 0xffff) ){ //0111,11111111<=11111111,11111111
			count_bytes = 3; // 1110xxxx 10xxxxxx[2](MaxBits: 16*1)
			byte_one = 0xf; // 00001111
			utf_one = 0xe0; // 11100000
			break;
		}
		if ( (src_wchar > 0xffff) && (src_wchar <= 0x1fffff) ){ //??UCS-4?????..
			count_bytes = 4; // 11110xxx 10xxxxxx[3](MaxBits: 21*1)
			byte_one = 0x7; // 00000111
			utf_one = 0xf0; // 11110000
			break;
		}
		if ( (src_wchar > 0x1fffff) && (src_wchar <= 0x3ffffff) ){
			count_bytes = 5; // 111110xx 10xxxxxx[4](MaxBits: 26*1)
			byte_one = 0x3; // 00000011
			utf_one = 0xf8; // 11111000
			break;
		}
		if ( (src_wchar > 0x3ffffff) && (src_wchar <= 0x7fffffff) ){
			count_bytes = 6; // 1111110x 10xxxxxx[5](MaxBits: 31*1)
			byte_one = 0x1; // 00000001
			utf_one = 0xfc; // 11111100
			break;
		}
		return (size_t)-1; // ????????????????????
	}
	// ???�??????????????????, ???????UTF-8???????????
	tmp_wchar = src_wchar;
	for (int i = count_bytes; i > 1; i--)
	{ // ?????????????????
		tmp_char = (unsigned char)(tmp_wchar & byte_other);///??6???byte_other 00111111
		dest_str[i - 1] = (tmp_char | utf_other);/// ??????10----??10000000??
		tmp_wchar >>= 6;//????6?
	}
	//??????i=1
	//??UTF-8?????????????
	//???????????"1"???????????????????????
	tmp_char = (unsigned char)(tmp_wchar & byte_one);//??????????????????????
	dest_str[0] = (tmp_char | utf_one);//???????????? 1?????
	// ?????????__End!
	return count_bytes;
}

/*-----------------------------------------------------------------------------
char *-->wchar_t
???????????????????????.
??????????????.
dest_wchar:
??????????????????????.
src_str:
???????UTF-8?????????????????.
?????:
???????????????????. ???????????????????????, ???-1.

???! ??????????????????????????UTF-8????????.
------------------------------------------------------------------------------*/
size_t g_f_u8towc(wchar_t &dest_wchar, const unsigned char * src_str)
{
	int count_bytes = 0;
	unsigned char byte_one = 0, byte_other = 0x3f; // ?????????????????? 0x3f-->00111111
	wchar_t tmp_wchar = L'0';

	if (!src_str)
		return (size_t)-1;

	for (;;) // ?????????????,?????????????1??????
	{
		if (src_str[0] <= 0x7f){
			count_bytes = 1; // ASCII???: 0xxxxxxx( ~ 01111111)
			byte_one = 0x7f; // ???????????, ????????, ??? 01111111
			break;
		}
		if ( (src_str[0] >= 0xc0) && (src_str[0] <= 0xdf) ){
			count_bytes = 2; // 110xxxxx(110 00000 ~ 110 111111)
			byte_one = 0x1f; //00011111 ???????????????
			break;
		}
		if ( (src_str[0] >= 0xe0) && (src_str[0] <= 0xef) ){
			count_bytes = 3; // 1110xxxx(1110 0000 ~ 1110 1111)
			byte_one = 0xf; //00001111
			break;
		}
		if ( (src_str[0] >= 0xf0) && (src_str[0] <= 0xf7) ){
			count_bytes = 4; // 11110xxx(11110 000 ~ 11110 111)
			byte_one = 0x7;
			break;
		}
		if ( (src_str[0] >= 0xf8) && (src_str[0] <= 0xfb) ){
			count_bytes = 5; // 111110xx(111110 00 ~ 111110 11)
			byte_one = 0x3;
			break;
		}
		if ( (src_str[0] >= 0xfc) && (src_str[0] <= 0xfd) ){
			count_bytes = 6; // 1111110x(1111110 0 ~ 1111110 1)
			byte_one = 0x1;
			break;
		}
		return (size_t)-1; // ????????????????????
	}
	// ???�??????UTF-8???????????????????
	//??�?????????????????
	tmp_wchar = src_str[0] & byte_one;
	for (int i=1; i<count_bytes; i++)
	{
		tmp_wchar <<= 6; // ????6?????????????????"???"???
		tmp_wchar = tmp_wchar | (src_str[i] & byte_other);//??????
	}
	// ?????__End!
	dest_wchar = tmp_wchar;
	return count_bytes;
}
