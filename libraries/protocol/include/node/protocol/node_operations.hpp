#pragma once
#include <node/protocol/base.hpp>
#include <node/protocol/block_header.hpp>
#include <node/protocol/asset.hpp>

#include <fc/utf8.hpp>
#include <fc/crypto/equihash.hpp>

namespace node { namespace protocol {

   inline void validate_account_name( const string& name )
   {
      FC_ASSERT( is_valid_account_name( name ), "Account name ${n} is invalid", ("n", name) );
   }

   inline void validate_permlink( const string& permlink )
   {
      FC_ASSERT( permlink.size() < MAX_PERMLINK_LENGTH, "permlink is too long" );
      FC_ASSERT( fc::is_utf8( permlink ), "permlink not formatted in UTF8" );
   }

   struct accountCreate_operation : public base_operation
   {
      asset             fee;
      account_name_type creator;
      account_name_type newAccountName;
      authority         owner;
      authority         active;
      authority         posting;
      public_key_type   memoKey;
      string            json;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(creator); }
   };


   struct accountCreateWithDelegation_operation : public base_operation
   {
      asset             fee;
      asset             delegation;
      account_name_type creator;
      account_name_type newAccountName;
      authority         owner;
      authority         active;
      authority         posting;
      public_key_type   memoKey;
      string            json;

