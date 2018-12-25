#clear
cc -o pulse_send.run pulse_send.c
cc -o pulse_receive.run pulse_receive.c
echo Finish compiling!
echo Starting program...
./pulse_receive.run