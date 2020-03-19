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
   using node::protocol::price;
   using node::protocol::option_strike;
   using node::protocol::asset_unit;
   using node::protocol::authority;
   using node::protocol::signed_transaction;
   using node::protocol::operation;

   using node::protocol::chain_properties;
   using node::protocol::digest_type;
   using node::protocol::public_key_type;
   using node::protocol::version;
   using node::protocol::hardfork_version;

   using node::protocol::price;
   using node::protocol::asset;
   using node::protocol::option_strike;
   using node::protocol::price_feed;
   using node::protocol::asset_unit;

   using node::protocol::share_type;
   using node::protocol::ratio_type;

   using node::protocol::block_id_type;
   using node::protocol::transaction_id_type;
   using node::protocol::chain_id_type;

   using node::protocol::account_name_type;
   using node::protocol::community_name_type;
   using node::protocol::tag_name_type;
   using node::protocol::asset_symbol_type;
   using node::protocol::graph_node_name_type;
   using node::protocol::graph_edge_name_type;
   
   using node::protocol::encrypted_keypair_type;
   using node::protocol::date_type;
  
   using node::protocol::beneficiary_route_type;
   using node::protocol::executive_officer_set;
   using chainbase::shared_vector;

   using node::protocol::community_privacy_type;
   using node::protocol::business_structure_type;
   using node::protocol::membership_tier_type;
   using node::protocol::network_officer_role_type;
   using node::protocol::executive_role_type;
   using node::protocol::proposal_distribution_type;
   using node::protocol::product_sale_type;
   using node::protocol::asset_property_type;
   using node::protocol::ad_format_type;
   using node::protocol::post_format_type;
   using node::protocol::ad_metric_type;
   using node::protocol::connection_tier_type;
   using node::protocol::feed_reach_type;
   using node::protocol::blog_reach_type;
   using node::protocol::sort_time_type;
   using node::protocol::sort_option_type;
   using node::protocol::post_time_type;
   using node::protocol::asset_issuer_permission_flags;
   using node::protocol::community_permission_flags;


   /**
    * Set of Network parameters that are selected by block producers.
    * 
    * Producers vote on how to set certain chain properties
    * to ensure a smooth and well functioning network, 
    * and can be responsive to changing network conditions.
    * 
    * The active set of producers will be used to
    * control the blockchain configuration by
    * selecting the median value of all properties listed.
    */
   class median_chain_property_object : public object< median_chain_property_object_type, median_chain_property_object>
   {
      median_chain_property_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         median_chain_property_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         id_type                id;

         asset                  account_creation_fee = MIN_ACCOUNT_CREATION_FEE;               ///< Minimum fee required to create a new account by staking.

         uint64_t               maximum_block_size = MAX_BLOCK_SIZE;                           ///< The maximum block size of the network in bytes. No Upper bound on block size limit.

         fc::microseconds       pow_target_time = POW_TARGET_TIME;                             ///< The targeted time for each proof of work

         fc::microseconds       pow_decay_time = POW_DECAY_TIME;                               ///< Time over which proof of work output is averaged over

         fc::microseconds       txn_stake_decay_time = TXN_STAKE_DECAY_TIME;                   ///< Time over which transaction stake is averaged over

         uint16_t               escrow_bond_percent = ESCROW_BOND_PERCENT;                     ///< Percentage of an escrow transfer that is deposited for dispute resolution

         uint16_t               credit_interest_rate = CREDIT_INTEREST_RATE;                   ///< The credit interest rate paid to holders of network credit assets.

         uint16_t               credit_open_ratio = CREDIT_OPEN_RATIO;                         ///< The minimum required collateralization ratio for a credit loan to be opened. 

         uint16_t               credit_liquidation_ratio = CREDIT_LIQUIDATION_RATIO;           ///< The minimum permissible collateralization ratio before a loan is liquidated. 

         uint16_t               credit_min_interest = CREDIT_MIN_INTEREST;                     ///< The minimum component of credit pool interest rates. 

         uint16_t               credit_variable_interest = CREDIT_VARIABLE_INTEREST;           ///< The variable component of credit pool interest rates, applied at equal base and borrowed balances.

         uint16_t               market_max_credit_ratio = MARKET_MAX_CREDIT_RATIO;             ///< The maximum percentage of core asset liquidity balances that can be loaned.

         uint16_t               margin_open_ratio = MARGIN_OPEN_RATIO;                         ///< The minimum required collateralization ratio for a credit loan to be opened. 

         uint16_t               margin_liquidation_ratio = MARGIN_LIQUIDATION_RATIO;           ///< The minimum permissible collateralization ratio before a loan is liquidated. 

         uint16_t               maximum_asset_feed_publishers = MAX_ASSET_FEED_PUBLISHERS;     ///< The maximum number of accounts that can publish price feeds for a bitasset.

         asset                  membership_base_price = MEMBERSHIP_FEE_BASE;                   ///< The price for standard membership per month.

         asset                  membership_mid_price = MEMBERSHIP_FEE_MID;                     ///< The price for Mezzanine membership per month.

         asset                  membership_top_price = MEMBERSHIP_FEE_TOP;                     ///< The price for top level membership per month.

         uint32_t               author_reward_percent = AUTHOR_REWARD_PERCENT;                 ///< The percentage of content rewards distributed to post authors.

         uint32_t               vote_reward_percent = VOTE_REWARD_PERCENT;                     ///< The percentage of content rewards distributed to post voters.

         uint32_t               view_reward_percent = VIEW_REWARD_PERCENT;                     ///< The percentage of content rewards distributed to post viewers.

         uint32_t               share_reward_percent = SHARE_REWARD_PERCENT;                   ///< The percentage of content rewards distributed to post sharers.

         uint32_t               comment_reward_percent = COMMENT_REWARD_PERCENT;               ///< The percentage of content rewards distributed to post commenters.

         uint32_t               storage_reward_percent = STORAGE_REWARD_PERCENT;               ///< The percentage of content rewards distributed to viewing supernodes.

         uint32_t               moderator_reward_percent = MODERATOR_REWARD_PERCENT;           ///< The percentage of content rewards distributed to community moderators.

         fc::microseconds       content_reward_decay_rate = CONTENT_REWARD_DECAY_RATE;         ///< The time over which content rewards are distributed

         fc::microseconds       content_reward_interval = CONTENT_REWARD_INTERVAL;             ///< Time taken per distribution of content rewards.

         uint32_t               vote_reserve_rate = VOTE_RESERVE_RATE;                         ///< The number of votes regenerated per day.

         uint32_t               view_reserve_rate = VIEW_RESERVE_RATE;                         ///< The number of views regenerated per day.

         uint32_t               share_reserve_rate = SHARE_RESERVE_RATE;                       ///< The number of shares regenerated per day.

         uint32_t               comment_reserve_rate = COMMENT_RESERVE_RATE;                   ///< The number of comments regenerated per day.

         fc::microseconds       vote_recharge_time = VOTE_RECHARGE_TIME;                       ///< Time taken to fully recharge voting power.

         fc::microseconds       view_recharge_time = VIEW_RECHARGE_TIME;                       ///< Time taken to fully recharge viewing power.

         fc::microseconds       share_recharge_time = SHARE_RECHARGE_TIME;                     ///< Time taken to fully recharge sharing power.

         fc::microseconds       comment_recharge_time = COMMENT_RECHARGE_TIME;                 ///< Time taken to fully recharge commenting power.

         fc::microseconds       curation_auction_decay_time = CURATION_AUCTION_DECAY_TIME;     ///< time of curation reward decay after a post is created. 

         double                 vote_curation_decay = VOTE_CURATION_DECAY;                     ///< Number of votes for the half life of voting curation reward decay.

         double                 view_curation_decay = VIEW_CURATION_DECAY;                     ///< Number of views for the half life of viewer curation reward decay.

         double                 share_curation_decay = SHARE_CURATION_DECAY;                   ///< Number of shares for the half life of sharing curation reward decay.

         double                 comment_curation_decay = COMMENT_CURATION_DECAY;               ///< Number of comments for the half life of comment curation reward decay.

         fc::microseconds       supernode_decay_time = SUPERNODE_DECAY_TIME;                   ///< Amount of time to average the supernode file weight over. 

         uint16_t               enterprise_vote_percent_required = ENTERPRISE_VOTE_THRESHOLD_PERCENT;     ///< Percentage of total voting power required to approve enterprise milestones. 

         uint64_t               maximum_asset_whitelist_authorities = MAX_ASSET_WHITELIST_AUTHORITIES;  ///< The maximum amount of whitelisted or blacklisted authorities for user issued assets 

         uint8_t                max_stake_intervals = MAX_ASSET_STAKE_INTERVALS;               ///< Maximum weeks that an asset can stake over.

         uint8_t                max_unstake_intervals = MAX_ASSET_UNSTAKE_INTERVALS;           ///< Maximum weeks that an asset can unstake over.

         asset                  max_exec_budget = MAX_EXEC_BUDGET;                             ///< Maximum budget that an executive board can claim.
   };

   
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
    * An offer to sell a amount of a asset at a 
    * specified exchange rate by a certain time.
    */
   class limit_order_object : public object< limit_order_object_type, limit_order_object >
   {
      limit_order_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         limit_order_object( Constructor&& c, allocator< Allocator > a ) :
         order_id(a)
         {
            c( *this );
         }

         id_type                id;

         account_name_type      seller;            ///< Selling account name of the trading order.

         shared_string          order_id;          ///< UUIDv4 of the order for each account.

         share_type             for_sale;          ///< asset symbol is sell_price.base.symbol.

         price                  sell_price;        ///< Base price is the asset being sold.

         account_name_type      interface;         ///< The interface account that created the order.

         time_point             created;           ///< Time that the order was created.

         time_point             last_updated;      ///< Time that the order was last modified.

         time_point             expiration;        ///< Expiration time of the order.

         pair< asset_symbol_type, asset_symbol_type > get_market()const
         {
            return sell_price.base.symbol < sell_price.quote.symbol ?
                std::make_pair( sell_price.base.symbol, sell_price.quote.symbol ) :
                std::make_pair( sell_price.quote.symbol, sell_price.base.symbol );
         }

         asset                amount_for_sale()const 
         { 
            return asset( for_sale, sell_price.base.symbol );
         }

         asset                amount_to_receive()const 
         { 
            return amount_for_sale() * sell_price;
         }

         asset_symbol_type    sell_asset()const 
         { 
            return sell_price.base.symbol;
         }

         asset_symbol_type    receive_asset()const 
         { 
            return sell_price.quote.symbol;
         }

         double               real_price()const 
         { 
            return sell_price.to_real();
         }
   };


   /**
    * Creates an object that holds a debt position to issue
    * a market issued asset backed by collateral.
    * Enables a market issued asset to access the call order as a 
    * repurchase order if the call price falls below the 
    * market collateralization requirement.
    */
   class call_order_object : public object< call_order_object_type, call_order_object >
   {
      call_order_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         call_order_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         id_type                 id;

         account_name_type       borrower;                    ///< Account that is the borrower of the stablecoin asset. 

         asset                   collateral;                  ///< call_price.base.symbol, access via get_collateral

         asset                   debt;                        ///< call_price.quote.symbol, access via get_debt

         price                   call_price;                  ///< Collateral / Debt

         optional< uint16_t >    target_collateral_ratio;     ///< Maximum CR to maintain when selling collateral on margin call

         account_name_type       interface;                   ///< The interface account that created the order

         time_point              created;                     ///< Time that the order was created.

         time_point              last_updated;                ///< Time that the order was last modified.

         double                  real_price()const 
         { 
            return call_price.to_real(); 
         }

         pair< asset_symbol_type, asset_symbol_type > get_market()const
         {
            auto tmp = std::make_pair( call_price.base.symbol, call_price.quote.symbol );
            if( tmp.first > tmp.second ) std::swap( tmp.first, tmp.second );
            return tmp;
         }

         asset amount_to_receive()const { return debt; }

         asset amount_for_sale()const { return collateral; }

         asset_symbol_type debt_type()const { return debt.symbol; }

         asset_symbol_type collateral_type()const { return collateral.symbol; }

         price collateralization()const { return collateral / debt; }

         /**
          *  Calculate maximum quantity of debt to cover to satisfy @ref target_collateral_ratio.
          * 
          * 
            target_CR = max( target_CR, MCR )
            target_CR = new_collateral / ( new_debt / feed_price )
                     = ( collateral - max_amount_to_sell ) * feed_price
                        / ( debt - amount_to_get )
                     = ( collateral - max_amount_to_sell ) * feed_price
                        / ( debt - round_down(max_amount_to_sell * match_price ) )
                     = ( collateral - max_amount_to_sell ) * feed_price
                        / ( debt - (max_amount_to_sell * match_price - x) )
            Note: x is the fraction, 0 <= x < 1
            =>
            max_amount_to_sell = ( (debt + x) * target_CR - collateral * feed_price )
                                 / (target_CR * match_price - feed_price)
                              = ( (debt + x) * tCR / DENOM - collateral * fp_debt_amt / fp_coll_amt )
                                 / ( (tCR / DENOM) * (mp_debt_amt / mp_coll_amt) - fp_debt_amt / fp_coll_amt )
                              = ( (debt + x) * tCR * fp_coll_amt * mp_coll_amt - collateral * fp_debt_amt * DENOM * mp_coll_amt)
                                 / ( tCR * mp_debt_amt * fp_coll_amt - fp_debt_amt * DENOM * mp_coll_amt )
            max_debt_to_cover = max_amount_to_sell * match_price
                              = max_amount_to_sell * mp_debt_amt / mp_coll_amt
                              = ( (debt + x) * tCR * fp_coll_amt * mp_debt_amt - collateral * fp_debt_amt * DENOM * mp_debt_amt)
                              / (tCR * mp_debt_amt * fp_coll_amt - fp_debt_amt * DENOM * mp_coll_amt)
          *
          *  @param match_price the matching price if this call order is margin called
          *  @param feed_price median settlement price of debt asset
          *  @param maintenance_collateral_ratio median maintenance collateral ratio of debt asset
          *  @param maintenance_collateralization maintenance collateralization of debt asset
          *  @return maximum amount of debt that can be called
          */
         share_type get_max_debt_to_cover(
            price match_price,
            price feed_price,
            const uint16_t maintenance_collateral_ratio,
            const optional<price>& maintenance_collateralization = optional<price>()
         )const 
         { try {
         
            if( feed_price.base.symbol != call_price.base.symbol )
            {
               feed_price = ~feed_price;      // feed_price is in collateral / debt format
            }
               
            FC_ASSERT( feed_price.base.symbol == call_price.base.symbol &&
               feed_price.quote.symbol == call_price.quote.symbol, 
               "Feed and call price must be the same asset pairing." );

            FC_ASSERT( maintenance_collateralization->base.symbol == call_price.base.symbol && 
               maintenance_collateralization->quote.symbol == call_price.quote.symbol );
            
            if( collateralization() > *maintenance_collateralization )
            {
               return 0;
            }
               
            if( !target_collateral_ratio.valid() ) // target cr is not set
            {
               return debt.amount;
            }
               
            uint16_t tcr = std::max( *target_collateral_ratio, maintenance_collateral_ratio );    // use mcr if target cr is too small

            price target_collateralization = feed_price * ratio_type( tcr, COLLATERAL_RATIO_DENOM );

            if( match_price.base.symbol != call_price.base.symbol )
            {
               match_price = ~match_price;       // match_price is in collateral / debt format
            }

            FC_ASSERT( match_price.base.symbol == call_price.base.symbol
                     && match_price.quote.symbol == call_price.quote.symbol );

            int256_t mp_debt_amt = match_price.quote.amount.value;
            int256_t mp_coll_amt = match_price.base.amount.value;
            int256_t fp_debt_amt = feed_price.quote.amount.value;
            int256_t fp_coll_amt = feed_price.base.amount.value;
            int256_t numerator = fp_coll_amt * mp_debt_amt * debt.amount.value * tcr - fp_debt_amt * mp_debt_amt * collateral.amount.value * COLLATERAL_RATIO_DENOM;

            if( numerator < 0 ) 
            {
               return 0;
            }

            int256_t denominator = fp_coll_amt * mp_debt_amt * tcr - fp_debt_amt * mp_coll_amt * COLLATERAL_RATIO_DENOM;
            if( denominator <= 0 )
            {
               return debt.amount;     // black swan
            } 

            int256_t to_cover_i256 = ( numerator / denominator );
            if( to_cover_i256 >= debt.amount.value )
            { 
               return debt.amount;
            }

            share_type to_cover_amt = static_cast< int64_t >( to_cover_i256 );
            asset to_pay = asset( to_cover_amt, debt_type() ) * match_price;
            asset to_cover = to_pay * match_price;
            to_pay = to_cover.multiply_and_round_up( match_price );

            if( to_cover.amount >= debt.amount || to_pay.amount >= collateral.amount )
            {
               return debt.amount;
            }

            FC_ASSERT( to_pay.amount < collateral.amount && to_cover.amount < debt.amount );
            price new_collateralization = ( collateral - to_pay ) / ( debt - to_cover );

            if( new_collateralization > target_collateralization )
            {
               return to_cover.amount;
            }

            // to_cover is too small due to rounding. deal with the fraction

            numerator += fp_coll_amt * mp_debt_amt * tcr; // plus the fraction
            to_cover_i256 = ( numerator / denominator ) + 1;

            if( to_cover_i256 >= debt.amount.value )
            {
               to_cover_i256 = debt.amount.value;
            }

            to_cover_amt = static_cast< int64_t >( to_cover_i256 );

            asset max_to_pay = ( ( to_cover_amt == debt.amount.value ) ? collateral : asset( to_cover_amt, debt_type() ).multiply_and_round_up( match_price ) );
            if( max_to_pay.amount > collateral.amount )
            { 
               max_to_pay.amount = collateral.amount; 
            }

            asset max_to_cover = ( ( max_to_pay.amount == collateral.amount ) ? debt : ( max_to_pay * match_price ) );

            if( max_to_cover.amount >= debt.amount )
            {
               max_to_pay.amount = collateral.amount;
               max_to_cover.amount = debt.amount;
            }

            if( max_to_pay <= to_pay || max_to_cover <= to_cover ) // strange data. should skip binary search and go on, but doesn't help much
            { 
               return debt.amount; 
            }

            FC_ASSERT( max_to_pay > to_pay && max_to_cover > to_cover );

            asset min_to_pay = to_pay;
            asset min_to_cover = to_cover;

            // try with binary search to find a good value
            // note: actually binary search can not always provide perfect result here,
            //       due to rounding, collateral ratio is not always increasing while to_pay or to_cover is increasing
            bool max_is_ok = false;
            while( true )
            {
               // get the mean
               if( match_price.base.amount < match_price.quote.amount ) // step of collateral is smaller
               {
                  to_pay.amount = ( min_to_pay.amount + max_to_pay.amount + 1 ) / 2; // should not overflow. round up here
                  if( to_pay.amount == max_to_pay.amount )
                     to_cover.amount = max_to_cover.amount;
                  else
                  {
                     to_cover = to_pay * match_price;
                     if( to_cover.amount >= max_to_cover.amount ) // can be true when max_is_ok is false
                     {
                        to_pay.amount = max_to_pay.amount;
                        to_cover.amount = max_to_cover.amount;
                     }
                     else
                     {
                        to_pay = to_cover.multiply_and_round_up( match_price ); // stabilization, no change or become smaller
                        FC_ASSERT( to_pay.amount < max_to_pay.amount );
                     }
                  }
               }
               else // step of debt is smaller or equal
               {
                  to_cover.amount = ( min_to_cover.amount + max_to_cover.amount ) / 2; // should not overflow. round down here
                  if( to_cover.amount == max_to_cover.amount )
                     to_pay.amount = max_to_pay.amount;
                  else
                  {
                     to_pay = to_cover.multiply_and_round_up( match_price );
                     if( to_pay.amount >= max_to_pay.amount ) // can be true when max_is_ok is false
                     {
                        to_pay.amount = max_to_pay.amount;
                        to_cover.amount = max_to_cover.amount;
                     }
                     else
                     {
                        to_cover = to_pay * match_price; // stabilization, to_cover should have increased
                        if( to_cover.amount >= max_to_cover.amount ) // to be safe
                        {
                           to_pay.amount = max_to_pay.amount;
                           to_cover.amount = max_to_cover.amount;
                        }
                     }
                  }
               }

               // check again to see if we've moved away from the minimums, if not, use the maximums directly
               if( to_pay.amount <= min_to_pay.amount || 
                  to_cover.amount <= min_to_cover.amount || 
                  to_pay.amount > max_to_pay.amount || 
                  to_cover.amount > max_to_cover.amount )
               {
                  to_pay.amount = max_to_pay.amount;
                  to_cover.amount = max_to_cover.amount;
               }

               // check the mean
               if( to_pay.amount == max_to_pay.amount && ( max_is_ok || to_pay.amount == collateral.amount ) )
               {
                  return to_cover.amount;
               }

               FC_ASSERT( to_pay.amount < collateral.amount && to_cover.amount < debt.amount );

               price new_collateralization = ( collateral - to_pay ) / ( debt - to_cover );

               // Check whether the result is good
               if( new_collateralization > target_collateralization )
               {
                  if( to_pay.amount == max_to_pay.amount )
                  {
                     return to_cover.amount;
                  }  
                  max_to_pay.amount = to_pay.amount;
                  max_to_cover.amount = to_cover.amount;
                  max_is_ok = true;
               }
               else           // not good
               {
                  if( to_pay.amount == max_to_pay.amount )
                  {
                     break;
                  }
                  min_to_pay.amount = to_pay.amount;
                  min_to_cover.amount = to_cover.amount;
               }
            }

            // be here, max_to_cover is too small due to rounding. search forward
            for( uint64_t d1 = 0, d2 = 1, d3 = 1; ; d1 = d2, d2 = d3, d3 = d1 + d2 ) // 1,1,2,3,5,8,...
            {
               if( match_price.base.amount > match_price.quote.amount ) // step of debt is smaller
               {
                  to_pay.amount += d2;
                  if( to_pay.amount >= collateral.amount )
                  {
                     return debt.amount;
                  }
                  to_cover = to_pay * match_price;
                  if( to_cover.amount >= debt.amount )
                  {
                     return debt.amount;
                  }
                  to_pay = to_cover.multiply_and_round_up( match_price ); // stabilization
                  if( to_pay.amount >= collateral.amount )
                  {
                     return debt.amount;
                  }
               }
               else // step of collateral is smaller or equal
               {
                  to_cover.amount += d2;
                  if( to_cover.amount >= debt.amount )
                  {
                     return debt.amount;
                  }
                  to_pay = to_cover.multiply_and_round_up( match_price );
                  if( to_pay.amount >= collateral.amount )
                  {
                     return debt.amount;
                  }
                  to_cover = to_pay * match_price; // stabilization
                  if( to_cover.amount >= debt.amount )
                  {
                     return debt.amount;
                  }
               }

               FC_ASSERT( to_pay.amount < collateral.amount && to_cover.amount < debt.amount );
               price new_collateralization = ( collateral - to_pay ) / ( debt - to_cover );

               if( new_collateralization > target_collateralization )
               {
                  return to_cover.amount;
               }
            }
      } FC_CAPTURE_AND_RETHROW() }  
   };


   /**
    * Holds an auction order that exchanges at the daily clearing price.
    * 
    * Auction orders match against other auction orders once per day,
    * if the daily clearing price is above the min_exchange_rate of the order.
    */
   class auction_order_object : public object< auction_order_object_type, auction_order_object >
   {
      auction_order_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         auction_order_object( Constructor&& c, allocator< Allocator > a ) :
         order_id(a)
         {
            c( *this );
         }

         id_type                  id;

         account_name_type        owner;                    ///< Owner of the Auction order.

         shared_string            order_id;                 ///< uuidv4 of the order for reference.

         asset                    amount_to_sell;           ///< Amount of asset to sell at auction clearing price.

         price                    min_exchange_rate;        ///< The asset pair price to sell the amount at the auction clearing price.

         account_name_type        interface;                ///< Name of the interface that created the transaction.

         time_point               expiration;               ///< Time that the order expires.

         time_point               created;                  ///< Time that the order was created.

         time_point               last_updated;             ///< Time that the order was last modified.

         asset                    amount_for_sale()const
         { 
            return amount_to_sell;
         }

         asset                    amount_to_receive()const
         { 
            return amount_to_sell * min_exchange_rate;
         }

         asset_symbol_type        sell_asset()const
         { 
            return min_exchange_rate.base.symbol;
         }

         asset_symbol_type        receive_asset()const
         { 
            return min_exchange_rate.quote.symbol;
         }

         double                   real_price()const
         { 
            return min_exchange_rate.to_real();
         }
   };


   /**
    * Holds an option position that issue new units of option contract assets.
    * 
    * Option assets can be exercised by the holder to execute a 
    * trade at the specified strike price before the expiration date.
    * 
    * Can be closed by repaying the option asset back into the order.
    * 
    * Orders are assigned the trade execution if the options 
    * are exercised in order of creation.
    */
   class option_order_object : public object< option_order_object_type, option_order_object >
   {
      option_order_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         option_order_object( Constructor&& c, allocator< Allocator > a ) :
         order_id(a)
         {
            c( *this );
         }

         id_type                      id;

         account_name_type            owner;                    ///< Owner of the Option order.

         shared_string                order_id;                 ///< uuidv4 of the order for reference.

         asset                        amount_to_issue;          ///< Amount of assets to issue covered options contract assets against. Must be a multiple of 100 units.

         asset                        option_position;          ///< Amount of option assets generated by the position. Debt owed by the order. 

         option_strike                strike_price;             ///< The asset pair strike price at which the options can be exercised at any time before expiration.

         account_name_type            interface;                ///< Name of the interface that created the transaction.

         time_point                   created;                  ///< Time that the order was created.

         time_point                   last_updated;             ///< Time that the order was last modified.

         asset                        amount_to_receive()const { return option_position; }

         asset                        amount_for_sale()const { return amount_to_issue; }

         asset_symbol_type            debt_type()const { return option_position.symbol; }

         asset_symbol_type            collateral_type()const { return amount_to_issue.symbol; }

         time_point                   expiration()const { return strike_price.expiration(); }

         price                        option_price()const { return strike_price.strike_price; }
   };


   /**
    *  Tracks bitassets scheduled for force settlement at some point in the future.
    *  On the settlement_date the balance will be converted to the collateral asset
    *  and paid to owner and then this object will be deleted.
    */
   class force_settlement_object : public object< force_settlement_object_type, force_settlement_object >
   {
      force_settlement_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         force_settlement_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         id_type                id;

         account_name_type      owner;             ///< Name of the account that is force settling the asset

         asset                  balance;           ///< Amount of debt asset being settled

         time_point             settlement_date;   ///< Date of asset settlement for collateral

         account_name_type      interface;         ///< The interface account that created the order

         time_point             created;           ///< Time that the settlement was created.

         time_point             last_updated;      ///< Time that the settlement was last modified.

         asset_symbol_type      settlement_asset_symbol()const { return balance.symbol; }
   };

   /**
    * Collateral bids of collateral for debt after a black swan
    * There should only be one collateral_bid_object per asset per account, and
    * only for smartcoin assets that have a global settlement_price.
    */
   class collateral_bid_object : public object< collateral_bid_object_type, collateral_bid_object >
   {
      collateral_bid_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         collateral_bid_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         id_type               id;

         account_name_type     bidder;           ///< Bidding Account name.

         asset                 collateral;       ///< Collateral bidded to obtain debt from a global settlement.

         asset                 debt;             ///< Debt requested for bid.

         time_point            created;          ///< Time that the bid was created.

         time_point            last_updated;     ///< Time that the bid was last adjusted.

         price                 inv_swan_price()const
         {
            return price( collateral, debt );    ///< Collateral / Debt.
         }

         asset_symbol_type     debt_type()const 
         { 
            return debt.symbol;
         } 
   };


   /**
    * Credit collateral object holds assets in collateral for use to support a
    * credit borrowing order from the asset's credit pool, or a margin order by substracting collateral from 
    * the object to include in a margin order's collateral.
    */
   class credit_collateral_object : public object< credit_collateral_object_type, credit_collateral_object >
   {
      credit_collateral_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         credit_collateral_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         id_type                    id;

         account_name_type          owner;               ///< Collateral owners account name.

         asset_symbol_type          symbol;              ///< Asset symbol being collateralized. 

         asset                      collateral;          ///< Asset balance that is being locked in for loan backing for loan or margin orders.

         time_point                 created;             ///< Time that collateral was created.

         time_point                 last_updated;        ///< Time that the order was last modified.
   };


   /**
    * Credit collateral object holds assets in collateral for use to support a
    * credit borrowing order from the asset's credit pool, 
    * or a margin order by substracting collateral from 
    * the object to include in a margin order's collateral.
    */
   class credit_loan_object : public object< credit_loan_object_type, credit_loan_object >
   {
      credit_loan_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         credit_loan_object( Constructor&& c, allocator< Allocator > a ) :
         loan_id(a)
         {
            c( *this );
         }

         id_type                    id;

         account_name_type          owner;                   ///< Collateral owner's account name

         shared_string              loan_id;                 ///< UUIDV4 for the loan to uniquely identify it for reference. 

         asset                      debt;                    ///< Amount of an asset borrowed. Limit of 75% of collateral value. Increases with interest charged.

         asset                      interest;                ///< Total Amount of interest accrued on the loan. 

         asset                      collateral;              ///< Amount of an asset to use as collateral for the loan. 

         price                      loan_price;              ///< Collateral / Debt. Must be higher than liquidation price to remain solvent. 

         price                      liquidation_price;       ///< Collateral / max_debt value. Rises when collateral/debt market price falls.

         asset_symbol_type          symbol_a;                ///< The symbol of asset A in the debt / collateral exchange pair.

         asset_symbol_type          symbol_b;                ///< The symbol of asset B in the debt / collateral exchange pair.

         share_type                 last_interest_rate;      ///< Updates the interest rate of the loan hourly. 

         time_point                 created;                 ///< Time that the loan was taken out.

         time_point                 last_updated;            ///< Time that the loan details was last updated.

         time_point                 last_interest_time;      ///< Time that interest was last compounded. 

         asset_symbol_type          debt_asset()const { return debt.symbol; }

         asset_symbol_type          collateral_asset()const { return collateral.symbol; }

         price                      liquidation_spread()const { return loan_price - liquidation_price; }

         pair< asset_symbol_type, asset_symbol_type > get_market()const
         {
            return debt.symbol < collateral.symbol ?
                std::make_pair( debt.symbol, collateral.symbol ) :
                std::make_pair( collateral.symbol, debt.symbol );
         }
   };


   /**
    * Margin order object creates a borrowing balance in a specific asset, 
    * then uses that asset to buy another asset, the position asset.
    */
   class margin_order_object : public object< margin_order_object_type, margin_order_object >
   {
      margin_order_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         margin_order_object( Constructor&& c, allocator< Allocator > a ) :
         order_id(a)
         {
            c( *this );
         }

         id_type                    id;

         account_name_type          owner;                       ///< Margin order owners account name

         shared_string              order_id;                    ///< UUIDv4 Unique Identifier of the order for each account.

         price                      sell_price;                  ///< limit exchange price of the borrowed asset being sold for the position asset.

         asset                      collateral;                  ///< Collateral asset used to back the loan value; Returned to credit collateral object when position is closed. 

         asset                      debt;                        ///< Amount of asset borrowed to purchase the position asset. Repaid when the margin order is closed. 

         asset                      debt_balance;                ///< Debt asset that is held by the order when selling debt, or liquidating position.

         asset                      interest;                    ///< Amount of interest accrued on the borrowed asset into the debt value.

         asset                      position;                    ///< Minimum amount of asset to receive as margin position.

         asset                      position_balance;            ///< Amount of asset currently held within the order that has filled.                     

         share_type                 collateralization;           ///< Percentage ratio of ( Collateral + position_balance + debt_balance - debt ) / debt. Position is liquidated when ratio falls below liquidation requirement 

         account_name_type          interface;                   ///< The interface account that created the order.

         time_point                 created;                     ///< Time that the order was created.

         time_point                 last_updated;                ///< Time that the details of the order was last updated.

         time_point                 last_interest_time;          ///< Time that interest was last compounded on the margin order. 

         time_point                 expiration;                  ///< Expiration time of the order.

         asset                      unrealized_value = asset(0, debt.symbol);   ///< Current profit or loss that the position is holding.

         share_type                 last_interest_rate = 0;      ///< The interest rate that was last applied to the order.

         bool                       liquidating = false;         ///< Set to true to place the margin order back into the orderbook and liquidate the position at sell price.

         price                      stop_loss_price = price::min(sell_price.base.symbol, sell_price.quote.symbol);          ///< Price at which the position will be force liquidated if it falls into a net loss.

         price                      take_profit_price = price::max(sell_price.base.symbol, sell_price.quote.symbol);         ///< Price at which the position will be force liquidated if it rises into a net profit.

         price                      limit_stop_loss_price = price::min(sell_price.base.symbol, sell_price.quote.symbol);     ///< Price at which the position will be limit liquidated if it falls into a net loss.

         price                      limit_take_profit_price = price::max(sell_price.base.symbol, sell_price.quote.symbol);   ///< Price at which the position will be limit liquidated if it rises into a net profit.

         pair< asset_symbol_type, asset_symbol_type > get_market()const
         {
            return sell_price.base.symbol < sell_price.quote.symbol ?
                std::make_pair( sell_price.base.symbol, sell_price.quote.symbol ) :
                std::make_pair( sell_price.quote.symbol, sell_price.base.symbol );
         }

         asset_symbol_type debt_asset()const
         { 
            return debt.symbol;
         }

         asset_symbol_type position_asset()const 
         { 
            return position.symbol; 
         }

         asset_symbol_type collateral_asset()const
         { 
            return collateral.symbol; 
         } 

         asset                amount_for_sale()const   
         { 
            if( liquidating )
            {
               return position_balance;
            }
            else
            {
               return debt_balance;
            }
         }

         asset                amount_to_receive()const { return amount_for_sale() * sell_price; }

         bool                 filled()const
         { 
            if( liquidating )
            {
               return position_balance.amount == 0;    ///< No position left to sell
            }
            else
            {
               return debt_balance.amount == 0;     ///< No debt left to sell.
            }
         }

         asset_symbol_type    sell_asset()const
         { 
            return sell_price.base.symbol; 
         }

         asset_symbol_type    receive_asset()const
         { 
            return sell_price.quote.symbol; 
         }

         double               real_price()const
         { 
            return sell_price.to_real(); 
         }
   };


   class escrow_object : public object< escrow_object_type, escrow_object >
   {
      escrow_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         escrow_object( Constructor&& c, allocator< Allocator > a ) :
         escrow_id(a), memo(a), json(a)
         {
            c( *this );
         }

         id_type                                   id;

         account_name_type                         from;                   ///< Account sending funds

         account_name_type                         to;                     ///< Account receiving funds

         account_name_type                         from_mediator;          ///< Representative of the sending account

         account_name_type                         to_mediator;            ///< Representative of the receiving account

         asset                                     payment;                ///< Total payment to be transferred

         asset                                     balance;                ///< Current funds deposited in the escrow

         shared_string                             escrow_id;              ///< uuidv4 referring to the escrow payment

         shared_string                             memo;                   ///< Details of the transaction for reference. 

         shared_string                             json;                   ///< Additonal JSON object attribute details

         time_point                                acceptance_time;        ///< time that the transfer must be approved by

         time_point                                escrow_expiration;      ///< Time that the escrow is able to be claimed by either TO or FROM

         time_point                                dispute_release_time;   ///< Time that the balance is distributed to median release percentage

         flat_set< account_name_type >             mediators;              ///< Set of accounts able to mediate the dispute

         flat_map< account_name_type, uint16_t >   release_percentages;    ///< Declared release percentages of all accounts

         flat_map< account_name_type, bool >       approvals;              ///< Map of account approvals, paid into balance

         time_point                                created;                ///< Time that the order was created

         time_point                                last_updated;           ///< Time that the order was last updated, approved, or disputed

         bool                                      disputed = false;       ///< True when escrow is in dispute

         bool                                      from_approved()const    ///< True when the from account has approved
         {
            return approvals.at( from );
         };

         bool                                      to_approved()const      ///< True when the to account has approved
         {
            return approvals.at( to );
         };

         bool                                      from_mediator_approved()const    ///< True when the mediator added by from account has approved
         {
            if( from_mediator != account_name_type() )
            {
               return approvals.at( from_mediator );
            }
            else
            {
               return false;
            }
         };

         bool                                      to_mediator_approved()const    ///< True when the mediator added by to account has approved
         {
            if( to_mediator != account_name_type() )
            {
               return approvals.at( to_mediator );
            }
            else
            {
               return false;
            }
         };

         bool                                      is_approved()const      ///< True when all participating accounts have approved
         { 
            return from_approved() && to_approved() && from_mediator_approved() && to_mediator_approved();
         };

         bool                                      is_mediator( const account_name_type& account )const    ///< True when an acocunt is a mediator
         {
            return std::find( mediators.begin(), mediators.end(), account ) != mediators.end();
         };
   };


   class product_object : public object< product_object_type, product_object >
   {
      product_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         product_object( Constructor&& c, allocator< Allocator > a ) :
         product_id(a), name(a), product_variants(a), details(a), images(a), product_prices(a), stock_available(a), json(a), url(a), delivery_variants(a), delivery_prices(a)
         {
            c( *this );
         }

         id_type                                      id;

         account_name_type                            account;             ///< The Seller of the product.

         shared_string                                product_id;          ///< uuidv4 referring to the product. Unique for each account.

         shared_string                                name;                ///< The Retail market facing name of the product. 

         flat_set< shared_string >                    product_variants;    ///< The collection of product variants. Each map must have a key for each variant.

         product_sale_type                            sale_type;           ///< The type of sale to be used for the product.

         flat_map< shared_string, shared_string >     details;             ///< The Description details of each variant of the product.

         flat_map< shared_string, shared_string >     images;              ///< IPFS references to images of each product variant.

         flat_map< shared_string, shared_string >     product_prices;      ///< The price (or min auction price) for each variant of the product.

         flat_map< shared_string, uint32_t >          stock_available;     ///< The available stock of each variant of the product.

         flat_map< shared_string, shared_string >     json;                ///< JSON metadata attributes of each product variant.

         shared_string                                url;                 ///< Reference URL of the product or seller.

         flat_set< shared_string >                    delivery_variants;   ///< The types of product delivery available to purchasers.

         flat_map< shared_string, asset >             delivery_prices;     ///< The price for each variant of delivery.

         time_point                                   created;             ///< Time that the order was created.

         time_point                                   last_updated;        ///< Time that the order was last updated, approved, or disputed.
   };


   class savings_withdraw_object : public object< savings_withdraw_object_type, savings_withdraw_object >
   {
      savings_withdraw_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         savings_withdraw_object( Constructor&& c, allocator< Allocator > a ) :
         memo(a), request_id(a)
         {
            c( *this );
         }

         id_type                 id;

         account_name_type       from;         ///< Account that is withdrawing savings balance

         account_name_type       to;           ///< Account to direct withdrawn assets to

         shared_string           memo;         ///< Reference memo for the transaction

         shared_string           request_id;   ///< uuidv4 reference of the withdrawl instance

         asset                   amount;       ///< Amount to withdraw
         
         time_point              complete;     ///< Time that the withdrawal will complete
   };


   /**
    * A route to send unstaked assets.
    */
   class unstake_asset_route_object : public object< unstake_asset_route_object_type, unstake_asset_route_object >
   {
      unstake_asset_route_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         unstake_asset_route_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         id_type                 id;

         account_name_type       from;                 ///< Account that is unstaking the asset balance

         account_name_type       to;                    ///< Account name that receives the unstaked assets

         asset_symbol_type       symbol;                ///< Asset to be unstaked

         uint16_t                percent = 0;           ///< Percentage of unstaking asset that should be directed to this route.

         bool                    auto_stake = false;    ///< Automatically stake the asset on the receiving account
   };


   class decline_voting_rights_request_object : public object< decline_voting_rights_request_object_type, decline_voting_rights_request_object >
   {
      decline_voting_rights_request_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         decline_voting_rights_request_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         id_type             id;

         account_name_type   account;
         
         time_point          effective_date;
   };


   enum curve_id
   {
      quadratic,                   ///< Returns the square of the reward, with a constant added
      quadratic_curation,          ///< Returns an amount converging to linear with reward
      linear,                      ///< Returns exactly the reward, without using constant
      square_root,                 ///< returns exactly the square root of reward
      convergent_semi_quadratic    ///< Returns an amount converging to the reward, to the power of 1.5, which decays over the time period specified
   };


   class reward_fund_object : public object< reward_fund_object_type, reward_fund_object >
   {
      reward_fund_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         reward_fund_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         id_type                 id;

         asset_symbol_type       symbol;                                                    ///< Currency symbol of the asset that the reward fund issues.

         asset                   content_reward_balance;                                    ///< Balance awaiting distribution to content creators.

         asset                   validation_reward_balance;                                 ///< Balance distributed to block validating producers. 

         asset                   txn_stake_reward_balance;                                  ///< Balance distributed to block producers based on the stake weighted transactions in each block.

         asset                   work_reward_balance;                                       ///< Balance distributed to each proof of work block producer. 

         asset                   producer_activity_reward_balance;                          ///< Balance distributed to producers that receive activity reward votes.

         asset                   supernode_reward_balance;                                  ///< Balance distributed to supernodes, based on stake weighted comment views.

         asset                   power_reward_balance;                                      ///< Balance distributed to staked units of the currency.

         asset                   community_fund_balance;                                    ///< Balance distributed to community proposal funds on the currency. 

         asset                   development_reward_balance;                                ///< Balance distributed to elected developers. 

         asset                   marketing_reward_balance;                                  ///< Balance distributed to elected marketers. 

         asset                   advocacy_reward_balance;                                   ///< Balance distributed to elected advocates. 

         asset                   activity_reward_balance;                                   ///< Balance distributed to content creators that are active each day. 

         asset                   premium_partners_fund_balance;                             ///< Receives income from memberships, distributed to premium creators. 

         asset                   total_pending_reward_balance;                              ///< Total of all reward balances. 

         uint128_t               recent_content_claims = 0;                                 ///< Recently claimed content reward balance shares.

         uint128_t               recent_activity_claims = 0;                                ///< Recently claimed activity reward balance shares.

         uint128_t               content_constant = CONTENT_CONSTANT;                       ///< Contstant added to content claim shares.

         fc::microseconds        content_reward_decay_rate = CONTENT_REWARD_DECAY_RATE;     ///< Time taken to distribute all content rewards.

         fc::microseconds        content_reward_interval = CONTENT_REWARD_INTERVAL;         ///< Time between each individual distribution of content rewards. 

         curve_id                author_reward_curve = convergent_semi_quadratic;           ///< Type of reward curve used for author content reward calculation. 

         curve_id                curation_reward_curve = convergent_semi_quadratic;         ///< Type of reward curve used for curation content reward calculation.

         time_point              last_updated;                                              ///< Time that the reward fund was last updated. 

         void                    adjust_content_reward_balance( const asset& delta )
         { try {
            FC_ASSERT( delta.symbol == symbol );
            content_reward_balance += delta;
            total_pending_reward_balance += delta;
         } FC_CAPTURE_AND_RETHROW() }

         void                    adjust_validation_reward_balance( const asset& delta )
         { try {
            FC_ASSERT( delta.symbol == symbol );
            validation_reward_balance += delta;
            total_pending_reward_balance += delta;
         } FC_CAPTURE_AND_RETHROW() }

         void                    adjust_txn_stake_reward_balance( const asset& delta )
         { try {
            FC_ASSERT( delta.symbol == symbol );
            txn_stake_reward_balance += delta;
            total_pending_reward_balance += delta;
         } FC_CAPTURE_AND_RETHROW() }

         void                    adjust_work_reward_balance( const asset& delta )
         { try {
            FC_ASSERT( delta.symbol == symbol );
            work_reward_balance += delta;
            total_pending_reward_balance += delta;
         } FC_CAPTURE_AND_RETHROW() }

         void                    adjust_producer_activity_reward_balance( const asset& delta )
         { try {
            FC_ASSERT( delta.symbol == symbol );
            activity_reward_balance += delta;
            total_pending_reward_balance += delta;
         } FC_CAPTURE_AND_RETHROW() }

         void                    adjust_supernode_reward_balance( const asset& delta )
         { try {
            FC_ASSERT( delta.symbol == symbol );
            supernode_reward_balance += delta;
            total_pending_reward_balance += delta;
         } FC_CAPTURE_AND_RETHROW() }

         void                    adjust_power_reward_balance( const asset& delta )
         { try {
            FC_ASSERT( delta.symbol == symbol );
            power_reward_balance += delta;
            total_pending_reward_balance += delta;
         } FC_CAPTURE_AND_RETHROW() }

         void                    adjust_community_fund_balance( const asset& delta )
         { try {
            FC_ASSERT( delta.symbol == symbol );
            community_fund_balance += delta;
            total_pending_reward_balance += delta;
         } FC_CAPTURE_AND_RETHROW() }

         void                    adjust_development_reward_balance( const asset& delta )
         { try {
            FC_ASSERT( delta.symbol == symbol );
            development_reward_balance += delta;
            total_pending_reward_balance += delta;
         } FC_CAPTURE_AND_RETHROW() }

         void                    adjust_marketing_reward_balance( const asset& delta )
         { try {
            FC_ASSERT( delta.symbol == symbol );
            marketing_reward_balance += delta;
            total_pending_reward_balance += delta;
         } FC_CAPTURE_AND_RETHROW() }

         void                    adjust_advocacy_reward_balance( const asset& delta )
         { try {
            FC_ASSERT( delta.symbol == symbol );
            advocacy_reward_balance += delta;
            total_pending_reward_balance += delta;
         } FC_CAPTURE_AND_RETHROW() }

         void                    adjust_activity_reward_balance( const asset& delta )
         { try {
            FC_ASSERT( delta.symbol == symbol );
            activity_reward_balance += delta;
            total_pending_reward_balance += delta;
         } FC_CAPTURE_AND_RETHROW() }

         void                    adjust_premium_partners_fund_balance( const asset& delta )
         { try {
            FC_ASSERT( delta.symbol == symbol );
            premium_partners_fund_balance += delta;
            total_pending_reward_balance += delta;
         } FC_CAPTURE_AND_RETHROW() }
   };


   typedef multi_index_container<
      median_chain_property_object,
      indexed_by<
         ordered_unique< tag< by_id >,
            member< median_chain_property_object, median_chain_property_id_type, &median_chain_property_object::id > >
      >,
      allocator< median_chain_property_object >
   > median_chain_property_index;


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

   struct by_price;
   struct by_account;
   
   typedef multi_index_container<
      limit_order_object,
      indexed_by<
         ordered_unique< tag< by_id >, 
            member< limit_order_object, limit_order_id_type, &limit_order_object::id > >,
         ordered_non_unique< tag< by_expiration >, 
            member< limit_order_object, time_point, &limit_order_object::expiration > >,
         ordered_unique< tag< by_price >,
            composite_key< limit_order_object,
               member< limit_order_object, price, &limit_order_object::sell_price >,
               member< limit_order_object, limit_order_id_type, &limit_order_object::id >
            >,
            composite_key_compare< 
               std::greater< price >,
               std::less< limit_order_id_type >
            >
         >,
         ordered_unique< tag< by_account >,
            composite_key< limit_order_object,
               member< limit_order_object, account_name_type, &limit_order_object::seller >,
               member< limit_order_object, shared_string, &limit_order_object::order_id >
            >,
            composite_key_compare< 
               std::less< account_name_type >, 
               strcmp_less 
            >
         >
      >,
      allocator< limit_order_object >
   > limit_order_index;

   struct by_account;
   struct by_debt;
   struct by_collateral;
   struct by_position;
   struct by_collateralization;
   struct by_debt_collateral_position;

   typedef multi_index_container<
      margin_order_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< margin_order_object, margin_order_id_type, &margin_order_object::id > >,
         ordered_non_unique< tag< by_expiration >, member< margin_order_object, time_point, &margin_order_object::expiration > >,
         ordered_unique< tag< by_price >,
            composite_key< margin_order_object,
               const_mem_fun< margin_order_object, bool, &margin_order_object::filled >,
               member< margin_order_object, price, &margin_order_object::sell_price >,
               member< margin_order_object, margin_order_id_type, &margin_order_object::id >
            >,
            composite_key_compare<
               std::less< bool >,
               std::greater< price >,
               std::less< margin_order_id_type >
            >
         >,
         ordered_unique< tag< by_account >,
            composite_key< margin_order_object,
               member< margin_order_object, account_name_type, &margin_order_object::owner >,
               member< margin_order_object, shared_string, &margin_order_object::order_id >
            >,
            composite_key_compare< 
               std::less< account_name_type >, 
               strcmp_less 
            >
         >,
         ordered_unique< tag< by_debt >,
            composite_key< margin_order_object,
               const_mem_fun< margin_order_object, asset_symbol_type, &margin_order_object::debt_asset >,
               member< margin_order_object, margin_order_id_type, &margin_order_object::id >
            >,
            composite_key_compare< 
               std::less< asset_symbol_type >, 
               std::less< margin_order_id_type > 
            >
         >,
         ordered_unique< tag< by_position >,
            composite_key< margin_order_object,
               const_mem_fun< margin_order_object, asset_symbol_type, &margin_order_object::position_asset >,
               member< margin_order_object, margin_order_id_type, &margin_order_object::id >
            >,
            composite_key_compare< 
               std::less< asset_symbol_type >, 
               std::less< margin_order_id_type > 
            >
         >,
         ordered_unique< tag< by_collateral >,
            composite_key< margin_order_object,
               const_mem_fun< margin_order_object, asset_symbol_type, &margin_order_object::collateral_asset >,
               member< margin_order_object, margin_order_id_type, &margin_order_object::id >
            >,
            composite_key_compare< 
               std::less< asset_symbol_type >,
               std::less< margin_order_id_type >
            >
         >,
         ordered_unique< tag< by_debt_collateral_position >,
            composite_key< margin_order_object,
               const_mem_fun< margin_order_object, asset_symbol_type, &margin_order_object::debt_asset >,
               const_mem_fun< margin_order_object, asset_symbol_type, &margin_order_object::collateral_asset >,
               const_mem_fun< margin_order_object, asset_symbol_type, &margin_order_object::position_asset >,
               member< margin_order_object, margin_order_id_type, &margin_order_object::id >
            >,
            composite_key_compare< 
               std::less< asset_symbol_type >, 
               std::less< asset_symbol_type >, 
               std::less< asset_symbol_type >, 
               std::less< margin_order_id_type > 
            >
         >,
         ordered_unique< tag< by_collateralization >,
            composite_key< margin_order_object,
               member< margin_order_object, share_type, &margin_order_object::collateralization >,
               member< margin_order_object, margin_order_id_type, &margin_order_object::id >
            >,
            composite_key_compare< 
               std::less< share_type >, 
               std::less< margin_order_id_type > 
            >
         >
      >,
      allocator< margin_order_object >
   > margin_order_index;

   struct by_owner;
   struct by_conversion_date;
   struct by_volume_weight;
   struct by_withdraw_route;
   struct by_destination;

   typedef multi_index_container<
      unstake_asset_route_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< unstake_asset_route_object, unstake_asset_route_id_type, &unstake_asset_route_object::id > >,
         ordered_unique< tag< by_withdraw_route >,
            composite_key< unstake_asset_route_object,
               member< unstake_asset_route_object, account_name_type, &unstake_asset_route_object::from >,
               member< unstake_asset_route_object, account_name_type, &unstake_asset_route_object::to >
            >,
            composite_key_compare< 
               std::less< account_name_type >, 
               std::less< account_name_type > 
            >
         >,
         ordered_unique< tag< by_destination >,
            composite_key< unstake_asset_route_object,
               member< unstake_asset_route_object, account_name_type, &unstake_asset_route_object::to >,
               member< unstake_asset_route_object, unstake_asset_route_id_type, &unstake_asset_route_object::id >
            >,
            composite_key_compare< 
               std::less< account_name_type >,
               std::less< unstake_asset_route_id_type >
            >
         >
      >,
      allocator< unstake_asset_route_object >
   > unstake_asset_route_index;

   struct by_from_id;
   struct by_to;
   struct by_acceptance_time;
   struct by_dispute_release_time;
   struct by_balance;

   typedef multi_index_container<
      escrow_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< escrow_object, escrow_id_type, &escrow_object::id > >,
         ordered_unique< tag< by_from_id >,
            composite_key< escrow_object,
               member< escrow_object, account_name_type,  &escrow_object::from >,
               member< escrow_object, shared_string, &escrow_object::escrow_id >
            >,
            composite_key_compare< 
               std::less< account_name_type >, 
               strcmp_less 
            >
         >,
         ordered_unique< tag< by_to >,
            composite_key< escrow_object,
               member< escrow_object, account_name_type, &escrow_object::to >,
               member< escrow_object, escrow_id_type, &escrow_object::id >
            >,
            composite_key_compare< 
               std::less< account_name_type >, 
               std::less< escrow_id_type >
            >
         >,
         ordered_unique< tag< by_acceptance_time >,
            composite_key< escrow_object,
               const_mem_fun< escrow_object, bool, &escrow_object::is_approved >,
               member< escrow_object, time_point, &escrow_object::acceptance_time >,
               member< escrow_object, escrow_id_type, &escrow_object::id >
            >,
            composite_key_compare< 
               std::less< bool >,
               std::less< time_point >,
               std::less< escrow_id_type >
            >
         >,
         ordered_unique< tag< by_dispute_release_time >,
            composite_key< escrow_object,
               member< escrow_object, bool, &escrow_object::disputed >,
               member< escrow_object, time_point, &escrow_object::dispute_release_time >,
               member< escrow_object, escrow_id_type, &escrow_object::id >
            >,
            composite_key_compare< 
               std::less< bool >,
               std::less< time_point >,
               std::less< escrow_id_type >
            >
         >,
         ordered_unique< tag< by_balance >,
            composite_key< escrow_object,
               member< escrow_object, asset, &escrow_object::balance >,
               member< escrow_object, escrow_id_type, &escrow_object::id >
            >,
            composite_key_compare<
               std::greater< asset >,
               std::less< escrow_id_type >
            >
         >
      >,
      allocator< escrow_object >
   > escrow_index;

   struct by_product_id;
   struct by_last_updated;

   typedef multi_index_container<
      product_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< product_object, product_id_type, &product_object::id > >,
         ordered_unique< tag< by_product_id >,
            composite_key< product_object,
               member< product_object, account_name_type,  &product_object::account >,
               member< product_object, shared_string, &product_object::product_id >
            >,
            composite_key_compare<
               std::less< account_name_type >,
               strcmp_less
            >
         >,
         ordered_unique< tag< by_last_updated >,
            composite_key< product_object,
               member< product_object, time_point, &product_object::last_updated >,
               member< product_object, product_id_type, &product_object::id >
            >,
            composite_key_compare< 
               std::greater< time_point >, 
               std::less< product_id_type >
            >
         >
      >,
      allocator< product_object >
   > product_index;


   struct by_request_id;
   struct by_to_complete;
   struct by_complete_from_request_id;

   typedef multi_index_container<
      savings_withdraw_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< savings_withdraw_object, savings_withdraw_id_type, &savings_withdraw_object::id > >,
         ordered_unique< tag< by_request_id >,
            composite_key< savings_withdraw_object,
               member< savings_withdraw_object, account_name_type,  &savings_withdraw_object::from >,
               member< savings_withdraw_object, shared_string, &savings_withdraw_object::request_id >
            >,
            composite_key_compare<
               std::less< account_name_type >,
               strcmp_less
            >
         >,
         ordered_unique< tag< by_to_complete >,
            composite_key< savings_withdraw_object,
               member< savings_withdraw_object, account_name_type,  &savings_withdraw_object::to >,
               member< savings_withdraw_object, time_point,  &savings_withdraw_object::complete >,
               member< savings_withdraw_object, savings_withdraw_id_type, &savings_withdraw_object::id >
            >,
            composite_key_compare<
               std::less< account_name_type >,
               std::less< time_point >,
               std::less< savings_withdraw_id_type >
            >
         >,
         ordered_unique< tag< by_complete_from_request_id >,
            composite_key< savings_withdraw_object,
               member< savings_withdraw_object, time_point,  &savings_withdraw_object::complete >,
               member< savings_withdraw_object, account_name_type,  &savings_withdraw_object::from >,
               member< savings_withdraw_object, shared_string, &savings_withdraw_object::request_id >
            >,
            composite_key_compare< 
               std::less< time_point >, 
               std::less< account_name_type >, 
               strcmp_less 
            >
         >
      >,
      allocator< savings_withdraw_object >
   > savings_withdraw_index;

   struct by_account;
   struct by_effective_date;

   typedef multi_index_container<
      decline_voting_rights_request_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< decline_voting_rights_request_object, decline_voting_rights_request_id_type, &decline_voting_rights_request_object::id > >,
         ordered_unique< tag< by_account >,
            member< decline_voting_rights_request_object, account_name_type, &decline_voting_rights_request_object::account >
         >,
         ordered_unique< tag< by_effective_date >,
            composite_key< decline_voting_rights_request_object,
               member< decline_voting_rights_request_object, time_point, &decline_voting_rights_request_object::effective_date >,
               member< decline_voting_rights_request_object, account_name_type, &decline_voting_rights_request_object::account >
            >,
            composite_key_compare< 
               std::less< time_point >, 
               std::less< account_name_type > 
            >
         >
      >,
      allocator< decline_voting_rights_request_object >
   > decline_voting_rights_request_index;

   struct by_name;
   struct by_symbol;

   typedef multi_index_container<
      reward_fund_object,
      indexed_by<
         ordered_unique< tag< by_id >,
            member< reward_fund_object, reward_fund_id_type, &reward_fund_object::id > >,
         ordered_unique< tag< by_symbol >,
            member< reward_fund_object, asset_symbol_type, &reward_fund_object::symbol > >
      >,
      allocator< reward_fund_object >
   > reward_fund_index;

   struct by_collateral;
   struct by_debt;
   struct by_account;
   struct by_price;

   typedef multi_index_container<
      call_order_object,
      indexed_by<
         ordered_unique< tag< by_id >,
            member< call_order_object, call_order_id_type, &call_order_object::id > >,
         ordered_unique< tag< by_price >,
            composite_key< call_order_object,
               member< call_order_object, price, &call_order_object::call_price >,
               member< call_order_object, call_order_id_type, &call_order_object::id >
            >,
            composite_key_compare< 
               std::less< price >, 
               std::less< call_order_id_type > 
            >
         >,
         ordered_unique< tag< by_debt >,
            composite_key< call_order_object,
               const_mem_fun< call_order_object, asset_symbol_type, &call_order_object::debt_type >,
               const_mem_fun< call_order_object, price, &call_order_object::collateralization >,
               member< call_order_object, call_order_id_type, &call_order_object::id >
            >,
            composite_key_compare< 
               std::less< asset_symbol_type >,
               std::less< price >,
               std::less< call_order_id_type >
            >
         >,
         ordered_unique< tag< by_account >,
            composite_key< call_order_object,
               member< call_order_object, account_name_type, &call_order_object::borrower >,
               const_mem_fun< call_order_object, asset_symbol_type, &call_order_object::debt_type >
            >,
            composite_key_compare< 
               std::less< account_name_type >, 
               std::less< asset_symbol_type >
            >
         >,
         ordered_unique< tag< by_collateral >,
            composite_key< call_order_object,
               const_mem_fun< call_order_object, price, &call_order_object::collateralization >,
               member< call_order_object, call_order_id_type, &call_order_object::id >
            >,
            composite_key_compare< 
               std::less< price >, 
               std::less< call_order_id_type > 
            >
         >
      >, 
      allocator < call_order_object >
   > call_order_index;


   typedef multi_index_container<
      auction_order_object,
      indexed_by<
         ordered_unique< tag< by_id >,
            member< auction_order_object, auction_order_id_type, &auction_order_object::id > >,
         ordered_non_unique< tag< by_expiration >, 
            member< auction_order_object, time_point, &auction_order_object::expiration > >,
         ordered_unique< tag< by_price >,
            composite_key< auction_order_object,
               member< auction_order_object, price, &auction_order_object::min_exchange_rate >,
               member< auction_order_object, auction_order_id_type, &auction_order_object::id >
            >,
            composite_key_compare< 
               std::less< price >, 
               std::less< auction_order_id_type > 
            >
         >,
         ordered_unique< tag< by_account >,
            composite_key< auction_order_object,
               member< auction_order_object, account_name_type, &auction_order_object::owner >,
               member< auction_order_object, shared_string, &auction_order_object::order_id >
            >,
            composite_key_compare< 
               std::less< account_name_type >, 
               strcmp_less 
            >
         >
      >, 
      allocator < auction_order_object >
   > auction_order_index;

   struct by_strike_price;

   typedef multi_index_container<
      option_order_object,
      indexed_by<
         ordered_unique< tag< by_id >,
            member< option_order_object, option_order_id_type, &option_order_object::id > >,
         ordered_non_unique< tag< by_expiration >, 
            const_mem_fun< option_order_object, time_point, &option_order_object::expiration > >,
         ordered_unique< tag< by_strike_price >,
            composite_key< option_order_object,
               member< option_order_object, option_strike, &option_order_object::strike_price >,
               member< option_order_object, option_order_id_type, &option_order_object::id >
            >,
            composite_key_compare< 
               std::less< option_strike >, 
               std::less< option_order_id_type > 
            >
         >,
         ordered_unique< tag< by_account >,
            composite_key< option_order_object,
               member< option_order_object, account_name_type, &option_order_object::owner >,
               member< option_order_object, shared_string, &option_order_object::order_id >
            >,
            composite_key_compare< 
               std::less< account_name_type >, 
               strcmp_less 
            >
         >
      >, 
      allocator < option_order_object >
   > option_order_index;


   struct by_expiration;
   struct by_account_asset;
   
   typedef multi_index_container<
      force_settlement_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< force_settlement_object, force_settlement_id_type, &force_settlement_object::id > >,
         ordered_unique< tag< by_account_asset >,
            composite_key< force_settlement_object,
               member< force_settlement_object, account_name_type, &force_settlement_object::owner >,
               const_mem_fun< force_settlement_object, asset_symbol_type, &force_settlement_object::settlement_asset_symbol >,
               member< force_settlement_object, force_settlement_id_type, &force_settlement_object::id >
            >,
            composite_key_compare< 
               std::less< account_name_type >,
               std::less< asset_symbol_type >,
               std::less< force_settlement_id_type >
            >
         >,
         ordered_unique< tag< by_expiration >,
            composite_key< force_settlement_object,
               const_mem_fun< force_settlement_object, asset_symbol_type, &force_settlement_object::settlement_asset_symbol >,
               member< force_settlement_object, time_point, &force_settlement_object::settlement_date >,
               member< force_settlement_object, force_settlement_id_type, &force_settlement_object::id >
            >,
            composite_key_compare< 
               std::less< asset_symbol_type >,
               std::less< time_point >,
               std::less< force_settlement_id_type >
            >
         >
      >,
      allocator < force_settlement_object >
   > force_settlement_index;


   typedef multi_index_container<
      collateral_bid_object,
      indexed_by<
         ordered_unique< tag< by_id >,
            member< collateral_bid_object, collateral_bid_id_type, &collateral_bid_object::id > >,
         ordered_unique< tag< by_account >,
            composite_key< collateral_bid_object,
               member< collateral_bid_object, account_name_type, &collateral_bid_object::bidder >,
               const_mem_fun< collateral_bid_object, asset_symbol_type, &collateral_bid_object::debt_type >
            >,
            composite_key_compare<
               std::less< account_name_type >,
               std::less< asset_symbol_type >
            >
         >,
         ordered_unique< tag< by_price >,
            composite_key< collateral_bid_object,
               const_mem_fun< collateral_bid_object, asset_symbol_type, &collateral_bid_object::debt_type >,
               const_mem_fun< collateral_bid_object, price, &collateral_bid_object::inv_swan_price >,
               member< collateral_bid_object, collateral_bid_id_type, &collateral_bid_object::id >
            >,
            composite_key_compare<
               std::less< asset_symbol_type >,
               std::greater< price >,
               std::less< collateral_bid_id_type >
            >
         >
      >,
      allocator < collateral_bid_object >
   > collateral_bid_index;

   struct by_owner_symbol;

   typedef multi_index_container<
      credit_collateral_object,
      indexed_by<
         ordered_unique< tag< by_id >,
            member< credit_collateral_object, credit_collateral_id_type, &credit_collateral_object::id > >,
         ordered_unique< tag< by_owner_symbol >,
            composite_key< credit_collateral_object,
               member< credit_collateral_object, account_name_type, &credit_collateral_object::owner >,
               member< credit_collateral_object, asset_symbol_type, &credit_collateral_object::symbol >
            >,
            composite_key_compare<
               std::less< account_name_type >,
               std::less< asset_symbol_type >
            >
         >,
         ordered_unique< tag<by_owner>,
            composite_key< credit_collateral_object,
               member< credit_collateral_object, account_name_type, &credit_collateral_object::owner >,
               member< credit_collateral_object, credit_collateral_id_type, &credit_collateral_object::id >
            >,
            composite_key_compare<
               std::less< account_name_type >,
               std::less< credit_collateral_id_type >
            >
         >
      >,
      allocator < credit_collateral_object >
   > credit_collateral_index;

   struct by_loan_id;
   struct by_owner;
   struct by_owner_price;
   struct by_owner_debt;
   struct by_owner_collateral;
   struct by_last_updated; 
   struct by_debt;
   struct by_collateral;
   struct by_liquidation_spread;

   typedef multi_index_container<
      credit_loan_object,
      indexed_by<
         ordered_unique< tag< by_id >,
            member< credit_loan_object, credit_loan_id_type, &credit_loan_object::id > >,
         ordered_unique< tag< by_owner >,
            composite_key< credit_loan_object,
               member< credit_loan_object, account_name_type, &credit_loan_object::owner >,
               member< credit_loan_object, credit_loan_id_type, &credit_loan_object::id >
            >,
            composite_key_compare< 
               std::less< account_name_type >, 
               std::less< credit_loan_id_type > 
            >
         >,
         ordered_unique< tag<by_loan_id>,
            composite_key< credit_loan_object,
               member< credit_loan_object, account_name_type, &credit_loan_object::owner >,
               member< credit_loan_object, shared_string, &credit_loan_object::loan_id >
            >,
            composite_key_compare<
               std::less< account_name_type >,
               strcmp_less
            >
         >,
         ordered_unique< tag< by_owner_price >,
            composite_key< credit_loan_object,
               member< credit_loan_object, account_name_type, &credit_loan_object::owner >,
               member< credit_loan_object, price, &credit_loan_object::loan_price >,
               member< credit_loan_object, credit_loan_id_type, &credit_loan_object::id >
            >,
            composite_key_compare< 
               std::less< account_name_type >, 
               std::less< price >, 
               std::less< credit_loan_id_type > 
            >
         >,
         ordered_unique< tag< by_owner_debt >,
            composite_key< credit_loan_object,
               member< credit_loan_object, account_name_type, &credit_loan_object::owner >,
               const_mem_fun< credit_loan_object, asset_symbol_type, &credit_loan_object::debt_asset >,
               member< credit_loan_object, credit_loan_id_type, &credit_loan_object::id >
            >,
            composite_key_compare< 
               std::less< account_name_type >, 
               std::less< asset_symbol_type >, 
               std::less< credit_loan_id_type > 
            >
         >,
         ordered_unique< tag< by_owner_collateral >,
            composite_key< credit_loan_object,
               member< credit_loan_object, account_name_type, &credit_loan_object::owner >,
               const_mem_fun< credit_loan_object, asset_symbol_type, &credit_loan_object::collateral_asset >,
               member< credit_loan_object, credit_loan_id_type, &credit_loan_object::id >
            >,
            composite_key_compare< 
               std::less< account_name_type >, 
               std::less< asset_symbol_type >, 
               std::less< credit_loan_id_type > 
            >
         >,
         ordered_unique< tag< by_liquidation_spread >,
            composite_key< credit_loan_object,
               const_mem_fun< credit_loan_object, asset_symbol_type, &credit_loan_object::debt_asset >,
               const_mem_fun< credit_loan_object, asset_symbol_type, &credit_loan_object::collateral_asset >,
               const_mem_fun< credit_loan_object, price, &credit_loan_object::liquidation_spread >,
               member< credit_loan_object, credit_loan_id_type, &credit_loan_object::id >
            >,
            composite_key_compare< 
               std::less< asset_symbol_type >,
               std::less< asset_symbol_type >,
               std::less< price >,
               std::less< credit_loan_id_type >
            >
         >,
         ordered_unique< tag< by_last_updated >,
            composite_key< credit_loan_object,
               member< credit_loan_object, time_point, &credit_loan_object::last_updated >,
               member< credit_loan_object, credit_loan_id_type, &credit_loan_object::id >
            >,
            composite_key_compare< 
               std::greater< time_point >, 
               std::less< credit_loan_id_type > 
            >
         >,
         ordered_unique< tag< by_debt >,
            composite_key< credit_loan_object,
               const_mem_fun< credit_loan_object, asset_symbol_type, &credit_loan_object::debt_asset >,
               member< credit_loan_object, credit_loan_id_type, &credit_loan_object::id >
            >,
            composite_key_compare< 
               std::less< asset_symbol_type >, 
               std::less< credit_loan_id_type > 
            >
         >,
         ordered_unique< tag< by_collateral >,
            composite_key< credit_loan_object,
               const_mem_fun< credit_loan_object, asset_symbol_type, &credit_loan_object::collateral_asset >,
               member< credit_loan_object, credit_loan_id_type, &credit_loan_object::id >
            >,
            composite_key_compare< 
               std::less< asset_symbol_type >,
               std::less< credit_loan_id_type >
            >
         >
      >, 
      allocator < credit_loan_object >
   > credit_loan_index;

} } // node::chain

