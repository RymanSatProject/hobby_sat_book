/**
 * @file
 * @brief
 */

#ifndef __RPI_HPP__
#define __RPI_HPP__

#include "device/device.hpp"

constexpr uint32_t RPIBUF_LEN = 2560;
constexpr uint32_t BASE64BUF_LEN = RPIBUF_LEN * 4 / 3 + 4;

constexpr uint32_t LEN_BUF_RPI = 2560 * 4 / 3 + 128; // = 2858 bytes

constexpr uint8_t NUM_RPI_RECV_TRIAL = 3;
constexpr uint8_t NUM_RPI_SEND_TRIAL = 3;

/**< RPiとの通信状態 */
typedef enum _rpi_commstat : uint8_t {
    RPI_COMM_SUCCESS = 0x0,
    RPI_COMM_FAILURE = 0x2,
    RPI_COMM_SEND_FAILURE = 0x4, // RPiへの送信失敗
    RPI_COMM_RECV_FAILURE = 0x8  // RPiからの受信失敗
} rpi_commstat_t;

/**
 * @brief picinfoを格納します。
 */
struct PicInfo {
    uint16_t id;
    uint16_t sub_id;
    uint8_t  type;
    uint16_t width;
    uint16_t height;
    uint32_t size;
    uint8_t  categ;
};

/**
 * @brief カメラの設定を格納します。
 */
typedef struct camcfg {
    uint16_t iso;
    uint16_t shutter;
    uint8_t  quality;
    int8_t   sharpness;
    int8_t   contrast;
    uint8_t  brightness;
    int8_t   saturation;
    uint8_t  exposure;
} camcfg_t;

/**
 * @brief RPiを扱います。
 */
class RPi : public Device {
public:
    RPi(RawSerial *_serial);
    ~RPi();

    int16_t temp; /**< 温度 [C] */

    /**
     * @brief RPiのシャットダウンを行います。
     * @details コマンド CMD_KILL_RPI によって呼び出されます。
     */
    uint8_t kill(uint8_t *result);

    /**
     * @brief コマンド CMD_TAKE_PIC 画像の撮影
     */
    uint8_t take_pic(uint16_t interval, uint16_t duration, uint32_t start,
                     uint8_t *result);

    /**
     * @brief コマンド CMD_GET_PIC_LIST 画像IDのリストの取得
     * @param start: リストの開始番号
     * @param end: リストの終了番号
     * @param list: RasPiから返ってくるID
     */
    uint8_t get_pic_list(uint16_t start, uint16_t end, uint32_t *list,
                         uint16_t &count, uint8_t &result);

    /**
     * @brief コマンド CMD_GET_PIC_INFO 画像情報の取得
     */
    uint8_t get_pic_info(uint16_t id, uint16_t sub_id, uint8_t type,
                         PicInfo *info, uint16_t &count, uint8_t &result);
    /**
     * @brief コマンド CMD_GET_PIC_DATA 画像データの取得
     */
    uint8_t get_pic_data(uint8_t type, uint16_t id, uint16_t sub_id,
                         uint32_t pos, uint16_t size, uint8_t data[],
                         uint16_t *data_size);

    /**
     * @brief コマンド CMD_DELETE_PIC 画像の削除
     */
    uint8_t delete_pic(uint16_t id, uint16_t sub_id, uint8_t mode,
                       uint8_t *result);

    /**
     * @brief コマンド CMD_SET_PIC_SIZE 画像サイズの設定
     */
    uint8_t set_pic_size(uint16_t width, uint16_t height, uint8_t *result);

    /**
     * @brief コマンド CMD_SET_COMP_PIC 縮小画像の設定
     */
    uint8_t set_pic_compression(uint16_t width, uint16_t height,
                                uint8_t *result);

    /**
     * @brief コマンド CMD_SET_CAM_CONFIG カメラ設定の取得
     */
    uint8_t get_cam_config(camcfg_t *cfg);

