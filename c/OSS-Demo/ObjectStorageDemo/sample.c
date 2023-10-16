#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "oci_common.h"
#include "curl/curl.h"

#include <unistd.h>
#define BUF_SIZE 1024
#define APPEND_OBJECT_URL "http://158.178.240.219:5000/write-bytes?bucket=%s&name=%s&position=%lld&append=%d"


size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}

// 定义回调函数，用于处理响应数据
size_t write_callback(void *buffer, size_t size, size_t nmemb, void *userdata) {
    // 将响应数据写入到用户自定义的缓冲区
    size_t total_size = size * nmemb;
    char *response_buffer = (char *)userdata;
    strncat(response_buffer, (char *)buffer, total_size);
    return total_size;
}

int debug_callback(CURL *handle, curl_infotype type, char *data, size_t size, void *userptr) {
    (void)handle;
    (void)userptr;
    // 打印日志消息
    printf("[CURL DEBUG] %.*s", (int)size, data);
    return 0;
}

void upload_file(const S3Credentials *credentials, const char *file_path, const char *bucket_name, const char *object_key) {
    char amz_date[17];
    char date[9];
    get_date(amz_date,date);
    
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize libcurl\n");
        return;
    }

    FILE *file = fopen(file_path, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", file_path);
        curl_easy_cleanup(curl);
        return;
    }
    
    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // 启用详细日志
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            
    //设置日志回调函数
    curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, debug_callback);

    char s3_url[512];
    snprintf(s3_url, sizeof(s3_url), "https://%s/%s/%s", HOST, bucket_name, object_key);
    
    

    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, s3_url);
    curl_easy_setopt(curl, CURLOPT_READDATA, file);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, create_common_http_heaer(credentials, "PUT", bucket_name, object_key,
                                                                  0,amz_date,date));
    

    char content_length_header[100];
    snprintf(content_length_header, sizeof(content_length_header), "Content-Length:%lld", file_size);
    
    headers = curl_slist_append(headers, content_length_header);
 
    
    headers = curl_slist_append(headers, "Content-Type:application/octet-stream");
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_size);
    
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "Failed to upload file: %s\n", curl_easy_strerror(res));
    }
    
    long response_code;
    // 获取返回码
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    if (response_code == 200){
        printf("Upload successful. Response code: %ld\n", response_code);
    }
    

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    fclose(file);
}

int append_object(const char *data, const char *bucket_name, const char *object_key, long long *position){
    char url[128];
    snprintf(url, sizeof(url), APPEND_OBJECT_URL, bucket_name, object_key, position, 1);
    
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        
        // 启用详细日志
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
                
        //设置日志回调函数
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, debug_callback);
        
        //size_t data_size = sizeof(data);

        // 设置URL和其他选项
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        //curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long long)data_size);
        
        char response_buffer[4096];
        response_buffer[0] = '\0';  // 初始化为空字符串

        // 设置回调函数，用于处理响应数据
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_buffer);

        // 执行上传
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }


        long response_code;

        // 获取返回码
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        if (response_code == 200){
            printf("append upload successful. Response code: %ld\n", response_code);
            printf("响应数据: %s\n", response_buffer);
        }


        
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return 0;
}

int append_object_end(const char *bucket_name, const char *object_key){
    char url[128];
    snprintf(url, sizeof(url), APPEND_OBJECT_URL, bucket_name, object_key, 0ll, 0);
    
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        
        // 启用详细日志
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
                
        //设置日志回调函数
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, debug_callback);
        
        //size_t data_size = sizeof(data);

        // 设置URL和其他选项
        curl_easy_setopt(curl, CURLOPT_URL, url);
        
        char response_buffer[4096];
        response_buffer[0] = '\0';  // 初始化为空字符串

        // 设置回调函数，用于处理响应数据
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS,"");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_buffer);

        // 执行上传
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }


        long response_code;

        // 获取返回码
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        printf("code: %ld", response_code);
        if (response_code == 200){
            printf("append upload end successful. Response code: %ld\n", response_code);
            printf("响应数据: %s\n", response_buffer);
        }


        
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return 0;
}


