#!/usr/bin/fish

for folder in */*
    echo
    echo ==================================================
    echo
    cd $folder
    echo '>> ' $folder
    make clean
    echo '>> Status:' (sed '5q;d' README.md)
    make test
    echo '>> Done!'
    sleep 1s
    cd ../..
end
