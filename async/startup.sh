killall -9 AsynServ 
cd log
rm -rf 2017*
cd ..
sleep 1
./AsynServ ./bench.conf
#./AsynServ ./config.lua
