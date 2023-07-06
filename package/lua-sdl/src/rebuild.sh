
rm -r -f build

mkdir build

cd build

cmake -DLua_LIBRARY=/usr/lib/x86_64-linux-gnu/liblua5.3.so -DLua_INCLUDE_DIR=/usr/include/lua5.3 .. && make -j10 && sudo make install


