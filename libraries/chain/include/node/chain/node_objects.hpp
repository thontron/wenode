#pragma once

#include <node/protocol/authority.hpp>
#include <node/protocol/node_operations.hpp>

#include <node/chain/node_object_types.hpp>

#include <boost/multi_index/composite_key.hpp>
#include <boost/multiprecision/cpp_int.hpp>


namespace node { namespace chain {

   using node::protocol::asset;
   using node::protocol::price;
   using node::protocol::asset_symbol_type;

   typedef protocol::fixed_string_16 reward_fund_name_type;

   /**
    *  This object is used to track pending requests to convert TSD to TME
    */
   class convert_request_object : public object< convert_request_object_type, convert_request_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         convert_request_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         convert_request_object(){}

         id_type           id;

         account_name_type owner;
         uint32_t          requestid = 0; ///< id set by owner, the owner,requestid pair must be unique
         asset             amount;
         time_point_sec    conversion_date; ///< at this time the feed_history_median_price * amount
   };


   class escrow_object : public object< escrow_object_type, escrow_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         escrow_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         escrow_object(){}

         id_type           id;

         uint32_t          escrow_id = 20;
         account_name_type from;
         account_name_type to;
         account_name_type agent;
         time_point_sec    ratification_deadline;
         time_point_sec    escrow_expiration;
         asset             TSDbalance;
         asset             TMEbalance;
         asset             pending_fee;
         bool              to_approved = false;
         bool              agent_approved = false;
         bool              disputed = false;

