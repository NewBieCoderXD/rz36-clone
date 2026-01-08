# Build

```
mkdir build
gcc sim21/*.h sim21/*.c -o build/sim21
gcc rz36/comp/*.h rz36/comp/*.c -o build/rz36
gcc as21/*.h as21/*.c -o build/as21
```

# Running

```
mkdir output
./build/rz36 test/fac.txt > output/fac_out.txt
./build/as21 output/fac_out.txt
./build/sim21 output/fac_out.obj
```