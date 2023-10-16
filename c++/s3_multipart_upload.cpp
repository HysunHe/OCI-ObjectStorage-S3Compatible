#include <aws/core/Aws.h>
#include <aws/core/utils/UUID.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/client/ClientConfiguration.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/ListObjectsV2Request.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/HeadObjectRequest.h>
#include <aws/s3/model/DeleteObjectRequest.h>
#include <aws/s3/model/CreateMultipartUploadRequest.h>
#include <aws/s3/model/UploadPartRequest.h>
#include <aws/s3/model/CompleteMultipartUploadRequest.h>
#include <aws/s3/model/AbortMultipartUploadRequest.h>
#include <aws/s3/model/ListMultipartUploadsRequest.h>
#include <aws/s3/model/ListPartsRequest.h>
#include <aws/core/utils/memory/stl/AWSString.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <curl/curl.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <fcntl.h>
#include <unistd.h>

#define AWS_UPLOAD_PART_SIZE                (5 * 1024 * 1024) // 5MB

#define AWS_BUCKET_NAME                     ""
#define AWS_ACCESS_KEY                      ""
#define AWS_ACCESS_SECRET                   ""

const Aws::String ORACLE_REGION = "ap-chuncheon-1";
const Aws::String ORACLE_NAMESPACE = "sehubjapacprod";
const Aws::String ORACLE_BUCKET = "Wilbur-Bucket";

