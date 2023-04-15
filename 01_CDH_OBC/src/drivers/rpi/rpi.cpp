/**
 * @file
 * @brief
 */

#include "rpi.hpp"
#include "common/logger.hpp"
#include "lib/crc/crc.hpp"
#include "lib/json/json.hpp"
#include "lib/base64/base64.hpp"

constexpr uint8_t PIC_LIST_MAXNUM = 48;
constexpr uint8_t GOOD_PIC_LIST_MAXNUM = 48;

char  jsonbuf[LEN_BUF_RPI];
char  base64buf[BASE64BUF_LEN];
char* piclist_idlist[4 * PIC_LIST_MAXNUM];
char* goodpiclist_idlist[4 * PIC_LIST_MAXNUM];

constexpr uint8_t exe_result_val[] = {10, 11, 12, 13, 14};
const char* exe_result_str[] = {"success", "invalid command", "unknown error",
                                "command error", "invalid args"};
const char  exe_result_invalid[] = "invalid";

char* exe_result_to_str(uint8_t exe_result) {
    for (int i = 0; i < 5; i++) {
        if (exe_result == exe_result_val[i]) {
            return (char*)exe_result_str[i];
        }
    }

    return (char*)exe_result_invalid;
}

RPi::RPi(RawSerial* _serial) {
    timer = new Timer;
    serial = _serial;

    // flush remaining data
    while (serial->readable()) {
        serial->getc();
    }

    timer->start();
}

RPi::~RPi() {
    delete timer;
    delete serial;
}

uint8_t RPi::readline(char dat[], uint16_t len) {
    Watchdog::get_instance().kick();

    int msec_start = timer->read_ms();

    memset(dat, '\0', len);

    uint16_t i = 0;
    uint8_t  ret;
    while (true) {
        if (serial->readable()) {
            dat[i] = serial->getc();

            if (dat[i] == '\n') {
                // 残った受信バイトを全て棄却する
                while (serial->readable()) {
                    serial->getc();
                }
                ret = RPI_COMM_SUCCESS;
                break;
            }
            i++;

            if (i > len) {
                logger_debug("[rpi.readline] Too much reception\n");
                // flush remaining data
                while (serial->readable()) {
                    serial->getc();
                }

                // got data anyway, so return success
                ret = RPI_COMM_SUCCESS;
                break;
            }

        } else {
            if (timer->read_ms() > msec_start + 10000) {
                logger_debug("[rpi.%s] TIMEOUT\n", __func__);
                memset(dat, '\0', len);
                ret = RPI_COMM_FAILURE;
                break;
            }
        }
    }

    logger_debug("[rpi.%s] Received: %s\n", __func__, (char*)dat);

    return ret;
}

uint8_t RPi::receive(char dat[], uint16_t len) {
    dat[0] = 0;

    uint8_t _commstat = RPI_COMM_RECV_FAILURE;

    // Re-send up to NUM_RPI_RECV_TRIAL times
    for (uint8_t i = 0; i < NUM_RPI_RECV_TRIAL; i++) {
        Watchdog::get_instance().kick();

        logger_debug("[rpi.%s] Trial %d\n", __func__, i);

        uint8_t ret_readline = readline(dat, len);

        if (ret_readline == RPI_COMM_SUCCESS) {
            char* p = strrchr(dat, ',');

            if (p == NULL) {
                logger_error("[rpi.%s] Invalid string\n", __func__);
                continue;
            }

            p[3] = 0;

            int16_t crc_received = strtol(p + 1, NULL, 16);

            *p = 0;

            int16_t crc_calculated = crc8((uint8_t*)dat, (uint16_t)strlen(dat));

            if ((crc_calculated == crc_received) && (dat[0] != 0)) {
                serial->printf("OK\n");
                logger_debug("[rpi.%s] success\n", __func__);
                _commstat = RPI_COMM_SUCCESS;
                break;

            } else {
                serial->printf("NG\n");
                logger_error("[rpi.%s] CRC NG\n", __func__);
            }

        } else {
            logger_error("[rpi.%s] FAILURE\n", __func__);
        }
    }

    return _commstat;
}

