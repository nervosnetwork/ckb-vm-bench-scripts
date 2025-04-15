#![cfg_attr(target_arch = "riscv64", no_std)]
#![cfg_attr(not(test), no_main)]

#[cfg(test)]
extern crate alloc;

#[cfg(test)]
mod tests;

#[cfg(not(test))]
ckb_std::entry!(program_entry);
#[cfg(not(test))]
// By default, the following heap configuration is used:
// * 16KB fixed heap
// * 1.2MB(rounded up to be 16-byte aligned) dynamic heap
// * Minimal memory block in dynamic heap is 64 bytes
// For more details, please refer to ckb-std's default_alloc macro
// and the buddy-alloc alloc implementation.
ckb_std::default_alloc!(16384, 1258306, 64);

use ed25519_dalek::{self, Signature, VerifyingKey};
use hex_literal::hex;
use sha2::{digest::Digest, Sha512};

pub fn program_entry() -> i8 {
    // Test case from: https://github.com/dalek-cryptography/ed25519-dalek/blob/02001d8c3422fb0314b541fdb09d04760f7ab4ba/tests/ed25519.rs#L102
    let pub_bytes = hex!("ec172b93ad5e563bf4932c70e1245034c35467ef2efd4d64ebf819683467e2bf");
    let msg_bytes = hex!("616263");
    let sig_bytes = hex!("98a70222f0b8121aa9d30f813d683f809e462b469c7ff87639499bb94e6dae4131f85042463c2a355a2003d062adf5aaa10b8c61e636062aaad11c2a26083406");

    let verifying_key = VerifyingKey::from_bytes(&pub_bytes).unwrap();
    let sig = Signature::try_from(&sig_bytes[..]).unwrap();

    let mut prehash_for_verifying = Sha512::default();
    prehash_for_verifying.update(&msg_bytes[..]);

    assert!(verifying_key
        .verify_prehashed(prehash_for_verifying.clone(), None, &sig)
        .is_ok());

    0
}
