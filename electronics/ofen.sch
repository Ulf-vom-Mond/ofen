EESchema Schematic File Version 4
EELAYER 30 0
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
L Device:Q_NPN_CBE Q1
U 1 1 5FF22A9D
P 1100 3500
F 0 "Q1" H 1291 3546 50  0000 L CNN
F 1 "Q_NPN_CBE" H 1291 3455 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92L_Inline_Wide" H 1300 3600 50  0001 C CNN
F 3 "~" H 1100 3500 50  0001 C CNN
	1    1100 3500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 5FF24607
P 1200 2950
F 0 "R1" H 1270 2996 50  0000 L CNN
F 1 "6,8k" H 1270 2905 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 1130 2950 50  0001 C CNN
F 3 "~" H 1200 2950 50  0001 C CNN
	1    1200 2950
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 5FF363B1
P 1550 3200
F 0 "R3" V 1343 3200 50  0000 C CNN
F 1 "22k" V 1434 3200 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 1480 3200 50  0001 C CNN
F 3 "~" H 1550 3200 50  0001 C CNN
	1    1550 3200
	0    1    1    0   
$EndComp
$Comp
L Device:R R2
U 1 1 5FF36A96
P 1200 4050
F 0 "R2" H 1270 4096 50  0000 L CNN
F 1 "1k" H 1270 4005 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 1130 4050 50  0001 C CNN
F 3 "~" H 1200 4050 50  0001 C CNN
	1    1200 4050
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 5FF37689
P 1800 4050
F 0 "R4" H 1870 4096 50  0000 L CNN
F 1 "22k" H 1870 4005 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 1730 4050 50  0001 C CNN
F 3 "~" H 1800 4050 50  0001 C CNN
	1    1800 4050
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NPN_CBE Q2
U 1 1 5FF37B26
P 2100 3500
F 0 "Q2" H 2291 3546 50  0000 L CNN
F 1 "Q_NPN_CBE" H 2291 3455 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92L_Inline_Wide" H 2300 3600 50  0001 C CNN
F 3 "~" H 2100 3500 50  0001 C CNN
	1    2100 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1200 3100 1200 3200
Wire Wire Line
	1400 3200 1200 3200
Connection ~ 1200 3200
Wire Wire Line
	1200 3200 1200 3300
Wire Wire Line
	1200 3900 1200 3800
Wire Wire Line
	1200 3800 2200 3800
Wire Wire Line
	2200 3800 2200 3700
Connection ~ 1200 3800
Wire Wire Line
	1200 3800 1200 3700
Wire Wire Line
	1900 3500 1800 3500
Wire Wire Line
	1800 3500 1800 3200
Wire Wire Line
	1800 3200 1700 3200
Wire Wire Line
	1800 3500 1800 3900
Connection ~ 1800 3500
$Comp
L Device:R R5
U 1 1 5FF3A9E2
P 2200 2950
F 0 "R5" H 2270 2996 50  0000 L CNN
F 1 "33k" H 2270 2905 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 2130 2950 50  0001 C CNN
F 3 "~" H 2200 2950 50  0001 C CNN
	1    2200 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2200 3100 2200 3200
$Comp
L power:GND #PWR01
U 1 1 5FF3D50C
P 1200 4300
F 0 "#PWR01" H 1200 4050 50  0001 C CNN
F 1 "GND" H 1205 4127 50  0000 C CNN
F 2 "" H 1200 4300 50  0001 C CNN
F 3 "" H 1200 4300 50  0001 C CNN
	1    1200 4300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 5FF3DBB3
P 1800 4300
F 0 "#PWR02" H 1800 4050 50  0001 C CNN
F 1 "GND" H 1805 4127 50  0000 C CNN
F 2 "" H 1800 4300 50  0001 C CNN
F 3 "" H 1800 4300 50  0001 C CNN
	1    1800 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	1200 4300 1200 4200
Wire Wire Line
	1800 4300 1800 4200
$Comp
L Device:Q_NPN_CBE Q3
U 1 1 5FF44C67
P 3100 4000
F 0 "Q3" H 3291 4046 50  0000 L CNN
F 1 "Q_NPN_CBE" H 3291 3955 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92L_Inline_Wide" H 3300 4100 50  0001 C CNN
F 3 "~" H 3100 4000 50  0001 C CNN
	1    3100 4000
	1    0    0    -1  
$EndComp
$Comp
L Device:R R6
U 1 1 5FF44C6D
P 3200 3450
F 0 "R6" H 3270 3496 50  0000 L CNN
F 1 "6,8k" H 3270 3405 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 3130 3450 50  0001 C CNN
F 3 "~" H 3200 3450 50  0001 C CNN
	1    3200 3450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R8
U 1 1 5FF44C73
P 3550 3700
F 0 "R8" V 3343 3700 50  0000 C CNN
F 1 "22k" V 3434 3700 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 3480 3700 50  0001 C CNN
F 3 "~" H 3550 3700 50  0001 C CNN
	1    3550 3700
	0    1    1    0   
$EndComp
$Comp
L Device:R R7
U 1 1 5FF44C79
P 3200 4550
F 0 "R7" H 3270 4596 50  0000 L CNN
F 1 "1k" H 3270 4505 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 3130 4550 50  0001 C CNN
F 3 "~" H 3200 4550 50  0001 C CNN
	1    3200 4550
	1    0    0    -1  