      extensions_type   extensions;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(creator); }
   };


   struct accountUpdate_operation : public base_operation
   {
      account_name_type             account;
      optional< authority >         owner;
      optional< authority >         active;
      optional< authority >         posting;
      public_key_type               memoKey;
      string                        json;

      void validate()const;

      void get_required_owner_authorities( flat_set<account_name_type>& a )const
      { if( owner ) a.insert( account ); }

      void get_required_active_authorities( flat_set<account_name_type>& a )const
      { if( !owner ) a.insert( account ); }
   };


   struct comment_operation : public base_operation
   {
      account_name_type parent_author;
      string            parent_permlink;

      account_name_type author;
      string            permlink;

      string            title;
      string            body;
      string            json;

      void validate()const;
      void get_required_posting_authorities( flat_set<account_name_type>& a )const{ a.insert(author); }
   };

   struct beneficiary_route_type
   {
      beneficiary_route_type() {}
      beneficiary_route_type( const account_name_type& a, const uint16_t& w ) : account( a ), weight( w ){}

      account_name_type account;
      uint16_t          weight;

      // For use by std::sort such that the route is sorted first by name (ascending)
      bool operator < ( const beneficiary_route_type& o )const { return account < o.account; }
   };

   struct comment_payout_beneficiaries
   {
      vector< beneficiary_route_type > beneficiaries;

      void validate()const;
   };

   typedef static_variant<
            comment_payout_beneficiaries
           > comment_options_extension;

   typedef flat_set< comment_options_extension > comment_options_extensions_type;

   /**
    *  Authors of posts may not want all of the benefits that come from creating a post. This
    *  operation allows authors to update properties associated with their post.
    *
    *  The max_accepted_payout may be decreased, but never increased.
    *  The percent_TSD may be decreased, but never increased
    *
    */
   struct comment_options_operation : public base_operation
   {
      account_name_type author;
      string            permlink;

      asset             max_accepted_payout    = asset( 1000000000, SYMBOL_USD );       /// TSD value of the maximum payout this post will receive
      uint16_t          percent_TSD  = PERCENT_100; /// the percent of TSD to key, unkept amounts will be received as WeYouMe Power
      bool              allow_votes            = true;      /// allows a post to receive votes;
      bool              allow_curationRewards = true; /// allows voters to recieve curation rewards. Rewards return to reward fund.
      comment_options_extensions_type extensions;

      void validate()const;
      void get_required_posting_authorities( flat_set<account_name_type>& a )const{ a.insert(author); }
   };


   struct challenge_authority_operation : public base_operation
   {
      account_name_type challenger;
      account_name_type challenged;
      bool              require_owner = false;

      void validate()const;

      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(challenger); }
   };

   struct prove_authority_operation : public base_operation
   {
      account_name_type challenged;
      bool              require_owner = false;

      void validate()const;

      void get_required_active_authorities( flat_set<account_name_type>& a )const{ if( !require_owner ) a.insert(challenged); }
      void get_required_owner_authorities( flat_set<account_name_type>& a )const{  if(  require_owner ) a.insert(challenged); }
   };


   struct deleteComment_operation : public base_operation
   {
      account_name_type author;
      string            permlink;

      void validate()const;
      void get_required_posting_authorities( flat_set<account_name_type>& a )const{ a.insert(author); }
   };


   struct vote_operation : public base_operation
   {
      account_name_type    voter;
      account_name_type    author;
      string               permlink;
      int16_t              weight = 0;

      void validate()const;
      void get_required_posting_authorities( flat_set<account_name_type>& a )const{ a.insert(voter); }
   };


   /**
    * @ingroup operations
    *
    * @brief Transfers TME from one account to another.
    */
   struct transfer_operation : public base_operation
   {
      account_name_type from;
      /// Account to transfer asset to
      account_name_type to;
      /// The amount of asset to transfer from @ref from to @ref to
      asset             amount;

      /// The memo is plain-text, any encryption on the memo is up to
      /// a higher level protocol.
      string            memo;

      void              validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ if(amount.symbol != SYMBOL_SCORE) a.insert(from); }
      void get_required_owner_authorities( flat_set<account_name_type>& a )const { if(amount.symbol == SYMBOL_SCORE) a.insert(from); }
   };


   /**
    *  The purpose of this operation is to enable someone to send money contingently to
    *  another individual. The funds leave the *from* account and go into a temporary balance
    *  where they are held until *from* releases it to *to* or *to* refunds it to *from*.
    *
    *  In the event of a dispute the *agent* can divide the funds between the to/from account.
    *  Disputes can be raised any time before or on the dispute deadline time, after the escrow
    *  has been approved by all parties.
    *
    *  This operation only creates a proposed escrow transfer. Both the *agent* and *to* must
    *  agree to the terms of the arrangement by approving the escrow.
    *
    *  The escrow agent is paid the fee on approval of all parties. It is up to the escrow agent
    *  to determine the fee.
    *
    *  Escrow transactions are uniquely identified by 'from' and 'escrow_id', the 'escrow_id' is defined
    *  by the sender.
    */
   struct escrow_transfer_operation : public base_operation
   {
      account_name_type from;
      account_name_type to;
      account_name_type agent;
      uint32_t          escrow_id = 30;

      asset             TSDamount = asset( 0, SYMBOL_USD );
      asset             TMEamount = asset( 0, SYMBOL_COIN );
      asset             fee;

      time_point_sec    ratification_deadline;
      time_point_sec    escrow_expiration;

      string            json;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(from); }
   };


   /**
    *  The agent and to accounts must approve an escrow transaction for it to be valid on
    *  the blockchain. Once a part approves the escrow, the cannot revoke their approval.
    *  Subsequent escrow approve operations, regardless of the approval, will be rejected.
    */
   struct escrow_approve_operation : public base_operation
   {
      account_name_type from;
      account_name_type to;
      account_name_type agent;
      account_name_type who; // Either to or agent

      uint32_t          escrow_id = 30;
      bool              approve = true;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(who); }
   };


   /**
    *  If either the sender or receiver of an escrow payment has an issue, they can
    *  raise it for dispute. Once a payment is in dispute, the agent has authority over
    *  who gets what.
    */
   struct escrow_dispute_operation : public base_operation
   {
      account_name_type from;
      account_name_type to;
      account_name_type agent;
      account_name_type who;

      uint32_t          escrow_id = 30;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(who); }
   };


   /**
    *  This operation can be used by anyone associated with the escrow transfer to
    *  release funds if they have permission.
    *
    *  The permission scheme is as follows:
    *  If there is no dispute and escrow has not expired, either party can release funds to the other.
    *  If escrow expires and there is no dispute, either party can release funds to either party.
    *  If there is a dispute regardless of expiration, the agent can release funds to either party
    *     following whichever agreement was in place between the parties.
    */
   struct escrow_release_operation : public base_operation
   {
      account_name_type from;
      account_name_type to; ///< the original 'to'
      account_name_type agent;
      account_name_type who; ///< the account that is attempting to release the funds, determines valid 'receiver'
      account_name_type receiver; ///< the account that should receive funds (might be from, might be to)

      uint32_t          escrow_id = 30;
      asset             TSDamount = asset( 0, SYMBOL_USD ); ///< the amount of TSD to release
      asset             TMEamount = asset( 0, SYMBOL_COIN ); ///< the amount of TME to release

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(who); }
   };


   /**
    *  This operation converts TME into SCORE (SCORE) at
    *  the current exchange rate. With this operation it is possible to
    *  give another account SCORE so that faucets can
    *  pre-fund new accounts with SCORE.
    */
   struct transferTMEtoSCOREfund_operation : public base_operation
   {
      account_name_type from;
      account_name_type to; ///< if null, then same as from
      asset             amount; ///< must be TME

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(from); }
   };


   /**
    * At any given point in time an account can be withdrawing from their
    * SCORE. A user may change the number of SCORE they wish to
    * cash out at any time between 0 and their total SCORE.
    *
    * After applying this operation, SCORE will be withdrawn
    * at a rate of SCORE/104 per week for two years starting
    * one week after this operation is included in the blockchain.
    *
    * This operation is not valid if the user has no SCORE.
    */
   struct withdrawSCORE_operation : public base_operation
   {
      account_name_type account;
      asset             SCORE;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(account); }
   };


   /**
    * Allows an account to setup an SCORE withdraw but with the additional
    * request for the funds to be transferred directly to another account's
    * balance rather than the withdrawing account. In addition, those funds
    * can be immediately scored again, circumventing the conversion from
    * SCORE to TME and back, guaranteeing they maintain their value.
    */
   struct setWithdrawSCOREasTMEroute_operation : public base_operation
   {
      account_name_type from_account;
      account_name_type to_account;
      uint16_t          percent = 0;
      bool              autoSCORE = false;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const { a.insert( from_account ); }
   };


   /**
    * Witnesses must vote on how to set certain chain properties to ensure a smooth
    * and well functioning network.  Any time @owner is in the active set of witnesses these
    * properties will be used to control the blockchain configuration.
    */
   struct chain_properties
   {
      /**
       *  This fee, paid in TME, is converted into SCORE for the new account. Accounts
       *  without SCORE cannot earn usage rations and therefore are powerless. This minimum
       *  fee requires all accounts to have some kind of commitment to the network that includes the
       *  ability to vote and make transactions.
       */
      asset             account_creation_fee =
         asset( MIN_ACCOUNT_CREATION_FEE, SYMBOL_COIN );

      /**
       *  This witnesses vote for the maximum_block_size which is used by the network
       *  to tune rate limiting and capacity
       */
      uint32_t          maximum_block_size = MIN_BLOCK_SIZE_LIMIT * 2;
      uint16_t          TSD_interest_rate  = DEFAULT_TSD_INTEREST_RATE;

      void validate()const
      {
         FC_ASSERT( account_creation_fee.amount >= MIN_ACCOUNT_CREATION_FEE);
         FC_ASSERT( maximum_block_size >= MIN_BLOCK_SIZE_LIMIT);
         FC_ASSERT( TSD_interest_rate >= 0 );
         FC_ASSERT( TSD_interest_rate <= PERCENT_100 );
      }
   };


   /**
    *  Users who wish to become a witness must pay a fee acceptable to
    *  the current witnesses to apply for the position and allow voting
    *  to begin.
    *
    *  If the owner isn't a witness they will become a witness.  Witnesses
    *  are charged a fee equal to 1 weeks worth of witness pay which in
    *  turn is derived from the current share supply.  The fee is
    *  only applied if the owner is not already a witness.
    *
    *  If the block_signing_key is null then the witness is removed from
    *  contention.  The network will pick the top 21 witnesses for
    *  producing blocks.
    */
   struct witness_update_operation : public base_operation
   {
      account_name_type owner;
      string            url;
      public_key_type   block_signing_key;
      chain_properties  props;
      asset             fee; ///< the fee paid to register a new witness, should be 10x current block production pay

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(owner); }
   };


   /**
    * All accounts with a SCORE can vote for or against any witness.
    *
    * If a proxy is specified then all existing votes are removed.
    */
   struct accountWitnessVote_operation : public base_operation
   {
      account_name_type account;
      account_name_type witness;
      bool              approve = true;

      void validate() const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(account); }
   };


   struct account_witness_proxy_operation : public base_operation
   {
      account_name_type account;
      account_name_type proxy;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(account); }
   };


   /**
    * @brief provides a generic way to add higher level protocols on top of witness consensus
    * @ingroup operations
    *
    * There is no validation for this operation other than that required auths are valid
    */
   struct custom_operation : public base_operation
   {
      flat_set< account_name_type > required_auths;
      uint16_t                      id = 0;
      vector< char >                data;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ for( const auto& i : required_auths ) a.insert(i); }
   };


   /** serves the same purpose as custom_operation but also supports required posting authorities. Unlike custom_operation,
    * this operation is designed to be human readable/developer friendly.
    **/
   struct customJson_operation : public base_operation
   {
      flat_set< account_name_type > required_auths;
      flat_set< account_name_type > required_posting_auths;
      string                        id; ///< must be less than 32 characters long
      string                        json; ///< must be proper utf8 / JSON string.

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ for( const auto& i : required_auths ) a.insert(i); }
      void get_required_posting_authorities( flat_set<account_name_type>& a )const{ for( const auto& i : required_posting_auths ) a.insert(i); }
   };


   struct custom_binary_operation : public base_operation
   {
      flat_set< account_name_type > required_owner_auths;
      flat_set< account_name_type > required_active_auths;
      flat_set< account_name_type > required_posting_auths;
      vector< authority >           required_auths;

      string                        id; ///< must be less than 32 characters long
      vector< char >                data;

      void validate()const;
      void get_required_owner_authorities( flat_set<account_name_type>& a )const{ for( const auto& i : required_owner_auths ) a.insert(i); }
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ for( const auto& i : required_active_auths ) a.insert(i); }
      void get_required_posting_authorities( flat_set<account_name_type>& a )const{ for( const auto& i : required_posting_auths ) a.insert(i); }
      void get_required_authorities( vector< authority >& a )const{ for( const auto& i : required_auths ) a.push_back( i ); }
   };


   /**
    *  Feeds can only be published by the top N witnesses which are included in every round and are
    *  used to define the exchange rate between TME and the dollar.
    */
   struct feed_publish_operation : public base_operation
   {
      account_name_type publisher;
      price             exchange_rate;

      void  validate()const;
      void  get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(publisher); }
   };


   /**
    *  This operation instructs the blockchain to start a conversion between TME and TSD,
    *  The funds are deposited after CONVERSION_DELAY
    */
   struct convert_operation : public base_operation
   {
      account_name_type owner;
      uint32_t          requestid = 0;
      asset             amount;

      void  validate()const;
      void  get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(owner); }
   };


   /**
    * This operation creates a limit order and matches it against existing open orders.
    */
   struct limit_order_create_operation : public base_operation
   {
      account_name_type owner;
      uint32_t          orderid = 0; /// an ID assigned by owner, must be unique
      asset             amount_to_sell;
      asset             min_to_receive;
      bool              fill_or_kill = false;
      time_point_sec    expiration = time_point_sec::maximum();

      void  validate()const;
      void  get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(owner); }

      price             get_price()const { return amount_to_sell / min_to_receive; }

      pair< asset_symbol_type, asset_symbol_type > get_market()const
      {
         return amount_to_sell.symbol < min_to_receive.symbol ?
                std::make_pair(amount_to_sell.symbol, min_to_receive.symbol) :
                std::make_pair(min_to_receive.symbol, amount_to_sell.symbol);
      }
   };


   /**
    *  This operation is identical to limit_order_create except it serializes the price rather
    *  than calculating it from other fields.
    */
   struct limit_order_create2_operation : public base_operation
   {
      account_name_type owner;
      uint32_t          orderid = 0; /// an ID assigned by owner, must be unique
      asset             amount_to_sell;
      bool              fill_or_kill = false;
      price             exchange_rate;
      time_point_sec    expiration = time_point_sec::maximum();

      void  validate()const;
      void  get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(owner); }

      price             get_price()const { return exchange_rate; }

      pair< asset_symbol_type, asset_symbol_type > get_market()const
      {
         return exchange_rate.base.symbol < exchange_rate.quote.symbol ?
                std::make_pair(exchange_rate.base.symbol, exchange_rate.quote.symbol) :
                std::make_pair(exchange_rate.quote.symbol, exchange_rate.base.symbol);
      }
   };


   /**
    *  Cancels an order and returns the balance to owner.
    */
   struct limit_order_cancel_operation : public base_operation
   {
      account_name_type owner;
      uint32_t          orderid = 0;

      void  validate()const;
      void  get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(owner); }
   };


   struct pow
   {
      public_key_type worker;
      digest_type     input;
      signature_type  signature;
      digest_type     work;

      void create( const fc::ecc::private_key& w, const digest_type& i );
      void validate()const;
   };


   struct pow_operation : public base_operation
   {
      account_name_type worker_account;
      block_id_type     block_id;
      uint64_t          nonce = 0;
      pow               work;
      chain_properties  props;

      void validate()const;
      fc::sha256 work_input()const;

      const account_name_type& get_worker_account()const { return worker_account; }

      /** there is no need to verify authority, the proof of work is sufficient */
      void get_required_active_authorities( flat_set<account_name_type>& a )const{  }
   };


   struct pow2_input
   {
      account_name_type worker_account;
      block_id_type     prev_block;
      uint64_t          nonce = 0;
   };


   struct pow2
   {
      pow2_input        input;
      uint32_t          pow_summary = 0;

      void create( const block_id_type& prev_block, const account_name_type& account_name, uint64_t nonce );
      void validate()const;
   };

   struct equihash_pow
   {
      pow2_input           input;
      fc::equihash::proof  proof;
      block_id_type        prev_block;
      uint32_t             pow_summary = 0;

      void create( const block_id_type& recent_block, const account_name_type& account_name, uint32_t nonce );
      void validate() const;
   };

   typedef fc::static_variant< pow2, equihash_pow > pow2_work;

   struct pow2_operation : public base_operation
   {
      pow2_work                     work;
      optional< public_key_type >   new_owner_key;
      chain_properties              props;

      void validate()const;

      void get_required_active_authorities( flat_set<account_name_type>& a )const;

      void get_required_authorities( vector< authority >& a )const
      {
         if( new_owner_key )
         {
            a.push_back( authority( 1, *new_owner_key, 1 ) );
         }
      }
   };


   /**
    * This operation is used to report a miner who signs two blocks
    * at the same time. To be valid, the violation must be reported within
    * MAX_WITNESSES blocks of the head block (1 round) and the
    * producer must be in the ACTIVE witness set.
    *
    * Users not in the ACTIVE witness set should not have to worry about their
    * key getting compromised and being used to produced multiple blocks so
    * the attacker can report it and steel their SCORE.
    *
    * The result of the operation is to transfer the full SCORE balance
    * of the block producer to the reporter.
    */
   struct report_over_production_operation : public base_operation
   {
      account_name_type    reporter;
      signed_block_header  first_block;
      signed_block_header  second_block;

      void validate()const;
   };


   /**
    * All account recovery requests come from a listed recovery account. This
    * is secure based on the assumption that only a trusted account should be
    * a recovery account. It is the responsibility of the recovery account to
    * verify the identity of the account holder of the account to recover by
    * whichever means they have agreed upon. The blockchain assumes identity
    * has been verified when this operation is broadcast.
    *
    * This operation creates an account recovery request which the account to
    * recover has 24 hours to respond to before the request expires and is
    * invalidated.
    *
    * There can only be one active recovery request per account at any one time.
    * Pushing this operation for an account to recover when it already has
    * an active request will either update the request to a new new owner authority
    * and extend the request expiration to 24 hours from the current head block
    * time or it will delete the request. To cancel a request, simply set the
    * weight threshold of the new owner authority to 0, making it an open authority.
    *
    * Additionally, the new owner authority must be satisfiable. In other words,
    * the sum of the key weights must be greater than or equal to the weight
    * threshold.
    *
    * This operation only needs to be signed by the the recovery account.
    * The account to recover confirms its identity to the blockchain in
    * the recover account operation.
    */
   struct request_account_recovery_operation : public base_operation
   {
      account_name_type recoveryAccount;       ///< The recovery account is listed as the recovery account on the account to recover.

      account_name_type accountToRecover;     ///< The account to recover. This is likely due to a compromised owner authority.

      authority         new_owner_authority;    ///< The new owner authority the account to recover wishes to have. This is secret
                                                ///< known by the account to recover and will be confirmed in a recover_account_operation

      extensions_type   extensions;             ///< Extensions. Not currently used.

      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert( recoveryAccount ); }

      void validate() const;
   };


   /**
    * Recover an account to a new authority using a previous authority and verification
    * of the recovery account as proof of identity. This operation can only succeed
    * if there was a recovery request sent by the account's recover account.
    *
    * In order to recover the account, the account holder must provide proof
    * of past ownership and proof of identity to the recovery account. Being able
    * to satisfy an owner authority that was used in the past 30 days is sufficient
    * to prove past ownership. The get_owner_history function in the database API
    * returns past owner authorities that are valid for account recovery.
    *
    * Proving identity is an off chain contract between the account holder and
    * the recovery account. The recovery request contains a new authority which
    * must be satisfied by the account holder to regain control. The actual process
    * of verifying authority may become complicated, but that is an application
    * level concern, not a blockchain concern.
    *
    * This operation requires both the past and future owner authorities in the
    * operation because neither of them can be derived from the current chain state.
    * The operation must be signed by keys that satisfy both the new owner authority
    * and the recent owner authority. Failing either fails the operation entirely.
    *
    * If a recovery request was made inadvertantly, the account holder should
    * contact the recovery account to have the request deleted.
    *
    * The two setp combination of the account recovery request and recover is
    * safe because the recovery account never has access to secrets of the account
    * to recover. They simply act as an on chain endorsement of off chain identity.
    * In other systems, a fork would be required to enforce such off chain state.
    * Additionally, an account cannot be permanently recovered to the wrong account.
    * While any owner authority from the past 30 days can be used, including a compromised
    * authority, the account can be continually recovered until the recovery account
    * is confident a combination of uncompromised authorities were used to
    * recover the account. The actual process of verifying authority may become
    * complicated, but that is an application level concern, not the blockchain's
    * concern.
    */
   struct recover_account_operation : public base_operation
   {
      account_name_type accountToRecover;        ///< The account to be recovered

      authority         new_owner_authority;       ///< The new owner authority as specified in the request account recovery operation.

      authority         recent_owner_authority;    ///< A previous owner authority that the account holder will use to prove past ownership of the account to be recovered.

      extensions_type   extensions;                ///< Extensions. Not currently used.

      void get_required_authorities( vector< authority >& a )const
      {
         a.push_back( new_owner_authority );
         a.push_back( recent_owner_authority );
      }

      void validate() const;
   };


   /**
    *  This operation allows recovery_accoutn to change account_to_reset's owner authority to
    *  new_owner_authority after 60 days of inactivity.
    */
   struct reset_account_operation : public base_operation {
      account_name_type reset_account;
      account_name_type account_to_reset;
      authority         new_owner_authority;

      void get_required_active_authorities( flat_set<account_name_type>& a )const { a.insert( reset_account ); }
      void validate()const;
   };

   /**
    * This operation allows 'account' owner to control which account has the power
    * to execute the 'reset_account_operation' after 60 days.
    */
   struct set_reset_account_operation : public base_operation {
      account_name_type account;
      account_name_type current_reset_account;
      account_name_type reset_account;
      void validate()const;
      void get_required_owner_authorities( flat_set<account_name_type>& a )const
      {
         if( current_reset_account.size() )
            a.insert( account );
      }

      void get_required_posting_authorities( flat_set<account_name_type>& a )const
      {
         if( !current_reset_account.size() )
            a.insert( account );
      }
   };


   /**
    * Each account lists another account as their recovery account.
    * The recovery account has the ability to create account_recovery_requests
    * for the account to recover. An account can change their recovery account
    * at any time with a 30 day delay. This delay is to prevent
    * an attacker from changing the recovery account to a malicious account
    * during an attack. These 30 days match the 30 days that an
    * owner authority is valid for recovery purposes.
    *
    * On account creation the recovery account is set either to the creator of
    * the account (The account that pays the creation fee and is a signer on the transaction)
    * or to the empty string if the account was mined. An account with no recovery
    * has the top voted witness as a recovery account, at the time the recover
    * request is created. Note: This does mean the effective recovery account
    * of an account with no listed recovery account can change at any time as
    * witness vote weights. The top voted witness is explicitly the most trusted
    * witness according to stake.
    */
   struct change_recoveryAccount_operation : public base_operation
   {
      account_name_type accountToRecover;     ///< The account that would be recovered in case of compromise
      account_name_type new_recoveryAccount;   ///< The account that creates the recover request
      extensions_type   extensions;             ///< Extensions. Not currently used.

      void get_required_owner_authorities( flat_set<account_name_type>& a )const{ a.insert( accountToRecover ); }
      void validate() const;
   };


   struct transferToSavings_operation : public base_operation {
      account_name_type from;
      account_name_type to;
      asset             amount;
      string            memo;

      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert( from ); }
      void validate() const;
   };


   struct transferFromSavings_operation : public base_operation {
      account_name_type from;
      uint32_t          request_id = 0;
      account_name_type to;
      asset             amount;
      string            memo;

      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert( from ); }
      void validate() const;
   };


   struct cancelTransferFromSavings_operation : public base_operation {
      account_name_type from;
      uint32_t          request_id = 0;

      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert( from ); }
      void validate() const;
   };


   struct decline_voting_rights_operation : public base_operation
   {
      account_name_type account;
      bool              decline = true;

      void get_required_owner_authorities( flat_set<account_name_type>& a )const{ a.insert( account ); }
      void validate() const;
   };

   struct claimRewardBalance_operation : public base_operation
   {
      account_name_type account;
      asset             TMEreward;
      asset             TSDreward;
      asset             SCOREreward;

      void get_required_posting_authorities( flat_set< account_name_type >& a )const{ a.insert( account ); }
      void validate() const;
   };

   /**
    * Delegate SCORE from one account to the other. The SCORE are still owned
    * by the original account, but content voting rights and bandwidth allocation are transferred
    * to the receiving account. This sets the delegation to `SCORE`, increasing it or
    * decreasing it as needed. (i.e. a delegation of 0 removes the delegation)
    *
    * When a delegation is removed the SCORE are placed in limbo for a week to prevent a satoshi
    * of SCORE from voting on the same content twice.
    */
   struct delegateSCORE_operation : public base_operation
   {
      account_name_type delegator;        ///< The account delegating SCORE
      account_name_type delegatee;        ///< The account receiving SCORE
      asset             SCORE;   ///< The amount of SCORE delegated

      void get_required_active_authorities( flat_set< account_name_type >& a ) const { a.insert( delegator ); }
      void validate() const;
   };
} } // node::protocol


