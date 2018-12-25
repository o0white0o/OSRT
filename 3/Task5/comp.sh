#clear
cc -o watcher.run watcher.c
cc -o server.run server.c
echo Finish compiling!
echo Starting program...
cp watcher.run /net/QNX2/root
./server.run 