    /**
     * @brief コマンド CMD_SET_CAM_CONFIG カメラ設定の変更
     */
    uint8_t set_cam_config(camcfg_t *cfg);

    /**
     * @brief コマンド CMD_SEND_FILE 地上からRasPiへファイルを送信する
     * @param filename: .
     * @param pos: .
     * @param size: .
     * @param data: .
     * @param ret: .
     */
    uint8_t uplink_file(char *filename, uint32_t pos, uint16_t size,
                        char *data);

    /**
     * @brief コマンド CMD_GET_FILE 指定されたファイルを地上がRasPiから取得する
     * @param filepath
     * @param pos
     * @param size
     * @param data
     * @param data_size
     */
    uint8_t downlink_file(char *filepath, uint32_t pos, uint16_t size,
                          uint8_t *data, uint16_t *data_size);

    /**
     * @brief コマンド CMD_START_PIC_CATEG 画像分類開始/中止
     */
    uint8_t start_pic_categ(uint8_t work_type, uint8_t is_all, uint8_t do_wait,
                            uint8_t *exe_result, uint8_t *cmd_result);

    /**
     * @brief コマンド CMD_CHECK_PIC_CATEG 画像分類状況確認
     */
    uint8_t check_pic_categ(uint8_t *exe_result, uint8_t *cmd_result);

    /**
     * @brief コマンド CMD_ABORT_PIC_CATEG 画像分類強制終了
     */
    uint8_t abort_pic_categ(uint8_t *exe_result, uint8_t *cmd_result);

    /**
     * @brief コマンド CMD_GET_PIC_CATEG 画像分類の取得
     * @param work_type: 1 for "tf", 2 for "no"
     */
    uint8_t get_pic_categ(uint8_t work_type, uint16_t id, uint16_t sub_id,
                          uint8_t *exe_result, uint8_t *cmd_result,
                          uint8_t *categ, uint8_t *percent);

    /**
     * @brief コマンド CMD_GET_GOOD_PIC_IDS
     * 画像分類の結果、goodと判定されたidを返す
     * @param ids: goodと判定されたid
     */
    uint8_t get_good_pic_ids(uint8_t *exe_result, uint8_t *cmd_result,
                             uint8_t *count, uint16_t ids[],
                             uint16_t sub_ids[]);

    /**
     * @brief コマンド CMD_EXEC_SHELL 任意シェル実行
     * @param cmd: コマンド
     * @param msglen: 受信する文字列割り当て領域のサイズ
     * @param msg: RasPiから受信する文字列
     */
    uint8_t exec_shell(char *cmd, uint16_t msglen, char *msg);

    /**
     * @brief RPiの温度を取得します。
     */
    uint8_t get_temp(void);

    /**
     * @brief RPiとの通信テストを行います。
     * @return 通信の成否
     */
    uint8_t comm_check(void);

    int _shutdown(void);

    int  wakeup(void) override;
    int  shutdown(void) override;
    void monitor(void) override;

private:
    Timer *    timer;
    RawSerial *serial;

    /**
     * @brief RPiから送信されたデータを取得します。
     * @param dat: 受信データを格納するための配列
     * @param len: 受信するバイト数
     * @return 通信の成否
     */
    uint8_t readline(char dat[], uint16_t len);

    /**
     * @brief RPiからデータを受信します。
     * @param dat: 受信データを格納するための配列
     * @param len: 受信するバイト数
     * @return 通信の成否
     */
    uint8_t receive(char dat[], uint16_t len);

    /**
     * @brief RPiへデータを送信します。
     * @param dat: 送信データ
     * @return 通信の成否
     */
    uint8_t send(char dat[]);

    /**
     * @brief RPiとデータを送受信します。
     * @param [inout] dat: 送受信データ
     * @param len: 受信するバイト数
     * @return 通信の成否
     */
    uint8_t communicate(char dat[], uint16_t len);
};

#endif
