#clear
cc -o sem_receive.run sem_receive.c
cc -o sem_send.run sem_send.c
echo Finish compiling!
echo Starting program...'\n'
./sem_receive.run