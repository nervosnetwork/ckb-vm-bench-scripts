set -ex

rm -rf binary
mkdir -p binary

cp c/secp256k1_ecdsa/build/main binary/secp256k1_ecdsa
cp c/secp256k1_schnorr/build/main binary/secp256k1_schnorr
cp rust/ed25519/target/riscv64imac-unknown-none-elf/release/ed25519 binary
cp rust/k256_ecdsa/target/riscv64imac-unknown-none-elf/release/k256_ecdsa binary
cp rust/k256_schnorr/target/riscv64imac-unknown-none-elf/release/k256_schnorr binary
cp rust/p256/target/riscv64imac-unknown-none-elf/release/p256 binary
cp rust/rsa/target/riscv64imac-unknown-none-elf/release/rsa binary

report() {
    du -h binary/$1 >> report.txt
    ckb-debugger --bin binary/$1 >> report.txt
    echo "" >> report.txt
}

report secp256k1_ecdsa
report secp256k1_schnorr
report ed25519
report k256_ecdsa
report k256_schnorr
report p256
report rsa

tar zcvf binary.tar.gz binary
