cd /opt/rapidio/rapidio_sw/common/libmport
echo Read Tsi721 0x120
./riodp_test_misc -O 0x120
./riodp_test_misc -O 0x120 -w -V 0x3900
./riodp_test_misc -O 0x120
echo Read Tsi721 0x124
./riodp_test_misc -O 0x124
./riodp_test_misc -O 0x124 -w -V 0x390
./riodp_test_misc -O 0x124
echo Read Tsi721 0x158
./riodp_test_misc -O 0x158

echo Clear errors
./riodp_test_misc -O 0x148 -w -V 0
./riodp_test_misc -O 0x158 -w -V 0xFFFFFFFF
./riodp_test_misc -O 0x100E0 -w -V 0x2000044
echo Ackid status
./riodp_test_misc -O 0x148
echo Read Tsi721 0x144
./riodp_test_misc -O 0x144
echo Port status
./riodp_test_misc -O 0x158 -w -V 0xFFFFFFFF
./riodp_test_misc -O 0x158 
echo Clear errors again
./riodp_test_misc -O 0x100E0 -w -V 0x2000044
echo Read Tsi721 0x144
./riodp_test_misc -O 0x144
echo Read Tsi721 0x158
./riodp_test_misc -O 0x158 -w -V 0xFFFFFFFF
./riodp_test_misc -O 0x158