uint8_t RPi::send(char dat[]) {
    logger_trace("[rpi.send] sending length = %d\n", strlen(dat));

    int16_t crc = crc8((uint8_t*)dat, (uint16_t)strlen(dat));

    char str_crc[4] = "";
    sprintf(str_crc, "%02x", crc);

    logger_debug("[rpi.send] Sending: %s, %s\n", dat, str_crc);

    // Flush remaining data
    while (serial->readable()) {
        serial->getc();
    }

    uint8_t _commstat = RPI_COMM_SEND_FAILURE;

    // Re-send up to NUM_RPI_RECV_TRIAL times
    for (uint8_t i = 0; i < NUM_RPI_SEND_TRIAL; i++) {
        Watchdog::get_instance().kick();

        logger_debug("[rpi.%s] trial %d\n", __func__, i);

        serial->printf("%s,%s\n", dat, str_crc);

        char    buf[4] = "";
        uint8_t ret_readline = readline(buf, 3);
        if (ret_readline == RPI_COMM_SUCCESS) {
            if (strncmp(buf, "OK", 2) == 0) {
                logger_debug("[rpi.%s] Success\n", __func__);
                _commstat = RPI_COMM_SUCCESS;
                break;

            } else {
                logger_error("[rpi.%s] Incorrect CRC\n", __func__);
            }
        } else {
            logger_error("[rpi.%s] FAILURE\n", __func__);
        }
    }

    return _commstat;
}

uint8_t RPi::communicate(char dat[], uint16_t len) {
    if (send(dat) == RPI_COMM_SEND_FAILURE) {
        logger_error("[rpi.%s] send error\n", __func__);
        return RPI_COMM_SEND_FAILURE;
    }

    uint8_t ret_receive = receive(dat, len);

    if (ret_receive == RPI_COMM_RECV_FAILURE) {
        logger_error("[rpi.%s] receive error\n", __func__);
    }

    return ret_receive;
}

uint8_t RPi::comm_check(void) {
    logger_debug("[rpi.%s]\n", __func__);
    sprintf(jsonbuf, "{\"cmd\":\"Test\"}");
    return communicate(jsonbuf, LEN_BUF_RPI);
}

uint8_t RPi::kill(uint8_t* result) {
    logger_debug("[rpi.%s]\n", __func__);

    sprintf(jsonbuf, "{\"cmd\":\"killMOBC\"}");
    uint8_t com_result = communicate(jsonbuf, LEN_BUF_RPI);
    if (com_result != RPI_COMM_SUCCESS) {
        return com_result;
    }

    *result = Json::get_long(jsonbuf, "arg", "result");

    return com_result;
}

uint8_t RPi::take_pic(uint16_t interval, uint16_t duration, uint32_t start,
                      uint8_t* result) {
    logger_debug("[rpi.%s]\n", __func__);

    sprintf(jsonbuf,
            "{\"cmd\":\"takePhoto\","
            "\"arg\":{\"interval\":%u,\"duration\":%u,"
            "\"start\":%lu}}",
            interval, duration, start);
    uint8_t com_result = communicate(jsonbuf, LEN_BUF_RPI);
    if (com_result != RPI_COMM_SUCCESS) {
        return com_result;
    }

    *result = Json::get_long(jsonbuf, "arg", "result");

    return com_result;
}

uint8_t RPi::get_pic_list(uint16_t start, uint16_t end, uint32_t* list,
                          uint16_t& count, uint8_t& result) {
    logger_debug("[rpi.%s]\n", __func__);

    sprintf(jsonbuf,
            "{\"cmd\":\"getPicList\","
            "\"arg\":{\"start\":%u,\"end\":%u}}",
            start, end);

    // bytes = 73 + 9 * (number of files)
    // 219 files maximum
    uint8_t com_result = communicate(jsonbuf, LEN_BUF_RPI);
    if (com_result != RPI_COMM_SUCCESS) {
        return com_result;
    }

    result = static_cast<uint16_t>(Json::get_long(jsonbuf, "arg", "result"));

    // Note: PicListの取得個数が閾値を超えた場合は処理を中断する
    count = static_cast<uint16_t>(Json::get_long(jsonbuf, "arg", "count"));

    if (count > PIC_LIST_MAXNUM) {
        logger_error("[rpi.%s] pic list too long: %d\n", __func__, count);
        count = PIC_LIST_MAXNUM;
        return com_result;
    }

    char** idlist = piclist_idlist;

    Json::get_str_array(jsonbuf, "arg", "list", idlist);

    for (uint16_t i = 0; i < count; i++) {
        // Extract id and sub ID
        char*    idchar = idlist[i];
        uint16_t id = static_cast<uint16_t>((idchar[0]) - '0') * 100 +
                      static_cast<uint16_t>((idchar[1]) - '0') * 10 +
                      (idchar[2] - '0');

        uint16_t subid = 0xFFFF;
        if (idchar[3] == '_') {
            subid = static_cast<uint16_t>((idchar[4]) - '0') * 1000 +
                    static_cast<uint16_t>((idchar[5]) - '0') * 100 +
                    static_cast<uint16_t>((idchar[6]) - '0') * 10 +
                    (idchar[7] - '0');

        } else if (idchar[3] == '\0') {
            logger_trace("[rpi.%s] %u: id only\n", __func__, i);
        } else {
            logger_debug("[rpi.%s] invalid id\n", __func__);
        }

        list[i] =
            static_cast<uint32_t>((static_cast<uint32_t>(id) << 16) + subid);
        logger_debug("[rpi.%s] id-subid[%03u]: 0x%04X-0x%04X\n", __func__, i,
                     list[i] >> 16, list[i] & 0xFFFF);
    }

    serial->printf("]\n");

    return com_result;
}

