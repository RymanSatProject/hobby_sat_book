/**
 * @file
 * @brief
 */

// ラズパイからは最大219ファイル取得できるが、
// 251byte、パケット分割しない範囲で62ファイル分に制限する
constexpr uint16_t MAINOBC_PIC_LIST_MAXNUM = 62;
