cd
curl https://javadbayat.github.io/Test/transfer/transfer.cpp -o transfer.cpp
clang++ transfer.cpp -o bin/transfer
echo ~/bin/transfer > ~/.bashrc

clear
printf "OK!\n"