$EndComp
$Comp
L Device:R R9
U 1 1 5FF44C7F
P 3800 4550
F 0 "R9" H 3870 4596 50  0000 L CNN
F 1 "22k" H 3870 4505 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 3730 4550 50  0001 C CNN
F 3 "~" H 3800 4550 50  0001 C CNN
	1    3800 4550
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NPN_CBE Q4
U 1 1 5FF44C85
P 4100 4000
F 0 "Q4" H 4291 4046 50  0000 L CNN
F 1 "Q_NPN_CBE" H 4291 3955 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92L_Inline_Wide" H 4300 4100 50  0001 C CNN
F 3 "~" H 4100 4000 50  0001 C CNN
	1    4100 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	3200 3600 3200 3700
Wire Wire Line
	3400 3700 3200 3700
Connection ~ 3200 3700
Wire Wire Line
	3200 3700 3200 3800
Wire Wire Line
	3200 4400 3200 4300
Wire Wire Line
	3200 4300 4200 4300
Wire Wire Line
	4200 4300 4200 4200
Connection ~ 3200 4300
Wire Wire Line
	3200 4300 3200 4200
Wire Wire Line
	3900 4000 3800 4000
Wire Wire Line
	3800 4000 3800 3700
Wire Wire Line
	3800 3700 3700 3700
Wire Wire Line
	3800 4000 3800 4400
Connection ~ 3800 4000
$Comp
L Device:R R10
U 1 1 5FF44C99
P 4200 3450
F 0 "R10" H 4270 3496 50  0000 L CNN
F 1 "33k" H 4270 3405 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 4130 3450 50  0001 C CNN
F 3 "~" H 4200 3450 50  0001 C CNN
	1    4200 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 3600 4200 3700
$Comp
L power:GND #PWR03
U 1 1 5FF44CA0
P 3200 4800
F 0 "#PWR03" H 3200 4550 50  0001 C CNN
F 1 "GND" H 3205 4627 50  0000 C CNN
F 2 "" H 3200 4800 50  0001 C CNN
F 3 "" H 3200 4800 50  0001 C CNN
	1    3200 4800
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 5FF44CA6
P 3800 4800
F 0 "#PWR04" H 3800 4550 50  0001 C CNN
F 1 "GND" H 3805 4627 50  0000 C CNN
F 2 "" H 3800 4800 50  0001 C CNN
F 3 "" H 3800 4800 50  0001 C CNN
	1    3800 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	3200 4800 3200 4700
Wire Wire Line
	3800 4800 3800 4700
Wire Wire Line
	4200 3700 4700 3700
Connection ~ 4200 3700
Wire Wire Line
	4200 3700 4200 3800
Wire Wire Line
	4700 2300 4600 2300
Wire Wire Line
	4200 2300 4200 3300
Wire Wire Line
	3200 2300 3200 3300
Connection ~ 4200 2300
Wire Wire Line
	4500 3200 4500 3600
Wire Wire Line
	4500 3600 4700 3600
Connection ~ 2200 3200
Wire Wire Line
	2200 3200 2200 3300
Wire Wire Line
	2200 2300 2200 2800
Wire Wire Line
	3200 2300 4200 2300
Wire Wire Line
	2200 2300 1200 2300
Wire Wire Line
	1200 2300 1200 2800
Connection ~ 2200 2300
$Comp
L Device:CP C1
U 1 1 5FF70A39
P 5800 3950
F 0 "C1" H 5918 3996 50  0000 L CNN
F 1 "22µ" H 5918 3905 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D6.3mm_P2.50mm" H 5838 3800 50  0001 C CNN
F 3 "~" H 5800 3950 50  0001 C CNN
	1    5800 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 3700 5800 3700
Wire Wire Line
	5800 3700 5800 3800
$Comp
L power:GND #PWR011
U 1 1 5FF779CB
P 5800 4200
F 0 "#PWR011" H 5800 3950 50  0001 C CNN
F 1 "GND" H 5805 4027 50  0000 C CNN
F 2 "" H 5800 4200 50  0001 C CNN
F 3 "" H 5800 4200 50  0001 C CNN
	1    5800 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	5800 4100 5800 4200
$Comp
L power:+3.3V #PWR08
U 1 1 5FF9FC78
P 4600 2200
F 0 "#PWR08" H 4600 2050 50  0001 C CNN
F 1 "+3.3V" H 4615 2373 50  0000 C CNN
F 2 "" H 4600 2200 50  0001 C CNN
F 3 "" H 4600 2200 50  0001 C CNN
	1    4600 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 2300 4600 2200
Connection ~ 4600 2300
Wire Wire Line
	4600 2300 4200 2300
$Comp
L power:GND #PWR07
U 1 1 5FFAF5E1
P 4500 2500
F 0 "#PWR07" H 4500 2250 50  0001 C CNN
F 1 "GND" H 4505 2327 50  0000 C CNN
F 2 "" H 4500 2500 50  0001 C CNN
F 3 "" H 4500 2500 50  0001 C CNN
	1    4500 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 2400 4500 2500
$Comp
L power:GND #PWR010
U 1 1 5FFB5847
P 5200 5200
F 0 "#PWR010" H 5200 4950 50  0001 C CNN
F 1 "GND" H 5205 5027 50  0000 C CNN
F 2 "" H 5200 5200 50  0001 C CNN
F 3 "" H 5200 5200 50  0001 C CNN
	1    5200 5200
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR09
U 1 1 5FFB6940
P 5200 4200
F 0 "#PWR09" H 5200 4050 50  0001 C CNN
F 1 "+3.3V" H 5215 4373 50  0000 C CNN
F 2 "" H 5200 4200 50  0001 C CNN
F 3 "" H 5200 4200 50  0001 C CNN
	1    5200 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	5200 4200 5200 4300
Wire Wire Line
	5200 4600 5200 4700
Wire Wire Line
	5200 5100 5200 5200
Wire Wire Line
	2900 4000 2800 4000
