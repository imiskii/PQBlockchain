/**
 * @file Consensus.hpp
 * @author Michal Ľaš
 * @brief 
 * @date 2024-03-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once


namespace PQB{


/**
 * "Pre check" - check just the signature and current balance. There can be situation when one account place 2 different transaction and it has
 * enough resources for the frst transaction and second transaction would be invalid, because there won't be enough resources. In this case
 * bot thransations stay walid after this check but after final check just the first transaction remain valid.
 * 
 */
void checkTranscation();

/**
 * Iterate trough consensus transaction pool. Transactions have to by ordered by sender ID and then by its sequence number (if there are multiple
 * transaction from the same sender).
 * For each transaction, check the balance of the sender account and create list (unordered_map: account diff list) of accounts with balance changes. The balance change
 * structure will be similar to AccountDifference structure in AccountStorage.hpp, but balanceDiff will be newBalance (PQB::cash) that should be written
 * to the database. For receiver change account balance as well (just create another record for in account diff list). Then use function from
 * AccountStorage: setBalancesByTxSet(std::set<TransactionPtr, TransactionPtrComparator> &txSet) and update all transactions. IMPORTANT
 * change this function to be for account diff list (unordered_map) like this: setBalancesByTxSet(std::unordered_map<std::string, AccountDifference> &accountDiffs);
 * 
 * If some transaction can not be made because blance of this account would be < 0, then put this transaction to another round
 * of Consensus (There has to be track of how many times one transaction go through the consensus if it will be a lot then discard it).
 * Also check if transaction made with local wallet are not among confirmed transaction, if so make confirmation in wallet.
 * |-> check if senderID is not local walletID if so take transaction ID and processed it in wallet
 * 
 */
void finalTransactionsCheck();


} // namespace PQB

/* END OF FILE */