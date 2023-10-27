/*
 * tlv_kaldi_dec_interface.h
 *
 *  Created on: Jan 5, 2019
 *      Author: jfyuan
 */

#ifndef TLV_KALDI_DEC_INTERFACE_H_
#define TLV_KALDI_DEC_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*cfg*/
struct tlv_kaldi_dec_cfg;
typedef struct tlv_kaldi_dec_cfg tlv_kaldi_dec_cfg_t;
tlv_kaldi_dec_cfg_t* tlv_kaldi_dec_cfg_new(const char* fn);
void tlv_kaldi_dec_cfg_delete(tlv_kaldi_dec_cfg_t* cfg);

/*
 * \brief get model version
 * \param [in] cfg
 * \param [out] version
 * \return return code: 0 for success, -1 for failed
 */
int tlv_kaldi_dec_cfg_get_version(tlv_kaldi_dec_cfg_t* cfg, char** version);
int tlv_kaldi_dec_cfg_get_sdk_version(tlv_kaldi_dec_cfg_t* cfg, char** version);

int tlv_kaldi_dec_cfg_get_is_en(tlv_kaldi_dec_cfg_t* cfg);
int tlv_kaldi_dec_cfg_get_language(tlv_kaldi_dec_cfg_t* cfg);

struct tlv_kaldi_dec;
typedef struct tlv_kaldi_dec tlv_kaldi_dec_t;
tlv_kaldi_dec_t* tlv_kaldi_dec_new(const tlv_kaldi_dec_cfg_t* cfg);
void tlv_enable_module_dec(tlv_kaldi_dec_t* dec, int module, int enable);// module:1=kws / enable=1 means enable
int tlv_kaldi_query_cfg_is_en(tlv_kaldi_dec_t* dec);//en:return 1
int tlv_kaldi_dec_start(tlv_kaldi_dec_t* dec, const char* params, int len);
int tlv_kaldi_dec_feed(tlv_kaldi_dec_t* dec, char* data, int len, unsigned char is_end);
int tlv_kaldi_dec_reset(tlv_kaldi_dec_t* dec);
int tlv_kaldi_dec_delete(tlv_kaldi_dec_t* dec);
int tlv_kaldi_dec_get_rslt(tlv_kaldi_dec_t* dec, char** rslt, int* rslt_len);

#ifdef __cplusplus
};
#endif

#endif /* TLV_KALDI_DEC_INTERFACE_H_ */
