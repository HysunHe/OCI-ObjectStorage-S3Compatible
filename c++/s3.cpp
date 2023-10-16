#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
using namespace std;

const Aws::String ORACLE_REGION = "ap-tokyo-1";
const Aws::String ORACLE_NAMESPACE = "sehubjapacprod";
const Aws::String ORACLE_BUCKET = "Wilbur-Bucket";

Aws::S3::S3Client getS3Client(){
    Aws::String endpoint = "https://" + ORACLE_NAMESPACE + ".compat.objectstorage." + ORACLE_REGION + ".oraclecloud.com/" + ORACLE_BUCKET + "/";
    
    Aws::Client::ClientConfiguration clientConfig;
    clientConfig.verifySSL = false;
    clientConfig.region = ORACLE_REGION;
    clientConfig.endpointOverride = endpoint;
    
    Aws::S3::S3Client s3_client(clientConfig, Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy::Never, false);
    return s3_client;
}

void uploadFile(Aws::S3::S3Client &s3, const Aws::String &keyName, const Aws::String &sourceFile){
    Aws::S3::Model::PutObjectRequest request;
    request.SetBucket(ORACLE_BUCKET);
    request.SetKey(keyName);

    std::shared_ptr<Aws::IOStream> inputData =
        Aws::MakeShared<Aws::FStream>("SampleAllocationTag",
        sourceFile.c_str(),
        std::ios_base::in | std::ios_base::binary);

    if (!*inputData) {
        std::cerr << "Error unable to read file " << sourceFile << std::endl;
        return ;
    }
    request.SetBody(inputData);

    Aws::S3::Model::PutObjectOutcome outcome = s3.PutObject(request);

    if (!outcome.IsSuccess()) {
        std::cerr << "Error: PutObject: " <<
                  outcome.GetError().GetMessage() << std::endl;
    }
    else {
        std::cout << "Added object '" << sourceFile << "' to bucket '" << ORACLE_BUCKET << "'." << std::endl;
    }
}

void downloadFile(Aws::S3::S3Client &s3, const Aws::String &keyName, const Aws::String &destFile){
    Aws::S3::Model::GetObjectRequest request;
    request.SetBucket(ORACLE_BUCKET);
    request.SetKey(keyName);

    Aws::S3::Model::GetObjectOutcome outcome = s3.GetObject(request);

    if (!outcome.IsSuccess()) {
        const Aws::S3::S3Error &err = outcome.GetError();
        std::cerr << "Error: GetObject: " <<
                  err.GetExceptionName() << ": " << err.GetMessage() << std::endl;
    }
    else {
        std::cout << "Successfully retrieved '" << keyName << "' from '" << ORACLE_BUCKET << "'." << std::endl;
        std::ofstream outFile;
        outFile.open(destFile);
        outFile << outcome.GetResult().GetBody().rdbuf();
        outFile.close();
    }

    std::cout << "Added object '" << destFile;
}

int main(){
    Aws::SDKOptions options;
    options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Debug;
    Aws::InitAPI(options);
    {
        Aws::S3::S3Client s3 = getS3Client();
        uploadFile(s3, "a5.txt","a1.txt");
        downloadFile(s3, "a5.txt","a5_down1.txt");
    }
    Aws::ShutdownAPI(options);
    return 0;
}
