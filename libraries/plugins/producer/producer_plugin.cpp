
#include <node/producer/producer_plugin.hpp>
#include <node/producer/producer_objects.hpp>
#include <node/producer/producer_operations.hpp>

#include <node/chain/account_object.hpp>
#include <node/chain/database.hpp>
#include <node/chain/database_exceptions.hpp>
#include <node/chain/generic_custom_operation_interpreter.hpp>
#include <node/chain/index.hpp>
#include <node/chain/node_objects.hpp>

#include <node/app/impacted.hpp>

#include <fc/time.hpp>

#include <graphene/utilities/key_conversion.hpp>

#include <fc/smart_ref_impl.hpp>
#include <fc/thread/thread.hpp>

#include <iostream>
#include <memory>


#define DISTANCE_CALC_PRECISION (10000)


namespace node { namespace producer {

namespace bpo = boost::program_options;

using std::string;
using std::vector;

using protocol::signed_transaction;
using chain::account_object;

void new_chain_banner( const node::chain::database& db )
{
  std::cerr << "\n"
      "********************************\n"
      "*                              *\n"
      "*   ------- NEW CHAIN ------   *\n"
      "*   -        Welcome       -   *\n"
      "*   ------------------------   *\n"
      "*                              *\n"
      "********************************\n"
      "\n"
			"\n";
  std::cerr << "\n"
      "********************************\n"
      "*   ---- INIT PUBLIC KEY ---   *\n"
      "*   " << INIT_PUBLIC_KEY_STR <<  "   *\n"
      "*   ------------------------   *\n"
      "*                              *\n"
      "********************************\n"
      "\n"
			"\n";

	#if SHOW_PRIVATE_KEYS
		std::cerr << "\n"
      "********************************\n"
      "*   ------- KEYPAIRS -------   *\n"
      "*   ------------------------   *\n";
			for (auto const& [key, val] : _private_keys){
				std::cerr << "*   -------- PUB ---------   *\n" 
				"*   " << key << "   *\n"
				"*   ------ PRIVATE -------  *\n" 
				"*   " << val << "   *\n";
			}
    std:cerr <<	"*   ------------------------   *\n"
      "*                              *\n"
      "********************************\n"
      "\n"
			"\n";
	#endif

  return;
}

namespace detail
{
   using namespace node::chain;

   class producer_plugin_impl
   {
      public:
         producer_plugin_impl( producer_plugin& plugin )
            : _self( plugin ){}

         void plugin_initialize();

         void pre_apply_block( const node::protocol::signed_block& blk );
         void pre_transaction( const signed_transaction& trx );
         void pre_operation( const operation_notification& note );
         void post_operation( const chain::operation_notification& note );
         void on_block( const signed_block& b );

         void update_account_bandwidth( const account_object& a, uint32_t trx_size, const bandwidth_type type );

         producer_plugin& _self;
         std::shared_ptr< generic_custom_operation_interpreter< producer_plugin_operation > > _custom_operation_interpreter;

         std::set< node::protocol::account_name_type >                     _dupe_customs;
   };

   void producer_plugin_impl::plugin_initialize()
   {
      _custom_operation_interpreter = std::make_shared< generic_custom_operation_interpreter< producer_plugin_operation > >( _self.database() );

      _custom_operation_interpreter->register_evaluator< enable_content_editing_evaluator >( &_self );

      _self.database().set_custom_operation_interpreter( _self.plugin_name(), _custom_operation_interpreter );
   }

   struct comment_options_extension_visitor
   {
      comment_options_extension_visitor( const comment_object& c, const database& db ) : _c( c ), _db( db ) {}

      typedef void result_type;

      const comment_object& _c;
      const database& _db;

      void operator()( const comment_payout_beneficiaries& cpb )const
      {
         ASSERT( cpb.beneficiaries.size() <= 8,
            chain::plugin_exception,
            "Cannot specify more than 8 beneficiaries." );
      }
   };

