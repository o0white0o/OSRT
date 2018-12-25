#clear
cc -o queue_receive_net.run queue_receive_net.c
cc -o queue_send.run queue_send.c
echo Finish compiling!
echo Starting program...'\n'
./queue_receive_net.run QNX2