#include <node/chain/producer_objects.hpp>
#include <node/chain/network_object.hpp>
#include <node/chain/asset_object.hpp>
#include <node/chain/comment_object.hpp>
#include <node/chain/account_object.hpp>
#include <node/chain/community_object.hpp>
#include <node/chain/ad_object.hpp>
#include <node/chain/graph_object.hpp>

FC_REFLECT( node::chain::median_chain_property_object,
         (id)
         (account_creation_fee)
         (maximum_block_size)
         (pow_target_time)
         (pow_decay_time)
         (txn_stake_decay_time)
         (escrow_bond_percent)
         (credit_interest_rate)
         (credit_open_ratio)
         (credit_liquidation_ratio)
         (credit_min_interest)
         (credit_variable_interest)
         (market_max_credit_ratio)
         (margin_open_ratio)
         (margin_liquidation_ratio)
         (maximum_asset_feed_publishers)
         (membership_base_price)
         (membership_mid_price)
         (membership_top_price)
         (author_reward_percent)
         (vote_reward_percent)
         (view_reward_percent)
         (share_reward_percent)
         (comment_reward_percent)
         (storage_reward_percent)
         (moderator_reward_percent)
         (content_reward_decay_rate)
         (content_reward_interval)
         (vote_reserve_rate)
         (view_reserve_rate)
         (share_reserve_rate)
         (comment_reserve_rate)
         (vote_recharge_time)
         (view_recharge_time)
         (share_recharge_time)
         (comment_recharge_time)
         (curation_auction_decay_time)
         (vote_curation_decay)
         (view_curation_decay)
         (share_curation_decay)
         (comment_curation_decay)
         (supernode_decay_time)
         (enterprise_vote_percent_required)
         (maximum_asset_whitelist_authorities)
         (max_stake_intervals)
         (max_unstake_intervals)
         (max_exec_budget)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::median_chain_property_object, node::chain::median_chain_property_index );

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