   void producer_plugin_impl::pre_apply_block( const node::protocol::signed_block& b )
   {
      _dupe_customs.clear();
   }

   void check_memo( const string& memo, const account_object& account, const account_authority_object& auth )
   {
      vector< public_key_type > keys;

      try
      {
         // Check if memo is a private key
         keys.push_back( fc::ecc::extended_private_key::from_base58( memo ).get_public_key() );
      }
      catch( fc::parse_error_exception& ) {}
      catch( fc::assert_exception& ) {}

      // Get possible keys if memo was an account password
      string owner_seed = account.name + "owner" + memo;
      auto owner_secret = fc::sha256::hash( owner_seed.c_str(), owner_seed.size() );
      keys.push_back( fc::ecc::private_key::regenerate( owner_secret ).get_public_key() );

      string active_seed = account.name + "active" + memo;
      auto active_secret = fc::sha256::hash( active_seed.c_str(), active_seed.size() );
      keys.push_back( fc::ecc::private_key::regenerate( active_secret ).get_public_key() );

      string posting_seed = account.name + "posting" + memo;
      auto posting_secret = fc::sha256::hash( posting_seed.c_str(), posting_seed.size() );
      keys.push_back( fc::ecc::private_key::regenerate( posting_secret ).get_public_key() );

      // Check keys against public keys in authorites
      for( auto& key_weight_pair : auth.owner.key_auths )
      {
         for( auto& key : keys )
            ASSERT( key_weight_pair.first != key, chain::plugin_exception,
               "Detected private owner key in memo field. You should change your owner keys." );
      }

      for( auto& key_weight_pair : auth.active.key_auths )
      {
         for( auto& key : keys )
            ASSERT( key_weight_pair.first != key, chain::plugin_exception,
               "Detected private active key in memo field. You should change your active keys." );
      }

      for( auto& key_weight_pair : auth.posting.key_auths )
      {
         for( auto& key : keys )
            ASSERT( key_weight_pair.first != key, chain::plugin_exception,
               "Detected private posting key in memo field. You should change your posting keys." );
      }

      const auto& secure_public_key = account.secure_public_key;
      for( auto& key : keys )
         ASSERT( secure_public_key != key, chain::plugin_exception,
            "Detected private memo key in memo field. You should change your memo key." );
   }

   struct operation_visitor
   {
      operation_visitor( const chain::database& db ) : _db( db ) {}

      const chain::database& _db;

      typedef void result_type;

      template< typename T >
      void operator()( const T& )const {}

      void operator()( const comment_operation& o )const
      {
         if( o.parent_author != ROOT_POST_PARENT )
         {
            const auto& parent = _db.find_comment( o.parent_author, o.parent_permlink );

            if( parent != nullptr )
            ASSERT( parent->depth < SOFT_MAX_COMMENT_DEPTH,
               chain::plugin_exception,
               "Comment is nested ${x} posts deep, maximum depth is ${y}.", ("x",parent->depth)("y",SOFT_MAX_COMMENT_DEPTH) );
         }

         auto itr = _db.find< comment_object, by_permlink >( boost::make_tuple( o.author, o.permlink ) );

         if( itr != nullptr && itr->cashout_time == fc::time_point::maximum() )
         {
            auto edit_lock = _db.find< content_edit_lock_object, by_account >( o.author );

            ASSERT( edit_lock != nullptr && _db.head_block_time() < edit_lock->lock_time,
               chain::plugin_exception,
               "The comment is archived" );
         }
      }

      void operator()( const transfer_operation& o )const
      {
         if( o.memo.length() > 0 )
            check_memo( o.memo,
                        _db.get< account_object, chain::by_name >( o.from ),
                        _db.get< account_authority_object, chain::by_account >( o.from ) );
      }

      void operator()( const transfer_to_savings_operation& o )const
      {
         if( o.memo.length() > 0 )
            check_memo( o.memo,
                        _db.get< account_object, chain::by_name >( o.from ),
                        _db.get< account_authority_object, chain::by_account >( o.from ) );
      }

