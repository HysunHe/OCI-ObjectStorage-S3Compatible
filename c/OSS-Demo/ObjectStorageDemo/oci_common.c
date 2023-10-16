//
//  oci_common.c
//  ObjectStorageDemo
//
//  Created by Bob Tang on 2023/7/17.
//

#include "oci_common.h"

void to_hex(const unsigned char *input, size_t input_len, char *output) {
    const char *hex = "0123456789abcdef";
    size_t i;
    for (i = 0; i < input_len; i++) {
        output[i * 2] = hex[input[i] >> 4];
        output[i * 2 + 1] = hex[input[i] & 0x0F];
    }
    
    output[input_len * 2] = '\0';
}

void get_signature(const char *secret_key, const char *date_stamp, const char *region_name, const char *service_name, const char *string_to_sign,unsigned char *signature) {
    unsigned char k_date[32];
    unsigned char k_region[32];
    unsigned char k_service[32];
    unsigned char k_signing[32];
    unsigned char k_signature[32];

    size_t secret_key_len = strlen(secret_key);
    size_t date_stamp_len = strlen(date_stamp);
    size_t region_name_len = strlen(region_name);
    size_t service_name_len = strlen(service_name);
    size_t string_to_sign_len = strlen(string_to_sign);

    mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),(const unsigned char *)secret_key, secret_key_len, (const unsigned char *) date_stamp, date_stamp_len, k_date);

    mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), k_date, 32, (const unsigned char *) region_name, region_name_len, k_region);

    mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), k_region, 32, (const unsigned char *) service_name, service_name_len, k_service);
    
    mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), k_service, 32, (const unsigned char *) "aws4_request", 12, k_signing);
    
    
    mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), k_signing, 32, (const unsigned char *) string_to_sign, string_to_sign_len, k_signature);
    
    to_hex(k_signature, 32, signature);
}


void sha256_hash(const unsigned char *input, size_t len, char *output) {
   //printf("data:\n%s\n", input);
   unsigned char digest[32];
   mbedtls_md_context_t md_ctx;
   mbedtls_md_init(&md_ctx);
   const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
   mbedtls_md_setup(&md_ctx, md_info, 0); // 0 表示禁用 HMAC
   mbedtls_md_starts(&md_ctx);
   mbedtls_md_update(&md_ctx, (const unsigned char *)input, len);
   mbedtls_md_finish(&md_ctx, digest);
   mbedtls_md_free(&md_ctx);
   
   for (int i = 0; i < mbedtls_md_get_size(md_info); i++) {
       sprintf(&output[i * 2], "%02x", digest[i]);
       
   }
   
   output[MBEDTLS_MD_MAX_SIZE * 2] = '\0';
   
}

void sign_request(const S3Credentials *credentials, const char *http_verb, const char *bucket_name, const char *object_key, char *authorization_header,size_t auth_len, char *amz_date, char *date, int *is_bulk_delete) {
   
   const char *service = "s3";
   
   char canonical_request[1024];
   
   if(is_bulk_delete == 0){
       snprintf(canonical_request, sizeof(canonical_request), "%s\n/%s/%s\n\nhost:%s\nx-amz-content-sha256:UNSIGNED-PAYLOAD\nx-amz-date:%s\n\nhost;x-amz-content-sha256;x-amz-date\nUNSIGNED-PAYLOAD", http_verb, bucket_name, object_key,HOST,amz_date);
   }else{
       snprintf(canonical_request, sizeof(canonical_request), "%s\n/%s\ndelete=\nhost:%s\nx-amz-content-sha256:UNSIGNED-PAYLOAD\nx-amz-date:%s\n\nhost;x-amz-content-sha256;x-amz-date\nUNSIGNED-PAYLOAD", http_verb, bucket_name,HOST,amz_date);
   }
   
   //printf("request:\n%s\n", canonical_request);
   
   //计算规范请求的SHA256哈希
   unsigned char digest[64];
   sha256_hash(canonical_request, strlen(canonical_request), digest);
   
   
   //printf("digest: %s\n", digest);
   
   char string_to_sign[1024];
   snprintf(string_to_sign, sizeof(string_to_sign), "AWS4-HMAC-SHA256\n%s\n%s/%s/%s/aws4_request\n%s", amz_date, date, credentials->region_name, service,digest);

   unsigned char signature[64];
   char key[64];
   snprintf(key, sizeof(key), "AWS4%s", credentials->secret_key);
   // 计算HMAC-SHA256签名
   get_signature(key, date, credentials->region_name, service,  string_to_sign, signature);

   snprintf(authorization_header, auth_len, "AWS4-HMAC-SHA256 Credential=%s/%s/%s/%s/aws4_request, SignedHeaders=host;x-amz-content-sha256;x-amz-date, Signature=%s", credentials->access_key, date, credentials->region_name, service, signature);
   
   //printf("%s\n", authorization_header);

}

int get_date(const *amz_date, const *date){
   time_t now = time(NULL);
   struct tm *tm_now = gmtime(&now);
   strftime(amz_date, 17, "%Y%m%dT%H%M%SZ", tm_now);
   strftime(date, 9, "%Y%m%d", tm_now);
   
   return 0;
}

int md5_base64(const char *input, size_t input_len, char *output, size_t output_size) {
    unsigned char md5_hash[16];

    mbedtls_md5((const unsigned char *)input, input_len, md5_hash);

    size_t base64_len;
    mbedtls_base64_encode((unsigned char *)output, output_size, &base64_len, md5_hash, sizeof(md5_hash));

    return base64_len;
}

char * create_common_http_heaer(const S3Credentials *credentials, const char *http_verb,const char *bucket_name, const char *object_key, int *is_bulk_delete, char *amz_date, char *date){
    // 设置请求头
    char authorization[1024];
    sign_request(credentials, http_verb, bucket_name, object_key, authorization,sizeof(authorization), amz_date, date, is_bulk_delete);
    char authorization_header[540];
    snprintf(authorization_header, sizeof(authorization_header), "Authorization:%s",authorization);
    
    char date_header[50];
    snprintf(date_header, sizeof(date_header), "X-Amz-Date:%s",amz_date);
    
    char host_header[100];
    snprintf(host_header, sizeof(host_header), "Host:%s",HOST);
    
    char http_header[1024];
    snprintf(http_header,sizeof(http_header),"%s\r\n%s\r\nx-amz-content-sha256:UNSIGNED-PAYLOAD\r\n%s\r",host_header,date_header,authorization_header);
    //printf("Common Header:\n%s\n", http_header);
    return http_header;
}
