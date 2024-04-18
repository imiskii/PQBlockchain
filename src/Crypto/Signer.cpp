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
#include "Log.hpp"

namespace PQB{

SignAlgorithmPtr Signer::GetInstance(std::string chosenAlgorithm){
    static SignAlgorithmPtr __algorithmInstance = nullptr;
    if(__algorithmInstance == nullptr){
        if(chosenAlgorithm == "falcon1024"){
            __algorithmInstance = std::make_shared<Falcon1024>();
        } else if (chosenAlgorithm == "falcon512") {
            __algorithmInstance = std::make_shared<Falcon512>();
        } else if (chosenAlgorithm == "dilithium5") {
            __algorithmInstance = std::make_shared<Dilithium5>();
        } else if (chosenAlgorithm == "dilithium3") {
            __algorithmInstance = std::make_shared<Dilithium3>();
        } else if (chosenAlgorithm == "dilithium2") {
            __algorithmInstance = std::make_shared<Dilithium2>();
        } else if (chosenAlgorithm == "ed25519"){
            __algorithmInstance = std::make_shared<Ed25519>();
        } else if (chosenAlgorithm == "ecdsa"){
            __algorithmInstance = std::make_shared<ECDSA>();
        } else {
            PQB_LOG_ERROR("SIGNER", "Chosen unknow algorithm for digital signature: {}", chosenAlgorithm);
            throw PQB::Exceptions::Signer("Chosen unknow algorithm for digital signature!");
        }
        PQB_LOG_INFO("SIGNER", "{} was chosen for digital signatures", chosenAlgorithm);
    }
    return __algorithmInstance;
}

/*********** Falcon-1024 ***********/

void Falcon1024::genKeys(PQB::byteBuffer &privateKey, PQB::byteBuffer &publicKey){
    privateKey.resize(privateKeySize);
    publicKey.resize(publicKeySize);

    int ok = PQCLEAN_FALCON1024_AVX2_crypto_sign_keypair(publicKey.data(), privateKey.data());
    if(ok){
        throw PQB::Exceptions::Signer("Key generation: key generation failure!");
    }
}

size_t Falcon1024::sign(PQB::byteBuffer &signature, const PQB::byte *dataToSign, size_t dataSize, const PQB::byteBuffer &privateKey){
    signature.resize(signatureSize);
    size_t signatureLength;

    int ok = PQCLEAN_FALCON1024_AVX2_crypto_sign_signature(signature.data(), &signatureLength, dataToSign, dataSize, privateKey.data());
    if(ok){
        throw PQB::Exceptions::Signer("Signing: message signing failure!");
    }

    signature.resize(signatureLength);
    return signatureLength;
}

bool Falcon1024::verify(const PQB::byteBuffer &signature, const PQB::byte *signedData, size_t dataSize, const PQB::byteBuffer &publicKey){
    int ok = PQCLEAN_FALCON1024_AVX2_crypto_sign_verify(signature.data(), signature.size(), signedData, dataSize, publicKey.data());
    return (ok == 0 ? true : false);
}

/*********** Falcon-512 ***********/

void Falcon512::genKeys(PQB::byteBuffer &privateKey, PQB::byteBuffer &publicKey){
    privateKey.resize(privateKeySize);
    publicKey.resize(publicKeySize);

    int ok = PQCLEAN_FALCON512_AVX2_crypto_sign_keypair(publicKey.data(), privateKey.data());
    if(ok){
        throw PQB::Exceptions::Signer("Key generation: key generation failure!");
    }
}

size_t Falcon512::sign(PQB::byteBuffer &signature, const PQB::byte *dataToSign, size_t dataSize, const PQB::byteBuffer &privateKey){
    signature.resize(signatureSize);
    size_t signatureLength;

    int ok = PQCLEAN_FALCON512_AVX2_crypto_sign_signature(signature.data(), &signatureLength, dataToSign, dataSize, privateKey.data());
    if(ok){
        throw PQB::Exceptions::Signer("Signing: message signing failure!");
    }

    signature.resize(signatureLength);
    return signatureLength;
}

bool Falcon512::verify(const PQB::byteBuffer &signature, const PQB::byte *signedData, size_t dataSize, const PQB::byteBuffer &publicKey){
    int ok = PQCLEAN_FALCON512_AVX2_crypto_sign_verify(signature.data(), signature.size(), signedData, dataSize, publicKey.data());
    return (ok == 0 ? true : false);
}

/*********** Dilithium5 ***********/

void Dilithium5::genKeys(PQB::byteBuffer &privateKey, PQB::byteBuffer &publicKey){
    privateKey.resize(privateKeySize);
    publicKey.resize(publicKeySize);

    int ok = PQCLEAN_DILITHIUM5_AVX2_crypto_sign_keypair(publicKey.data(), privateKey.data());
    if(ok){
        throw PQB::Exceptions::Signer("Key generation: key generation failure!");
    }
}

size_t Dilithium5::sign(PQB::byteBuffer &signature, const PQB::byte *dataToSign, size_t dataSize, const PQB::byteBuffer &privateKey){
    signature.resize(signatureSize);
    size_t signatureLength;

    int ok = PQCLEAN_DILITHIUM5_AVX2_crypto_sign_signature(signature.data(), &signatureLength, dataToSign, dataSize, privateKey.data());
    if(ok){
        throw PQB::Exceptions::Signer("Signing: message signing failure!");
    }

    signature.resize(signatureLength);
    return signatureLength;
}

bool Dilithium5::verify(const PQB::byteBuffer &signature, const PQB::byte *signedData, size_t dataSize, const PQB::byteBuffer &publicKey){
    int ok = PQCLEAN_DILITHIUM5_AVX2_crypto_sign_verify(signature.data(), signature.size(), signedData, dataSize, publicKey.data());
    return (ok == 0 ? true : false);
}

/*********** Dilithium3 ***********/

void Dilithium3::genKeys(PQB::byteBuffer &privateKey, PQB::byteBuffer &publicKey){
    privateKey.resize(privateKeySize);
    publicKey.resize(publicKeySize);

    int ok = PQCLEAN_DILITHIUM3_AVX2_crypto_sign_keypair(publicKey.data(), privateKey.data());
    if(ok){
        throw PQB::Exceptions::Signer("Key generation: key generation failure!");
    }
}

size_t Dilithium3::sign(PQB::byteBuffer &signature, const PQB::byte *dataToSign, size_t dataSize, const PQB::byteBuffer &privateKey){
    signature.resize(signatureSize);
    size_t signatureLength;

    int ok = PQCLEAN_DILITHIUM3_AVX2_crypto_sign_signature(signature.data(), &signatureLength, dataToSign, dataSize, privateKey.data());
    if(ok){
        throw PQB::Exceptions::Signer("Signing: message signing failure!");
    }

    signature.resize(signatureLength);
    return signatureLength;
}

bool Dilithium3::verify(const PQB::byteBuffer &signature, const PQB::byte *signedData, size_t dataSize, const PQB::byteBuffer &publicKey){
    int ok = PQCLEAN_DILITHIUM3_AVX2_crypto_sign_verify(signature.data(), signature.size(), signedData, dataSize, publicKey.data());
    return (ok == 0 ? true : false);
}

/*********** Dilithium2 ***********/

void Dilithium2::genKeys(PQB::byteBuffer &privateKey, PQB::byteBuffer &publicKey){
    privateKey.resize(privateKeySize);
    publicKey.resize(publicKeySize);

    int ok = PQCLEAN_DILITHIUM2_AVX2_crypto_sign_keypair(publicKey.data(), privateKey.data());
    if(ok){
        throw PQB::Exceptions::Signer("Key generation: key generation failure!");
    }
}

size_t Dilithium2::sign(PQB::byteBuffer &signature, const PQB::byte *dataToSign, size_t dataSize, const PQB::byteBuffer &privateKey){
    signature.resize(signatureSize);
    size_t signatureLength;

    int ok = PQCLEAN_DILITHIUM2_AVX2_crypto_sign_signature(signature.data(), &signatureLength, dataToSign, dataSize, privateKey.data());
    if(ok){
        throw PQB::Exceptions::Signer("Signing: message signing failure!");
    }

    signature.resize(signatureLength);
    return signatureLength;
}

bool Dilithium2::verify(const PQB::byteBuffer &signature, const PQB::byte *signedData, size_t dataSize, const PQB::byteBuffer &publicKey){
    int ok = PQCLEAN_DILITHIUM2_AVX2_crypto_sign_verify(signature.data(), signature.size(), signedData, dataSize, publicKey.data());
    return (ok == 0 ? true : false);
}

/*********** Ed25519 ***********/

void Ed25519::genKeys(PQB::byteBuffer& privateKey, PQB::byteBuffer& publicKey){
    privateKey.resize(privateKeySize);
    publicKey.resize(publicKeySize);

    CryptoPP::AutoSeededRandomPool prng;
    CryptoPP::ed25519::Signer signer;
    signer.AccessPrivateKey().GenerateRandom(prng);
    CryptoPP::ed25519::Verifier verifier(signer);
    const CryptoPP::ed25519PrivateKey& privKey = dynamic_cast<const CryptoPP::ed25519PrivateKey&>(signer.GetPrivateKey());
    const CryptoPP::Integer& x = privKey.GetPrivateExponent();
    x.Encode(privateKey.data(), privateKey.size()); // BIG ENDIAN !!!

    const CryptoPP::ed25519PublicKey& pubKey = dynamic_cast<const CryptoPP::ed25519PublicKey&>(verifier.GetPublicKey());
    const CryptoPP::Integer& y = pubKey.GetPublicElement();
    y.Encode(publicKey.data(), publicKey.size()); // BIG ENDIAN !!!
}

size_t Ed25519::sign(PQB::byteBuffer& signature, const PQB::byte* dataToSign, size_t dataSize, const PQB::byteBuffer& privateKey){
    CryptoPP::AutoSeededRandomPool prng;
    CryptoPP::Integer privKey(privateKey.data(), privateKey.size());
    CryptoPP::ed25519::Signer signer(privKey);

    /*
    const CryptoPP::ed25519PrivateKey& sk = dynamic_cast<const CryptoPP::ed25519PrivateKey&>(signer.GetPrivateKey());
    if(!sk.Validate(prng, 3)){ // 3 is level of check validation (there are levels 1,2 and 3)
        throw PQB::Exceptions::Signer("Signing: private key is not valid!");
    }
    */

    size_t signatureLength = signer.MaxSignatureLength();
    signature.resize(signatureLength);
    signatureLength = signer.SignMessage(prng, dataToSign, dataSize, signature.data());
    signature.resize(signatureLength);
    return signatureLength;
}

bool Ed25519::verify(const PQB::byteBuffer &signature, const PQB::byte *signedData, size_t dataSize, const PQB::byteBuffer &publicKey){
    CryptoPP::Integer pubKey(publicKey.data(), publicKey.size());
    CryptoPP::ed25519::Verifier verifier(pubKey);
    return verifier.VerifyMessage(signedData, dataSize, signature.data(), signature.size());
}

/*********** ECDSA ***********/

void ECDSA::genKeys(PQB::byteBuffer &privateKey, PQB::byteBuffer &publicKey){
    privateKey.resize(privateKeySize);
    publicKey.resize(publicKeySize);

    using ECDSA = CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>;
    CryptoPP::AutoSeededRandomPool prng;

    ECDSA::PrivateKey sk;
    sk.Initialize(prng, CryptoPP::ASN1::secp256k1());
    const CryptoPP::Integer& x = sk.GetPrivateExponent();
    x.Encode(privateKey.data(), privateKey.size());

    ECDSA::PublicKey pk;
    sk.MakePublicKey(pk);
    const CryptoPP::ECP::Point& point = pk.GetPublicElement();
    const CryptoPP::Integer& y1 = point.x;
    const CryptoPP::Integer& y2 = point.y;
    // skip the first byte
    y1.Encode(publicKey.data()+1, (publicKey.size() - 1));
    // on the first byte put 0x02 if y2 is even or 0x03 if y2 is odd
    if (y2.IsEven()){
        publicKey[0] = 0x02;
    } else {
        publicKey[0] = 0x03;
    }
}

size_t ECDSA::sign(PQB::byteBuffer &signature, const PQB::byte *dataToSign, size_t dataSize, const PQB::byteBuffer &privateKey){
    using ECDSA = CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>;
    CryptoPP::AutoSeededRandomPool prng;
    CryptoPP::Integer privKey(privateKey.data(), privateKey.size());

    ECDSA::PrivateKey sk;
    sk.Initialize(CryptoPP::ASN1::secp256k1(), privKey);
    ECDSA::Signer signer(sk);

    size_t signatureLength = signer.MaxSignatureLength();
    signature.resize(signatureLength);
    signatureLength = signer.SignMessage(prng, dataToSign, dataSize, signature.data());
    signature.resize(signatureLength);
    return signatureLength;
}

bool ECDSA::verify(const PQB::byteBuffer &signature, const PQB::byte *signedData, size_t dataSize, const PQB::byteBuffer &publicKey){
    using ECDSA = CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>;
    ECDSA::PublicKey pk;
    CryptoPP::ECP::Point point;
    pk.AccessGroupParameters().Initialize(CryptoPP::ASN1::secp256k1());
    pk.GetGroupParameters().GetCurve().DecodePoint(point, publicKey.data(), publicKey.size());
    pk.Initialize(CryptoPP::ASN1::secp256k1(), point);

    ECDSA::Verifier verifier(pk);
    return verifier.VerifyMessage(signedData, dataSize, signature.data(), signature.size());
}

} // PQB namespace

/* END OF FILE */