uint8_t RPi::get_pic_info(uint16_t start, uint16_t end, uint8_t type,
                          PicInfo* info, uint16_t& count, uint8_t& result) {
    logger_debug("[rpi.%s]\n", __func__);

    sprintf(jsonbuf,
            "{\"cmd\":\"getPicInfo\","
            "\"arg\":{\"start\":%u,\"end\":%u,\"type\":%u}}",
            start, end, type);

    // ラスパイからのデータサイズは149byte + 81byte * ファイル数
    // バッファが2048byteなので取得できるのは最大23ファイルまで
    // 23以上の連続撮影をすると対応できなくなる
    uint8_t com_result = communicate(jsonbuf, LEN_BUF_RPI);
    if (com_result != RPI_COMM_SUCCESS) {
        return com_result;
    }

    count = (uint16_t)Json::get_long(jsonbuf, "arg", "count");

    for (uint16_t i = 0; i < count; i++) {
        info[i].id = (uint16_t)Json::get_long(jsonbuf, "arg", "info", "id", i);
        info[i].sub_id =
            (uint16_t)Json::get_long(jsonbuf, "arg", "info", "sub_id", i);
        info[i].type =
            (uint8_t)Json::get_long(jsonbuf, "arg", "info", "type", i);
        info[i].width =
            (uint16_t)Json::get_long(jsonbuf, "arg", "info", "width", i);
        info[i].height =
            (uint16_t)Json::get_long(jsonbuf, "arg", "info", "height", i);
        info[i].size =
            (uint32_t)Json::get_long(jsonbuf, "arg", "info", "size", i);
        info[i].categ =
            (uint8_t)Json::get_long(jsonbuf, "arg", "info", "categ", i);

        logger_debug("[rpi.%s] picInfo[%d]: id=%d sub_id= type=%d "
                     "width=%d height=%d categ=%d\n",
                     __func__, i, info[i].id, info[i].sub_id, info[i].type,
                     info[i].width, info[i].height, info[i].categ);
    }

    return com_result;
}

uint8_t RPi::get_pic_data(uint8_t type, uint16_t id, uint16_t sub_id,
                          uint32_t pos, uint16_t size, uint8_t data[],
                          uint16_t* data_size) {
    logger_debug("[rpi.%s]\n", __func__);

    sprintf(jsonbuf,
            "{\"cmd\":\"getPicData\","
            "\"arg\":{\"type\":%u,\"id\":%u,\"sub_id\":%u,"
            "\"pos\":%lu,\"size\":%u}}",
            type, id, sub_id, pos, size);
    uint8_t com_result = communicate(jsonbuf, LEN_BUF_RPI);
    if (com_result != RPI_COMM_SUCCESS) {
        return com_result;
    }

    // Get base64-encoded data
    uint16_t    base64len = (uint16_t)Json::get_long(jsonbuf, "arg", "size");
    const char* argdata = Json::get_strptr(jsonbuf, "arg", "data");

    // +1 for terminating character
    strncpy((char*)base64buf, argdata, base64len + 1);

    logger_debug("[rpi.%s] dat: %s\n", __func__, base64buf);

    *data_size = base64_decode(base64buf, (char*)data, base64len);
    logger_debug("[rpi.%s] Got %u bytes\n", __func__, *data_size);

    return com_result;
}

