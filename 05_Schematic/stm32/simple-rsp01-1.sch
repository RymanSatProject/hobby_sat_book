EESchema Schematic File Version 4
LIBS:simple-rsp01-1-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector_Generic:Conn_02x19_Odd_Even J1
U 1 1 600BBF76
P 2250 3400
F 0 "J1" H 2300 4517 50  0000 C CNN
F 1 "Conn_02x19_Odd_Even" H 2300 4426 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_2x19_P2.54mm_Vertical" H 2250 3400 50  0001 C CNN
F 3 "~" H 2250 3400 50  0001 C CNN
	1    2250 3400
	1    0    0    -1  
$EndComp
$Comp
L Sensor_Temperature:LM35-LP U1
U 1 1 600BC07E
P 3650 3800
F 0 "U1" H 3320 3846 50  0000 R CNN
F 1 "LM35-LP" H 3320 3755 50  0000 R CNN
F 2 "Package_TO_SOT_THT:TO-92" H 3700 3550 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/lm35.pdf" H 3650 3800 50  0001 C CNN
	1    3650 3800
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2550 3800 3250 3800
$Comp
L power:GND #PWR0101
U 1 1 600BC694
P 3650 4200
F 0 "#PWR0101" H 3650 3950 50  0001 C CNN
F 1 "GND" H 3655 4027 50  0000 C CNN
F 2 "" H 3650 4200 50  0001 C CNN
F 3 "" H 3650 4200 50  0001 C CNN
	1    3650 4200
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR0102
U 1 1 600BC6DF
P 3650 3350
F 0 "#PWR0102" H 3650 3200 50  0001 C CNN
F 1 "+3.3V" H 3665 3523 50  0000 C CNN
F 2 "" H 3650 3350 50  0001 C CNN
F 3 "" H 3650 3350 50  0001 C CNN
	1    3650 3350
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 4100 3650 4200
Wire Wire Line
	3650 3350 3650 3500
$Comp
L power:GND #PWR0103
U 1 1 600BC738
P 2800 3450
F 0 "#PWR0103" H 2800 3200 50  0001 C CNN
F 1 "GND" H 2805 3277 50  0000 C CNN
F 2 "" H 2800 3450 50  0001 C CNN
F 3 "" H 2800 3450 50  0001 C CNN
	1    2800 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 3400 2800 3400
Wire Wire Line
	2800 3400 2800 3450
$Comp
L power:+3.3V #PWR0104
U 1 1 600BC767
P 2800 3150
F 0 "#PWR0104" H 2800 3000 50  0001 C CNN
F 1 "+3.3V" H 2815 3323 50  0000 C CNN
F 2 "" H 2800 3150 50  0001 C CNN
F 3 "" H 2800 3150 50  0001 C CNN
	1    2800 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 3200 2800 3200
Wire Wire Line
	2800 3200 2800 3150
$Comp
L Connector_Generic:Conn_02x19_Odd_Even J2
U 1 1 600BBFC2
P 5500 3400
F 0 "J2" H 5550 4517 50  0000 C CNN
F 1 "Conn_02x19_Odd_Even" H 5550 4426 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_2x19_P2.54mm_Vertical" H 5500 3400 50  0001 C CNN
F 3 "~" H 5500 3400 50  0001 C CNN
	1    5500 3400
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x06 J3
U 1 1 607BCAA8
P 8350 2550
F 0 "J3" H 8150 2150 50  0000 L CNN
F 1 "Conn_01x06" H 8150 2050 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Horizontal" H 8350 2550 50  0001 C CNN
F 3 "~" H 8350 2550 50  0001 C CNN
	1    8350 2550
	1    0    0    -1  
$EndComp
Text GLabel 5950 2600 2    50   Output ~ 0
SCL
Text GLabel 5950 2700 2    50   BiDi ~ 0
SDA
Text GLabel 5950 3400 2    50   Input ~ 0
FROM_ESP32_TXD
Text GLabel 5150 2600 0    50   Output ~ 0
TO_ESP32_RXD
Text GLabel 5950 3500 2    50   Output ~ 0
TO_Raspi_RXD
Text GLabel 2700 2500 2    50   Output ~ 0
ESP32_GPIO
Text GLabel 2700 3900 2    50   Input ~ 0
SLIDE_VOL
Wire Wire Line
	2550 3900 2700 3900
Text GLabel 5950 4100 2    50   Input ~ 0
FROM_Raspi_TX
$Comp
L power:GND #PWR0105
U 1 1 607BDAC0
P 7800 2050
F 0 "#PWR0105" H 7800 1800 50  0001 C CNN
F 1 "GND" H 7805 1877 50  0000 C CNN
F 2 "" H 7800 2050 50  0001 C CNN
F 3 "" H 7800 2050 50  0001 C CNN
	1    7800 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	7800 2050 8050 2050
Wire Wire Line
	8050 2050 8050 2350
Wire Wire Line
	8050 2350 8150 2350
$Comp
L power:+3.3V #PWR0106
U 1 1 607BDDA7
P 7900 2450
F 0 "#PWR0106" H 7900 2300 50  0001 C CNN
F 1 "+3.3V" H 7750 2500 50  0000 C CNN
F 2 "" H 7900 2450 50  0001 C CNN
F 3 "" H 7900 2450 50  0001 C CNN
	1    7900 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	7900 2450 8150 2450
Text GLabel 8000 2850 0    50   BiDi ~ 0
SDA
Text GLabel 8000 2750 0    50   Input ~ 0
SCL
Wire Wire Line
	8000 2650 8150 2650
Wire Wire Line
	8000 2550 8150 2550
Text GLabel 8000 2650 0    50   Input ~ 0
TO_ESP32_RXD
Text GLabel 8000 2550 0    50   Output ~ 0
FROM_ESP32_TXD
Wire Wire Line
	8000 2750 8150 2750
