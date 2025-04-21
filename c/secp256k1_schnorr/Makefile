CC := clang
OBJCOPY := llvm-objcopy

CFLAGS_BASE = --target=riscv64 -march=rv64imc_zba_zbb_zbc_zbs -O2 -fdata-sections -ffunction-sections
CFLAGS_SECP256K1 = $(CFLAGS_BASE) \
	-isystem musl/release/include \
	-DECMULT_WINDOW_SIZE=6 -DENABLE_MODULE_RECOVERY -DENABLE_MODULE_SCHNORRSIG -DENABLE_MODULE_EXTRAKEYS
CFLAGS_MAIN = $(CFLAGS_BASE) \
	-isystem musl/release/include \
	-Isecp256k1/include -Isecp256k1/src \
	-Lmusl/release/lib -Lcompiler-rt-builtins-riscv/build \
	-lc -lgcc -lcompiler-rt \
	-Wl,--gc-sections

build/main: build compiler-rt-builtins-riscv musl secp256k1
	$(CC) $(CFLAGS_MAIN) -o build/main main.c build/secp256k1/*
	$(OBJCOPY) --strip-debug --strip-all build/main

build:
	mkdir build
	mkdir build/secp256k1

clean:
	rm -rf build
	rm -f compiler-rt-builtins-riscv
	rm -f musl
	rm -f secp256k1

compiler-rt-builtins-riscv:
	ln -s ../deps/compiler-rt-builtins-riscv .
	cd compiler-rt-builtins-riscv && make

musl:
	ln -s ../deps/musl .
	cd musl && ./ckb/build.sh

secp256k1:
	ln -s ../deps/secp256k1 .
	$(CC) $(CFLAGS_SECP256K1) -c -o build/secp256k1/precomputed_ecmult.o secp256k1/src/precomputed_ecmult.c
	$(CC) $(CFLAGS_SECP256K1) -c -o build/secp256k1/precomputed_ecmult_gen.o secp256k1/src/precomputed_ecmult_gen.c
	$(CC) $(CFLAGS_SECP256K1) -c -o build/secp256k1/secp256k1.o secp256k1/src/secp256k1.c