uint8_t RPi::delete_pic(uint16_t id, uint16_t sub_id, uint8_t mode,
                        uint8_t* result) {
    logger_debug("[rpi.%s]\n", __func__);

    sprintf(jsonbuf,
            "{\"cmd\":\"deletePic\","
            "\"arg\":{\"id\":%u,\"sub_id\":%u,\"mode\":%u}}",
            id, sub_id, mode);

    uint8_t com_result = communicate(jsonbuf, LEN_BUF_RPI);
    if (com_result != RPI_COMM_SUCCESS) {
        return com_result;
    }

    *result = Json::get_long(jsonbuf, "arg", "result");

    logger_debug("[rpi.%s] Deletion %s\n", __func__,
                 *result == 0 ? "success" : "failure");

    return com_result;
}

uint8_t RPi::set_pic_size(uint16_t width, uint16_t height, uint8_t* result) {
    logger_debug("[rpi.%s]\n", __func__);

    sprintf(jsonbuf,
            "{\"cmd\":\"setPicSize\","
            "\"arg\":{\"width\":%u,\"height\":%u}}",
            width, height);

    uint8_t com_result = communicate(jsonbuf, LEN_BUF_RPI);
    if (com_result != RPI_COMM_SUCCESS) {
        return com_result;
    }

    *result = Json::get_long(jsonbuf, "arg", "result");

    logger_debug("[rpi.%s] result = %d\n", __func__, *result);
    return com_result;
}

uint8_t RPi::set_pic_compression(uint16_t width, uint16_t height,
                                 uint8_t* result) {
    logger_debug("[rpi.%s]\n", __func__);

    sprintf(jsonbuf,
            "{\"cmd\":\"setCompPic\","
            "\"arg\":{\"thumb_width\":%u,\"thumb_height\":%u}}",
            width, height);

    uint8_t com_result = communicate(jsonbuf, LEN_BUF_RPI);
    if (com_result != RPI_COMM_SUCCESS) {
        return com_result;
    }

    *result = Json::get_long(jsonbuf, "arg", "result");

    return com_result;
}

uint8_t RPi::get_cam_config(camcfg_t* cfg) {
    logger_debug("[rpi.%s]\n", __func__);

    sprintf(jsonbuf, "{\"cmd\":\"setCamConfig\",\"arg\":{\"mode\":0}}");

    uint8_t com_result = communicate(jsonbuf, LEN_BUF_RPI);
    if (com_result != RPI_COMM_SUCCESS) {
        return com_result;
    }

    int16_t sharpness = 0xFF & Json::get_long(jsonbuf, "arg", "sharpness");
    int16_t contrast = 0xFF & Json::get_long(jsonbuf, "arg", "contrast");
    int16_t saturation = 0xFF & Json::get_long(jsonbuf, "arg", "saturation");

    if (sharpness > 127) {
        sharpness = sharpness - 256;
    }

    if (contrast > 127) {
        contrast = contrast - 256;
    }

    if (saturation > 127) {
        saturation = saturation - 256;
    }

    cfg->iso = 0xFFFF & Json::get_long(jsonbuf, "arg", "iso");
    cfg->shutter = 0xFFFF & Json::get_long(jsonbuf, "arg", "shutter");
    cfg->quality = 0xFF & Json::get_long(jsonbuf, "arg", "quality");
    cfg->sharpness = sharpness;
    cfg->contrast = contrast;
    cfg->brightness = 0xFF & Json::get_long(jsonbuf, "arg", "brightness");
    cfg->saturation = saturation;
    cfg->exposure = 0xFF & Json::get_long(jsonbuf, "arg", "exposure");

    logger_debug("[%s] iso=%u shutter=%u quality=%u sharpness=%u\n", __func__,
                 cfg->iso, cfg->shutter, cfg->quality, cfg->sharpness);
    logger_debug("[%s] contrast=%d brightness=%d saturation=%d exposure=%u\n",
                 __func__, cfg->contrast, cfg->brightness, cfg->saturation,
                 cfg->exposure);

    return com_result;
}