//普通下载
int download_file(const S3Credentials *credentials, const char *file_path, const char *bucket_name, const char *object_key){
    char amz_date[17];
    char date[9];
    get_date(amz_date,date);

    char url[128];
    snprintf(url, sizeof(url), "https://%s/%s/%s", HOST, bucket_name, object_key);
    
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        FILE *fp = fopen(file_path, "wb");
        if (fp == NULL) {
            printf("Failed to open file for writing.\n");
            return 1;
        }
        
        // 启用详细日志
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
                
        //设置日志回调函数
        //curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, debug_callback);



        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, create_common_http_heaer(credentials, "GET", bucket_name, object_key, 0, amz_date, date));
        headers = curl_slist_append(headers, "Content-Type: application/octet-stream");

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        long response_code;

        // 获取返回码
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        if (response_code == 200){
            printf("download successful. Response code: %ld\n", response_code);
        }
        

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        fclose(fp);
    }

    curl_global_cleanup();

    return 0;
}

//获取对象大小
size_t head_object(const S3Credentials *credentials,const char *bucket_name, const char *object_key){
    char amz_date[17];
    char date[9];
    get_date(amz_date,date);
    
    size_t object_size = 0;

    char url[128];
    snprintf(url, sizeof(url), "https://%s/%s/%s", HOST, bucket_name, object_key);
    
    //printf("url: %s\n", url);

    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        // 启用详细日志
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        //设置日志回调函数
        //curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, debug_callback);
        
        char *common_header = create_common_http_heaer(credentials, "HEAD", bucket_name, object_key, 0, amz_date, date);
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, common_header);
            

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "HEAD");
        // 设置头回调函数，用于处理响应头数据
        
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "head object failed: %s\n", curl_easy_strerror(res));
        }

        long response_code;

        //获取返回码
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        printf("Response code: %ld\n", response_code);
        if (response_code == 200){
            printf("head object successful. Response code: %ld\n", response_code);
            
            curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &object_size);
        }
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return object_size;
}



//流式下载
int download_stream(const S3Credentials *credentials, const char *file_path, const char *bucket_name, const char *object_key){
    char amz_date[17];
    char date[9];
    get_date(amz_date,date);

    char url[128];
    snprintf(url, sizeof(url), "https://%s/%s/%s", HOST, bucket_name, object_key);
    
    //printf("url: %s\n", url);

    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        FILE *fp = fopen(file_path, "wb");
        if (fp == NULL) {
            printf("Failed to open file for writing.\n");
            return 1;
        }
        
        // 启用详细日志
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        //设置日志回调函数
        //curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, debug_callback);
        
        // 设置范围步长和初始范围
        int step = 1000;
        long long offset = 0;
        
        char *common_header = create_common_http_heaer(credentials, "GET", bucket_name, object_key, 0, amz_date, date);
        
        long long object_size = head_object(credentials, bucket_name, object_key);
        
        while (1) {
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, common_header);
            headers = curl_slist_append(headers, "Content-Type: application/octet-stream");
            
            //构建范围头字符串
            char range[50];
            sprintf(range, "Range: bytes=%lld-%lld", offset, offset + step - 1);
            headers = curl_slist_append(headers, range);

            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

            res = curl_easy_perform(curl);

            if (res != CURLE_OK) {
                fprintf(stderr, "download failed: %s\n", curl_easy_strerror(res));
            }

            long response_code;

            //获取返回码
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            printf("Response code: %ld\n", response_code);
            if (response_code == 206){
                printf("download stream successful. Response code: %ld\n", response_code);
                
                break;
            }
            
            offset += step;
            // 检查是否到达文件末尾
            long long content_length;
            curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &content_length);
            if (content_length < 20000) {
                break;
            }

            curl_slist_free_all(headers);

        }
        curl_easy_cleanup(curl);
        fclose(fp);
    }

    curl_global_cleanup();

    return 0;
}


