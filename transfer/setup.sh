# Run with:
# curl https://javadbayat.github.io/Test/transfer/setup.sh | bash

cd
pkg upgrade -y
pkg install busybox clang net-tools grep gawk -y
termux-setup-storage

curl https://javadbayat.github.io/Test/transfer/transfer.cpp -o transfer.cpp
mkdir bin
curl https://javadbayat.github.io/Test/transfer/termux-file-editor -o bin/termux-file-editor
clang++ transfer.cpp -o bin/transfer
curl https://javadbayat.github.io/Test/transfer/shortcut.sh >> ~/.bashrc

clear
printf "Installation completed!\n"
