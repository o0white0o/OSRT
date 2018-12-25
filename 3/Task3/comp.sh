#clear
cc -o server_timer.run server_timer.c
cc -o client_timer.run client_timer.c
echo Finish compiling!
echo Starting program...
cp client_timer.run /net/QNX2/root
./server_timer.run 

