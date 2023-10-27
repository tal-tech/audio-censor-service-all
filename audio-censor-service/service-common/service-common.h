//
// Created by alex on 2020/1/20.
//

#ifndef CPPSERVICE_SERVICE_COMMON_H
#define CPPSERVICE_SERVICE_COMMON_H

#include <string>
#include "base/logging.h"

namespace ServiceCommon {
#define PaaSLOG(severity, e_code, msg) \
    GLOG(severity) << " alertcode:" << e_code << ",alerturl:" << Configer::GetValue(SETTING_SERVER_ROUTE, "") \
    << ",alertmsg:ecode:" << e_code << " " << msg;

    enum ErrorCode {//5xx for http; 8xx for websocket
        kNoError = 20000,//no error
        kInvalidJson = 500,
        kAlgError = 501,// algorithm error
        kKafkaSendError = 504, // send kafka error
        kWrongSampleRate = 505, // wrong sample rate error
        kBase64DecodeError = 506,
        kDownloadError = 507,
        kNoValidDataError = 508,
        kEmptyDataError = 509,
        kOversizeData = 510,
        kASRDecodeError = 511,
        kASRResultError = 512,


        kNoMoreConnections = 800,
        kPongTimeOut = 801,
        kInitConnectionsError = 802,
        kMsgProcessError = 803,
        kDecodeError = 804,
        kBadConnections = 805,
        kEnd
    };

    uint64_t RemakeErrorCode(ErrorCode e_code);

    void InitEureka();

    void StopEureka();

    void ServiceInit();
};


#endif //CPPSERVICE_SERVICECOMMON_H
