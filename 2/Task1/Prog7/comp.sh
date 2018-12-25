#clear
cc -o fifo_receive_net.run fifo_receive_net.c
cc -o fifo_send.run fifo_send.c
echo Finish compiling!
echo Starting program...'\n'
./fifo_receive_net.run QNX2