Wire Wire Line
	900  3500 800  3500
Wire Wire Line
	5200 4700 4600 4700
Connection ~ 5200 4700
Wire Wire Line
	5200 4700 5200 4800
Wire Wire Line
	6200 3500 5700 3500
Wire Wire Line
	6100 3600 5700 3600
Wire Wire Line
	6200 5600 6200 3500
Wire Wire Line
	6100 3600 6100 5500
Wire Wire Line
	800  3500 800  5600
Wire Wire Line
	2900 5500 2900 5700
Wire Wire Line
	6100 5500 4600 5500
Wire Wire Line
	3000 5600 6200 5600
Wire Wire Line
	3000 5700 3000 5600
Wire Wire Line
	4600 4700 4600 5500
Wire Wire Line
	2700 5600 2700 5700
$Comp
L Connector_Generic:Conn_01x04 J1
U 1 1 5FF7D88D
P 2900 5900
F 0 "J1" V 2772 6080 50  0000 L CNN
F 1 "motor" V 2863 6080 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 2900 5900 50  0001 C CNN
F 3 "~" H 2900 5900 50  0001 C CNN
	1    2900 5900
	0    1    1    0   
$EndComp
Wire Wire Line
	2800 4000 2800 5700
Wire Wire Line
	2900 5500 4600 5500
Connection ~ 4600 5500
Wire Wire Line
	2600 1600 2600 1500
Wire Wire Line
	2600 1500 1700 1500
Wire Wire Line
	2200 1600 2200 1400
Wire Wire Line
	2200 1400 1700 1400
Wire Wire Line
	1800 1600 1800 1300
Wire Wire Line
	1800 1300 1700 1300
$Comp
L power:GND #PWR015
U 1 1 60030B1D
P 2600 2000
F 0 "#PWR015" H 2600 1750 50  0001 C CNN
F 1 "GND" H 2605 1827 50  0000 C CNN
F 2 "" H 2600 2000 50  0001 C CNN
F 3 "" H 2600 2000 50  0001 C CNN
	1    2600 2000
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR018
U 1 1 60031516
P 2200 2000
F 0 "#PWR018" H 2200 1750 50  0001 C CNN
F 1 "GND" H 2205 1827 50  0000 C CNN
F 2 "" H 2200 2000 50  0001 C CNN
F 3 "" H 2200 2000 50  0001 C CNN
	1    2200 2000
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR021
U 1 1 60031896
P 1800 2000
F 0 "#PWR021" H 1800 1750 50  0001 C CNN
F 1 "GND" H 1805 1827 50  0000 C CNN
F 2 "" H 1800 2000 50  0001 C CNN
F 3 "" H 1800 2000 50  0001 C CNN
	1    1800 2000
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1800 2000 1800 1900
Wire Wire Line
	2200 2000 2200 1900
Wire Wire Line
	2600 2000 2600 1900
$Comp
L power:GND #PWR022
U 1 1 60037896
P 1200 2000
F 0 "#PWR022" H 1200 1750 50  0001 C CNN
F 1 "GND" H 1205 1827 50  0000 C CNN
F 2 "" H 1200 2000 50  0001 C CNN
F 3 "" H 1200 2000 50  0001 C CNN
	1    1200 2000
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1200 2000 1200 1500
Wire Wire Line
	1200 1500 1300 1500
Wire Wire Line
	1200 1500 1200 1400
Wire Wire Line
	1200 1400 1300 1400
Connection ~ 1200 1500
Wire Wire Line
	1200 1400 1200 1300
Wire Wire Line
	1200 1300 1300 1300
Connection ~ 1200 1400
Wire Wire Line
	2200 2300 3200 2300
Connection ~ 3200 2300
Wire Wire Line
	2200 3200 4500 3200
Wire Wire Line
	800  5600 2700 5600
$Comp
L power:+5V #PWR05
U 1 1 600EFA36
P 4200 1500
F 0 "#PWR05" H 4200 1350 50  0001 C CNN
F 1 "+5V" H 4215 1673 50  0000 C CNN
F 2 "" H 4200 1500 50  0001 C CNN
F 3 "" H 4200 1500 50  0001 C CNN
	1    4200 1500
	1    0    0    -1  
$EndComp
$Comp
L power:-5V #PWR06
U 1 1 600EFFDA
P 4200 1900
F 0 "#PWR06" H 4200 2000 50  0001 C CNN
F 1 "-5V" H 4215 2073 50  0000 C CNN
F 2 "" H 4200 1900 50  0001 C CNN
F 3 "" H 4200 1900 50  0001 C CNN
	1    4200 1900
	1    0    0    1   
$EndComp
Wire Wire Line
	4000 1600 4200 1600
Wire Wire Line
	4200 1600 4200 1500
Wire Wire Line
	4000 1700 4400 1700
Wire Wire Line
	4400 1700 4400 2400
Wire Wire Line
	4400 2400 4500 2400
Connection ~ 4500 2400
Wire Wire Line
	4200 1600 5800 1600
Wire Wire Line
	5800 2300 5700 2300
Connection ~ 4200 1600
$Comp
L Connector_Generic:Conn_01x03 J2
U 1 1 6011AE21
P 3800 1700
F 0 "J2" H 3718 2017 50  0000 C CNN
F 1 "DC" H 3718 1926 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 3800 1700 50  0001 C CNN
F 3 "~" H 3800 1700 50  0001 C CNN
	1    3800 1700
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5800 1600 5800 2300
Wire Wire Line
	4000 1800 4200 1800
Wire Wire Line
	4200 1800 4200 1900
