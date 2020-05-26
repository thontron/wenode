#pragma once

#include <node/protocol/authority.hpp>
#include <node/protocol/node_operations.hpp>
#include <node/protocol/block.hpp>
#include <node/chain/node_object_types.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <numeric>

namespace node { namespace chain {

   using node::protocol::asset;

   class transfer_request_object : public object< transfer_request_object_type, transfer_request_object >
   {
      transfer_request_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         transfer_request_object( Constructor&& c, allocator< Allocator > a ) :
         request_id(a), memo(a)
         {
            c( *this );
         }

         id_type                  id;

         account_name_type        to;             ///< Account requesting the transfer.
      
         account_name_type        from;           ///< Account that is being requested to accept the transfer.
      
         asset                    amount;         ///< The amount of asset to transfer.

         shared_string            request_id;     ///< uuidv4 of the request transaction.

         shared_string            memo;           ///< The memo is plain-text, encryption on the memo is advised. 

         time_point               expiration;     ///< time that the request expires. 
   };


   class transfer_recurring_object : public object< transfer_recurring_object_type, transfer_recurring_object >
   {
      transfer_recurring_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         transfer_recurring_object( Constructor&& c, allocator< Allocator > a ) :
         transfer_id(a), memo(a)
         {
            c( *this );
         }

         id_type                     id;

         account_name_type           from;                  ///< Sending account to transfer asset from.
      
         account_name_type           to;                    ///< Recieving account to transfer asset to.
      
         asset                       amount;                ///< The amount of asset to transfer for each payment interval.

         shared_string               transfer_id;           ///< uuidv4 of the request transaction.

         shared_string               memo;                  ///< The memo is plain-text, encryption on the memo is advised.

         time_point                  begin;                 ///< Starting time of the first payment.

         uint32_t                    payments;              ///< Number of payments to process in total.

         uint32_t                    payments_remaining;    ///< Number of payments processed so far.

         fc::microseconds            interval;              ///< Microseconds between each transfer event.

         time_point                  end;                   ///< Ending time of the recurring payment. 
   
         time_point                  next_transfer;         ///< Time of the next transfer.    

         bool                        extensible;            ///< True if the payment duration should be extended in the event a payment is missed.

         bool                        fill_or_kill;          ///< True if the payment should be cancelled if a payment is missed.
   };


   class transfer_recurring_request_object : public object< transfer_recurring_request_object_type, transfer_recurring_request_object >
   {
      transfer_recurring_request_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         transfer_recurring_request_object( Constructor&& c, allocator< Allocator > a ) :
         request_id(a), memo(a)
         {
            c( *this );
         }

         id_type                     id;

         account_name_type           from;              ///< Sending account to transfer asset from.
      
         account_name_type           to;                ///< Recieving account to transfer asset to.
      
         asset                       amount;            ///< The amount of asset to transfer for each payment interval.

         shared_string               request_id;        ///< uuidv4 of the request transaction.

         shared_string               memo;              ///< The memo is plain-text, encryption on the memo is advised.

         time_point                  begin;             ///< Starting time of the first payment.

         uint32_t                    payments;          ///< Number of payments to process in total.

         fc::microseconds            interval;          ///< Microseconds between each transfer event.

         time_point                  end;               ///< Ending time of the recurring payment. 

         time_point                  expiration;        ///< time that the request expires. 

         bool                        extensible;        ///< True if the payment duration should be extended in the event a payment is missed.

         bool                        fill_or_kill;      ///< True if the payment should be cancelled if a payment is missed.

         asset                       total_payment()
         {
            fc::microseconds time = end - begin;
            share_type payments = time.count() / interval.count();
            return amount * payments;
         };
   };


   /**
    * Holds an asset balance that can be spent by a public key.
    * 
    * Acts as a UTXO payment mechanism that splits balances
    * into equal chunks that can be shuffled to provide privacy.
    */
   class confidential_balance_object : public object< confidential_balance_object_type, confidential_balance_object >
   {
      confidential_balance_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         confidential_balance_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         id_type                       id;

