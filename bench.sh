FLAGS=(
    # ""
    # "-O1"
    # "-O2"
    "-O3"
    # "-Ofast"
    "-O3 -march=native"
    "-O3 -march=native -flto"
    "-O3 -march=native -flto -funroll-loops"
    "-O3 -march=native -flto -funroll-loops -fno-stack-protector"
    "-O3 -march=native -flto -funroll-loops -fno-stack-protector -ffast-math"
)
for flags in "${FLAGS[@]}"; do
    make clean > /dev/null
    echo "flags: $flags"
    make run CFLAGS="$flags" | grep "Kpos/sec"
    echo
done

# flags: -O3
# 51596 pos / 7 msec = 7370.9 Kpos/sec
# 8716732 pos / 1038 msec = 8397.6 Kpos/sec

# flags: -O3 -march=native
# 51596 pos / 7 msec = 7370.9 Kpos/sec
# 8716732 pos / 1030 msec = 8462.8 Kpos/sec

# flags: -O3 -march=native -flto
# 51596 pos / 6 msec = 8599.3 Kpos/sec
# 8716732 pos / 994 msec = 8769.3 Kpos/sec

# flags: -O3 -march=native -flto -funroll-loops
# 51596 pos / 6 msec = 8599.3 Kpos/sec
# 8716732 pos / 989 msec = 8813.7 Kpos/sec

# flags: -O3 -march=native -flto -funroll-loops -fno-stack-protector
# 51596 pos / 6 msec = 8599.3 Kpos/sec
# 8716732 pos / 988 msec = 8822.6 Kpos/sec

# flags: -O3 -march=native -flto -funroll-loops -fno-stack-protector -ffast-math
# 51596 pos / 6 msec = 8599.3 Kpos/sec
# 8716732 pos / 979 msec = 8903.7 Kpos/sec
