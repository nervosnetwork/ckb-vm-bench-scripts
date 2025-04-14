set -ex

CC="${CC:-clang}"
OBJCOPY="${OBJCOPY:-llvm-objcopy}"
CFLAGS_BASE="--target=riscv64 -march=rv64imc_zba_zbb_zbc_zbs -g -Oz -fdata-sections -ffunction-sections"

if ! [ -d build ]; then
    mkdir build
    mkdir build/secp256k1
fi

if ! [ -d compiler-rt-builtins-riscv ]; then
    git clone https://github.com/nervosnetwork/compiler-rt-builtins-riscv
    cd compiler-rt-builtins-riscv
    git checkout 9469bca
    make
    cd ..
fi

if ! [ -d musl ]; then
    git clone https://github.com/xxuejie/musl
	cd musl
    git checkout 01ff4b5
    ./ckb/build.sh
    cd ..
fi

CFLAGS_SECP256K1="${CFLAGS_BASE}"
CFLAGS_SECP256K1+=" -isystem musl/release/include"
CFLAGS_SECP256K1+=" -DECMULT_WINDOW_SIZE=6 -DENABLE_MODULE_RECOVERY"
if ! [ -d secp256k1 ]; then
    git clone https://github.com/bitcoin-core/secp256k1
	cd secp256k1
    git checkout 0cdc758
    cd ..
    $CC $CFLAGS_SECP256K1 -c -o build/secp256k1/precomputed_ecmult.o secp256k1/src/precomputed_ecmult.c
    $CC $CFLAGS_SECP256K1 -c -o build/secp256k1/precomputed_ecmult_gen.o secp256k1/src/precomputed_ecmult_gen.c
    $CC $CFLAGS_SECP256K1 -c -o build/secp256k1/secp256k1.o secp256k1/src/secp256k1.c
fi

CFLAGS_MAIN="${CFLAGS_BASE}"
CFLAGS_MAIN+=" -isystem musl/release/include"
CFLAGS_MAIN+=" -Isecp256k1/include -Isecp256k1/src"
CFLAGS_MAIN+=" -Lmusl/release/lib -Lcompiler-rt-builtins-riscv/build"
CFLAGS_MAIN+=" -lc -lgcc -lcompiler-rt"
if ! [ -f build/main ]; then
    $CC $CFLAGS_MAIN -o build/main main.c build/secp256k1/*
    $OBJCOPY --strip-debug --strip-all build/main
fi

ls -hl build/main
ckb-debugger --bin build/main
