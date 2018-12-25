#clear
cc -o msg_send.run msg_send.c
cc -o msg_receive.run msg_receive.c
echo Finish compiling!
echo Starting program...
./msg_receive.run -g