FC_REFLECT( node::protocol::transferToSavings_operation, (from)(to)(amount)(memo) )
FC_REFLECT( node::protocol::transferFromSavings_operation, (from)(request_id)(to)(amount)(memo) )
FC_REFLECT( node::protocol::cancelTransferFromSavings_operation, (from)(request_id) )

FC_REFLECT( node::protocol::reset_account_operation, (reset_account)(account_to_reset)(new_owner_authority) )
FC_REFLECT( node::protocol::set_reset_account_operation, (account)(current_reset_account)(reset_account) )


FC_REFLECT( node::protocol::report_over_production_operation, (reporter)(first_block)(second_block) )
FC_REFLECT( node::protocol::convert_operation, (owner)(requestid)(amount) )
FC_REFLECT( node::protocol::feed_publish_operation, (publisher)(exchange_rate) )
FC_REFLECT( node::protocol::pow, (worker)(input)(signature)(work) )
FC_REFLECT( node::protocol::pow2, (input)(pow_summary) )
FC_REFLECT( node::protocol::pow2_input, (worker_account)(prev_block)(nonce) )
FC_REFLECT( node::protocol::equihash_pow, (input)(proof)(prev_block)(pow_summary) )
FC_REFLECT( node::protocol::chain_properties, (account_creation_fee)(maximum_block_size)(TSD_interest_rate) );

