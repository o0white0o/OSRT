#clear
rm /tmp/transfer.txt
cc -o file_server.run file_server.c
cc -o file_client.run file_client.c
echo Finish compiling!
echo Starting program...
#cp inter_conf_net.run /net/QNX2/root
on -f /net/QNX2 ./file_client.run -n 
#./file_client.run -l &
./file_server.run -n 

