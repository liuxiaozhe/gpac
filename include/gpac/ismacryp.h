/*
 *			GPAC - Multimedia Framework C SDK
 *
 *			Authors: Jean Le Feuvre
 *			Copyright (c) Telecom ParisTech 2000-2012
 *					All rights reserved
 *
 *  This file is part of GPAC / Authoring Tools sub-project
 *
 *  GPAC is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  GPAC is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef _GF_ISMACRYP_H_
#define _GF_ISMACRYP_H_


#ifdef __cplusplus
extern "C" {
#endif

/*!
 *	\file <gpac/ismacryp.h>
 *	\brief Utility tools for ISMA and Common Encryption.
 */
	
/*!
 *	\addtogroup crypt_grp
 *	\ingroup media_grp
 *	\brief Utility tools for ISMA and Common Encryption.
 *
 *This section documents the encryption and decryption of ISOBMF files according to ISMA and CENC specifications.
 *	@{
 */

#include <gpac/isomedia.h>

#ifndef GPAC_DISABLE_AV_PARSERS
//FIXME - we should move AVC and HEVC parsing to gpac/avparse.h
#include <gpac/internal/media_dev.h>
#endif

/*loads key and salt from a LOCAL gpac-DRM file (cf MP4Box doc)*/
GF_Err gf_ismacryp_gpac_get_info(u32 stream_id, char *drm_file, char *key, char *salt);

/*loads key and salt for MPEG4IP protected files*/
Bool gf_ismacryp_mpeg4ip_get_info(char *kms_uri, char *key, char *salt);


enum
{
	/*ISMA E&A encryption*/
	GF_CRYPT_ISMA_CRYPT_TYPE	= GF_4CC( 'i', 'A', 'E', 'C' ),
	/*CENC CTR-128 encryption*/
	GF_CRYPT_CENC_CRYPT_TYPE	= GF_4CC('c','e','n','c'),
	/*CENC CBC-128 encryption*/
	GF_CRYPT_CBC1_CRYPT_TYPE	= GF_4CC('c','b','c','1'),
	/*Adobe CBC-128 encryption*/
	GF_CRYPT_ADOBE_CRYPT_TYPE	= GF_4CC('a','d','k','m'),
	/*CENC CTR-128 pattern encryption*/
	GF_CRYPT_CENS_CRYPT_TYPE	= GF_4CC('c','e','n','s'),
	/*CENC CBC-128 pattern encryption*/
	GF_CRYPT_CBCS_CRYPT_TYPE	= GF_4CC('c','b','c','s'),
};

enum
{
	/*no selective encryption*/
	GF_CRYPT_SELENC_NONE = 0,
	/*only encrypts RAP samples*/
	GF_CRYPT_SELENC_RAP = 1,
	/*only encrypts non-RAP samples*/
	GF_CRYPT_SELENC_NON_RAP = 2,
	/*selective encryption of random samples*/
	GF_CRYPT_SELENC_RAND = 3,
	/*selective encryption of a random sample in given range*/
	GF_CRYPT_SELENC_RAND_RANGE = 4,
	/*selective encryption of first sample in given range*/
	GF_CRYPT_SELENC_RANGE = 5,
	/*encryption of all samples but the preview range*/
	GF_CRYPT_SELENC_PREVIEW = 6,
	/*encryption of no samples*/
	GF_CRYPT_SELENC_CLEAR = 7,
};

