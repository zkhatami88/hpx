
ARGS="--nx 100000 --np 1000 --nt 100"

echo "1d_stencil_1_omp"
likwid-pin -c 0   ./bin/1d_stencil_1_omp --nx 100000000 --nt 100
likwid-pin -c 0-1 ./bin/1d_stencil_1_omp --nx 100000000 --nt 100
likwid-pin -c 0-3 ./bin/1d_stencil_1_omp --nx 100000000 --nt 100
likwid-pin -c 0-7 ./bin/1d_stencil_1_omp --nx 100000000 --nt 100
likwid-pin -c 0-9 ./bin/1d_stencil_1_omp --nx 100000000 --nt 100
likwid-pin -c 0-19 ./bin/1d_stencil_1_omp --nx 100000000 --nt 100
echo "1d_stencil_3_omp"
likwid-pin -c 0 ./bin/1d_stencil_3_omp $ARGS
likwid-pin -c 0-1 ./bin/1d_stencil_3_omp $ARGS
likwid-pin -c 0-3 ./bin/1d_stencil_3_omp $ARGS
likwid-pin -c 0-7 ./bin/1d_stencil_3_omp $ARGS
likwid-pin -c 0-9 ./bin/1d_stencil_3_omp $ARGS
likwid-pin -c 0-19 ./bin/1d_stencil_3_omp $ARGS
echo "1d_stencil_4"
for i in 1 2 4 8 10 20
do
    ./bin/1d_stencil_4 $ARGS -t$i
done
echo "1d_stencil_4_2"
for i in 1 2 4 8 10 20
do
    ./bin/1d_stencil_4_2 $ARGS -t$i
done
echo "1d_stencil_8"
for i in 1 2 4 8 10 20
do
    ./bin/1d_stencil_8 $ARGS -t$i
done
echo "1d_stencil_8 distr"
for i in 1 2 5 10
do
    mpirun -np 2 ./bin/1d_stencil_8 $ARGS -t$i
done
echo "1d_stencil_8_2"
for i in 1 2 4 8 10 20
do
    ./bin/1d_stencil_8_2 $ARGS -t$i
done
echo "1d_stencil_8_2 distr."
for i in 1 2 5 10
do
    mpirun -np 2 ./bin/1d_stencil_8_2 $ARGS -t$i
done
echo "1d_stencil_8_3"
for i in 1 2 4 8 10 20
do
    ./bin/1d_stencil_8_3 $ARGS -t$i
done
echo "1d_stencil_8_3 distr."
for i in 1 2 5 10
do
    mpirun -np 2 ./bin/1d_stencil_8_3 $ARGS -t$i
done