FC_REFLECT( node::chain::limit_order_object,
         (id)
         (seller)
         (order_id)
         (for_sale)
         (sell_price)
         (interface)
         (created)
         (last_updated)
         (expiration)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::limit_order_object, node::chain::limit_order_index );

FC_REFLECT( node::chain::call_order_object,
         (id)
         (borrower)
         (collateral)
         (debt)
         (call_price)
         (target_collateral_ratio)
         (interface)
         (created)
         (last_updated)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::call_order_object, node::chain::call_order_index );

FC_REFLECT( node::chain::auction_order_object,
         (id)
         (owner)
         (order_id)
         (amount_to_sell)
         (min_exchange_rate)
         (interface)
         (expiration)
         (created)
         (last_updated)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::auction_order_object, node::chain::auction_order_index );

FC_REFLECT( node::chain::option_order_object,
         (id)
         (owner)
         (order_id)
         (amount_to_issue)
         (option_position)
         (strike_price)
         (interface)
         (created)
         (last_updated)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::option_order_object, node::chain::option_order_index );

FC_REFLECT( node::chain::force_settlement_object, 
         (id)
         (owner)
         (balance)
         (settlement_date)
         (interface)
         (created)
         (last_updated)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::force_settlement_object, node::chain::force_settlement_index );