typedef struct
{
	/*0: ISMACryp - 1: OMA DRM*/
	u32 enc_type;//for now only used for ISMACrypt
	u32 trackID;
	unsigned char key[16];
	unsigned char salt[16];

	/*the rest is only used for encryption*/
	char KMS_URI[5000];
	char Scheme_URI[5000];
	/*selecive encryption type*/
	u32 sel_enc_type;
	u32 sel_enc_range;
	/*IPMP signaling: 0: none, 1: IPMP, 2: IPMPX
	when IPMP signaling is enabled, the OD stream will be updated with
	IPMP Update commands*/
	u32 ipmp_type;
	/*if not set and IPMP enabled, defaults to TrackID*/
	u32 ipmp_desc_id;
	/*type of box where sample auxiliary informations is saved, or 0 in case of ISMACrypt (it will be written in samples)*/
	u32 sai_saved_box_type;

	/*OMA extensions*/
	/*0: none, 1: AES CBC, 2: AES CTR*/
	u8 encryption;
	char TextualHeaders[5000];
	u32 TextualHeadersLen;
	char TransactionID[17];

	/*CENC extensions*/
	u32 IsEncrypted;
	u8 IV_size;
	bin128 default_KID;
	u32 KID_count;
	bin128 *KIDs;
	bin128 *keys;
	/*IV of first sample in track*/
	unsigned char first_IV[16];
	u32 defaultKeyIdx;
	u32 keyRoll;
	u8 crypt_byte_block, skip_byte_block;
	u8 constant_IV_size;
	unsigned char constant_IV[16];
	//true if using AES-CTR mode, false if using AES-CBC mode
	Bool ctr_mode;
	u32 cenc_scheme_type;
	//for avc1 ctr CENC edition 1
	Bool allow_encrypted_slice_header;


	char metadata[5000];
	u32 metadata_len;

#ifndef GPAC_DISABLE_AV_PARSERS
	AVCState avc;
#ifndef GPAC_DISABLE_HEVC
	HEVCState hevc;
#endif
	Bool slice_header_clear;
	Bool is_avc;
#endif

} GF_TrackCryptInfo;

#if !defined(GPAC_DISABLE_MCRYPT) && !defined(GPAC_DISABLE_ISOM_WRITE)

/*encrypts track - logs, progress: info callbacks, NULL for default*/
GF_Err gf_ismacryp_encrypt_track(GF_ISOFile *mp4, GF_TrackCryptInfo *tci, void (*progress)(void *cbk, u64 done, u64 total), void *cbk);

/*decrypts track - logs, progress: info callbacks, NULL for default*/
GF_Err gf_ismacryp_decrypt_track(GF_ISOFile *mp4, GF_TrackCryptInfo *tci, void (*progress)(void *cbk, u64 done, u64 total), void *cbk);

/*Common Encryption*/
/*AES-CTR*/
GF_Err gf_cenc_encrypt_track(GF_ISOFile *mp4, GF_TrackCryptInfo *tci, void (*progress)(void *cbk, u64 done, u64 total), void *cbk);
GF_Err gf_cenc_decrypt_track(GF_ISOFile *mp4, GF_TrackCryptInfo *tci, void (*progress)(void *cbk, u64 done, u64 total), void *cbk);
/*AES-CBC*/
GF_Err gf_cbc_encrypt_track(GF_ISOFile *mp4, GF_TrackCryptInfo *tci, void (*progress)(void *cbk, u64 done, u64 total), void *cbk);
GF_Err gf_cbc_decrypt_track(GF_ISOFile *mp4, GF_TrackCryptInfo *tci, void (*progress)(void *cbk, u64 done, u64 total), void *cbk);

/*ADOBE*/
GF_Err gf_adobe_encrypt_track(GF_ISOFile *mp4, GF_TrackCryptInfo *tci, void (*progress)(void *cbk, u64 done, u64 total), void *cbk);
GF_Err gf_adobe_decrypt_track(GF_ISOFile *mp4, GF_TrackCryptInfo *tci, void (*progress)(void *cbk, u64 done, u64 total), void *cbk);


/*decrypt a file
@drm_file: location of DRM data (cf MP4Box doc).
@LogMsg: redirection for message or NULL for default
*/
GF_Err gf_decrypt_file(GF_ISOFile *mp4file, const char *drm_file);

/*Crypt a the file
@drm_file: location of DRM data.
@LogMsg: redirection for message or NULL for default
*/
GF_Err gf_crypt_file(GF_ISOFile *mp4file, const char *drm_file);

#endif /*!defined(GPAC_DISABLE_MCRYPT) && !defined(GPAC_DISABLE_ISOM_WRITE)*/

/*! @} */

#ifdef __cplusplus
}
#endif


#endif	/*_GF_ISMACRYP_H_*/

