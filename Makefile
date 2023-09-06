CC := riscv64-unknown-elf-gcc
DUMP := riscv64-unknown-elf-objdump
HOST := riscv64-elf
CFLAGS := -Ideps/secp256k1/src -Ideps/secp256k1 -O3
LDFLAGS := -Wl,-static -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-s
SECP256K1_LIB := deps/secp256k1/src/ecmult_static_pre_context.h
SECP256K1_BENCH_BIN := build/secp256k1_bench

all: $(SECP256K1_BENCH_BIN) build/schnorr_bench

$(SECP256K1_BENCH_BIN): c/secp256k1_bench.c c/sha3.h $(SECP256K1_LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<
	$(DUMP) -d $@ > $@_dump.txt

build/schnorr_bench: c/schnorr_bench.c c/sha3.h $(SECP256K1_LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<
	$(DUMP) -d $@ > $@_dump.txt

$(SECP256K1_LIB):
	cd deps/secp256k1 && \
		./autogen.sh && \
		CC=$(CC) LD=$(CC) ./configure --with-bignum=no --enable-ecmult-static-precomputation --enable-endomorphism --enable-module-recovery --enable-module-extrakeys --enable-module-schnorrsig --enable-experimental --host=$(HOST) && \
		make

clean:
	cd deps/secp256k1 && make clean
	rm -rf $(SECP256K1_BENCH_BIN)
	rm -rf build/schnorr_bench
	rm -rf build/*.txt

.PHONY: all clean
