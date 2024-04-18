


#include <iostream>
#include <string.h>
#include "Signer.hpp"
#include "Log.hpp"

#include "HexBase.hpp"

extern "C"{
    #include <falcon1024.h>
}


using namespace std;

int main(){

    PQB::Log::init(); // to avoid segfault from uninitialized logger
    auto ss = PQB::Signer::GetInstance("ecdsa");

    std::string message = "Message to sign";
    PQB::byteBuffer pk;
    PQB::byteBuffer sk;
    PQB::byteBuffer signature;
    
    ss->genKeys(sk, pk);

    std::cout << "SK: " << PQB::bytesToHexString(sk.data(), sk.size()) << std::endl;
    std::cout << "PK: " << PQB::bytesToHexString(pk.data(), pk.size()) << std::endl;

    ss->sign(signature, (PQB::byte*)message.data(), message.size(), sk);
    std::cout << "SG: " << PQB::bytesToHexString(signature.data(), signature.size()) << std::endl;


    if (ss->verify(signature, (PQB::byte*)message.data(), message.size(), pk)){
        std::cout << "OK" << std::endl;
    } else {
        std::cout << "NOT OK" << std::endl;
    }

    message = "Altered message";
    if (ss->verify(signature, (PQB::byte*)message.data(), message.size(), pk)){
        std::cout << "NOT OK" << std::endl;
    } else {
        std::cout << "OK" << std::endl;
    }
    

    /*
    int ok = 0;
    char msg[] = "Hi, this is a message!";
    size_t msg_size = (strlen(msg) * sizeof(uint8_t));

    // KEYGEN

    uint8_t *sk = (uint8_t *) malloc(PQCLEAN_FALCON1024_AVX2_CRYPTO_SECRETKEYBYTES);
    if(sk == NULL){
        cerr << "sk memmory allocation failure!" << endl;
        return 1;
    }

    uint8_t *pk = (uint8_t *) malloc(PQCLEAN_FALCON1024_AVX2_CRYPTO_PUBLICKEYBYTES);
    if(pk == NULL){
        cerr << "pk memmory allocation failure!" << endl;
        return 1;
    }

    ok = PQCLEAN_FALCON1024_AVX2_crypto_sign_keypair(pk, sk);
    if(ok){
        cerr << "key generation failure!" << endl;
        return 1;
    }

    // SIGN

    uint8_t *sig = (uint8_t *) malloc(PQCLEAN_FALCON1024_AVX2_CRYPTO_BYTES);
    if(sig == NULL){
        cerr << "sig memmory allocation failure!" << endl;
        return 1;
    }

    size_t sig_len;

    ok = PQCLEAN_FALCON1024_AVX2_crypto_sign_signature(sig, &sig_len, (uint8_t *)msg, msg_size, sk);
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

    ok = PQCLEAN_FALCON1024_AVX2_crypto_sign_verify(sig, sig_len, (uint8_t *)msg, msg_size, pk);
    if(ok){
        cerr << "verify failure!" << endl;
        return 1;
    }

    // FINAL PRINT

    cout << "Message: " << msg << endl;
    cout << "Sig length: " << sig_len << endl; 

    std::string hexSK = PQB::bytesToHexString(sk, PQCLEAN_FALCON1024_AVX2_CRYPTO_SECRETKEYBYTES);
    std::string hexPK = PQB::bytesToHexString(pk, PQCLEAN_FALCON1024_AVX2_CRYPTO_PUBLICKEYBYTES);
    std::string hexSignature = PQB::bytesToHexString(sig, sig_len);

    std::cout << hexSK << std::endl << std::endl;
    std::cout << hexPK << std::endl << std::endl;
    std::cout << hexSignature << std::endl;

    // VERIFY altered message

    char msg2[] = "Altered secret";
    size_t msg_size2 = (strlen(msg2) * sizeof(uint8_t));

    ok = PQCLEAN_FALCON1024_AVX2_crypto_sign_verify(sig, sig_len, (uint8_t *)msg2, msg_size2, pk);
    if(ok){
        cerr << "Altered message!" << endl;
        return 1;
    }
    */

    return 0;
}