uint8_t RPi::set_cam_config(camcfg_t* cfg) {
    logger_debug("[rpi.%s]\n", __func__);

    int16_t sharpness = (0xFF & cfg->sharpness);
    int16_t contrast = (0xFF & cfg->contrast);
    int16_t saturation = (0xFF & cfg->saturation);

    if (sharpness > 127) {
        sharpness = sharpness - 256;
    }
    if (contrast > 127) {
        contrast = contrast - 256;
    }
    if (saturation > 127) {
        saturation = saturation - 256;
    }

    sprintf(jsonbuf,
            "{\"cmd\":\"setCamConfig\","
            "\"arg\":{\"mode\":1,"
            "\"iso\":%u,\"shutter\":%u,"
            "\"quality\":%u,\"sharpness\":%d,"
            "\"contrast\":%d,\"brightness\":%u,"
            "\"saturation\":%d,\"exposure\":%u}}",
            0xFFFF & cfg->iso, 0xFFFF & cfg->shutter, 0xFF & cfg->quality,
            sharpness, contrast, 0xFF & cfg->brightness, saturation,
            cfg->exposure);

    uint8_t com_result = communicate(jsonbuf, LEN_BUF_RPI);
    if (com_result != RPI_COMM_SUCCESS) {
        return com_result;
    }

    Json::get_long(jsonbuf, "arg", "result");

    return com_result;
}

uint8_t RPi::uplink_file(char* filename, uint32_t pos, uint16_t size,
                         char* data) {
    logger_debug("[rpi.%s]\n", __func__);

    sprintf(jsonbuf,
            "{\"cmd\":\"sendFile\","
            "\"arg\":{\"filename\":\"%s\",\"pos\":%lu,"
            "\"size\":%u,\"data\":\"%s\"}}",
            filename, pos, size, data);

    uint8_t com_result = communicate(jsonbuf, LEN_BUF_RPI);
    if (com_result != RPI_COMM_SUCCESS) {
        return com_result;
    }

    uint16_t sendFileSize = Json::get_long(jsonbuf, "arg", "size");
    if (sendFileSize != size) {
        logger_debug("[rpi.sendFile] Invalid size\n");
        return -1;
    }
    return com_result;
}

uint8_t RPi::downlink_file(char* filepath, uint32_t pos, uint16_t size,
                           uint8_t* data, uint16_t* data_size) {
    logger_debug("[rpi.%s]\n", __func__);

    sprintf(jsonbuf,
            "{\"cmd\":\"getFile\","
            "\"arg\":{\"filepath\":\"%s\",\"pos\":%lu,\"size\":%u}}",
            filepath, pos, size);
    uint8_t com_result = communicate(jsonbuf, LEN_BUF_RPI);
    if (com_result != RPI_COMM_SUCCESS) {
        return com_result;
    }

    uint16_t base64len = Json::get_long(jsonbuf, "arg", "size");
    Json::get_str(jsonbuf, "arg", "data", *data_size, base64buf);

    *data_size = base64_decode(base64buf, (char*)data, base64len);

    return com_result;
}

uint8_t RPi::start_pic_categ(uint8_t work_type, uint8_t is_all, uint8_t do_wait,
                             uint8_t* exe_result, uint8_t* cmd_result) {
    char work_type_str[4] = "";
    if (work_type == 0x01)
        strcpy(work_type_str, "tfl");
    else
        strcpy(work_type_str, "tf");

    char is_all_str[6] = "";
    if (is_all == 0x01)
        strcpy(is_all_str, "true");
    else
        strcpy(is_all_str, "false");

    char do_wait_str[6] = "";
    if (do_wait == 0x01)
        strcpy(do_wait_str, "true");
    else
        strcpy(do_wait_str, "false");

    logger_debug("[rpi.%s] Start\n", __func__);

    sprintf(jsonbuf,
            "{\"cmd\":\"startPicCateg\","
            "\"arg\":{\"work_type\":\"%s\","
            "\"all\":\"%s\","
            "\"wait\":\"%s\"}"
            "}",
            work_type_str, is_all_str, do_wait_str);
    uint8_t com_result = communicate(jsonbuf, LEN_BUF_RPI);
    if (com_result != RPI_COMM_SUCCESS) {
        return com_result;
    }

    *exe_result = (uint8_t)Json::get_long(jsonbuf, "arg", "exe_result");
    *cmd_result = (uint8_t)Json::get_long(jsonbuf, "arg", "cmd_result");

    logger_debug("[rpi.%s] exe_result = %s (0x%X) cmd_result=0x%X\n", __func__,
                 exe_result_to_str(*exe_result), *exe_result, *cmd_result);

    return com_result;
}