FC_REFLECT_TYPENAME( node::protocol::pow2_work )
FC_REFLECT( node::protocol::pow_operation, (worker_account)(block_id)(nonce)(work)(props) )
FC_REFLECT( node::protocol::pow2_operation, (work)(new_owner_key)(props) )

FC_REFLECT( node::protocol::accountCreate_operation,
            (fee)
            (creator)
            (newAccountName)
            (owner)
            (active)
            (posting)
            (memoKey)
            (json) )

FC_REFLECT( node::protocol::accountCreateWithDelegation_operation,
            (fee)
            (delegation)
            (creator)
            (newAccountName)
            (owner)
            (active)
            (posting)
            (memoKey)
            (json)
            (extensions) )

FC_REFLECT( node::protocol::accountUpdate_operation,
            (account)
            (owner)
            (active)
            (posting)
            (memoKey)
            (json) )

FC_REFLECT( node::protocol::transfer_operation, (from)(to)(amount)(memo) )
FC_REFLECT( node::protocol::transferTMEtoSCOREfund_operation, (from)(to)(amount) )
FC_REFLECT( node::protocol::withdrawSCORE_operation, (account)(SCORE) )
FC_REFLECT( node::protocol::setWithdrawSCOREasTMEroute_operation, (from_account)(to_account)(percent)(autoSCORE) )
FC_REFLECT( node::protocol::witness_update_operation, (owner)(url)(block_signing_key)(props)(fee) )
FC_REFLECT( node::protocol::accountWitnessVote_operation, (account)(witness)(approve) )
FC_REFLECT( node::protocol::account_witness_proxy_operation, (account)(proxy) )
FC_REFLECT( node::protocol::comment_operation, (parent_author)(parent_permlink)(author)(permlink)(title)(body)(json) )
FC_REFLECT( node::protocol::vote_operation, (voter)(author)(permlink)(weight) )
FC_REFLECT( node::protocol::custom_operation, (required_auths)(id)(data) )
FC_REFLECT( node::protocol::customJson_operation, (required_auths)(required_posting_auths)(id)(json) )
FC_REFLECT( node::protocol::custom_binary_operation, (required_owner_auths)(required_active_auths)(required_posting_auths)(required_auths)(id)(data) )
FC_REFLECT( node::protocol::limit_order_create_operation, (owner)(orderid)(amount_to_sell)(min_to_receive)(fill_or_kill)(expiration) )
FC_REFLECT( node::protocol::limit_order_create2_operation, (owner)(orderid)(amount_to_sell)(exchange_rate)(fill_or_kill)(expiration) )
FC_REFLECT( node::protocol::limit_order_cancel_operation, (owner)(orderid) )

