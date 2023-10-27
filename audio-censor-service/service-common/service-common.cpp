//
// Created by alex on 2020/1/20.
//

#include "service-common.h"

#include "base/files/file.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "configer.h"
#include "custom_thread.h"
#include "DataTransform.h"
#include "eureka_client.h"
#include "kafka_client.h"

EurekaClient *ec = nullptr;

namespace ServiceCommon{
    //初始化日志,将日志写入文件 log message
    void InitLog() {
        using namespace base;
        using namespace logging;
        LoggingSettings settings;
        //日志输出目的地(默认控制台)
        //LOG_TO_SYSTEM_DEBUG_GLOG(DEBUG控制台),LOG_TO_ALL(文件和控制台)
        settings.logging_dest = LOG_TO_SYSTEM_DEBUG_LOG;

        File::Error error;
        FilePath local_log_file_path = FilePath(Configer::GetValue(SETTING_LOG_DIR, "./log/service"));
        if (!CreateDirectoryAndGetError(local_log_file_path.DirName(), &error)) {
            GLOG(FATAL) << "failed to create log dir:" << Configer::GetValue("LogDir", "")
                        << " error:" << File::ErrorToString(error);
        }

        settings.log_file_size = Configer::GetIntValue(SETTING_LOG_SIZE, 4) * (1ul << 20);//4MB

        settings.log_file = local_log_file_path.value().c_str();

        // APPEND_TO_OLD_LOG_FILE(默认追加),DELETE_OLD_LOG_FILE(删除旧文件)
        settings.delete_old = APPEND_TO_OLD_LOG_FILE;

        InitLogging(settings);
        //参数1, 进程ID,
        //参数2, 线程ID,
        //参数3, 时间戳,
        //参数4, TickCount,默认只显示时间戳
        SetLogItems(false, false, true, false);

        SetMinLogLevel(LOG_VERBOSE);
    }

    void InitKafka(){
        KafkaClient::GetInstance()->Init(Configer::GetValue(SETTING_DATAWORK_HOST, ""));
    }

    void InitEureka(){
        if(!ec){
            ec = new EurekaClient;
            ec->Init();
        }
    }

    void StopEureka(){
        if(ec){
            ec->Stop();
            delete ec;
        }
    }

    void InitMysql(){

    }

    void InitRedis(){

    }

    void ServiceInit(){
        //初始化配置文件
        Configer::InitConf(Configer::FLAG_CMD | Configer::FLAG_APOLLO);

        //初始化日志
        InitLog();

        //初始化kafka
        InitKafka();

        InitMysql();
        
        InitRedis();

        //初始化线程池
        CppProcess::CustomThread::InitGlobal();
    }

    uint64_t RemakeErrorCode(ErrorCode e_code) {
        if(e_code == kNoError){
            return e_code;
        }else{
            return std::stoull("300" +
            std::to_string(Configer::GetIntValue(SETTING_APP_BUSCODE, 600)) +
            std::to_string(e_code));
        }
    }
}