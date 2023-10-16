<?php
require 'vendor/autoload.php';

use Aws\S3\S3Client;
use Aws\S3\Exception\S3Exception;


//define('ORACLE_ACCESS_KEY', '****'); //用环境变量AWS_ACCESS_KEY_ID代替
//define('ORACLE_SECRET_KEY', '***');//用环境变量AWS_SECRET_ACCESS_KEY代替
define('ORACLE_REGION', 'ap-tokyo-1');
define('ORACLE_NAMESPACE', 'sehubjapacprod');
define('ORACLE_BUCKET', 'Wilbur-Bucket');
       
function getS3Client(){
    $endpoint = "https://".ORACLE_NAMESPACE.".compat.objectstorage.".ORACLE_REGION.".oraclecloud.com/".ORACLE_BUCKET."/";
    $s3 = new S3Client(array(
        /*用环境变量代理
        'credentials' => [
            'key' => ORACLE_ACCESS_KEY,
            'secret' => ORACLE_SECRET_KEY,
        ],
        */
        'version' => 'latest',
        'region' => ORACLE_REGION,
        'signature_version' => 'v4',
        'use_path_style_endpoint' => true,
        'bucket_endpoint' => true,
        'endpoint' => $endpoint
    ));
    return $s3;
}

function uploadFile($s3, $keyName, $sourceFile){
    try {
        $objs = [
            'Bucket' => ORACLE_BUCKET,
            'Key'    => $keyName,
            'SourceFile' => $sourceFile
        ];

        //print_r($objs);
        $result = $s3->putObject($objs);
        //print_r($result);

        echo 'Uploaded. Object URL：' .$result['ObjectURL'] . PHP_EOL;
    } catch (S3Exception $e) {
        echo $e->getMessage() . PHP_EOL;
    }
}

function downloadFile($s3, $keyName, $destFile){
    try {
        $objs = [
            'Bucket' => ORACLE_BUCKET,
            'Key'    => $keyName,
        ];

        //print_r($objs);
        $result = $s3->getObject($objs);
        //print_r($result);

        $body = $result['Body']; 
        echo 'Downloaded. File content:'.$body. PHP_EOL;
        
        // 二进制流文件 $inflatedBody = new InflateStream($result['Body']); 

        $text_file = fopen($destFile, "w+");
        fwrite($text_file, $body);
        fclose($text_file);

    } catch (S3Exception $e) {
        echo $e->getMessage() . PHP_EOL;
    }
}

$s3 = getS3Client();
uploadFile($s3, "a4.txt","a1.txt");
downloadFile($s3, "a4.txt","a4_down1.txt");