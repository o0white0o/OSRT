#clear
cc -o signal_rec.run signal_rec.c
cc -o signal_send.run signal_send.c
echo Finish compiling!
echo Starting program...'\n'
./signal_rec.run &
./signal_send.run