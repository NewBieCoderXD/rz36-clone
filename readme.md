# Build

```
mkdir build
gcc sim21/*.h sim21/*.c -o build/sim21
gcc rz36/comp/*.h rz36/comp/*.c -o build/rz36
gcc LL1-parser-2/comp/*.h LL1-parser-2/comp/*.c -o build/ll1-parser-2
gcc as21/*.h as21/*.c -o build/as21
gcc rz33a/comp/*.h rz33a/comp/*.c -o build/rz33a
```

# Running

```
mkdir output
./build/rz36 test/fac.txt > output/fac_out.txt
# Remove Parse Tree information
./build/as21 output/fac_out.txt
./build/ll1-parser-2 output/fac_out.txt
./build/sim21 output/fac_out.obj
```