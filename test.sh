#sudo insmod kernel_module/npheap.ko
#sudo chmod 777 /dev/npheap
./benchmark/benchmark 10 8192 4
echo "finish benchmark"
cat *.log > trace
sort -n -k 3 trace > sorted_trace
./benchmark/validate 10 8192 < sorted_trace
echo "finish validate"
rm -f *.log
#sudo rmmod npheap