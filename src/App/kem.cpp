

#include <iostream>
#include <stdlib.h>

extern "C"{
    #include <kyber1024.h>
}


using namespace std;

int main(){

    int ok = 0;

    // KEY GENERATION

    uint8_t *sk = (uint8_t *) malloc(PQCLEAN_KYBER1024_CLEAN_CRYPTO_SECRETKEYBYTES);
    if(sk == NULL){
        cerr << "sk memmory allocation failure!" << endl;
        return 1;
    }

    uint8_t *pk = (uint8_t *) malloc(PQCLEAN_KYBER1024_CLEAN_CRYPTO_PUBLICKEYBYTES);
    if(pk == NULL){
        cerr << "pk memmory allocation failure!" << endl;
        return 1;
    }

    ok = PQCLEAN_KYBER1024_CLEAN_crypto_kem_keypair(pk, sk);
    if(ok){
        cerr << "keypair generation failure!" << endl;
        return 1;
    }

    // ENCRYPTION

    uint8_t *ct = (uint8_t *) malloc(PQCLEAN_KYBER1024_CLEAN_CRYPTO_CIPHERTEXTBYTES);
    if(ct == NULL){
        cerr << "ct memmory allocation failure!" << endl;
        return 1;
    }

    uint8_t *ss1 = (uint8_t *) malloc(PQCLEAN_KYBER1024_CLEAN_CRYPTO_BYTES);
    if(ss1 == NULL){
        cerr << "ss1 memmory allocation failure!" << endl;
        return 1;
    }

    ok = PQCLEAN_KYBER1024_CLEAN_crypto_kem_enc(ct, ss1, pk);
    if(ok){
        cerr << "encryption failure!" << endl;
        return 1;
    }

    // DECRYPTION

    uint8_t *ss2 = (uint8_t *) malloc(PQCLEAN_KYBER1024_CLEAN_CRYPTO_BYTES);
    if(ss2 == NULL){
        cerr << "ss2 memmory allocation failure!" << endl;
        return 1;
    }

    ok = PQCLEAN_KYBER1024_CLEAN_crypto_kem_dec(ss2, ct, sk);

    // FINAL PRINT

    if((*ss1) == (*ss2)){
        cout << endl << "SUCCESS!" << endl << endl;
    }else{
        cout << endl << "FAILURE!" << endl << endl;
    }

    printf("sk: %X\n", *sk);
    printf("pk: %X\n", *pk);
    printf("ct: %X\n", *ct);
    printf("ss1: %X\n", *ss1);
    printf("ss2: %X\n", *ss2);

    free(sk);
    free(pk);
    free(ct);
    free(ss1);
    free(ss2);

    return 0;
}