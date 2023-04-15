/**
 * @file
 * @brief
 */

#include "json.hpp"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

bool Json::isNumber(const char* key) {
    while (*key) {
        if (!isdigit(*key++)) {
            return false;
        }
    }
    return true;
}

void Json::get_str(const char* json, const char* topkey, const char* lowkey,
                   uint16_t bufSize, char buf[]) {
    const char* p;
    if (isNumber(topkey)) {
        short index = strtol(topkey, NULL, 10);
        // 配列にアクセスする
        p = strstr(json, "[");
        while (index--) {
            p = strstr(p, ",");
        }
    } else if (topkey) {
        // topkeyの先頭へ移動する
        p = strstr(json, topkey);
    } else {
        p = (char*)json;
    }

    // 初期値は空文字列としておく（終端が設定されない文字列を返却してしまう為）
    buf[0] = '\0';

    if (p == NULL) {
        return;
    }

    // valueの先頭へ移動する
    p = strstr(p, lowkey);
    // 子要素が存在しない場合は、取得なしで終了
    if (p == NULL) {
        return;
    }

    p = strchr(p, ':');

    const char* p0 = p;

    p = strstr(p, "\"");

    if (p == NULL) {
        p = strchr(p0, '\'');
    }

    if (p == NULL) {
        return;
    }
    p++;

    // 値を取得する
    uint16_t pos = 0;
    while (true) {
        char c = *p;
        p++;
        if (c == '\"') break;
        if (c == '\'') break;
        if (c == '}') break;
        if (c == ',') break;
        if (c == 0) break;
        if (pos > bufSize - 1) break;
        buf[pos] = c;
        pos++;
    }
    buf[pos] = '\0';
}

const char* Json::get_strptr(const char* json, const char* topkey,
                             const char* lowkey) {
    const char* p;

    if (isNumber(topkey)) {
        short index = strtol(topkey, NULL, 10);
        // 配列にアクセスする
        p = strstr(json, "[");
        while (index--) {
            p = strstr(p, ",");
        }
    } else if (topkey) {
        // topkeyの先頭へ移動する
        p = strstr(json, topkey);
    } else {
        p = (char*)json;
    }

    if (p == NULL) {
        return p;
    }

    // 値の先頭へ移動する
    p = strstr(p, lowkey);
    p = strchr(p, ':');

    const char* p0 = p;

    p = strchr(p, '\"');
    if (p == NULL) {
        p0 = strchr(p0, '\'');
        p0++;
        char* p_end = (char*)strchr(p0, '\'');
        *p_end = 0;
        return p0;

    } else {
        p++;
        char* p_end = (char*)strchr(p, '\"');
        *p_end = 0;
        return p;
    }
}

void Json::get_str_array(const char* json, const char* topkey,
                         const char* lowkey, char** strArray) {
    // topkeyの先頭へ移動する
    const char* p = topkey ? strstr(json, topkey) : json;
    if (p == NULL) {
        return;
    }

    const uint16_t array_size = get_array_size(json, topkey, lowkey);

    // 値の先頭へ移動する
    p = strstr(p, lowkey);
    p = strchr(p, ':');

    const char* p0 = p;

    for (uint16_t i = 0; i < array_size; i++) {
        p = strchr(p, '\"');
        if (p == NULL) {
            p0 = strchr(p0, '\'');
            p0++;
            char* p_end = (char*)strchr(p0, '\'');
            *p_end = 0;
            strArray[i] = (char*)p0;
            strArray[i + 1] = NULL;
            p = p_end++;
        } else {
            p++;
            const char*    p_end = strchr(p, '\"');
            const uint16_t item_size = (uint16_t)(p_end - p);
            strArray[i] = (char*)malloc(sizeof(char) * (item_size + 1));
            memset((char*)strArray[i], 0, item_size + 1);
            strncpy(strArray[i], p, item_size);
            strArray[i + 1] = NULL;
            p = p_end + 2;
        }
    }
}

uint16_t Json::count_str(char* myString, char* string2find) {
    uint16_t    count = 0;
    const char* p = myString;
    p = strstr(p, string2find);

    while (p) {
        count++;
        p++;
        p = strstr(p, string2find);
    }
    return count;
}

uint16_t Json::get_array_size(const char* json, const char* topkey,
                              const char* lowkey) {
    const char* p = topkey ? strstr(json, topkey) : json;
    if (p == NULL) {
        return 0;
    }

    // 値の先頭へ移動する
    p = strstr(p, lowkey);
    p = strchr(p, ':');
    p++;
    if (p[0] == '[') p++;

    if (p[0] == '{') {
        return count_str((char*)p, (char*)"{");

    } else if (p[0] != ']') {
        const char*    end_p = strchr(p, ']');
        const uint16_t item_size = end_p - p;

        char* item_p = (char*)malloc(sizeof(char) * (item_size + 1));
        strncpy(item_p, p, item_size);
        item_p[item_size + 1] = 0;
        uint16_t count = count_str((char*)item_p, (char*)",") + 1;

        if (item_p) free(item_p);
        return count;
    }

    return 0;
}

long Json::get_long(const char* json, const char* topkey, const char* lowkey) {
    const char* p;
    if (topkey) {
        p = strstr(json, topkey); // topkeyの先頭へ移動
    } else {
        p = json;
    }
    if (p == NULL) {
        return 0;
    }

    // 値の先頭へ移動する
    p = strstr(p, lowkey);
    p = strchr(p, ':');
    p++;

    if (p[0] == '\"') {
        p++;
    }

    long ret = strtol(p, NULL, 10);

    return ret;
}

long Json::get_long(const char* json, const char* topkey, const char* midkey,
                    const char* lowkey, short index) {
    const char* p;

    if (topkey) {
        p = strstr(json, topkey); // topkeyの先頭へ移動
    } else {
        p = json;
    }
    if (p == NULL) {
        return 0;
    }

    // 値の先頭へ移動する
    p = strstr(p, midkey);
    p = strchr(p, ':');

    // midkeyに属する括弧の中に入り、index番目の括弧内でlowkeyの値を見つける
    for (uint8_t i = 0; i < index + 1; i++) {
        p++;
        p = strstr(p, "{");
    }
    p = strstr(p, lowkey);
    p = strchr(p, ':');
    p++;

    long ret = strtol(p, NULL, 10);

    return ret;
}
