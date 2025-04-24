build:
	cd c/secp256k1_ecdsa && make
	cd c/secp256k1_schnorr && make
	cd rust/ed25519 && make
	cd rust/k256_ecdsa && make
	cd rust/k256_schnorr && make
	cd rust/p256 && make
	cd rust/rsa && make

	rm -rf binary
	mkdir -p binary
	mkdir -p binary/c
	mkdir -p binary/rust

	cp c/secp256k1_ecdsa/build/main binary/c/secp256k1_ecdsa
	cp c/secp256k1_schnorr/build/main binary/c/secp256k1_schnorr
	cp rust/ed25519/target/riscv64imac-unknown-none-elf/release/ed25519 binary/rust/ed25519
	cp rust/k256_ecdsa/target/riscv64imac-unknown-none-elf/release/k256_ecdsa binary/rust/k256_ecdsa
	cp rust/k256_schnorr/target/riscv64imac-unknown-none-elf/release/k256_schnorr binary/rust/k256_schnorr
	cp rust/p256/target/riscv64imac-unknown-none-elf/release/p256 binary/rust/p256
	cp rust/rsa/target/riscv64imac-unknown-none-elf/release/rsa binary/rust/rsa

clean:
	cd c/secp256k1_ecdsa && make clean
	cd c/secp256k1_schnorr && make clean
	cd rust/ed25519 && make clean
	cd rust/k256_ecdsa && make clean
	cd rust/k256_schnorr && make clean
	cd rust/p256 && make clean
	cd rust/rsa && make clean
	rm -rf binary
	rm -f binary.tar.gz
	rm -f report.txt

report:
	du -h binary/c/secp256k1_ecdsa && ckb-debugger --bin binary/c/secp256k1_ecdsa
	du -h binary/c/secp256k1_schnorr && ckb-debugger --bin binary/c/secp256k1_schnorr
	du -h binary/rust/ed25519 && ckb-debugger --bin binary/rust/ed25519
	du -h binary/rust/k256_ecdsa && ckb-debugger --bin binary/rust/k256_ecdsa
	du -h binary/rust/k256_schnorr && ckb-debugger --bin binary/rust/k256_schnorr
	du -h binary/rust/p256 && ckb-debugger --bin binary/rust/p256
	du -h binary/rust/rsa && ckb-debugger --bin binary/rust/rsa