FC_REFLECT( node::protocol::deleteComment_operation, (author)(permlink) );

FC_REFLECT( node::protocol::beneficiary_route_type, (account)(weight) )
FC_REFLECT( node::protocol::comment_payout_beneficiaries, (beneficiaries) )
FC_REFLECT_TYPENAME( node::protocol::comment_options_extension )
FC_REFLECT( node::protocol::comment_options_operation, (author)(permlink)(max_accepted_payout)(percent_TSD)(allow_votes)(allow_curationRewards)(extensions) )

FC_REFLECT( node::protocol::escrow_transfer_operation, (from)(to)(TSDamount)(TMEamount)(escrow_id)(agent)(fee)(json)(ratification_deadline)(escrow_expiration) );
FC_REFLECT( node::protocol::escrow_approve_operation, (from)(to)(agent)(who)(escrow_id)(approve) );
FC_REFLECT( node::protocol::escrow_dispute_operation, (from)(to)(agent)(who)(escrow_id) );
FC_REFLECT( node::protocol::escrow_release_operation, (from)(to)(agent)(who)(receiver)(escrow_id)(TSDamount)(TMEamount) );
FC_REFLECT( node::protocol::challenge_authority_operation, (challenger)(challenged)(require_owner) );
FC_REFLECT( node::protocol::prove_authority_operation, (challenged)(require_owner) );
FC_REFLECT( node::protocol::request_account_recovery_operation, (recoveryAccount)(accountToRecover)(new_owner_authority)(extensions) );
FC_REFLECT( node::protocol::recover_account_operation, (accountToRecover)(new_owner_authority)(recent_owner_authority)(extensions) );
FC_REFLECT( node::protocol::change_recoveryAccount_operation, (accountToRecover)(new_recoveryAccount)(extensions) );
FC_REFLECT( node::protocol::decline_voting_rights_operation, (account)(decline) );
FC_REFLECT( node::protocol::claimRewardBalance_operation, (account)(TMEreward)(TSDreward)(SCOREreward) )
FC_REFLECT( node::protocol::delegateSCORE_operation, (delegator)(delegatee)(SCORE) );