         authority                     owner;               ///< Owner Authoirty of the confidential balance.

         transaction_id_type           prev;                ///< Transaction ID of the transaction that created the balance.

         uint16_t                      op_in_trx;           ///< Number of the operation in the creating transaction.

         uint16_t                      index;               ///< Number of the balance created from the origin transaction.

         commitment_type               commitment;          ///< Commitment of the Balance.

         asset_symbol_type             symbol;              ///< Asset symbol of the balance.

         time_point                    created;             ///< Time that the balance was created.

         digest_type                   hash()const          ///< Hash of the balance.
         {
            digest_type::encoder enc;
            fc::raw::pack( enc, owner );
            fc::raw::pack( enc, prev );
            fc::raw::pack( enc, op_in_trx );
            fc::raw::pack( enc, index );
            fc::raw::pack( enc, commitment );
            fc::raw::pack( enc, symbol );
            fc::raw::pack( enc, created );
            return enc.result();
         }

         account_name_type             account_auth()const
         {
            return (*owner.account_auths.begin() ).first;
         }

         public_key_type               key_auth()const
         {
            return (*owner.key_auths.begin() ).first;
         }
   };


   struct by_expiration;
   struct by_request_id;
   struct by_from_account;

   typedef multi_index_container<
      transfer_request_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< transfer_request_object, transfer_request_id_type, &transfer_request_object::id > >,
         ordered_non_unique< tag< by_expiration >, member< transfer_request_object, time_point, &transfer_request_object::expiration > >,
         ordered_unique< tag< by_request_id >,
            composite_key< transfer_request_object,
               member< transfer_request_object, account_name_type, &transfer_request_object::to >,
               member< transfer_request_object, shared_string, &transfer_request_object::request_id >
            >,
            composite_key_compare< 
               std::less< account_name_type >, 
               strcmp_less
            >
         >,
         ordered_unique< tag< by_from_account >,
            composite_key< transfer_request_object,
               member< transfer_request_object, account_name_type, &transfer_request_object::from >,
               member< transfer_request_object, transfer_request_id_type, &transfer_request_object::id >
            >,
            composite_key_compare< 
               std::less< account_name_type >, 
               std::less< transfer_request_id_type > 
            >
         >
      >,
      allocator< transfer_request_object >
   > transfer_request_index;

   struct by_end;
   struct by_begin;
   struct by_next_transfer;
   struct by_transfer_id;
   struct by_to_account;

   typedef multi_index_container<
      transfer_recurring_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< transfer_recurring_object, transfer_recurring_id_type, &transfer_recurring_object::id > >,
         ordered_non_unique< tag< by_end >, member< transfer_recurring_object, time_point, &transfer_recurring_object::end > >,
         ordered_non_unique< tag< by_begin >, member< transfer_recurring_object, time_point, &transfer_recurring_object::begin > >,
         ordered_non_unique< tag< by_next_transfer >, member< transfer_recurring_object, time_point, &transfer_recurring_object::next_transfer > >,
         ordered_unique< tag< by_transfer_id >,
            composite_key< transfer_recurring_object,
               member< transfer_recurring_object, account_name_type, &transfer_recurring_object::from >,
               member< transfer_recurring_object, shared_string, &transfer_recurring_object::transfer_id >
            >,
            composite_key_compare<
               std::less< account_name_type >,
               strcmp_less
            >
         >,
         ordered_unique< tag< by_to_account >,
            composite_key< transfer_recurring_object,
               member< transfer_recurring_object, account_name_type, &transfer_recurring_object::to >,
               member< transfer_recurring_object, transfer_recurring_id_type, &transfer_recurring_object::id >
            >,
            composite_key_compare< 
               std::less< account_name_type >,
               std::less< transfer_recurring_id_type >
            >
         >
      >,
      allocator< transfer_recurring_object >
   > transfer_recurring_index;

   struct by_from_account;

