#clear
cc -o inter_conf_net.run inter_conf_net.c
cc -o inter_receive_net.run inter_receive_net.c
echo Finish compiling!
echo Starting program...
cp inter_conf_net.run /net/QNX2/root
./inter_receive_net.run 3