FC_REFLECT( node::chain::collateral_bid_object, 
         (id)
         (bidder)
         (collateral)
         (debt)
         (created)
         (last_updated)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::collateral_bid_object, node::chain::collateral_bid_index );

FC_REFLECT( node::chain::credit_collateral_object,
         (id)
         (owner)
         (symbol)
         (collateral)
         (created)
         (last_updated)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::credit_collateral_object, node::chain::credit_collateral_index );   

FC_REFLECT( node::chain::credit_loan_object,
         (id)
         (owner)
         (loan_id)
         (debt)
         (interest)
         (collateral)
         (loan_price)
         (liquidation_price)
         (symbol_a)
         (symbol_b)
         (last_interest_rate)
         (created)
         (last_updated)
         (last_interest_time)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::credit_loan_object, node::chain::credit_loan_index );

FC_REFLECT( node::chain::margin_order_object,
         (id)
         (owner)
         (order_id)
         (sell_price)
         (collateral)
         (debt)
         (debt_balance)
         (interest)
         (position)
         (position_balance)
         (collateralization)
         (interface)
         (created)
         (last_updated)
         (last_interest_time)
         (expiration)
         (unrealized_value)
         (last_interest_rate)
         (liquidating)
         (stop_loss_price)
         (take_profit_price)
         (limit_stop_loss_price)
         (limit_take_profit_price)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::margin_order_object, node::chain::margin_order_index );

