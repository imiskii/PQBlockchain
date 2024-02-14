/**
 * @file Signer.cpp
 * @author Michal Ľaš
 * @brief Signer class, digital signature algorithms
 * @date 2024-02-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "Signer.hpp"

#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>       // HexEncoder
#include <cryptopp/filters.h>   // ArraySink
#include "PQBExceptions.hpp"

namespace PQB{

SignAlgorithmPtr Signer::GetInstance(std::string chosenAlgorithm){
    static SignAlgorithmPtr __algorithmInstance = nullptr;
    if(__algorithmInstance == nullptr){
        if(chosenAlgorithm == "falcon1024"){
            __algorithmInstance = std::make_shared<Falcon1024>();
        } else if (chosenAlgorithm == "ed25519"){
            __algorithmInstance = std::make_shared<Ed25519>();
        }
    }
    return __algorithmInstance;
}

/*********** Falcon-1024 ***********/

void Falcon1024::genKeys(PQB::byteBuffer &privateKey, PQB::byteBuffer &publicKey){
    privateKey.resize(privateKeySize);
    publicKey.resize(publicKeySize);

    int ok = PQCLEAN_FALCON1024_CLEAN_crypto_sign_keypair(publicKey.data(), privateKey.data());
    if(ok){
        throw PQB::Exceptions::SignerEx("Key generation: key generation failure!");
    }
}

size_t Falcon1024::sign(PQB::byteBuffer &signature, const PQB::byte *dataToSign, size_t dataSize, const PQB::byteBuffer &privateKey){
    signature.resize(signatureSize);
    size_t signatureLength;

    int ok = PQCLEAN_FALCON1024_CLEAN_crypto_sign_signature(signature.data(), &signatureLength, dataToSign, dataSize, privateKey.data());
    if(ok){
        throw PQB::Exceptions::SignerEx("Signing: message signing failure!");
    }

    signature.resize(signatureLength);
    return signatureLength;
}

bool Falcon1024::verify(const PQB::byteBuffer &signature, const PQB::byte *signedData, size_t dataSize, const PQB::byteBuffer &publicKey){
    int ok = PQCLEAN_FALCON1024_CLEAN_crypto_sign_verify(signature.data(), signature.size(), signedData, dataSize, publicKey.data());
    return (ok == 0 ? true : false);
}

/*********** Ed25519 ***********/

void Ed25519::genKeys(PQB::byteBuffer& privateKey, PQB::byteBuffer& publicKey){
    privateKey.resize(privateKeySize);
    publicKey.resize(publicKeySize);

    CryptoPP::AutoSeededRandomPool prng;
    CryptoPP::ed25519::Signer signer;
    CryptoPP::ArraySink sinkSK(privateKey.data(), privateKeySize);
    CryptoPP::ArraySink sinkPK(publicKey.data(), publicKeySize);
    CryptoPP::ed25519::Verifier verifier(signer);
    signer.AccessPrivateKey().GenerateRandom(prng);

    signer.GetPrivateKey().Save(sinkSK);
    verifier.GetPublicKey().Save(sinkPK);
}

size_t Ed25519::sign(PQB::byteBuffer& signature, const PQB::byte* dataToSign, size_t dataSize, const PQB::byteBuffer& privateKey){
    CryptoPP::AutoSeededRandomPool prng;
    CryptoPP::ed25519::Signer signer(privateKey.data());

    const CryptoPP::ed25519PrivateKey& sk = dynamic_cast<const CryptoPP::ed25519PrivateKey&>(signer.GetPrivateKey());
    if(!sk.Validate(prng, 3)){ // 3 is level of check validation (there are levels 1,2 and 3)
        throw PQB::Exceptions::SignerEx("Signing: private key is not valid!");
    }

    size_t signatureLength = signer.MaxSignatureLength();
    signature.resize(signatureLength);
    signatureLength = signer.SignMessage(prng, dataToSign, dataSize, signature.data());
    signature.resize(signatureLength);
    return signatureLength;
}

bool Ed25519::verify(const PQB::byteBuffer &signature, const PQB::byte *signedData, size_t dataSize, const PQB::byteBuffer &publicKey){
    CryptoPP::ed25519::Verifier verifier(publicKey.data());
    return verifier.VerifyMessage(signedData, dataSize, signature.data(), signature.size());
}


} // PQB namespace

/* END OF FILE */