         bool              is_approved()const { return to_approved && agent_approved; }
   };


   class savings_withdraw_object : public object< savings_withdraw_object_type, savings_withdraw_object >
   {
      savings_withdraw_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         savings_withdraw_object( Constructor&& c, allocator< Allocator > a )
            :memo( a )
         {
            c( *this );
         }

         id_type           id;

         account_name_type from;
         account_name_type to;
         shared_string     memo;
         uint32_t          request_id = 0;
         asset             amount;
         time_point_sec    complete;
   };


   /**
    *  If last_update is greater than 1 week, then volume gets reset to 0
    *
    *  When a user is a maker, their volume increases
    *  When a user is a taker, their volume decreases
    *
    *  Every 1000 blocks, the account that has the highest volume_weight() is paid the maximum of
    *  1000 TME or 1000 * virtual_supply / (100*blocks_per_year) aka 10 * virtual_supply / blocks_per_year
    *
    *  After being paid volume gets reset to 0
    */
   class liquidity_reward_balance_object : public object< liquidity_reward_balance_object_type, liquidity_reward_balance_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         liquidity_reward_balance_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         liquidity_reward_balance_object(){}

         id_type           id;

         account_id_type   owner;
         int64_t           TME_volume = 0;
         int64_t           TSD_volume = 0;
         uint128_t         weight = 0;

         time_point_sec    last_update = fc::time_point_sec::min(); /// used to decay negative liquidity balances. block num

         /// this is the sort index
         uint128_t volume_weight()const
         {
            return TME_volume * TSD_volume * is_positive();
         }

         uint128_t min_volume_weight()const
         {
            return std::min(TME_volume,TSD_volume) * is_positive();
         }

         void update_weight( bool hf9 )
         {
             weight = hf9 ? min_volume_weight() : volume_weight();
         }

         inline int is_positive()const
         {
            return ( TME_volume > 0 && TSD_volume > 0 ) ? 1 : 0;
         }
   };


   /**
    *  This object gets updated once per hour, on the hour
    */
   class feed_history_object  : public object< feed_history_object_type, feed_history_object >
   {
      feed_history_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         feed_history_object( Constructor&& c, allocator< Allocator > a )
            :price_history( a.get_segment_manager() )
         {
            c( *this );
         }

         id_type                                   id;

         price                                     current_median_history; ///< the current median of the price history, used as the base for convert operations
         bip::deque< price, allocator< price > >   price_history; ///< tracks this last week of median_feed one per hour
   };


   /**
    *  @brief an offer to sell a amount of a asset at a specified exchange rate by a certain time
    *  @ingroup object
    *  @ingroup protocol
    *  @ingroup market
    *
    *  This limit_order_objects are indexed by @ref expiration and is automatically deleted on the first block after expiration.
    */
   class limit_order_object : public object< limit_order_object_type, limit_order_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         limit_order_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         limit_order_object(){}

         id_type           id;

         time_point_sec    created;
         time_point_sec    expiration;
         account_name_type seller;
         uint32_t          orderid = 0;
         share_type        for_sale; ///< asset id is sell_price.base.symbol
         price             sell_price;

         pair< asset_symbol_type, asset_symbol_type > get_market()const
         {
            return sell_price.base.symbol < sell_price.quote.symbol ?
                std::make_pair( sell_price.base.symbol, sell_price.quote.symbol ) :
                std::make_pair( sell_price.quote.symbol, sell_price.base.symbol );
         }

         asset amount_for_sale()const   { return asset( for_sale, sell_price.base.symbol ); }
         asset amount_to_receive()const { return amount_for_sale() * sell_price; }
   };


   /**
    * @breif a route to send withdrawn SCORE.
    */
   class withdrawSCORE_route_object : public object< withdrawSCORE_route_object_type, withdrawSCORE_route_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         withdrawSCORE_route_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         withdrawSCORE_route_object(){}

         id_type  id;

         account_id_type   from_account;
         account_id_type   to_account;
         uint16_t          percent = 0;
         bool              autoSCORE = false;
   };


   class decline_voting_rights_request_object : public object< decline_voting_rights_request_object_type, decline_voting_rights_request_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         decline_voting_rights_request_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         decline_voting_rights_request_object(){}

         id_type           id;

         account_id_type   account;
         time_point_sec    effective_date;
   };

   enum curve_id
   {
      quadratic,
      quadratic_curation,
      linear,
      square_root
   };

   class reward_fund_object : public object< reward_fund_object_type, reward_fund_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         reward_fund_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         reward_fund_object() {}

         reward_fund_id_type     id;
         reward_fund_name_type   name;
         asset                   reward_balance = asset( 0, SYMBOL_COIN );
         fc::uint128_t           recent_claims = 0;
         time_point_sec          last_update;
         uint128_t               content_constant = 0;
         uint16_t                percent_curationRewards = 0;
         uint16_t                percent_content_rewards = 0;
         curve_id                authorReward_curve = linear;
         curve_id                curationReward_curve = square_root;
   };

   struct by_price;
   struct by_expiration;
   struct by_account;
   typedef multi_index_container<
      limit_order_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< limit_order_object, limit_order_id_type, &limit_order_object::id > >,
         ordered_non_unique< tag< by_expiration >, member< limit_order_object, time_point_sec, &limit_order_object::expiration > >,
         ordered_unique< tag< by_price >,
            composite_key< limit_order_object,
               member< limit_order_object, price, &limit_order_object::sell_price >,
               member< limit_order_object, limit_order_id_type, &limit_order_object::id >
            >,
            composite_key_compare< std::greater< price >, std::less< limit_order_id_type > >
         >,
         ordered_unique< tag< by_account >,
            composite_key< limit_order_object,
               member< limit_order_object, account_name_type, &limit_order_object::seller >,
               member< limit_order_object, uint32_t, &limit_order_object::orderid >
            >
         >
      >,
      allocator< limit_order_object >
   > limit_order_index;

   struct by_owner;
   struct by_conversion_date;
   typedef multi_index_container<
      convert_request_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< convert_request_object, convert_request_id_type, &convert_request_object::id > >,
         ordered_unique< tag< by_conversion_date >,
            composite_key< convert_request_object,
               member< convert_request_object, time_point_sec, &convert_request_object::conversion_date >,
               member< convert_request_object, convert_request_id_type, &convert_request_object::id >
            >
         >,
         ordered_unique< tag< by_owner >,
            composite_key< convert_request_object,
               member< convert_request_object, account_name_type, &convert_request_object::owner >,
               member< convert_request_object, uint32_t, &convert_request_object::requestid >
            >
         >
      >,
      allocator< convert_request_object >
   > convert_request_index;

   struct by_owner;
   struct by_volume_weight;

   typedef multi_index_container<
      liquidity_reward_balance_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< liquidity_reward_balance_object, liquidity_reward_balance_id_type, &liquidity_reward_balance_object::id > >,
         ordered_unique< tag< by_owner >, member< liquidity_reward_balance_object, account_id_type, &liquidity_reward_balance_object::owner > >,
         ordered_unique< tag< by_volume_weight >,
            composite_key< liquidity_reward_balance_object,
                member< liquidity_reward_balance_object, fc::uint128, &liquidity_reward_balance_object::weight >,
                member< liquidity_reward_balance_object, account_id_type, &liquidity_reward_balance_object::owner >
            >,
            composite_key_compare< std::greater< fc::uint128 >, std::less< account_id_type > >
         >
      >,
      allocator< liquidity_reward_balance_object >
   > liquidity_reward_balance_index;

   typedef multi_index_container<
      feed_history_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< feed_history_object, feed_history_id_type, &feed_history_object::id > >
      >,
      allocator< feed_history_object >
   > feed_history_index;

   struct by_withdraw_route;
   struct by_destination;
   typedef multi_index_container<
      withdrawSCORE_route_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< withdrawSCORE_route_object, withdrawSCORE_route_id_type, &withdrawSCORE_route_object::id > >,
         ordered_unique< tag< by_withdraw_route >,
            composite_key< withdrawSCORE_route_object,
               member< withdrawSCORE_route_object, account_id_type, &withdrawSCORE_route_object::from_account >,
               member< withdrawSCORE_route_object, account_id_type, &withdrawSCORE_route_object::to_account >
            >,
            composite_key_compare< std::less< account_id_type >, std::less< account_id_type > >
         >,
         ordered_unique< tag< by_destination >,
            composite_key< withdrawSCORE_route_object,
               member< withdrawSCORE_route_object, account_id_type, &withdrawSCORE_route_object::to_account >,
               member< withdrawSCORE_route_object, withdrawSCORE_route_id_type, &withdrawSCORE_route_object::id >
            >
         >
      >,
      allocator< withdrawSCORE_route_object >
   > withdrawSCORE_route_index;

   struct by_from_id;
   struct by_to;
   struct by_agent;
   struct by_ratification_deadline;
   struct by_TSDbalance;
   typedef multi_index_container<
      escrow_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< escrow_object, escrow_id_type, &escrow_object::id > >,
         ordered_unique< tag< by_from_id >,
            composite_key< escrow_object,
               member< escrow_object, account_name_type,  &escrow_object::from >,
               member< escrow_object, uint32_t, &escrow_object::escrow_id >
            >
         >,
         ordered_unique< tag< by_to >,
            composite_key< escrow_object,
               member< escrow_object, account_name_type,  &escrow_object::to >,
               member< escrow_object, escrow_id_type, &escrow_object::id >
            >
         >,
         ordered_unique< tag< by_agent >,
            composite_key< escrow_object,
               member< escrow_object, account_name_type,  &escrow_object::agent >,
               member< escrow_object, escrow_id_type, &escrow_object::id >
            >
         >,
         ordered_unique< tag< by_ratification_deadline >,
            composite_key< escrow_object,
               const_mem_fun< escrow_object, bool, &escrow_object::is_approved >,
               member< escrow_object, time_point_sec, &escrow_object::ratification_deadline >,
               member< escrow_object, escrow_id_type, &escrow_object::id >
            >,
            composite_key_compare< std::less< bool >, std::less< time_point_sec >, std::less< escrow_id_type > >
         >,
         ordered_unique< tag< by_TSDbalance >,
            composite_key< escrow_object,
               member< escrow_object, asset, &escrow_object::TSDbalance >,
               member< escrow_object, escrow_id_type, &escrow_object::id >
            >,
            composite_key_compare< std::greater< asset >, std::less< escrow_id_type > >
         >
      >,
      allocator< escrow_object >
   > escrow_index;

   struct by_from_rid;
   struct by_to_complete;
   struct by_complete_from_rid;
   typedef multi_index_container<
      savings_withdraw_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< savings_withdraw_object, savings_withdraw_id_type, &savings_withdraw_object::id > >,
         ordered_unique< tag< by_from_rid >,
            composite_key< savings_withdraw_object,
               member< savings_withdraw_object, account_name_type,  &savings_withdraw_object::from >,
               member< savings_withdraw_object, uint32_t, &savings_withdraw_object::request_id >
            >
         >,
         ordered_unique< tag< by_to_complete >,
            composite_key< savings_withdraw_object,
               member< savings_withdraw_object, account_name_type,  &savings_withdraw_object::to >,
               member< savings_withdraw_object, time_point_sec,  &savings_withdraw_object::complete >,
               member< savings_withdraw_object, savings_withdraw_id_type, &savings_withdraw_object::id >
            >
         >,
         ordered_unique< tag< by_complete_from_rid >,
            composite_key< savings_withdraw_object,
               member< savings_withdraw_object, time_point_sec,  &savings_withdraw_object::complete >,
               member< savings_withdraw_object, account_name_type,  &savings_withdraw_object::from >,
               member< savings_withdraw_object, uint32_t, &savings_withdraw_object::request_id >
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
            member< decline_voting_rights_request_object, account_id_type, &decline_voting_rights_request_object::account >
         >,
         ordered_unique< tag< by_effective_date >,
            composite_key< decline_voting_rights_request_object,
               member< decline_voting_rights_request_object, time_point_sec, &decline_voting_rights_request_object::effective_date >,
               member< decline_voting_rights_request_object, account_id_type, &decline_voting_rights_request_object::account >
            >,
            composite_key_compare< std::less< time_point_sec >, std::less< account_id_type > >
         >
      >,
      allocator< decline_voting_rights_request_object >
   > decline_voting_rights_request_index;

   struct by_name;
   typedef multi_index_container<
      reward_fund_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< reward_fund_object, reward_fund_id_type, &reward_fund_object::id > >,
         ordered_unique< tag< by_name >, member< reward_fund_object, reward_fund_name_type, &reward_fund_object::name > >
      >,
      allocator< reward_fund_object >
   > reward_fund_index;

} } // node::chain