uint8_t RPi::check_pic_categ(uint8_t* exe_result, uint8_t* cmd_result) {
    logger_debug("[rpi.%s]\n", __func__);

    sprintf(jsonbuf, "{\"cmd\":\"checkPicCateg\"}");

    uint8_t com_result = communicate(jsonbuf, LEN_BUF_RPI);
    if (com_result != RPI_COMM_SUCCESS) {
        return com_result;
    }

    *exe_result = (uint8_t)Json::get_long(jsonbuf, "arg", "exe_result");
    *cmd_result = (uint8_t)Json::get_long(jsonbuf, "arg", "cmd_result");
    logger_debug("[rpi.%s] exe_result = %s (0x%X) cmd_result=0x%X\n", __func__,
                 exe_result_to_str(*exe_result), *exe_result, *cmd_result);

    return com_result;
}

uint8_t RPi::abort_pic_categ(uint8_t* exe_result, uint8_t* cmd_result) {
    logger_debug("[rpi.%s]\n", __func__);

    sprintf(jsonbuf, "{\"cmd\":\"abortPicCateg\"}");
    uint8_t com_result = communicate(jsonbuf, LEN_BUF_RPI);
    if (com_result != RPI_COMM_SUCCESS) {
        return com_result;
    }

    *exe_result = (uint8_t)Json::get_long(jsonbuf, "arg", "exe_result");
    *cmd_result = (uint8_t)Json::get_long(jsonbuf, "arg", "cmd_result");
    logger_debug("[rpi.%s] exe_result = %s (0x%X) cmd_result=0x%X\n", __func__,
                 exe_result_to_str(*exe_result), *exe_result, *cmd_result);
    return com_result;
}

uint8_t RPi::get_pic_categ(uint8_t work_type, uint16_t id, uint16_t sub_id,
                           uint8_t* exe_result, uint8_t* cmd_result,
                           uint8_t* categ, uint8_t* percent) {
    logger_debug("[rpi.%s]\n", __func__);

    char work_type_str[4] = "";
    if (work_type == 0x00) {
        strcpy(work_type_str, "tf");
    } else if (work_type == 0x01) {
        strcpy(work_type_str, "tfl");
    } else {
        strcpy(work_type_str, "no");
    }

    sprintf(jsonbuf,
            "{\"cmd\":\"getPicCateg\","
            "\"arg\":{"
            "\"work_type\":\"%s\","
            "\"id\":\"%u\","
            "\"sub_id\":\"%u\""
            "}}",
            work_type_str, id, sub_id);
    uint8_t com_result = communicate(jsonbuf, LEN_BUF_RPI);
    if (com_result != RPI_COMM_SUCCESS) {
        return com_result;
    }

    *exe_result = (uint8_t)Json::get_long(jsonbuf, "arg", "exe_result");
    *cmd_result = (uint8_t)Json::get_long(jsonbuf, "arg", "cmd_result");
    *categ = (uint8_t)Json::get_long(jsonbuf, "arg", "categ");
    *percent = (uint8_t)Json::get_long(jsonbuf, "arg", "percent");

    logger_debug("[rpi.%s] exe_result = %s (0x%X) cmd_result=0x%X\n", __func__,
                 exe_result_to_str(*exe_result), *exe_result, *cmd_result);
    logger_debug("[rpi.%s] categ=%d percent=%d\n", __func__, *categ, *percent);

    return com_result;
}

