# Raspberry Pi Zeroの設定について

Raspberry Pi Zeroの設定に関する補足事項などについて説明します。

## 1. IPアドレス
### 1.1 静的IPアドレスの設定
書籍では、Raspberry Pi ZeroのIPアドレス設定はデフォルトであるdhcpとなっています。dhcpの場合、Windowsとのインターネット共有の際にインターネット接続が可能となるまで１、２分かかる場合があります。   
静的IPアドレスの設定を行うことで、Raspberry Pi Zeroの起動後、ほぼ即座にインターネット接続状態になります。

静的IPアドレスの設定は以下の手順で行います。

1. sshなどでRaspberry Pi Zeroに接続します。
2. 以下コマンドで /etc/dhcpcd.conf を開きます。
```bash
sudo vi /etc/dhcpcd.conf
```
3. 「Example static IP configuration:」の下、47行目周辺に以下を追記し、保存終了します。（vi の操作方法はここでは割愛します）

```
interface usb0
static ip_address=192.168.137.10
static routers=192.168.137.1
static domain_name_servers=8.8.8.8
```

※routersのアドレスは、Windows側のUSB EthernetデバイスのIPアドレスを設定する事になります。（デフォルトで192.168.137.1が設定されます）   
※DNSはGoogle社のDNSを指しています。

4. 以下コマンドで /etc/network/interfaces を開きます。
```bash
sudo vi /etc/network/interfaces
```

5. ファイル末尾に以下を追記し、保存終了します。
```
auto lo usb0
allow-hotplug usb0
iface usb0 inet manual
```

6. Raspberry Pi Zeroを以下コマンドで再起動します。
```
sudo reboot
```

7. 再起動後、以下コマンドでIPアドレスの設定状態を確認します。
```
ip a
```

以下のように usb0 のIPアドレスが 192.168.137.10/24 で表示されることを確認します。

```
pi@raspberrypi:~$ ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host
       valid_lft forever preferred_lft forever
2: usb0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP group default qlen 1000
    link/ether d6:ed:b2:e8:fc:2f brd ff:ff:ff:ff:ff:ff
    inet 192.168.137.10/24 brd 192.168.137.255 scope global usb0
       valid_lft forever preferred_lft forever
    inet6 fe80::d4ed:b2ff:fee8:fc2f/64 scope link
       valid_lft forever preferred_lft forever
```

8. pingでインターネット接続を確認します。
```
ping rymansat.com
```

以下のような応答が得られることを確認します。

```
64 bytes from www2944.sakura.ne.jp (49.212.198.184): icmp_seq=1 ttl=55 time=12.2 ms
64 bytes from www2944.sakura.ne.jp (49.212.198.184): icmp_seq=2 ttl=55 time=12.3 ms
```

## 2. インターネット接続
### 2.1 WindowsとRaspberry Piのインターネット接続の共有

インターネット接続の共有は、Windowsを再起動すると解除されます。   
再起動後もインターネット接続の共有を維持する場合は、以下の設定を行います。

1. Power Shell を管理者権限で起動します。
2. 以下のコマンドを実行します。
```
# レジストリにインターネット接続共有の永続化を設定する
New-ItemProperty -Path HKLM:\Software\Microsoft\Windows\CurrentVersion\SharedAccess -Name EnableRebootPersistConnection -Value 1 -PropertyType dword

# 「Internet Connection Sharing (ICS)」サービスを自動起動に変更する
Set-Service SharedAccess -StartupType Automatic -PassThru
```