#include <node/chain/comment_object.hpp>
#include <node/chain/account_object.hpp>

FC_REFLECT_ENUM( node::chain::curve_id,
                  (quadratic)(quadratic_curation)(linear)(square_root))

FC_REFLECT( node::chain::limit_order_object,
             (id)(created)(expiration)(seller)(orderid)(for_sale)(sell_price) )
CHAINBASE_SET_INDEX_TYPE( node::chain::limit_order_object, node::chain::limit_order_index )

FC_REFLECT( node::chain::feed_history_object,
             (id)(current_median_history)(price_history) )
CHAINBASE_SET_INDEX_TYPE( node::chain::feed_history_object, node::chain::feed_history_index )

FC_REFLECT( node::chain::convert_request_object,
             (id)(owner)(requestid)(amount)(conversion_date) )
CHAINBASE_SET_INDEX_TYPE( node::chain::convert_request_object, node::chain::convert_request_index )

FC_REFLECT( node::chain::liquidity_reward_balance_object,
             (id)(owner)(TME_volume)(TSD_volume)(weight)(last_update) )
CHAINBASE_SET_INDEX_TYPE( node::chain::liquidity_reward_balance_object, node::chain::liquidity_reward_balance_index )

FC_REFLECT( node::chain::withdrawSCORE_route_object,
             (id)(from_account)(to_account)(percent)(autoSCORE) )
