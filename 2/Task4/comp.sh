#clear
cc -o server.run server.c
cc -o client.run client.c
echo Finish compiling!
echo Starting program...
#cp inter_conf_net.run /net/QNX2/root
#on -f /net/QNX2 ./file_client.run -n 
#./file_client.run -l &
./server.run 

