cd ../log/
rm -rf *
cd ../async/
killall -9 AsyncServer
make