CHAINBASE_SET_INDEX_TYPE( node::chain::withdrawSCORE_route_object, node::chain::withdrawSCORE_route_index )

FC_REFLECT( node::chain::savings_withdraw_object,
             (id)(from)(to)(memo)(request_id)(amount)(complete) )
CHAINBASE_SET_INDEX_TYPE( node::chain::savings_withdraw_object, node::chain::savings_withdraw_index )

FC_REFLECT( node::chain::escrow_object,
             (id)(escrow_id)(from)(to)(agent)
             (ratification_deadline)(escrow_expiration)
             (TSDbalance)(TMEbalance)(pending_fee)
             (to_approved)(agent_approved)(disputed) )
CHAINBASE_SET_INDEX_TYPE( node::chain::escrow_object, node::chain::escrow_index )

FC_REFLECT( node::chain::decline_voting_rights_request_object,
             (id)(account)(effective_date) )
CHAINBASE_SET_INDEX_TYPE( node::chain::decline_voting_rights_request_object, node::chain::decline_voting_rights_request_index )

FC_REFLECT( node::chain::reward_fund_object,
            (id)
            (name)
            (reward_balance)
            (recent_claims)
            (last_update)
            (content_constant)
            (percent_curationRewards)
            (percent_content_rewards)
            (authorReward_curve)
            (curationReward_curve)
         )
CHAINBASE_SET_INDEX_TYPE( node::chain::reward_fund_object, node::chain::reward_fund_index )