uint8_t RPi::get_good_pic_ids(uint8_t* exe_result, uint8_t* cmd_result,
                              uint8_t* count, uint16_t ids[],
                              uint16_t sub_ids[]) {
    logger_debug("[rpi.%s]\n", __func__);

    sprintf(jsonbuf, "{\"cmd\":\"getGoodPicIds\"}");
    uint8_t com_result = communicate(jsonbuf, LEN_BUF_RPI);
    if (com_result != RPI_COMM_SUCCESS) {
        return com_result;
    }

    *exe_result = (uint8_t)Json::get_long(jsonbuf, "arg", "exe_result");
    *cmd_result = (uint8_t)Json::get_long(jsonbuf, "arg", "cmd_result");
    *count = (uint8_t)Json::get_long(jsonbuf, "arg", "count");

    char** idlist = goodpiclist_idlist;
    Json::get_str_array(jsonbuf, "arg", "ids", idlist);

    for (uint16_t i = 0; i < *count; i++) {
        // Convert ID string to two uint16_t values
        char* idchar = idlist[i];
        for (uint16_t p = 0; p < 8; p++) {
            logger_trace("[rpi.%s] idchar[%d] = 0x%X\n", __func__, p,
                         idchar[p]);
        }

        ids[i] = static_cast<uint16_t>(idchar[0] - '0') * 100 +
                 static_cast<uint16_t>(idchar[1] - '0') * 10 +
                 (idchar[2] - '0');

        sub_ids[i] = 0xFFFF;
        if ((idchar[3]) == '_') {
            sub_ids[i] = static_cast<uint16_t>(idchar[4] - '0') * 1000 +
                         static_cast<uint16_t>(idchar[5] - '0') * 100 +
                         static_cast<uint16_t>(idchar[6] - '0') * 10 +
                         (idchar[7] - '0');
        } else if (idchar[3] == '\0') {
            logger_trace("[rpi.%s] ID only\n", __func__);
        } else {
            logger_trace("[rpi.%s] Invalid character\n", __func__);
        }
    }

    logger_debug("[rpi.%s] ids-subids:", __func__);
    for (uint16_t i = 0; i < *count; i++) {
        logger_debug("%d-%d\n", ids[i], sub_ids[i]);
    }

    return com_result;
}

uint8_t RPi::exec_shell(char* cmd, uint16_t msglen, char* msg) {
    logger_debug("[rpi.%s]\n", __func__);

    sprintf(jsonbuf,
            "{\"cmd\":\"execShell\","
            "\"arg\":{\"cmd\":\"%s\"}}",
            cmd);
    uint8_t com_result = communicate(jsonbuf, LEN_BUF_RPI);
    if (com_result != RPI_COMM_SUCCESS) {
        return com_result;
    }

    Json::get_str(jsonbuf, "arg", "msg", msglen, msg);

    logger_debug("[rpi.%s] msg: %s\n", __func__, msg);

    return com_result;
}

uint8_t RPi::get_temp(void) {
    logger_debug("[rpi.%s]\n", __func__);
    sprintf(jsonbuf, "{\"cmd\":\"getTemp\"}");
    uint8_t com_result = communicate(jsonbuf, LEN_BUF_RPI);
    if (com_result != RPI_COMM_SUCCESS) {
        return com_result;
    }

    temp = (int16_t)Json::get_long(jsonbuf, "arg", "temp");
    logger_debug("[rpi.%s] temp = %d\n", __func__, temp);

    return com_result;
}

int RPi::wakeup(void) {
    if (powstat == POWSTAT_OFF) {
        logger_debug("%s: Waking up\n", __func__);
        powdev->supply_on();
        powstat = POWSTAT_WAKING_UP;
        powtimer->start();
        return 0;
    }

    logger_error("%s: Rejected: currently at %u\n", __func__, powstat);
    return -EINVAL;
}

int RPi::_shutdown(void) {
    uint8_t result;
    uint8_t commstat = kill(&result);
    bool success = (commstat == RPI_COMM_SUCCESS || result == RPI_COMM_SUCCESS);
    return success ? 0 : -EIO;
}

int RPi::shutdown(void) {
    int err = 0;
    logger_debug("%s: Shutting down\n", __func__);
    err = _shutdown();
    powstat = POWSTAT_SHUTTING_DOWN;
    powtimer->stop();
    powtimer->reset();
    powtimer->start();

    return err;
}

constexpr int RPI_KILL_TO_OFF_MSEC = 180000;
constexpr int RPI_OFF_TO_ON_MSEC = 60000;

void RPi::monitor(void) {
    switch (powstat) {
        case POWSTAT_SHUTTING_DOWN:
            if (powtimer->read_ms() > RPI_KILL_TO_OFF_MSEC) {
                powdev->supply_off();
                powstat = POWSTAT_OFF;
                powtimer->stop();
                powtimer->reset();
                logger_info("RPi shut down\n", __func__);
            }
            break;

        case POWSTAT_WAKING_UP:
            if (powtimer->read_ms() > RPI_OFF_TO_ON_MSEC) {
                powstat = POWSTAT_ON;
                powtimer->stop();
                powtimer->reset();
                logger_info("RPi running\n", __func__);
            }
            break;

        default:
            break;
    }
}