$Comp
L Device:R R14
U 1 1 6015686F
P 6700 5250
F 0 "R14" H 6630 5204 50  0000 R CNN
F 1 "47k" H 6630 5295 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 6630 5250 50  0001 C CNN
F 3 "~" H 6700 5250 50  0001 C CNN
	1    6700 5250
	1    0    0    1   
$EndComp
$Comp
L Device:R R16
U 1 1 60156DB0
P 7500 5850
F 0 "R16" H 7430 5804 50  0000 R CNN
F 1 "2,7k" H 7430 5895 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 7430 5850 50  0001 C CNN
F 3 "~" H 7500 5850 50  0001 C CNN
	1    7500 5850
	1    0    0    1   
$EndComp
$Comp
L power:GND #PWR017
U 1 1 60162FC8
P 7500 6100
F 0 "#PWR017" H 7500 5850 50  0001 C CNN
F 1 "GND" H 7505 5927 50  0000 C CNN
F 2 "" H 7500 6100 50  0001 C CNN
F 3 "" H 7500 6100 50  0001 C CNN
	1    7500 6100
	-1   0    0    -1  
$EndComp
$Comp
L power:-5V #PWR014
U 1 1 60164767
P 7200 5300
F 0 "#PWR014" H 7200 5400 50  0001 C CNN
F 1 "-5V" H 7215 5473 50  0000 C CNN
F 2 "" H 7200 5300 50  0001 C CNN
F 3 "" H 7200 5300 50  0001 C CNN
	1    7200 5300
	-1   0    0    1   
$EndComp
$Comp
L power:+5V #PWR013
U 1 1 60167835
P 7200 4400
F 0 "#PWR013" H 7200 4250 50  0001 C CNN
F 1 "+5V" H 7215 4573 50  0000 C CNN
F 2 "" H 7200 4400 50  0001 C CNN
F 3 "" H 7200 4400 50  0001 C CNN
	1    7200 4400
	-1   0    0    -1  
$EndComp
Wire Wire Line
	7200 4600 7200 4500
Wire Wire Line
	7500 6100 7500 6000
Wire Wire Line
	7500 5000 7400 5000
Wire Wire Line
	6700 4900 6800 4900
Wire Wire Line
	7500 5000 7500 5300
Wire Wire Line
	7500 5700 7500 5600
Connection ~ 7500 5600
Wire Wire Line
	7200 5300 7200 5200
Wire Wire Line
	6700 4900 6700 5100
Wire Wire Line
	6700 5400 6700 5600
Wire Wire Line
	7500 5600 6700 5600
$Comp
L Device:Thermocouple TC1
U 1 1 601AFB7C
P 7900 4900
F 0 "TC1" H 7873 5225 50  0000 C CNN
F 1 "Thermocouple" H 7873 5134 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 7325 4950 50  0001 C CNN
F 3 "~" H 7325 4950 50  0001 C CNN
	1    7900 4900
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR019
U 1 1 601B14BE
P 7700 6100
F 0 "#PWR019" H 7700 5850 50  0001 C CNN
F 1 "GND" H 7705 5927 50  0000 C CNN
F 2 "" H 7700 6100 50  0001 C CNN
F 3 "" H 7700 6100 50  0001 C CNN
	1    7700 6100
	-1   0    0    -1  
$EndComp
Wire Wire Line
	7700 6100 7700 5000
Wire Wire Line
	7700 5000 7800 5000
Wire Wire Line
	7800 4800 7600 4800
Wire Wire Line
	6700 3400 5700 3400
Connection ~ 6700 4900
Wire Wire Line
	6700 3400 6700 4900
$Comp
L Relay_SolidState:MOC3041M U2
U 1 1 6022B679
P 6900 1200
F 0 "U2" H 6900 1525 50  0000 C CNN
F 1 "MOC3041M" H 6900 1434 50  0000 C CNN
F 2 "Package_DIP:DIP-6_W7.62mm_LongPads" H 6700 1000 50  0001 L CIN
F 3 "https://www.onsemi.com/pub/Collateral/MOC3043M-D.pdf" H 6900 1200 50  0001 L CNN
	1    6900 1200
	1    0    0    -1  
$EndComp
$Comp
L Triac_Thyristor:BTA16-600B Q5
U 1 1 6022EB07
P 7800 1350
F 0 "Q5" H 7929 1396 50  0000 L CNN
F 1 "BTA16-600B" H 7929 1305 50  0000 L CNN
F 2 "Connector_Wire:SolderWire-0.75sqmm_1x03_P4.8mm_D1.25mm_OD2.3mm" H 8000 1275 50  0001 L CIN
F 3 "https://www.st.com/resource/en/datasheet/bta16.pdf" H 7800 1350 50  0001 L CNN
	1    7800 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R15
U 1 1 60231050
P 7450 1100
F 0 "R15" V 7243 1100 50  0000 C CNN
F 1 "330" V 7334 1100 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 7380 1100 50  0001 C CNN
F 3 "~" H 7450 1100 50  0001 C CNN
	1    7450 1100
	0    1    1    0   
$EndComp
$Comp
L Device:R R17
U 1 1 602343EC
P 7550 1700
F 0 "R17" V 7343 1700 50  0000 C CNN
F 1 "330" V 7434 1700 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 7480 1700 50  0001 C CNN
F 3 "~" H 7550 1700 50  0001 C CNN
	1    7550 1700
	0    1    1    0   
$EndComp
$Comp
L Device:R R13
U 1 1 602375E1
P 6200 1350
F 0 "R13" H 6270 1396 50  0000 L CNN
F 1 "220" H 6270 1305 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 6130 1350 50  0001 C CNN
F 3 "~" H 6200 1350 50  0001 C CNN
	1    6200 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C6