      void operator()( const transfer_from_savings_operation& o )const
      {
         if( o.memo.length() > 0 )
            check_memo( o.memo,
                        _db.get< account_object, chain::by_name >( o.from ),
                        _db.get< account_authority_object, chain::by_account >( o.from ) );
      }
   };

   void producer_plugin_impl::pre_transaction( const signed_transaction& trx )
   {
      const auto& _db = _self.database();
      flat_set< account_name_type > required; vector<authority> other;
      trx.get_required_authorities( required, required, required, other );

      auto trx_size = fc::raw::pack_size(trx);

      for( const auto& auth : required )
      {
         const auto& acnt = _db.get_account( auth );

         update_account_bandwidth( acnt, trx_size, bandwidth_type::forum );

         for( const auto& op : trx.operations )
         {
            if( is_market_operation( op ) )
            {
               update_account_bandwidth( acnt, trx_size * 10, bandwidth_type::market );
               break;
            }
         }
      }
   }

   void producer_plugin_impl::pre_operation( const operation_notification& note )
   {
      const auto& _db = _self.database();
      if( _db.is_producing() )
      {
         note.op.visit( operation_visitor( _db ) );
      }
   }

   void producer_plugin_impl::post_operation( const operation_notification& note )
   {
      const auto& db = _self.database();

      switch( note.op.which() )
      {
         case operation::tag< custom_operation >::value:
         case operation::tag< custom_json_operation >::value:
         {
            flat_set< account_name_type > impacted;
            app::operation_get_impacted_accounts( note.op, impacted );

            for( auto& account : impacted )
               if( db.is_producing() )
                  ASSERT( _dupe_customs.insert( account ).second, plugin_exception,
                     "Account ${a} already submitted a custom json operation this block.",
                     ("a", account) );
         }
            break;
         default:
            break;
      }
   }

   void producer_plugin_impl::on_block( const signed_block& b )
   {
      auto& db = _self.database();
      const dynamic_global_property_object& props = db.get_dynamic_global_properties();
      const median_chain_property_object& median_props = db.get_median_chain_properties();
      int64_t max_block_size = median_props.maximum_block_size;

      auto reserve_ratio_ptr = db.find( reserve_ratio_id_type() );

      if( BOOST_UNLIKELY( reserve_ratio_ptr == nullptr ) )
      {
         db.create< reserve_ratio_object >([&]( reserve_ratio_object &r ) 
         {
            r.average_block_size = 0;
            r.current_reserve_ratio = MAX_RESERVE_RATIO * RESERVE_RATIO_PRECISION;
            r.max_virtual_bandwidth = ( uint128_t( MAX_BLOCK_SIZE * MAX_RESERVE_RATIO ) * BANDWIDTH_PRECISION * BANDWIDTH_AVERAGE_WINDOW.count() ) / uint128_t( BLOCK_INTERVAL.count() );
         });
      }
      else
      {
         db.modify( *reserve_ratio_ptr, [&]( reserve_ratio_object& r )
         {
            // 100 Block moving average block size.
            r.average_block_size = ( 99 * r.average_block_size + fc::raw::pack_size( b ) ) / 100;

            /**
            * About once per minute the average network use is consulted and used to
            * adjust the reserve ratio. Anything above 25% usage reduces the reserve
            * ratio proportional to the distance from 25%. If usage is at 50% then
            * the reserve ratio will half. Likewise, if it is at 12% it will increase by 50%.
            *
            * If the reserve ratio is consistently low, then it is probably time to increase
            * the capcacity of the network.
            *
            * This algorithm is designed to react quickly to observations significantly
            * different from past observed behavior and make small adjustments when
            * behavior is within expected norms.
            */
            if( props.head_block_number % BLOCKS_PER_MINUTE == 0 )
            {
               int64_t distance = ( ( r.average_block_size - ( max_block_size / 4 ) ) * DISTANCE_CALC_PRECISION )
                  / ( max_block_size / 4 );
               auto old_reserve_ratio = r.current_reserve_ratio;

               if( distance > 0 )
               {
                  r.current_reserve_ratio -= ( r.current_reserve_ratio * distance ) / ( distance + DISTANCE_CALC_PRECISION );

                  // We do not want the reserve ratio to drop below 1
                  if( r.current_reserve_ratio < RESERVE_RATIO_PRECISION )
                  {
                     r.current_reserve_ratio = RESERVE_RATIO_PRECISION;
                  }
               }
               else
               {
                  // By default, we should always slowly increase the reserve ratio.
                  r.current_reserve_ratio += std::max( RESERVE_RATIO_MIN_INCREMENT, ( r.current_reserve_ratio * distance ) / ( distance - DISTANCE_CALC_PRECISION ) );

                  if( r.current_reserve_ratio > MAX_RESERVE_RATIO * RESERVE_RATIO_PRECISION )
                  {
                     r.current_reserve_ratio = MAX_RESERVE_RATIO * RESERVE_RATIO_PRECISION;
                  }
               }

               if( old_reserve_ratio != r.current_reserve_ratio )
               {
                  ilog( "Reserve ratio updated from ${old} to ${new}. Block: ${blocknum}",
                     ("old", old_reserve_ratio)
                     ("new", r.current_reserve_ratio)
                     ("blocknum", db.head_block_num()) );
               }

               r.max_virtual_bandwidth = ( uint128_t( max_block_size ) * uint128_t( r.current_reserve_ratio )
                                         * uint128_t( BANDWIDTH_PRECISION * BANDWIDTH_AVERAGE_WINDOW.count() ) )
                                         / ( BLOCK_INTERVAL.count() * RESERVE_RATIO_PRECISION );
            }
         });
      }

      _dupe_customs.clear();
   }

