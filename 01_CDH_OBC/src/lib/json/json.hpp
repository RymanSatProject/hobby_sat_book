/**
@file json.hpp
@brief JSONを扱う関数を定義します。
*/

#ifndef __JSON_HPP__
#define __JSON_HPP__

#include "mbed.h"

class Json {
public:
    /**
     * @brief JSONメッセージからキーに対応した文字列を取り出します。
     * @param json: JSONメッセージ
     * @param topkey: 上位のキー
     * @param lowkey: 下位のキー
     * @param bufSize: 文字列取り出し先の領域サイズ
     * @param [out] buf: 取り出された文字列
     */
    static void get_str(const char* json, const char* topkey,
                        const char* lowkey, uint16_t bufSize, char buf[]);

    /**
     * @brief
     * JSONメッセージ内の、キーに対応した値へのポインタを取得します。
     * @param json: JSONメッセージ
     * @param topkey: 上位のキー
     * @param lowkey: 下位のキー
     * @return 取り出された値へのポインタ
     */
    static const char* get_strptr(const char* json, const char* topkey,
                                  const char* lowkey);

    /**
     * @brief
     * @param json:
     * @param topkey:
     * @param lowkey:
     * @param strArray:
     */
    static void get_str_array(const char* json, const char* topkey,
                              const char* lowkey, char** strArray);

    /**
     * @brief 配列のサイズを取得します。
     * @param json: A JSON message
     * @param topkey: Top key
     * @param lowkey: Lowe key
     * @return: Array size.
     */
    static uint16_t get_array_size(const char* json, const char* topkey,
                                   const char* lowkey);

    /**
     * @brief JSONメッセージから整数値を取得します。
     * @param json: JSONメッセージ
     * @param topkey: 上位キー
     * @param lowkey: 下位キー
     * @return: 取得された値
     */
    static long get_long(const char* json, const char* topkey,
                         const char* lowkey);

    /**
     * @brief JSONメッセージから整数値を取得します。
     * @param json: JSONメッセージ
     * @param topkey: 最上位キー
     * @param midkey: 中位キー（大括弧[]がこれに属する）
     * @param lowkey: 最下位キー
     * @param index:
     * 最下位キーのインデックス。大括弧[]は複数の中括弧{}を中に持ち、インデックスはどの{}を選ぶかを決める
     * @return 取得された値
     */
    static long get_long(const char* json, const char* topkey,
                         const char* midkey, const char* lowkey, short index);

private:
    /**
     * @brief 指定のキーが数字かどうか調べます。
     * @return 成否
     */
    static bool isNumber(const char* key);

    /**
     * @brief
     * @param myString:
     * @param string2find:
     * @return count
     */
    static uint16_t count_str(char* myString, char* string2find);
};

#endif // __JSON_HPP__