   typedef multi_index_container<
      transfer_recurring_request_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< transfer_recurring_request_object, transfer_recurring_request_id_type, &transfer_recurring_request_object::id > >,
         ordered_non_unique< tag< by_expiration >, member< transfer_recurring_request_object, time_point, &transfer_recurring_request_object::expiration > >,
         ordered_unique< tag< by_request_id >,
            composite_key< transfer_recurring_request_object,
               member< transfer_recurring_request_object, account_name_type, &transfer_recurring_request_object::to >,
               member< transfer_recurring_request_object, shared_string, &transfer_recurring_request_object::request_id >
            >,
            composite_key_compare<
               std::less< account_name_type >,
               strcmp_less
            >
         >,
         ordered_unique< tag< by_from_account >,
            composite_key< transfer_recurring_request_object,
               member< transfer_recurring_request_object, account_name_type, &transfer_recurring_request_object::from >,
               member< transfer_recurring_request_object, transfer_recurring_request_id_type, &transfer_recurring_request_object::id >
            >,
            composite_key_compare< 
               std::less< account_name_type >,
               std::less< transfer_recurring_request_id_type >
            >
         >
      >,
      allocator< transfer_recurring_request_object >
   > transfer_recurring_request_index;

   struct by_key;
   struct by_hash;
   struct by_created;
   struct by_commitment;
   struct by_key_auth;
   struct by_account_auth;

   typedef multi_index_container<
      confidential_balance_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< confidential_balance_object, confidential_balance_id_type, &confidential_balance_object::id > >,
         ordered_unique< tag< by_hash >, const_mem_fun< confidential_balance_object, digest_type, &confidential_balance_object::hash > >,
         ordered_non_unique< tag< by_created >, member< confidential_balance_object, time_point, &confidential_balance_object::created > >,
         ordered_unique< tag< by_commitment >, member< confidential_balance_object, commitment_type, &confidential_balance_object::commitment > >,
         ordered_unique< tag< by_key_auth >,
            composite_key< confidential_balance_object,
               const_mem_fun< confidential_balance_object, public_key_type, &confidential_balance_object::key_auth >,
               member< confidential_balance_object, confidential_balance_id_type, &confidential_balance_object::id >
            >,
            composite_key_compare< 
               std::less< public_key_type >,
               std::less< confidential_balance_id_type >
            >
         >,
         ordered_unique< tag< by_account_auth >, 
            composite_key< confidential_balance_object,
               const_mem_fun< confidential_balance_object, account_name_type, &confidential_balance_object::account_auth >,
               member< confidential_balance_object, confidential_balance_id_type, &confidential_balance_object::id >
            >,
            composite_key_compare< 
               std::less< account_name_type >,
               std::less< confidential_balance_id_type >
            >
         >
      >,
      allocator< confidential_balance_object >
   > confidential_balance_index;


} } // node::chain


FC_REFLECT( node::chain::transfer_request_object,
         (id)
         (to)
         (from)
         (amount)
         (request_id)
         (memo)
         (expiration)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::transfer_request_object, node::chain::transfer_request_index );

FC_REFLECT( node::chain::transfer_recurring_object,
         (id)
         (from)
         (to)
         (amount)
         (transfer_id)
         (memo)
         (begin)
         (payments)
         (payments_remaining)
         (interval)
         (end)
         (next_transfer)
         (extensible)
         (fill_or_kill)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::transfer_recurring_object, node::chain::transfer_recurring_index );

FC_REFLECT( node::chain::transfer_recurring_request_object,
         (id)
         (from)
         (to)
         (amount)
         (request_id)
         (memo)
         (begin)
         (payments)
         (interval)
         (end)
         (expiration)
         (extensible)
         (fill_or_kill)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::transfer_recurring_request_object, node::chain::transfer_recurring_request_index );

FC_REFLECT( node::chain::confidential_balance_object,
         (id)
         (owner)
         (prev)
         (op_in_trx)
         (index)
         (commitment)
         (symbol)
         (created)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::confidential_balance_object, node::chain::confidential_balance_index );