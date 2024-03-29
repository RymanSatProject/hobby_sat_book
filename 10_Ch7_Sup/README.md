# 第7章 補足説明

本フォルダ、第7章を進めるにあたって必要なソフトウェアの説明及び補足情報が入っています。

- 購入部品・工具リスト(代替部品記載有り): [こちら(Excel)](parts/RSP-01Lite_Parts_ToolList.xlsx)
- C系ソフトウェア: [sw-c.md](sw-c.md)
- T系ソフトウェア: [sw-t.md](sw-t.md)
- T系OBCとPCとの接続設定: [sw-t-pc.md](sw-t-pc.md)
- ターミナルについて: [sw-terminal.md](sw-terminal.md)
- Raspberry Pi Zeroの設定について: [pi-setting.md](pi-setting.md)
- コマンドリスト: [こちら(Excel)](design/RSP-01Lite_CommandList.xlsx)


## P144 図9でBMX055表示が出ない場合

### 1.STM32のリセットボタンを押す

STM32のB2（黒いボタン）を押すことでリセットがかかり、表示される場合があります。表示されない場合は配線が間違っている可能性があるので、もう一度配線を見直してください。


## P178 図66の組み上げ後、アームが動かない場合

### 1.モーターにコンデンサを追加する

モーターのノイズをI2Cが引き込んで動作しにくい場合があります。モーターに0.1μFのコンデンサを3個はんだ付けして見て下さい。参考写真[こちら](img/P178_fig66.jpg)

### 2.ギア、軸受けにグリスを塗布する

ギアボックスの各種ギアやシャフトの軸受けにグリスを塗布することで負荷が減ります。参考[タミヤ モリブデングリス](https://www.tamiya.com/japan/products/87022/index.html)

## P150 apt update で 404 Not Found が発生する場合
2023/10/7 時点で、``` apt update ``` を行うと以下のエラーメッセージが表示されます。

```
$ sudo apt update && sudo apt upgrade -y

Hit:1 http://archive.raspberrypi.org/debian stretch InRelease
Ign:2 http://raspbian.raspberrypi.org/raspbian stretch InRelease
Err:3 http://raspbian.raspberrypi.org/raspbian stretch Release
  404  Not Found
Reading package lists... Done
E: The repository 'http://raspbian.raspberrypi.org/raspbian stretch Release' does no longer have a Release file.
N: Updating from such a repository can't be done securely, and is therefore disabled by default.
N: See apt-secure(8) manpage for repository creation and user configuration details.
```

上記が発生した場合は、リポジトリの参照先を変更する必要があります。

```
# sources.list を開く
sudo vi /etc/apt/sources.list
```

以下のようにリポジトリの参照先を変更します。

```
#deb http://raspbian.raspberrypi.org/raspbian/ stretch main contrib non-free rpi
deb http://legacy.raspbian.org/raspbian/ stretch main contrib non-free rpi
```
