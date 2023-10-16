//
//  oci_common.h
//  ObjectStorageDemo
//
//  Created by Bob Tang on 2023/7/17.
//

#ifndef oci_common_h
#define oci_common_h

#include "mbedtls/md.h"
#include "mbedtls/hmac_drbg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mbedtls/md5.h"
#include "mbedtls/base64.h"
#define HOST "axj8tfp9f3vy.compat.objectstorage.ap-singapore-1.oraclecloud.com"
#define BUF_SIZE 1024

typedef struct {
    const  char *access_key;
    const  char *secret_key;
    const  char *region_name;
} S3Credentials;

int get_date(const *amz_date, const *date);

char * create_common_http_heaer(const S3Credentials *credentials, const char *http_verb,const char *bucket_name, const char *object_key, int *is_bulk_delete, char *amz_date, char *date);


int md5_base64(const char *input, size_t input_len, char *output, size_t output_size);


#endif /* oci_common_h */
