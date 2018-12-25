#clear
cc -o fifo_receive.run fifo_receive.c
cc -o fifo_send.run fifo_send.c
echo Finish compiling!
echo Starting program...'\n'
./fifo_receive.run