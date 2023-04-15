from reedsolo import *

from mission_main.communication import get_module_logger


class PacketWithECC:
    """
    誤り訂正符号付きパケットクラス

    Reed-Solomon符号を付与したパケット（バイナリ）を生成する。
    セッション無しでパケットを流し続ける簡易なプロトコルを想定。
    1パケットは最大255バイトまで（T系のバッファサイズにも依存するが）。
    地上局で誤り訂正を実施し、必要であればシーケンス番号から計算したデータの再送要求をコマンドで行う。
    """

    PACKET_SIZE = 253 # 255(TOBC packet) - 1(command number) - 1(CRC)
    RESULT_SIZE = 1 # 1(result)  MOBCで付加するresult
    HEADER_SIZE = 3 #  1(size) + 2(sequence)
    NSYM = 0  # number of ecc symbols
    PAYLOAD_SIZE = PACKET_SIZE - RESULT_SIZE - HEADER_SIZE - NSYM
    ADD_SEQ_NUM = False

    def __init__(self):
        self._logger = get_module_logger(__name__)
        self._rs = RSCodec(PacketWithECC.NSYM)  # 10 / 2 = 5 errors correction
        self._packet_with_ecc = None

        # Header
        self._payload_size = 0x00  # 1byte
        self._sequence = 0x0000  # 2byte location

        # Payload
        self._payload = [0]  # max 255byte - 8byte(header) / msg

    def _encode_packet(self):
        if PacketWithECC.ADD_SEQ_NUM:
            packet = [0] * PacketWithECC.HEADER_SIZE

            # header
            packet[0] = self._payload_size & 0xff
            packet[1] = (self._sequence >> 8) & 0xff
            packet[2] = self._sequence & 0xff

        else:
            packet = []

        packet.extend(self._payload)

        # add reed-solomon coding
        self._packet_with_ecc = self._rs.encode(packet)

    def decode_packet(self, packet_with_ecc):
        # TODO パケットのデータ有効性チェック
        # self._logger.debug(packet_with_ecc)
        self._packet_with_ecc = packet_with_ecc

        # RSCodec.decode()の戻り値は以下の通り。
        # ※Reedsolomonの符号が実データ末尾に付与されていない場合は、decoded_msgeccのみに実データが返却される
        #   decoded_msg       ：デコードされた (修正された) メッセージ
        #   decoded_msgecc    ：デコードされたメッセージとエラー訂正コード
        #   errata_pos        ：正誤表の位置のリスト
        decoded_msg, decoded_msgecc, errata_pos = self._rs.decode(packet_with_ecc)

        # NSYMが1以上の場合は、Reedsomon符号化あリと見なしdecoded_msgからメッセージを取得する
        msg = decoded_msgecc
        if PacketWithECC.NSYM > 0:
            msg = decoded_msg

        # 分割なし
        if not PacketWithECC.ADD_SEQ_NUM:
            self._payload = msg
            return self._payload

        # 分割あり
        self._payload_size = msg[0]             # 先頭はサイズ
        self._sequence = msg[1] << 8 + msg[2]   # 1,2は分割シーケンス
        self._payload = msg[3:]                 # 3以降がペイロード

        # ペイロードのサイズが異なる場合
        if self._payload_size != len(self._payload):
            self._logger.warn("payload length is not matched."
                          + "Expected " + str(self._payload_size) + ", but actual " + str(len(self._payload)))

        return self._payload

    def set_payload(self, payload):
        if len(payload) > PacketWithECC.PAYLOAD_SIZE and PacketWithECC.ADD_SEQ_NUM:
            raise ValueError('Too long payload: ' + str(len(payload)) + ' > ' + str(PacketWithECC.PAYLOAD_SIZE))
        self._payload = payload
        self._payload_size = len(payload)
        self._logger.debug("_payload_size = {}".format(self._payload_size))


    def get_payload(self):
        return self._payload

    def set_sequence(self, seq):
        self._sequence = seq

    def get_sequence(self):
        return self._sequence

    def get_packet_with_ecc(self):
        self._encode_packet()
        return self._packet_with_ecc

    @staticmethod
    def read_data(file, offset, read_len):
        total_read_len = 0
        packet = PacketWithECC()
        file.seek(offset)  # ファイルサイズ以上のoffsetの場合は何も読み込まれない

        if PacketWithECC.ADD_SEQ_NUM:
            # read_lenはファイルサイズ以上の値が指定される場合がある。
            # その場合は動作を優先し、ファイルの最後まで読んで返す。
            num_of_packet = int(read_len / PacketWithECC.PAYLOAD_SIZE)
            if read_len % PacketWithECC.PAYLOAD_SIZE != 0:
                num_of_packet += 1

            for seq in range(num_of_packet):
                read_len_of_seq = PacketWithECC.PAYLOAD_SIZE

                # 指定サイズの最後の端数分を読み込み
                if total_read_len + read_len_of_seq > read_len:
                    read_len_of_seq = read_len - total_read_len

                read_data = file.read(read_len_of_seq)

                # ファイル終端に到達したら終了する
                if not read_data:
                    break

                total_read_len += read_len_of_seq
                packet.set_payload(read_data)
                packet.set_sequence(seq)
                yield packet

        else:
            read_data = file.read(read_len)
            packet.set_payload(read_data)
            yield packet


    def write_data(self, file, write_data):
        self._logger.debug("write data")
        file.write(write_data)

    @staticmethod
    def split_packet(multi_packet):
        idx = 0
        while idx in range(len(multi_packet)):
            payload_size = multi_packet[idx]
            sequence = multi_packet[idx + 1] << 8 + multi_packet[idx + 2]
            payload = multi_packet[
                      idx + PacketWithECC.HEADER_SIZE: idx + PacketWithECC.HEADER_SIZE + payload_size + PacketWithECC.NSYM]

            encoded_packet = multi_packet[idx: idx + PacketWithECC.HEADER_SIZE + payload_size + PacketWithECC.NSYM]

            idx += PacketWithECC.HEADER_SIZE + payload_size + PacketWithECC.NSYM

            yield encoded_packet