U 1 1 602386E2
P 8600 1450
F 0 "C6" H 8715 1496 50  0000 L CNN
F 1 "10n" H 8715 1405 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.8mm_W2.6mm_P2.50mm" H 8638 1300 50  0001 C CNN
F 3 "~" H 8600 1450 50  0001 C CNN
	1    8600 1450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R18
U 1 1 6023B33E
P 8250 1100
F 0 "R18" V 8457 1100 50  0000 C CNN
F 1 "47" V 8366 1100 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 8180 1100 50  0001 C CNN
F 3 "~" H 8250 1100 50  0001 C CNN
	1    8250 1100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7200 1100 7300 1100
Wire Wire Line
	7600 1100 7800 1100
Wire Wire Line
	7800 1100 7800 1200
Wire Wire Line
	7200 1300 7300 1300
Wire Wire Line
	7300 1300 7300 1450
Wire Wire Line
	7300 1450 7650 1450
Wire Wire Line
	7300 1450 7300 1700
Wire Wire Line
	7300 1700 7400 1700
Connection ~ 7300 1450
Wire Wire Line
	7700 1700 7800 1700
Wire Wire Line
	7800 1700 7800 1500
Wire Wire Line
	7800 1700 8600 1700
Wire Wire Line
	8600 1700 8600 1600
Connection ~ 7800 1700
Wire Wire Line
	7800 1100 8000 1100
Connection ~ 7800 1100
Wire Wire Line
	8400 1100 8600 1100
Wire Wire Line
	8600 1100 8600 1300
Wire Wire Line
	6200 1200 6200 1100
Wire Wire Line
	6200 1100 6600 1100
$Comp
L power:GND #PWR012
U 1 1 602DC9A5
P 6500 1400
F 0 "#PWR012" H 6500 1150 50  0001 C CNN
F 1 "GND" H 6505 1227 50  0000 C CNN
F 2 "" H 6500 1400 50  0001 C CNN
F 3 "" H 6500 1400 50  0001 C CNN
	1    6500 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6500 1400 6500 1300
Wire Wire Line
	6500 1300 6600 1300
Wire Wire Line
	8000 800  8000 1100
Connection ~ 8000 1100
Wire Wire Line
	8000 1100 8100 1100
Wire Wire Line
	8600 1700 9100 1700
Connection ~ 8600 1700
$Comp
L Device:Heater R20
U 1 1 6031B0A5
P 9300 1700
F 0 "R20" H 9370 1746 50  0000 L CNN
F 1 "Heater" H 9370 1655 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_Altech_AK300-3_P5.00mm" V 9230 1700 50  0001 C CNN
F 3 "~" H 9300 1700 50  0001 C CNN
	1    9300 1700
	0    -1   -1   0   
$EndComp
$Comp
L Device:Transformer_1P_1S T1
U 1 1 60367066
P 9300 2200
F 0 "T1" V 9150 1850 50  0000 L CNN
F 1 "Transformer_1P_1S" V 9250 1200 50  0000 L CNN
F 2 "Connector_Wire:SolderWire-0.75sqmm_1x04_P4.8mm_D1.25mm_OD2.3mm" H 9300 2200 50  0001 C CNN
F 3 "~" H 9300 2200 50  0001 C CNN
	1    9300 2200
	0    1    1    0   
$EndComp
$Comp
L Device:Transformer_1P_1S T2
U 1 1 5FF7254B
P 9900 2200
F 0 "T2" V 9750 2450 50  0000 L CNN
F 1 "Transformer_1P_1S" V 9850 2450 50  0000 L CNN
F 2 "Connector_Wire:SolderWire-0.75sqmm_1x04_P4.8mm_D1.25mm_OD2.3mm" H 9900 2200 50  0001 C CNN
F 3 "~" H 9900 2200 50  0001 C CNN
	1    9900 2200
	0    1    1    0   
$EndComp
Wire Wire Line
	9100 1700 9150 1700
Connection ~ 9100 1700
Wire Wire Line
	9100 1700 9100 1800
Wire Wire Line
	9700 1700 9700 1800
$Comp
L Device:R R23
U 1 1 60009534
P 10400 3300
F 0 "R23" H 10470 3346 50  0000 L CNN
F 1 "3,3k" H 10470 3255 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 10330 3300 50  0001 C CNN
F 3 "~" H 10400 3300 50  0001 C CNN
	1    10400 3300
	1    0    0    -1  
$EndComp
$Comp
L Device:R R24
U 1 1 6000E066
P 10400 3800
F 0 "R24" H 10470 3846 50  0000 L CNN
F 1 "680" H 10470 3755 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 10330 3800 50  0001 C CNN
F 3 "~" H 10400 3800 50  0001 C CNN
	1    10400 3800
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR024
U 1 1 600101B9
P 10400 4150
F 0 "#PWR024" H 10400 3900 50  0001 C CNN
F 1 "GND" H 10405 3977 50  0000 C CNN
F 2 "" H 10400 4150 50  0001 C CNN
F 3 "" H 10400 4150 50  0001 C CNN
	1    10400 4150
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR023
U 1 1 600138D6
P 10400 3050
F 0 "#PWR023" H 10400 2900 50  0001 C CNN
F 1 "+3.3V" H 10550 3100 50  0000 C CNN
F 2 "" H 10400 3050 50  0001 C CNN
F 3 "" H 10400 3050 50  0001 C CNN
	1    10400 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	10400 3050 10400 3150
Wire Wire Line
	10400 3450 10400 3550
Wire Wire Line
	10400 3950 10400 4050
Wire Wire Line
	10400 3550 10100 3550
Connection ~ 10400 3550
Wire Wire Line
	10400 3550 10400 3650