FC_REFLECT( node::chain::escrow_object,
         (id)
         (from)
         (to)
         (from_mediator)
         (to_mediator)
         (payment)
         (balance)
         (escrow_id)
         (memo)
         (json)
         (acceptance_time)
         (escrow_expiration)
         (dispute_release_time)
         (mediators)
         (release_percentages)
         (approvals)
         (created)
         (last_updated)
         (disputed)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::escrow_object, node::chain::escrow_index );

FC_REFLECT( node::chain::product_object,
         (id)
         (account)
         (product_id)
         (name)
         (product_variants)
         (sale_type)
         (details)
         (images)
         (product_prices)
         (stock_available)
         (json)
         (url)
         (delivery_variants)
         (delivery_prices)
         (created)
         (last_updated)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::product_object, node::chain::product_index );

FC_REFLECT( node::chain::savings_withdraw_object,
         (id)
         (from)
         (to)
         (memo)
         (request_id)
         (amount)
         (complete)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::savings_withdraw_object, node::chain::savings_withdraw_index );

FC_REFLECT( node::chain::unstake_asset_route_object,
         (id)
         (from)
         (to)
         (symbol)
         (percent)
         (auto_stake)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::unstake_asset_route_object, node::chain::unstake_asset_route_index );

FC_REFLECT( node::chain::decline_voting_rights_request_object,
         (id)
         (account)
         (effective_date) 
         );
            
CHAINBASE_SET_INDEX_TYPE( node::chain::decline_voting_rights_request_object, node::chain::decline_voting_rights_request_index );

FC_REFLECT_ENUM( node::chain::curve_id,
         (quadratic)
         (quadratic_curation)
         (linear)
         (square_root)
         (convergent_semi_quadratic)
         );

FC_REFLECT( node::chain::reward_fund_object,
         (id)
         (content_reward_balance)
         (validation_reward_balance) 
         (txn_stake_reward_balance) 
         (work_reward_balance)
         (producer_activity_reward_balance) 
         (supernode_reward_balance)
         (power_reward_balance)
         (community_fund_balance)
         (development_reward_balance)
         (marketing_reward_balance)
         (advocacy_reward_balance)
         (activity_reward_balance)
         (premium_partners_fund_balance)
         (total_pending_reward_balance)
         (recent_content_claims)
         (recent_activity_claims)
         (content_constant)
         (content_reward_decay_rate)
         (author_reward_curve)
         (curation_reward_curve)
         (last_updated)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::reward_fund_object, node::chain::reward_fund_index );