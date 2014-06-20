/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 *
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 *
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 *
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.] */

#include <openssl/asn1.h>

#include <openssl/err.h>
#include <openssl/mem.h>


#if 0
#ifndef NO_ASN1_OLD

/* ASN1 packing and unpacking functions */

/* Turn an ASN1 encoded SEQUENCE OF into a STACK of structures */

STACK_OF(OPENSSL_BLOCK) *ASN1_seq_unpack(const unsigned char *buf, int len,
			 d2i_of_void *d2i, void (*free_func)(OPENSSL_BLOCK))
{
    STACK_OF(OPENSSL_BLOCK) *sk;
    const unsigned char *pbuf;
    pbuf =  buf;
    if (!(sk = d2i_ASN1_SET(NULL, &pbuf, len, d2i, free_func,
					V_ASN1_SEQUENCE, V_ASN1_UNIVERSAL)))
		 OPENSSL_PUT_ERROR(ASN1, XXX, ASN1_R_DECODE_ERROR);
    return sk;
}

/* Turn a STACK structures into an ASN1 encoded SEQUENCE OF structure in a
 * OPENSSL_malloc'ed buffer
 */

unsigned char *ASN1_seq_pack(STACK_OF(OPENSSL_BLOCK) *safes, i2d_of_void *i2d,
			     unsigned char **buf, int *len)
{
	int safelen;
	unsigned char *safe, *p;
	if (!(safelen = i2d_ASN1_SET(safes, NULL, i2d, V_ASN1_SEQUENCE,
					      V_ASN1_UNIVERSAL, IS_SEQUENCE))) {
		OPENSSL_PUT_ERROR(ASN1, XXX, ASN1_R_ENCODE_ERROR);
		return NULL;
	}
	if (!(safe = OPENSSL_malloc (safelen))) {
		OPENSSL_PUT_ERROR(ASN1, XXX, ERR_R_MALLOC_FAILURE);
		return NULL;
	}
	p = safe;
	i2d_ASN1_SET(safes, &p, i2d, V_ASN1_SEQUENCE, V_ASN1_UNIVERSAL,
								 IS_SEQUENCE);
	if (len) *len = safelen;
	if (buf) *buf = safe;
	return safe;
}

/* Extract an ASN1 object from an ASN1_STRING */

void *ASN1_unpack_string(ASN1_STRING *oct, d2i_of_void *d2i)
{
	const unsigned char *p;
	char *ret;

	p = oct->data;
	if(!(ret = d2i(NULL, &p, oct->length)))
		OPENSSL_PUT_ERROR(ASN1, XXX, ASN1_R_DECODE_ERROR);
	return ret;
}

/* Pack an ASN1 object into an ASN1_STRING */

ASN1_STRING *ASN1_pack_string(void *obj, i2d_of_void *i2d, ASN1_STRING **oct)
{
	unsigned char *p;
	ASN1_STRING *octmp;

	if (!oct || !*oct) {
		if (!(octmp = ASN1_STRING_new ())) {
			OPENSSL_PUT_ERROR(ASN1, XXX, ERR_R_MALLOC_FAILURE);
			return NULL;
		}
		if (oct) *oct = octmp;
	} else octmp = *oct;
		
	if (!(octmp->length = i2d(obj, NULL))) {
		OPENSSL_PUT_ERROR(ASN1, XXX, ASN1_R_ENCODE_ERROR);
		return NULL;
	}
	if (!(p = OPENSSL_malloc (octmp->length))) {
		OPENSSL_PUT_ERROR(ASN1, XXX, ERR_R_MALLOC_FAILURE);
		return NULL;
	}
	octmp->data = p;
	i2d (obj, &p);
	return octmp;
}

#endif
#endif

/* ASN1_ITEM versions of the above */

ASN1_STRING *ASN1_item_pack(void *obj, const ASN1_ITEM *it, ASN1_STRING **oct)
{
	ASN1_STRING *octmp;

	if (!oct || !*oct) {
		if (!(octmp = ASN1_STRING_new ())) {
			OPENSSL_PUT_ERROR(ASN1, ASN1_item_pack, ERR_R_MALLOC_FAILURE);
			return NULL;
		}
		if (oct) *oct = octmp;
	} else octmp = *oct;

	if(octmp->data) {
		OPENSSL_free(octmp->data);
		octmp->data = NULL;
	}
		
	if (!(octmp->length = ASN1_item_i2d(obj, &octmp->data, it))) {
		OPENSSL_PUT_ERROR(ASN1, ASN1_item_pack, ASN1_R_ENCODE_ERROR);
		return NULL;
	}
	if (!octmp->data) {
		OPENSSL_PUT_ERROR(ASN1, ASN1_item_pack, ERR_R_MALLOC_FAILURE);
		return NULL;
	}
	return octmp;
}

/* Extract an ASN1 object from an ASN1_STRING */

void *ASN1_item_unpack(ASN1_STRING *oct, const ASN1_ITEM *it)
{
	const unsigned char *p;
	void *ret;

	p = oct->data;
	if(!(ret = ASN1_item_d2i(NULL, &p, oct->length, it)))
		OPENSSL_PUT_ERROR(ASN1, ASN1_item_unpack, ASN1_R_DECODE_ERROR);
	return ret;
}