int aws_s3_multipart_upload(const char *local_file, const char *desc_file) {
    RM_BOOL new_req_upload = RM_TRUE;
    int partNumber = 0;
    Aws::String uploadId;

    /* 创建 S3 客户端 */
    Aws::Client::ClientConfiguration clientConfig;
    clientConfig.verifySSL = false;
    clientConfig.region = ORACLE_REGION;
    clientConfig.endpointOverride = "https://" + ORACLE_NAMESPACE + ".compat.objectstorage." + ORACLE_REGION + ".oraclecloud.com/" + ORACLE_BUCKET + "/";
    Aws::S3::S3Client s3_client(clientConfig, Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy::Never, false);

    /* 打开文件流 */
    std::shared_ptr<Aws::IOStream> inputData = 
        Aws::MakeShared<Aws::FStream>("SampleAllocationTag",
                                      local_file,
                                      std::ios_base::in | std::ios_base::binary);
    if (!*inputData) {
        printf("Read uploaded file failed!\n");
        return -1;
    }

    /* 获取文件大小 */
    inputData->seekg(0, std::ios::end);
    auto fileSize = inputData->tellg();
    inputData->seekg(0, std::ios::beg);
    // 计算文件需要分成多少块上传
    int totalParts = static_cast<int>((static_cast<double>(fileSize) + 
        AWS_UPLOAD_PART_SIZE - 1) / AWS_UPLOAD_PART_SIZE);

    // 初始化分块列表
    std::vector<Aws::S3::Model::CompletedPart> completedParts(totalParts);

    /* 获取该文件已上传的块序号 */
    // 构造 ListMultipartUploads 请求
    Aws::S3::Model::ListMultipartUploadsRequest listMultiPartUploadsReq;
    listMultiPartUploadsReq.SetBucket(AWS_BUCKET_NAME);
    // 发送 ListMultipartUploads 请求
    auto listMultipartUploadsOutcome = s3_client.ListMultipartUploads(listMultiPartUploadsReq);
    if (listMultipartUploadsOutcome.IsSuccess()) {
        // 获取未完成的分块上传任务列表
        auto list_multipart_uploads_result = listMultipartUploadsOutcome.GetResult();
        for (const auto& upload : list_multipart_uploads_result.GetUploads()) {
            // 检查上传任务是否匹配
            if (upload.GetKey() == desc_file) {
                new_req_upload = RM_FALSE;
                // 获取上传 ID
                uploadId = upload.GetUploadId();
                // 使用 uploadId 执行后续操作，如获取已上传的块、继续上传等
                Aws::S3::Model::ListPartsRequest listPartsReq;
                listPartsReq.SetBucket(AWS_BUCKET_NAME);
                listPartsReq.SetKey(desc_file);
                listPartsReq.SetUploadId(uploadId);
                auto listPartsOutcome = s3_client.ListParts(listPartsReq);
                if (listPartsOutcome.IsSuccess()) {
                    auto listPartsRet = listPartsOutcome.GetResult();
                    for (const auto& part : listPartsRet.GetParts()) {
                        // 处理已上传的块信息
                        partNumber = part.GetPartNumber() - 1;
                        inputData->seekg(AWS_UPLOAD_PART_SIZE, std::ios::cur);
                        // 将已完成的分块添加到分块列表
                        Aws::S3::Model::CompletedPart completedPart;
                        completedPart.SetETag(part.GetETag());
                        completedPart.SetPartNumber(partNumber + 1);
                        completedParts[partNumber] = completedPart;
                    }
                    if(listPartsRet.GetParts().size() == 0) {
                        new_req_upload = RM_TRUE;
                        continue;
                    } else {
                        partNumber++;
                    }
                } else {
                    printf("s3_client.ListParts() failed: %s\n", 
                        listPartsOutcome.GetError().GetMessage().c_str());
                    return -1;
                }
                break;
            }
        }
    } else {
        printf("ListMultipartUploads failed: %s\n", 
            listMultipartUploadsOutcome.GetError().GetMessage().c_str());
        return -1;
    }

    if(new_req_upload) {
        /* 创建分块上传请求 */
        Aws::Map<Aws::String, Aws::String> metadata;
        metadata.emplace("s3-test", "test");
        Aws::S3::Model::CreateMultipartUploadRequest createMultipartUploadReq;
        createMultipartUploadReq.SetBucket(AWS_BUCKET_NAME);
        createMultipartUploadReq.SetKey(desc_file);
        createMultipartUploadReq.SetMetadata(metadata);
        auto createMultipartUploadOutcome = 
            s3_client.CreateMultipartUpload(createMultipartUploadReq);
        if (!createMultipartUploadOutcome.IsSuccess()) {
            printf("Create block to upload failed: %s", 
                createMultipartUploadOutcome.GetError().GetMessage().c_str());
            return -1;
        }
        // 获取上传ID和响应的ETag值
        uploadId = createMultipartUploadOutcome.GetResult().GetUploadId();
        printf("Start to be uploaded file.\n");
    } else if(!new_req_upload) {
        printf("Continue uploading file.\n");
    }

    /* 开始分块上传 */
    printf("Upload id = %s\n", uploadId.c_str());
    Aws::S3::Model::UploadPartOutcome uploadPartOutcome;
    for(int p_n = partNumber; p_n < totalParts; ++p_n) {
        // 读取分块数据
        std::vector<char> buffer(AWS_UPLOAD_PART_SIZE);
        inputData->read(buffer.data(), AWS_UPLOAD_PART_SIZE);
        std::streamsize bytesRead = inputData->gcount();
        printf("正在上传大小为 %.2f MB 的分块 %d, 共 %d 块\n", (float)bytesRead / 1024 / 1024, p_n + 1, totalParts);
        // 上传分块
        Aws::S3::Model::UploadPartRequest uploadPartRequest;
        uploadPartRequest.SetBucket(AWS_BUCKET_NAME);
        uploadPartRequest.SetKey(desc_file);
        uploadPartRequest.SetPartNumber(p_n + 1);
        uploadPartRequest.SetUploadId(uploadId);
        // 设置分块数据
        std::shared_ptr<Aws::IOStream> partStream = 
            Aws::MakeShared<Aws::StringStream>("SampleApp");
        partStream->write(buffer.data(), bytesRead);
        uploadPartRequest.SetBody(partStream);
        uploadPartOutcome = s3_client.UploadPart(uploadPartRequest);
        if (!uploadPartOutcome.IsSuccess()) {
            printf("Upload block %d failed: %s\n", p_n + 1, 
                uploadPartOutcome.GetError().GetMessage().c_str());
            return -1;
        }
        // 将已完成的分块添加到分块列表
        Aws::S3::Model::CompletedPart completedPart;
        completedPart.SetETag(uploadPartOutcome.GetResult().GetETag());
        completedPart.SetPartNumber(p_n + 1);
        completedParts[p_n] = completedPart;
    }
    /* 完成分块上传，进行收尾 */
    Aws::S3::Model::CompleteMultipartUploadRequest compMultiPartUploadReq;
    compMultiPartUploadReq.SetBucket(AWS_BUCKET_NAME);
    compMultiPartUploadReq.SetKey(desc_file);
    compMultiPartUploadReq.SetUploadId(uploadId);
    Aws::S3::Model::CompletedMultipartUpload completedUpload;
    completedUpload.SetParts(completedParts);
    compMultiPartUploadReq.WithMultipartUpload(completedUpload);
    
    auto completeMultipartUploadOutcome = s3_client.CompleteMultipartUpload(compMultiPartUploadReq);
    if (completeMultipartUploadOutcome.IsSuccess()) {
        printf("uploaded successfully!\n");
        for(int i = 0; i < completedParts.size(); i++) {
            Aws::S3::Model::CompletedPart part = completedParts[i];
            printf("Block %d ETag: %s\n", part.GetPartNumber(), part.GetETag().c_str());
        }
    } else {
        printf("Complete block upload failed: %s\n", 
            completeMultipartUploadOutcome.GetError().GetMessage().c_str());
        return -1;
    }
    // 关闭文件流
    inputData.reset();

	return 0;
}
