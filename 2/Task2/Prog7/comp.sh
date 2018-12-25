#clear
cc -o inter_conf.run inter_conf.c
cc -o inter_receive.run inter_receive.c
echo Finish compiling!
echo Starting program...
./inter_receive.run 3 &