Wire Wire Line
	10100 3550 9800 3550
Wire Wire Line
	9500 3050 9500 2600
Wire Wire Line
	5700 3100 7100 3100
Wire Wire Line
	7100 3100 7100 3800
Wire Wire Line
	7100 3800 9100 3800
Wire Wire Line
	5700 3200 7000 3200
Wire Wire Line
	7000 3200 7000 3900
Wire Wire Line
	7000 3900 9700 3900
Wire Wire Line
	9700 3900 9700 3350
$Comp
L Device:R R22
U 1 1 60071CDB
P 9900 2700
F 0 "R22" V 9800 2700 50  0000 C CNN
F 1 "5" V 9900 2700 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 9830 2700 50  0001 C CNN
F 3 "~" H 9900 2700 50  0001 C CNN
	1    9900 2700
	0    1    1    0   
$EndComp
Wire Wire Line
	10100 3550 10100 3350
Wire Wire Line
	10050 2700 10100 2700
Connection ~ 10100 2700
Wire Wire Line
	10100 2700 10100 2600
Wire Wire Line
	9750 2700 9700 2700
Connection ~ 9700 2700
Wire Wire Line
	9700 2700 9700 2600
Wire Wire Line
	5700 3300 6900 3300
Wire Wire Line
	6900 3300 6900 4000
Wire Wire Line
	6900 4000 9800 4000
Wire Wire Line
	9800 4000 9800 3550
$Comp
L Device:R R19
U 1 1 6010194C
P 9100 2850
F 0 "R19" H 9170 2896 50  0000 L CNN
F 1 "6,8k" H 9170 2805 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 9030 2850 50  0001 C CNN
F 3 "~" H 9100 2850 50  0001 C CNN
	1    9100 2850
	1    0    0    -1  
$EndComp
$Comp
L Device:R R21
U 1 1 60109E51
P 9300 3050
F 0 "R21" V 9200 3050 50  0000 C CNN
F 1 "240" V 9300 3050 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 9230 3050 50  0001 C CNN
F 3 "~" H 9300 3050 50  0001 C CNN
	1    9300 3050
	0    -1   -1   0   
$EndComp
Wire Wire Line
	9100 2600 9100 2700
Wire Wire Line
	9450 3050 9500 3050
Connection ~ 9500 3050
Wire Wire Line
	9150 3050 9100 3050
Wire Wire Line
	9100 3050 9100 3000
Wire Wire Line
	9100 3050 9100 3550
Connection ~ 9100 3050
$Comp
L Device:R R12
U 1 1 5FF9E194
P 5200 4950
F 0 "R12" H 5270 4996 50  0000 L CNN
F 1 "120" H 5270 4905 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 5130 4950 50  0001 C CNN
F 3 "~" H 5200 4950 50  0001 C CNN
	1    5200 4950
	1    0    0    -1  
$EndComp
$Comp
L Device:R R11
U 1 1 5FF9D680
P 5200 4450
F 0 "R11" H 5270 4496 50  0000 L CNN
F 1 "390" H 5270 4405 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 5130 4450 50  0001 C CNN
F 3 "~" H 5200 4450 50  0001 C CNN
	1    5200 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	9450 1700 9700 1700
Wire Wire Line
	9500 1800 9500 1600
Wire Wire Line
	9500 1600 10100 1600
Wire Wire Line
	10100 1600 10100 1800
$Comp
L Switch:SW_Push SW2
U 1 1 6027947B
P 1500 1300
F 0 "SW2" H 1500 1700 50  0000 C CNN
F 1 "SW_Push" H 1800 1700 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 1500 1500 50  0001 C CNN
F 3 "~" H 1500 1500 50  0001 C CNN
	1    1500 1300
	-1   0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW3
U 1 1 6027B7C7
P 1500 1400
F 0 "SW3" H 1500 1800 50  0000 C CNN
F 1 "SW_Push" H 1800 1800 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 1500 1600 50  0001 C CNN
F 3 "~" H 1500 1600 50  0001 C CNN
	1    1500 1400
	-1   0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW4
U 1 1 6027BD9C
P 1500 1500
F 0 "SW4" H 1500 1900 50  0000 C CNN
F 1 "SW_Push" H 1800 1900 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 1500 1700 50  0001 C CNN
F 3 "~" H 1500 1700 50  0001 C CNN
	1    1500 1500
	-1   0    0    -1  
$EndComp
Connection ~ 9500 1600
$Comp
L Device:R R25
U 1 1 60457D14
P 9900 2900
F 0 "R25" V 10000 2900 50  0000 C CNN
F 1 "5" V 9900 2900 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 9830 2900 50  0001 C CNN
F 3 "~" H 9900 2900 50  0001 C CNN
	1    9900 2900
	0    1    1    0   
$EndComp
Wire Wire Line
	9750 2900 9700 2900
Connection ~ 9700 2900
Wire Wire Line
	9700 2900 9700 2700
Wire Wire Line
	10050 2900 10100 2900
Connection ~ 10100 2900
Wire Wire Line
	10100 2900 10100 2700
$Comp
L Device:C C5
U 1 1 604A7238
P 9300 3550
F 0 "C5" V 9048 3550 50  0000 C CNN
F 1 "470n" V 9139 3550 50  0000 C CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W4.5mm_P5.00mm" H 9338 3400 50  0001 C CNN
F 3 "~" H 9300 3550 50  0001 C CNN
	1    9300 3550
	0    1    1    0   
$EndComp
Wire Wire Line
	9150 3550 9100 3550
Connection ~ 9100 3550
Wire Wire Line
	9100 3550 9100 3800
Wire Wire Line
	9500 3050 9500 3550