//单个对象删除
int delete_object(const S3Credentials *credentials,const char *bucket_name, const char *object_key){
    char amz_date[17];
    char date[9];
    get_date(amz_date,date);

    char url[128];
    snprintf(url, sizeof(url), "https://%s/%s/%s", HOST, bucket_name, object_key);
    
    //printf("url: %s\n", url);

    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        // 启用详细日志
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
                
        //设置日志回调函数
        //curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, debug_callback);



        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, create_common_http_heaer(credentials, "DELETE", bucket_name, object_key,
                                                                      0, amz_date, date));

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        long response_code;

        // 获取返回码
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        if (response_code == 204){
            printf("delete successful. Response code: %ld\n", response_code);
        }
        

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return 0;
}

char* data_xml(const char** strArray, int arrayLength) {
    char* result = (char*)malloc(2048);
    if (result == NULL) {
        printf("Failed to allocate memory.\n");
        return NULL;
    }
    strcpy(result, "");
    strcat(result, "<Delete>");
    for (int i = 0; i < arrayLength; i++) {
        char object[64];
        snprintf(object, sizeof(object), "<Object><Key>%s</Key></Object>", strArray[i]);
        strcat(result, object);
        
    }
    
    strcat(result, "</Delete>");
    
    return result;
}

int bulk_delete_object(const S3Credentials *credentials,const char *bucket_name, char **object, int object_count){
    char amz_date[17];
    char date[9];
    get_date(amz_date,date);

    char url[128];
    snprintf(url, sizeof(url), "https://%s/%s?delete", HOST, bucket_name);

    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        // 启用详细日志
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        //设置日志回调函数
        //curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, debug_callback);

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, create_common_http_heaer(credentials, "POST", bucket_name, "", 1, amz_date, date));
        
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        
        char *data = data_xml(object, object_count);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(data));
        
        char md5_base4_data[32];
        md5_base64(data, strlen(data), md5_base4_data, sizeof(md5_base4_data));
        
        char md5_header[64];
        snprintf(md5_header, sizeof(md5_header), "Content-MD5:%s", md5_base4_data);
        headers = curl_slist_append(headers, md5_header);
        
        char response_buffer[4096];
        response_buffer[0] = '\0';  // 初始化为空字符串

        // 设置回调函数，用于处理响应数据
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_buffer);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        long response_code;

        // 获取返回码
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        if (response_code == 200){
            printf("bulk delete successful. Response code: %ld\n", response_code);
            printf("响应数据: %s\n", response_buffer);
        }
        

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return 0;
}

int main(void) {
    S3Credentials credentials = {
        .access_key = "42649d6afc0e3f8dc33eaf72a89880f226e1b023",
        .secret_key = "opDnrBHyY+yJhw4qR4nFG4qmHoqfE6RliupITeyRR/Q=",
        .region_name = "ap-singapore-1"
    };

    // 文件上传示例
    printf("===================Object Upload=====================\n");
    upload_file(&credentials, "/Users/bob/Desktop/sample.c", "test", "bob-03");
    
    sleep(5);
    
    printf("===================Object Append Upload=====================\n");
    const char *data = "Object Append Upload test append-1";
    append_object(data, "test", "append_object-01", 0ll);
    
    printf("===================Object Append Upload=====================\n");
    data = "Object Append Upload test append-2";
    append_object(data, "test", "append_object-01", 34LL);
    
    sleep(5);
    
    printf("===================Object Append Upload End=====================\n");
    append_object_end("test", "append_object-01");
    
    sleep(5);
    
    printf("===================Object Download====================\n");
    download_file(&credentials, "/Users/bob/Desktop/bob-05",  "test", "bob-03");
    
    sleep(5);
    
    printf("===================Get Object Size====================\n");
    long long length = head_object(&credentials, "test", "bob-03");
    
    printf("===================Object Stream Download==============\n");
    download_stream(&credentials, "/Users/bob/Desktop/bob-06",  "test", "bob-03");
    
    sleep(5);
    
    printf("===================Object Delete Download===============\n");
    delete_object(&credentials, "test", "bob-s3");
    
    sleep(5);
    
    printf("===================Objects Bulk Delete  Download========\n");
    const char *object[] = {
        "bob-01",
        "bob-02",
        "bob-03"
    };
    
    bulk_delete_object(&credentials, "test", object, sizeof(object)/sizeof(object[0]));
    
    return 0;
}
