#ifndef __LPR_UTILS_H__
#define __LPR_UTILS_H__


#ifdef __cplusplus
extern "C" {
#endif

	// モデルデータを読み込みます
	int lpr_read_data_from_file(const char *path, char **out_data);

	/// tensor 情報を出力します
	void lpr_dump_tensor_attr(rknn_tensor_attr *attr);


#ifdef __cplusplus
}
#endif

#endif //__LPR_UTILS_H__