Wire Wire Line
	9500 3550 9450 3550
Wire Wire Line
	9800 3550 9500 3550
Connection ~ 9800 3550
Connection ~ 9500 3550
$Comp
L Device:C C7
U 1 1 604F2868
P 9900 3350
F 0 "C7" V 9648 3350 50  0000 C CNN
F 1 "470n" V 9739 3350 50  0000 C CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W4.5mm_P5.00mm" H 9938 3200 50  0001 C CNN
F 3 "~" H 9900 3350 50  0001 C CNN
	1    9900 3350
	0    1    1    0   
$EndComp
Connection ~ 10100 3550
Wire Wire Line
	10050 3350 10100 3350
Connection ~ 10100 3350
Wire Wire Line
	10100 3350 10100 2900
Wire Wire Line
	9750 3350 9700 3350
Connection ~ 9700 3350
Wire Wire Line
	9700 3350 9700 2900
$Comp
L Device:C C8
U 1 1 605692A9
P 10800 3800
F 0 "C8" H 10685 3754 50  0000 R CNN
F 1 "470n" H 10685 3845 50  0000 R CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W4.5mm_P5.00mm" H 10838 3650 50  0001 C CNN
F 3 "~" H 10800 3800 50  0001 C CNN
	1    10800 3800
	-1   0    0    1   
$EndComp
Wire Wire Line
	10800 3650 10800 3550
Wire Wire Line
	10800 3550 10400 3550
Wire Wire Line
	10800 4050 10800 3950
Wire Wire Line
	10800 4050 10400 4050
Connection ~ 10400 4050
Wire Wire Line
	10400 4050 10400 4150
$Comp
L Device:C C2
U 1 1 600EFE48
P 1800 1750
F 0 "C2" V 1548 1750 50  0000 C CNN
F 1 "22n" V 1639 1750 50  0000 C CNN
F 2 "Capacitor_THT:C_Disc_D4.3mm_W1.9mm_P5.00mm" H 1838 1600 50  0001 C CNN
F 3 "~" H 1800 1750 50  0001 C CNN
	1    1800 1750
	1    0    0    1   
$EndComp
$Comp
L Device:C C3
U 1 1 600F3BF9
P 2200 1750
F 0 "C3" V 1948 1750 50  0000 C CNN
F 1 "22n" V 2039 1750 50  0000 C CNN
F 2 "Capacitor_THT:C_Disc_D4.3mm_W1.9mm_P5.00mm" H 2238 1600 50  0001 C CNN
F 3 "~" H 2200 1750 50  0001 C CNN
	1    2200 1750
	1    0    0    1   
$EndComp
$Comp
L Device:C C4
U 1 1 600F400E
P 2600 1750
F 0 "C4" V 2348 1750 50  0000 C CNN
F 1 "22n" V 2439 1750 50  0000 C CNN
F 2 "Capacitor_THT:C_Disc_D4.3mm_W1.9mm_P5.00mm" H 2638 1600 50  0001 C CNN
F 3 "~" H 2600 1750 50  0001 C CNN
	1    2600 1750
	1    0    0    1   
$EndComp
Wire Wire Line
	4700 3300 4550 3300
Wire Wire Line
	4550 3300 4550 3000
Wire Wire Line
	4550 3000 3300 3000
Wire Wire Line
	3300 3000 3300 1500
Wire Wire Line
	3300 1500 2600 1500
Connection ~ 2600 1500
Wire Wire Line
	4700 3200 4600 3200
Wire Wire Line
	3400 1400 2200 1400
Connection ~ 2200 1400
Wire Wire Line
	4700 3100 4650 3100
Wire Wire Line
	3500 1300 1800 1300
Connection ~ 1800 1300
$Comp
L Connector:Screw_Terminal_01x03 J3
U 1 1 602C6C50
P 9800 900
F 0 "J3" H 9880 942 50  0000 L CNN
F 1 "AC_IN" H 9880 851 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_Altech_AK300-3_P5.00mm" H 9800 900 50  0001 C CNN
F 3 "~" H 9800 900 50  0001 C CNN
	1    9800 900 
	1    0    0    1   
$EndComp
Wire Wire Line
	8000 800  8700 800 
Wire Wire Line
	7000 2500 7000 2600
Wire Wire Line
	6900 2500 7000 2500
$Comp
L power:GND #PWR020
U 1 1 600486BE
P 7000 2600
F 0 "#PWR020" H 7000 2350 50  0001 C CNN
F 1 "GND" H 7005 2427 50  0000 C CNN
F 2 "" H 7000 2600 50  0001 C CNN
F 3 "" H 7000 2600 50  0001 C CNN
	1    7000 2600
	-1   0    0    -1  
$EndComp
$Comp
L Switch:SW_SPST SW1
U 1 1 60040B90
P 6700 2500
F 0 "SW1" H 6700 2735 50  0000 C CNN
F 1 "SW_SPST" H 6700 2644 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 6700 2500 50  0001 C CNN
F 3 "~" H 6700 2500 50  0001 C CNN
	1    6700 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	6500 2500 5700 2500
Wire Wire Line
	6200 1500 6200 2600
Wire Wire Line
	6200 2600 5700 2600
$Comp
L Device:R R26
U 1 1 60135609
P 7600 5050
F 0 "R26" H 7700 4900 50  0000 R CNN
F 1 "15k" V 7600 5125 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 7530 5050 50  0001 C CNN
F 3 "~" H 7600 5050 50  0001 C CNN
	1    7600 5050
	1    0    0    1   
$EndComp
Wire Wire Line
	7600 4900 7600 4800
Connection ~ 7600 4800
Wire Wire Line
	7600 4800 7400 4800