Wire Wire Line
	8000 2850 8150 2850
Text GLabel 8000 3400 0    50   Input ~ 0
ESP32_GPIO
Wire Wire Line
	8000 3900 8150 3900
Text GLabel 8000 4000 0    50   Input ~ 0
TO_Raspi_RXD
Text GLabel 8000 3900 0    50   Output ~ 0
FROM_Raspi_TX
Wire Wire Line
	5800 2600 5950 2600
Wire Wire Line
	5800 2700 5950 2700
Wire Wire Line
	8000 3800 8150 3800
Wire Wire Line
	2550 2500 2700 2500
Wire Wire Line
	8000 3400 8150 3400
Text GLabel 8000 3700 0    50   Output ~ 0
SLIDE_VOL
$Comp
L power:GND #PWR0107
U 1 1 6099A5D9
P 7250 3800
F 0 "#PWR0107" H 7250 3550 50  0001 C CNN
F 1 "GND" H 7255 3627 50  0000 C CNN
F 2 "" H 7250 3800 50  0001 C CNN
F 3 "" H 7250 3800 50  0001 C CNN
	1    7250 3800
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR0108
U 1 1 6099AAD7
P 7250 3500
F 0 "#PWR0108" H 7250 3350 50  0001 C CNN
F 1 "+3.3V" H 7100 3550 50  0000 C CNN
F 2 "" H 7250 3500 50  0001 C CNN
F 3 "" H 7250 3500 50  0001 C CNN
	1    7250 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 2600 5300 2600
Wire Wire Line
	5800 3400 5950 3400
Wire Wire Line
	7250 3500 8150 3500
Wire Wire Line
	7250 3600 8150 3600
Wire Wire Line
	7250 3600 7250 3800
Wire Wire Line
	8000 3700 8150 3700
Wire Notes Line
	8450 3500 8600 3500
Wire Notes Line
	8600 3500 8600 3700
Wire Notes Line
	8600 3700 8450 3700
Text Notes 8650 3650 0    50   ~ 0
To Slide Volume
Wire Notes Line
	8450 3800 8600 3800
Wire Notes Line
	8600 3800 8600 4000
Wire Notes Line
	8600 4000 8450 4000
Text Notes 8650 3900 0    50   ~ 0
To Raspi Board J1 1,2,3 pin
Text Notes 8650 3450 0    50   ~ 0
To ESP32 Board J2 3pin
Wire Notes Line
	8450 3400 8600 3400
Wire Notes Line
	8450 2350 8600 2350
Wire Notes Line
	8600 2350 8600 2850
Wire Notes Line
	8600 2850 8450 2850
Text Notes 8650 2650 0    50   ~ 0
To ESP32 Board J1
Wire Wire Line
	5800 3500 5950 3500
Wire Wire Line
	5800 4100 5950 4100
$Comp
L Connector_Generic:Conn_01x07 J4
U 1 1 60A9C57D
P 8350 3700
F 0 "J4" H 8300 3300 50  0000 L CNN
F 1 "Conn_01x07" H 8300 3200 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x07_P2.54mm_Horizontal" H 8350 3700 50  0001 C CNN
F 3 "~" H 8350 3700 50  0001 C CNN
	1    8350 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 4000 8150 4000
Text GLabel 8000 3800 0    50   Input ~ 0
Raspy_Pwrctl
Text GLabel 2700 4000 2    50   Output ~ 0
Raspy_Pwrctl
Wire Wire Line
	2550 4000 2700 4000
NoConn ~ 2050 2500
NoConn ~ 2050 2600
NoConn ~ 2050 2700
NoConn ~ 2050 2800
NoConn ~ 2050 2900
NoConn ~ 2050 3000
NoConn ~ 2050 3100
NoConn ~ 2050 3200
NoConn ~ 2050 3300
NoConn ~ 2050 3400
NoConn ~ 2550 3300
NoConn ~ 2550 3100
NoConn ~ 2550 3000
NoConn ~ 2550 2900
NoConn ~ 2550 2800
NoConn ~ 2550 2700
NoConn ~ 2550 2600
NoConn ~ 2550 3500
NoConn ~ 2550 3600
NoConn ~ 2550 3700
NoConn ~ 2550 4100
NoConn ~ 2550 4200
NoConn ~ 2550 4300
NoConn ~ 2050 4300
NoConn ~ 2050 4200
NoConn ~ 2050 4100
NoConn ~ 2050 4000
NoConn ~ 2050 3900
NoConn ~ 2050 3800
NoConn ~ 2050 3700
NoConn ~ 2050 3600
NoConn ~ 2050 3500
NoConn ~ 5300 2500
NoConn ~ 5800 2500
NoConn ~ 5800 2800
NoConn ~ 5800 2900
NoConn ~ 5300 2700
NoConn ~ 5300 2800
NoConn ~ 5300 2900
NoConn ~ 5300 3000
NoConn ~ 5300 3100
NoConn ~ 5300 3200
NoConn ~ 5800 3000
NoConn ~ 5800 3100
NoConn ~ 5800 3200
NoConn ~ 5800 3300
NoConn ~ 5300 3300
NoConn ~ 5300 3400
NoConn ~ 5300 3500
NoConn ~ 5300 3600
NoConn ~ 5300 3700
NoConn ~ 5300 3800
NoConn ~ 5300 3900
NoConn ~ 5300 4000
NoConn ~ 5300 4100
NoConn ~ 5300 4200
NoConn ~ 5300 4300
NoConn ~ 5800 4300
NoConn ~ 5800 4200
NoConn ~ 5800 4000
NoConn ~ 5800 3900
NoConn ~ 5800 3800
NoConn ~ 5800 3700
$EndSCHEMATC
