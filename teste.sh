for i in 1000 10000 100000 1000000; do
do  
    ./client --- 8005 $i >> /dev/null
    sleep 2
done