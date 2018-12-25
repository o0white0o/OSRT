#clear
cc -o queue_receive.run queue_receive.c
cc -o queue_send.run queue_send.c
echo Finish compiling!
echo Starting program...'\n'
./queue_receive.run