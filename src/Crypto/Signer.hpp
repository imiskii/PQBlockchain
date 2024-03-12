/**
 * @file Signer.hpp
 * @author Michal Ľaš
 * @brief Signer class, digital signature algorithms
 * @date 2024-02-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once

#include <string>
#include <vector>
#include <memory>
#include "PQBtypedefs.hpp"
#include "HexBase.hpp"

#include <cryptopp/xed25519.h>
extern "C"{
    #include <falcon1024.h>
}

namespace PQB{

/**
 * @brief Abstract class for signature algorithms, utilize methods that each signature algorithm has to implement
 * 
 */
class SignAlgorithm{
public:
    /**
     * @brief Generate new pair key pair
     * 
     * @param privateKey output buffer for the private key
     * @param publicKey output buffer for the public key
     */
    virtual void genKeys(PQB::byteBuffer& privateKey, PQB::byteBuffer& publicKey) = 0;

    /**
     * @brief 
     * 
     * @param signature output buffer for signature
     * @param dataToSign pointer to the data
     * @param dataSize size of the data
     * @param privateKey hexadecimal representation of the key to make the signature with
     * @return size_t length of the signature. In case of hybrid encryption only the length of the first signature!
     * 
     * @exception PQB::Exceptions::Signer if private key is invalid
     */
    virtual size_t sign(PQB::byteBuffer& signature, const PQB::byte* dataToSign, size_t dataSize, const PQB::byteBuffer& privateKey) = 0;

    /**
     * @brief 
     * 
     * @param signature signature to be verified
     * @param signedData pointer to the data which was signed
     * @param dataSize size of the data
     * @param publicKey hexadecimal representation of the key for signature validation
     * @return true successful validation
     * @return false validation failure
     */
    virtual bool verify(const PQB::byteBuffer& signature, const PQB::byte* signedData, size_t dataSize, const PQB::byteBuffer& publicKey) = 0;
    virtual size_t getPrivateKeySize() = 0;
    virtual size_t getPublicKeySize() = 0;
    virtual size_t getSignatureSize() = 0;
    virtual ~SignAlgorithm() = default;
};


typedef std::shared_ptr<SignAlgorithm> SignAlgorithmPtr;

/**
 * @brief Singleton class, for storing chosen sign algorithm;
 * Algorithms: falcon1024, ed25519
 *
 * 
 * @note The algorithm is chosen by first invocation of GetInstance(chosenAlgorithm) method, by default is used algorithm Falcon-1024.
 * 
 * 
 */
class Signer{
public:
    Signer(Signer &other) = delete;
    void operator=(const Signer &) = delete;

    /**
     * @brief Getter for program Digital signature algorithm. The first call will choose the signature algorithm (default is falcon1024).
     * Other calls will just return this choosen algorithm no metter if different algorithm was specified.
     * 
     * @param chosenAlgorithm Choose digital signature algorithm that will be used. Has effect just at the first call of this method.
     * After first call this parameter makes no other difference in returned digital signature algorithm.
     * @return SignAlgorithmPtr Instance of choosen digital signature algorithm
     */
    static SignAlgorithmPtr GetInstance(std::string chosenAlgorithm = "falcon1024");
};


class Falcon1024 : public SignAlgorithm{
public:
    void genKeys(PQB::byteBuffer& privateKey, PQB::byteBuffer& publicKey) override;
    size_t sign(PQB::byteBuffer& signature, const PQB::byte* dataToSign, size_t dataSize, const PQB::byteBuffer& privateKey) override;
    bool verify(const PQB::byteBuffer& signature, const PQB::byte* signedData, size_t dataSize, const PQB::byteBuffer& publicKey) override;
    size_t getPrivateKeySize() override { return privateKeySize; }
    size_t getPublicKeySize() override { return publicKeySize; }
    size_t getSignatureSize() override { return signatureSize; }
private:
    static constexpr size_t privateKeySize = PQCLEAN_FALCON1024_CLEAN_CRYPTO_SECRETKEYBYTES;
    static constexpr size_t publicKeySize = PQCLEAN_FALCON1024_CLEAN_CRYPTO_PUBLICKEYBYTES;
    static constexpr size_t signatureSize = PQCLEAN_FALCON1024_CLEAN_CRYPTO_BYTES;
};


class Ed25519 : public SignAlgorithm{
public:
    void genKeys(PQB::byteBuffer& privateKey, PQB::byteBuffer& publicKey) override;
    size_t sign(PQB::byteBuffer& signature, const PQB::byte* dataToSign, size_t dataSize, const PQB::byteBuffer& privateKey) override;
    bool verify(const PQB::byteBuffer& signature, const PQB::byte* signedData, size_t dataSize, const PQB::byteBuffer& publicKey) override;
    size_t getPrivateKeySize() override { return privateKeySize; }
    size_t getPublicKeySize() override { return publicKeySize; }
    size_t getSignatureSize() override { return signatureSize; }
private:
    static constexpr size_t privateKeySize = CryptoPP::ed25519PrivateKey::SECRET_KEYLENGTH;
    static constexpr size_t publicKeySize = CryptoPP::ed25519PublicKey::PUBLIC_KEYLENGTH;
    static constexpr size_t signatureSize = CryptoPP::ed25519PrivateKey::SIGNATURE_LENGTH;
};


} // PQB namespace


/* END OF FILE */