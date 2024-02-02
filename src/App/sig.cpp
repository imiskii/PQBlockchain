


#include <iostream>
#include <stdlib.h>
#include <string.h>

extern "C"{
    #include <falcon1024.h>
}


using namespace std;

int main(){

    int ok = 0;
    char msg[] = "Secret message";
    size_t msg_size = (strlen(msg) * sizeof(uint8_t));

    // KEYGEN

    uint8_t *sk = (uint8_t *) malloc(PQCLEAN_FALCON1024_CLEAN_CRYPTO_SECRETKEYBYTES);
    if(sk == NULL){
        cerr << "sk memmory allocation failure!" << endl;
        return 1;
    }

    uint8_t *pk = (uint8_t *) malloc(PQCLEAN_FALCON1024_CLEAN_CRYPTO_SECRETKEYBYTES);
    if(pk == NULL){
        cerr << "pk memmory allocation failure!" << endl;
        return 1;
    }

    ok = PQCLEAN_FALCON1024_CLEAN_crypto_sign_keypair(pk, sk);
    if(ok){
        cerr << "key generation failure!" << endl;
        return 1;
    }

    // SIGN

    uint8_t *sig = (uint8_t *) malloc(PQCLEAN_FALCON1024_CLEAN_CRYPTO_BYTES);
    if(sig == NULL){
        cerr << "sig memmory allocation failure!" << endl;
        return 1;
    }

    size_t sig_len;

    ok = PQCLEAN_FALCON1024_CLEAN_crypto_sign_signature(sig, &sig_len, (uint8_t *)msg, msg_size, sk);
    if(ok){
        cerr << "sign failure!" << endl;
        return 1;
    }

    // Realloc memmory to signature length
    sig = (uint8_t *) realloc(sig, sig_len);
    if(sig == NULL){
        cerr << "sig memmory reallocation failure!" << endl;
        return 1;
    }

    // VERIFY

    ok = PQCLEAN_FALCON1024_CLEAN_crypto_sign_verify(sig, sig_len, (uint8_t *)msg, msg_size, pk);
    if(ok){
        cerr << "verify failure!" << endl;
        return 1;
    }

    // FINAL PRINT

    cout << "Message: " << msg << endl;
    cout << "Sig length: " << sig_len << endl; 

    printf("pk: %X\n", *pk);
    printf("sk: %X\n", *sk);
    printf("sig: %X\n", *sig);

    // VERIFY altered message

    char msg2[] = "Altered secret";
    size_t msg_size2 = (strlen(msg2) * sizeof(uint8_t));

    ok = PQCLEAN_FALCON1024_CLEAN_crypto_sign_verify(sig, sig_len, (uint8_t *)msg2, msg_size2, pk);
    if(ok){
        cerr << "Altered message!" << endl;
        return 1;
    }

    return 0;
}