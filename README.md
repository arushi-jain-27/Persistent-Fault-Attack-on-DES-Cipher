# Persistent-Fault-Attack-on-DES-Cipher

To run the DES encryption and decryption:
1. Run des.cpp
2. Input the desired 64-bit key in hexadecimal
The program encrypts and decrypts a randomly generated 64-bit Plaintext.

To run the PFA on DES:
1. Run pfa.cpp
2. Input the desired 64-bit key in hexadecimal
The program first generates the 56-bit master key and 16 48-bit round keys.
Then the program uses persistent fault attack to recover the 56-bit master key.