   void producer_plugin_impl::update_account_bandwidth( const account_object& a, uint32_t trx_size, const bandwidth_type type )
   {
      database& _db = _self.database();
      const dynamic_global_property_object& props = _db.get_dynamic_global_properties();
      bool has_bandwidth = true;

      if( props.total_voting_power > 0 )
      {
         auto band = _db.find< account_bandwidth_object, by_account_bandwidth_type >( boost::make_tuple( a.name, type ) );

         if( band == nullptr )
         {
            band = &_db.create< account_bandwidth_object >( [&]( account_bandwidth_object& b )
            {
               b.account = a.name;
               b.type = type;

            });
         }

         share_type new_bandwidth;
         share_type trx_bandwidth = trx_size * BANDWIDTH_PRECISION;
         fc::microseconds delta_time = _db.head_block_time() - band->last_bandwidth_update;

         if( delta_time > BANDWIDTH_AVERAGE_WINDOW )
         {
            new_bandwidth = 0;
         }
            
         else
         {
            new_bandwidth = ( ( ( BANDWIDTH_AVERAGE_WINDOW - delta_time ).to_seconds() * fc::uint128( band->average_bandwidth.value ) )
               / BANDWIDTH_AVERAGE_WINDOW.to_seconds() ).to_uint64();
         }
            

         new_bandwidth += trx_bandwidth;

         _db.modify( *band, [&]( account_bandwidth_object& b )
         {
            b.average_bandwidth = new_bandwidth;
            b.lifetime_bandwidth += trx_bandwidth;
            b.last_bandwidth_update = _db.head_block_time();
         });

         fc::uint128 account_voting_power( _db.get_voting_power(a).value );
         fc::uint128 total_voting_power( props.total_voting_power );
         fc::uint128 account_average_bandwidth( band->average_bandwidth.value );
         share_type account_lifetime_bandwidth( band->lifetime_bandwidth );
         fc::uint128 max_virtual_bandwidth( _db.get( reserve_ratio_id_type() ).max_virtual_bandwidth );
         // in effect
         // if a users voting power times the global max virtual bandwidth is great than the accounts average bandwidth 
         // times the total score of the network then it has enough bandwidth
         // max network bandwidth * account score // 50 * 10 = 500
         // account average bandwidth * total network score // 1 * 1000 = 1000
         // max network bandwidth * account score // 50 * 10 = 500
         // 528482304000000000000 * 0 = 0
         // needs to be greater than
         // account average bandwidth * total network score
         // 288000000 * 842277942443315491 = 2.4257604742367484e+26

         // This basically says that if it's the accounts first transaction they have the bandwidth.
         // This is a hack to get around not being able to claim score reward balance in the first place due to not having enough score to do so.....
//         if(trx_bandwidth.value != account_lifetime_bandwidth.value && trx_bandwidth.value != new_bandwidth.value){
//         }
         has_bandwidth = ( account_voting_power * max_virtual_bandwidth ) > ( account_average_bandwidth * total_voting_power );

         if( _db.is_producing() )
            ASSERT( has_bandwidth, chain::plugin_exception,
               "Account: ${account} bandwidth limit exceeded. Please wait to transact or increase staked balance.",
               ("account", a.name)
               ("account_voting_power", account_voting_power)
               ("account_average_bandwidth", account_average_bandwidth)
               ("max_virtual_bandwidth", max_virtual_bandwidth)
               ("total_voting_power", total_voting_power) );
      }
   }
}

producer_plugin::producer_plugin( application* app )
   : plugin( app ), _my( new detail::producer_plugin_impl( *this ) ) {}

producer_plugin::~producer_plugin()
{
   try
   {
      if( _block_production_task.valid() )
         _block_production_task.cancel_and_wait(__FUNCTION__);
   }
   catch(fc::canceled_exception&)
   {
      //Expected exception. Move along.
   }
   catch(fc::exception& e)
   {
      edump((e.to_detail_string()));
   }
}

void producer_plugin::plugin_set_program_options(
   boost::program_options::options_description& command_line_options,
   boost::program_options::options_description& config_file_options)
{
   string producer_id_example = "init_producer";
   command_line_options.add_options()
         ("enable-stale-production", bpo::bool_switch()->notifier([this](bool e){_production_enabled = e;}), "Enable block production, even if the chain is stale.")
         ("required-participation", bpo::bool_switch()->notifier([this](int e){_required_producer_participation = uint32_t(e*PERCENT_1);}), "Percent of producers (0-99) that must be participating in order to produce blocks")
         ("producer,w", bpo::value<vector<string>>()->composing()->multitoken(),
          ("name of producer controlled by this node (e.g. " + producer_id_example+" )" ).c_str())
         ("private-key", bpo::value<vector<string>>()->composing()->multitoken(), "WIF PRIVATE KEY to be used by one or more producers or miners" )
         ;
   config_file_options.add(command_line_options);
}

std::string producer_plugin::plugin_name()const
{
   return "producer";
}

using std::vector;
using std::pair;
using std::string;

void producer_plugin::plugin_initialize(const boost::program_options::variables_map& options)
{ try {
   _options = &options;
   LOAD_VALUE_SET(options, "producer", _producers, string)

   if( options.count("private-key") )
   {
      const std::vector<std::string> keys = options["private-key"].as<std::vector<std::string>>();
      for (const std::string& wif_key : keys )
      {
         fc::optional<fc::ecc::private_key> private_key = graphene::utilities::wif_to_key(wif_key);
         FC_ASSERT( private_key.valid(), "unable to parse private key" );
         _private_keys[private_key->get_public_key()] = *private_key;
      }
   }

   chain::database& db = database();

   db.post_apply_operation.connect( [&]( const operation_notification& note ){ _my->post_operation( note ); } );
   db.pre_apply_block.connect( [&]( const signed_block& b ){ _my->pre_apply_block( b ); } );
   db.on_pre_apply_transaction.connect( [&]( const signed_transaction& tx ){ _my->pre_transaction( tx ); } );
   db.pre_apply_operation.connect( [&]( const operation_notification& note ){ _my->pre_operation( note ); } );
   db.applied_block.connect( [&]( const signed_block& b ){ _my->on_block( b ); } );

   add_plugin_index< account_bandwidth_index >( db );
   add_plugin_index< content_edit_lock_index >( db );
   add_plugin_index< reserve_ratio_index     >( db );
} FC_LOG_AND_RETHROW() }

void producer_plugin::plugin_startup()
{ try {
   ilog("producer plugin:  plugin_startup() begin");
   chain::database& d = database();

   if( !_producers.empty() )
   {
      ilog("Launching block production for ${n} producers.", ("n", _producers.size()));
      idump( (_producers) );
      app().set_block_production(true);
      if( _production_enabled )
      {
         if( d.head_block_num() == 0 )
         {
            new_chain_banner(d);
         }
         _production_skip_flags |= node::chain::database::skip_undo_history_check;
      }
      schedule_production_loop();
   }
   else
   {
      elog("No producers configured! Please add producer names and private keys to configuration.");
   }
   ilog("producer plugin:  plugin_startup() end");
} FC_CAPTURE_AND_RETHROW() }

void producer_plugin::plugin_shutdown()
{
   return;
}

void producer_plugin::schedule_production_loop()
{
   //Schedule for the next second's tick regardless of chain state
   // If we would wait less than 50ms, wait for the whole second.
   fc::time_point fc_now = fc::time_point::now();
   int64_t time_to_next_second = 1000000 - (fc_now.time_since_epoch().count() % 1000000);
   if( time_to_next_second < 50000 )      // we must sleep for at least 50ms
       time_to_next_second += 1000000;

   fc::time_point next_wakeup( fc_now + fc::microseconds( time_to_next_second ) );

   //wdump( (now.time_since_epoch().count())(next_wakeup.time_since_epoch().count()) );
   _block_production_task = fc::schedule([this]{block_production_loop();},
                                         next_wakeup, "producer Block Production");
}

block_production_condition::block_production_condition_enum producer_plugin::block_production_loop()
{
   if( fc::time_point::now() < fc::time_point(GENESIS_TIME) )
   {
      wlog( "waiting until genesis time to produce block: ${t}", ("t",GENESIS_TIME) );
      schedule_production_loop();
      return block_production_condition::wait_for_genesis;
   }

   block_production_condition::block_production_condition_enum result;
   fc::mutable_variant_object capture;
   try
   {
      result = maybe_produce_block(capture);
   }
   catch( const fc::canceled_exception& )
   {
      //We're trying to exit. Go ahead and let this one out.
      throw;
   }
   catch( const node::chain::unknown_hardfork_exception& e )
   {
      // Hit a hardfork that the current node know nothing about, stop production and inform user
      elog( "${e}\nNode may be out of date...", ("e", e.to_detail_string()) );
      throw;
   }
   catch( const fc::exception& e )
   {
      elog("Got exception while generating block:\n${e}", ("e", e.to_detail_string()));
      result = block_production_condition::exception_producing_block;
   }

   switch( result )
   {
      case block_production_condition::produced:
         ilog("Producer: ${w} Successfully Generated block #${n} with timestamp ${t}.", (capture));
         break;
      case block_production_condition::not_synced:
         ilog("Not Producing Block: Production is disabled until we receive a recent block.");
         break;
      case block_production_condition::not_my_turn:
         ilog("Not Producing Block: Awaiting Allocated production time slot.");
         break;
      case block_production_condition::not_time_yet:
         ilog("Not Producing Block: Production Slot has not yet arrived.");
         break;
      case block_production_condition::no_private_key:
         ilog("Not Producing Block: Node with Producer: ${sw} does not have access to the private key for Publick key: ${sk}", (capture) );
         break;
      case block_production_condition::low_participation:
         elog("Not Producing Block: Node appears to be on a minority fork with only ${pct}% producer participation.", (capture) );
         break;
      case block_production_condition::lag:
         elog("Not Producing Block: Node unresponsive within 500ms of the allocated slot time. Actual response time: ${lag} ms");
         break;
      case block_production_condition::consecutive:
         elog("Not Producing Block: Last block was generated by the same producer.\nThis node is probably disconnected from the network so block production has been disabled.\nDisable this check with --allow-consecutive option.");
         break;
      case block_production_condition::exception_producing_block:
         elog("Failure when producing block with no transactions");
         break;
      case block_production_condition::wait_for_genesis:
         break;
   }

   schedule_production_loop();
   return result;
}

block_production_condition::block_production_condition_enum producer_plugin::maybe_produce_block( fc::mutable_variant_object& capture )
{
   chain::database& db = database();
   fc::time_point now_fine = fc::time_point::now();
   fc::time_point now = now_fine + fc::microseconds( 500000 );

   // If the next block production opportunity is in the present or future, we're synced.
   if( !_production_enabled )
   {
      if( db.get_slot_time(1) >= now )
         _production_enabled = true;
      else
         return block_production_condition::not_synced;
   }

   // is anyone scheduled to produce now or one second in the future?
   uint32_t slot = db.get_slot_at_time( now );
   if( slot == 0 )
   {
      capture("next_time", db.get_slot_time(1));
      return block_production_condition::not_time_yet;
   }

   // this assert should not fail, because now <= db.head_block_time()
   // should have resulted in slot == 0.
   //
   // if this assert triggers, there is a serious bug in get_slot_at_time()
   // which would result in allowing a later block to have a timestamp
   // less than or equal to the previous block
	 
   assert( now > db.head_block_time() );

   string scheduled_producer = db.get_scheduled_producer( slot );
	 ilog("the scheduled_producer is ${producer}", ("producer", scheduled_producer));
   // we must control the producer scheduled to produce the next block.
   if( _producers.find( scheduled_producer ) == _producers.end() )
   {
      capture("sw", scheduled_producer);
      return block_production_condition::not_my_turn;
   }

   const auto& producer_by_name = db.get_index< chain::producer_index >().indices().get< chain::by_name >();
   auto itr = producer_by_name.find( scheduled_producer );

   fc::time_point scheduled_time = db.get_slot_time( slot );
   node::protocol::public_key_type scheduled_key = itr->signing_key;
   auto private_key_itr = _private_keys.find( scheduled_key );

   if( private_key_itr == _private_keys.end() )
   {
      capture("sw", scheduled_producer);
      capture("sk", scheduled_key);
      return block_production_condition::no_private_key;
   }

   uint32_t prate = db.producer_participation_rate();
   if( prate < _required_producer_participation )
   {
      capture("pct", uint32_t(100*uint64_t(prate) / PERCENT_1));
      return block_production_condition::low_participation;
   }

   if( llabs((scheduled_time - now).count()) > fc::milliseconds( 500 ).count() )
   {
      capture("st", scheduled_time)("now", now)("lag", (scheduled_time - now).count()/1000 );
      return block_production_condition::lag;
   }

   int retry = 0;
   do
   {
      try
      {
      auto block = db.generate_block(
         scheduled_time,
         scheduled_producer,
         private_key_itr->second,
         _production_skip_flags
         );
         capture("n", block.block_num())("t", block.timestamp)("c", now)("w",scheduled_producer);
         fc::async( [this,block](){ p2p_node().broadcast(graphene::net::block_message(block)); } );

         return block_production_condition::produced;
      }
      catch( fc::exception& e )
      {
         elog( "${e}", ("e",e.to_detail_string()) );
         elog( "Clearing pending transactions and attempting again" );
         db.clear_pending();
         retry++;
      }
   } while( retry < 2 );

   return block_production_condition::exception_producing_block;
}

} } // node::producer

DEFINE_PLUGIN( producer, node::producer::producer_plugin )