Wire Wire Line
	7600 5200 7600 5300
Wire Wire Line
	7600 5300 7500 5300
Connection ~ 7500 5300
Wire Wire Line
	7500 5300 7500 5600
NoConn ~ 4700 3400
NoConn ~ 4700 3500
$Comp
L Amplifier_Operational:LM6361 U3
U 1 1 6023E246
P 7100 4900
F 0 "U3" H 7150 5150 50  0000 C CNN
F 1 "LM6361" H 7250 5050 50  0000 C CNN
F 2 "Package_DIP:DIP-8_W7.62mm_Socket_LongPads" H 7150 5100 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lm6161.pdf" H 7250 5200 50  0001 C CNN
	1    7100 4900
	-1   0    0    -1  
$EndComp
$Comp
L Device:R_POT_TRIM RV1
U 1 1 60241F8D
P 6700 5950
F 0 "RV1" H 6600 5900 50  0000 R CNN
F 1 "10k" H 6600 6000 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 6700 5950 50  0001 C CNN
F 3 "~" H 6700 5950 50  0001 C CNN
	1    6700 5950
	-1   0    0    1   
$EndComp
Wire Wire Line
	7000 5200 7000 5700
Wire Wire Line
	7000 5700 6700 5700
Wire Wire Line
	6700 5700 6700 5800
Wire Wire Line
	6700 6100 6700 6200
Wire Wire Line
	6700 6200 7100 6200
Wire Wire Line
	7100 6200 7100 5200
Wire Wire Line
	6550 5950 6450 5950
Wire Wire Line
	6450 5950 6450 4500
Wire Wire Line
	6450 4500 7200 4500
Connection ~ 7200 4500
Wire Wire Line
	7200 4500 7200 4400
Wire Wire Line
	9500 1000 9600 1000
Wire Wire Line
	9500 1000 9500 1200
$Comp
L power:GND #PWR025
U 1 1 6032D73A
P 9300 1000
F 0 "#PWR025" H 9300 750 50  0001 C CNN
F 1 "GND" H 9305 827 50  0000 C CNN
F 2 "" H 9300 1000 50  0001 C CNN
F 3 "" H 9300 1000 50  0001 C CNN
	1    9300 1000
	1    0    0    -1  
$EndComp
Wire Wire Line
	9300 1000 9300 900 
Wire Wire Line
	4700 2400 5700 2400
Wire Wire Line
	4500 2400 4700 2400
Connection ~ 4700 2400
Wire Wire Line
	5700 2700 5700 2800
Connection ~ 5700 2800
Wire Wire Line
	5700 2800 5700 2900
Connection ~ 5700 2900
Wire Wire Line
	5700 2900 5700 3000
$Comp
L power:GND #PWR016
U 1 1 6034FD19
P 5900 2800
F 0 "#PWR016" H 5900 2550 50  0001 C CNN
F 1 "GND" H 5905 2627 50  0000 C CNN
F 2 "" H 5900 2800 50  0001 C CNN
F 3 "" H 5900 2800 50  0001 C CNN
	1    5900 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	5900 2800 5900 2700
Wire Wire Line
	5900 2700 5700 2700
Connection ~ 5700 2700
Wire Wire Line
	3500 2800 3500 1300
Wire Wire Line
	3400 2900 3400 1400
$Comp
L ofen-rescue:ESP32-Custom U1
U 1 1 5FF1B380
P 5200 3000
F 0 "U1" H 5200 3965 50  0000 C CNN
F 1 "ESP32" H 5200 3874 50  0000 C CNN
F 2 "custom:ESP32-DevModule" H 5200 3100 50  0001 C CNN
F 3 "" H 5200 3100 50  0001 C CNN
	1    5200 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 2800 3500 2800
Wire Wire Line
	4650 3100 4650 2800
Wire Wire Line
	4600 2900 3400 2900
Wire Wire Line
	4600 3200 4600 2900
Wire Wire Line
	4700 2400 4700 2500
Connection ~ 4700 2500
Wire Wire Line
	4700 2500 4700 2600
Connection ~ 4700 2600
Wire Wire Line
	4700 2600 4700 2700
$Comp
L Connector_Generic:Conn_01x02 J5
U 1 1 6038E74D
P 9800 1200
F 0 "J5" H 9880 1192 50  0000 L CNN
F 1 "AC_OUT" H 9880 1101 50  0000 L CNN
F 2 "Connector_Wire:SolderWire-0.75sqmm_1x02_P4.8mm_D1.25mm_OD2.3mm" H 9800 1200 50  0001 C CNN
F 3 "~" H 9800 1200 50  0001 C CNN
	1    9800 1200
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J4
U 1 1 60390C15
P 8900 1100
F 0 "J4" H 8980 1092 50  0000 L CNN
F 1 "AC_OUT" H 8980 1001 50  0000 L CNN
F 2 "Connector_Wire:SolderWire-0.75sqmm_1x02_P4.8mm_D1.25mm_OD2.3mm" H 8900 1100 50  0001 C CNN
F 3 "~" H 8900 1100 50  0001 C CNN
	1    8900 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	8700 1200 8700 1100
Wire Wire Line
	8700 1100 8700 800 
Connection ~ 8700 1100
Wire Wire Line
	9600 1300 9500 1300
Connection ~ 9500 1300
Wire Wire Line
	9500 1300 9500 1600
Wire Wire Line
	9600 1200 9500 1200
Connection ~ 9500 1200
Wire Wire Line
	9500 1200 9500 1300
Wire Wire Line
	9300 900  9600 900 
Wire Wire Line
	9600 800  8700 800 
Connection ~ 8700 800 
$EndSCHEMATC
