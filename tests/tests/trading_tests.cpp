#include <boost/test/unit_test.hpp>
#include <node/protocol/exceptions.hpp>
#include <node/chain/database.hpp>
#include <node/chain/database_exceptions.hpp>
#include <node/chain/util/reward.hpp>
#include <fc/crypto/digest.hpp>
#include "../common/database_fixture.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>

using namespace node;
using namespace node::chain;
using namespace node::protocol;
using std::string;

BOOST_FIXTURE_TEST_SUITE( trading_operation_tests, clean_database_fixture );



   //=======================//
   // === Trading Tests === //
   //=======================//



BOOST_AUTO_TEST_CASE( limit_order_operation_test )
{
   try
   {
      BOOST_TEST_MESSAGE( "├── Testing: LIMIT ORDER" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: creation of limit order" );

      ACTORS( (alice)(bob)(candice)(dan)(elon)(fred)(george)(haz) );

      fund_liquid( "alice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_stake( "alice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_liquid( "alice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );

      fund_liquid( "bob", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_stake( "bob", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_liquid( "bob", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      
      fund_liquid( "candice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_stake( "candice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_liquid( "candice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );

      fund_liquid( "dan", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_stake( "dan", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_liquid( "dan", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      
      signed_transaction tx;

      limit_order_operation limit;

      limit.signatory = "alice";
      limit.owner = "alice";
      limit.order_id = "db35cabd-2aee-41b1-84ab-7372c4b6f8e5";
      limit.amount_to_sell = asset( 1000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN );
      limit.exchange_rate = price( asset( 1, SYMBOL_COIN ), asset( 1, SYMBOL_USD ) );
      limit.expiration = now() + fc::days( 30 );
      limit.interface = INIT_ACCOUNT;
      limit.fill_or_kill = false;
      limit.opened = true;
      limit.validate();

      tx.operations.push_back( limit );
      tx.set_expiration( now() + fc::seconds( MAX_TIME_UNTIL_EXPIRATION ) );
      tx.sign( alice_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      const limit_order_object& alice_order = db.get_limit_order( account_name_type( "alice" ), string( "db35cabd-2aee-41b1-84ab-7372c4b6f8e5" ) );

      BOOST_REQUIRE( alice_order.seller == limit.owner );
      BOOST_REQUIRE( alice_order.for_sale == limit.amount_to_sell.amount );
      BOOST_REQUIRE( alice_order.sell_price == limit.exchange_rate );
      BOOST_REQUIRE( alice_order.interface == limit.interface );
      BOOST_REQUIRE( alice_order.expiration == limit.expiration );
      BOOST_REQUIRE( alice_order.amount_for_sale() == limit.amount_to_sell );
      BOOST_REQUIRE( alice_order.amount_to_receive() == limit.amount_to_sell * limit.exchange_rate );
      
      validate_database();

      BOOST_TEST_MESSAGE( "│   ├── Passed: creation of limit order" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: failure when account does not have required funds" );

      limit.order_id = "6bb04e7a-d350-4dba-90c0-08b58712160a";
      limit.amount_to_sell = asset( 100000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN );
      
      tx.operations.push_back( limit );
      tx.sign( alice_private_active_key, db.get_chain_id() );
      REQUIRE_THROW( db.push_transaction( tx, 0 ), fc::exception );

      tx.operations.clear();
      tx.signatures.clear();

      validate_database();

      BOOST_TEST_MESSAGE( "│   ├── Passed: failure when account does not have required funds" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: failure when amount to sell is 0" );

      limit.order_id = "6bb04e7a-d350-4dba-90c0-08b58712160a";
      limit.amount_to_sell = asset( 0, SYMBOL_COIN );
      limit.exchange_rate = price( asset( 1, SYMBOL_COIN ), asset( 1, SYMBOL_USD ) );
      
      tx.operations.push_back( limit );
      tx.sign( alice_private_active_key, db.get_chain_id() );
      REQUIRE_THROW( db.push_transaction( tx, 0 ), fc::exception );

      tx.operations.clear();
      tx.signatures.clear();

      validate_database();

      BOOST_TEST_MESSAGE( "│   ├── Passed: failure when amount to sell is 0" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: failure when fill or kill order is not filled" );

      limit.order_id = "6bb04e7a-d350-4dba-90c0-08b58712160a";
      limit.amount_to_sell = asset( 100 * BLOCKCHAIN_PRECISION, SYMBOL_COIN );
      limit.exchange_rate = price( asset( 1, SYMBOL_COIN ), asset( 1, SYMBOL_USD ) );
      limit.fill_or_kill = true;
      
      tx.operations.push_back( limit );
      tx.sign( alice_private_active_key, db.get_chain_id() );
      REQUIRE_THROW( db.push_transaction( tx, 0 ), fc::exception );

      tx.operations.clear();
      tx.signatures.clear();

      validate_database();

      BOOST_TEST_MESSAGE( "│   ├── Passed: failure when fill or kill order is not filled" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: Filling existing limit order fully, but the new order partially" );

      limit.signatory = "bob";
      limit.owner = "bob";
      limit.order_id = "10f11157-9460-4505-b346-28e5b9ed77ed";
      limit.amount_to_sell = asset( 2000 * BLOCKCHAIN_PRECISION, SYMBOL_USD );
      limit.exchange_rate = price( asset( 1, SYMBOL_USD ), asset( 1, SYMBOL_COIN ) );
      limit.expiration = now() + fc::days( 30 );
      limit.interface = INIT_ACCOUNT;
      limit.fill_or_kill = false;
      limit.validate();

      tx.operations.push_back( limit );
      tx.sign( bob_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      const limit_order_object& bob_order = db.get_limit_order( account_name_type( "bob" ), string( "10f11157-9460-4505-b346-28e5b9ed77ed" ) );

      BOOST_REQUIRE( bob_order.seller == limit.owner );
      BOOST_REQUIRE( bob_order.sell_price == limit.exchange_rate );
      BOOST_REQUIRE( bob_order.interface == limit.interface );
      BOOST_REQUIRE( bob_order.expiration == limit.expiration );
      BOOST_REQUIRE( bob_order.amount_for_sale() == asset( 1000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      BOOST_REQUIRE( bob_order.amount_to_receive() == asset( 1000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );

      const auto& limit_idx = db.get_index< limit_order_index >().indices().get< by_account >();
      auto limit_itr = limit_idx.find( std::make_tuple( account_name_type( "alice" ), string( "db35cabd-2aee-41b1-84ab-7372c4b6f8e5" ) ) );

      BOOST_REQUIRE( limit_itr == limit_idx.end() );

      BOOST_TEST_MESSAGE( "│   ├── Passed: Filling existing limit order fully, but the new order partially" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: Filling existing limit order and the new order fully" );

      limit.signatory = "candice";
      limit.owner = "candice";
      limit.order_id = "ad3cf087-bf74-41d8-9ff5-6e302fff2446";
      limit.amount_to_sell = asset( 1000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN );
      limit.exchange_rate = price( asset( 1, SYMBOL_COIN ), asset( 1, SYMBOL_USD ) );
      limit.expiration = now() + fc::days( 30 );
      limit.interface = INIT_ACCOUNT;
      limit.fill_or_kill = false;
      limit.validate();

      tx.operations.push_back( limit );
      tx.sign( candice_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      limit_itr = limit_idx.find( std::make_tuple( account_name_type( "candice" ), string( "ad3cf087-bf74-41d8-9ff5-6e302fff2446" ) ) );

      BOOST_REQUIRE( limit_itr == limit_idx.end() );

      validate_database();

      BOOST_TEST_MESSAGE( "│   ├── Passed: Filling existing limit order and the new order fully" );

      BOOST_TEST_MESSAGE( "├── Testing: LIMIT ORDER" );
   }
   FC_LOG_AND_RETHROW()
}



BOOST_AUTO_TEST_CASE( margin_order_operation_test )
{
   try
   {
      BOOST_TEST_MESSAGE( "├── Testing: MARGIN ORDER" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: Creation sequence of margin order" );

      ACTORS( (alice)(bob)(candice)(dan)(elon)(fred)(george)(haz) );

      fund_liquid( "alice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_stake( "alice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_liquid( "alice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );

      fund_liquid( "bob", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_stake( "bob", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_liquid( "bob", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      
      fund_liquid( "candice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_stake( "candice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_liquid( "candice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );

      fund_liquid( "dan", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_stake( "dan", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_liquid( "dan", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );

      fund_liquid( "elon", asset( 100000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_stake( "elon", asset( 100000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_liquid( "elon", asset( 100000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      fund_liquid( "elon", asset( 100000 * BLOCKCHAIN_PRECISION, SYMBOL_CREDIT ) );
      
      signed_transaction tx;

      liquidity_pool_fund_operation fund;

      fund.signatory = "elon";
      fund.account = "elon";
      fund.amount = asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN );
      fund.pair_asset = SYMBOL_USD;
      fund.validate();

      tx.operations.push_back( fund );
      tx.set_expiration( now() + fc::seconds( MAX_TIME_UNTIL_EXPIRATION ) );
      tx.sign( elon_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      fund.amount = asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_USD );
      fund.pair_asset = SYMBOL_COIN;

      tx.operations.push_back( fund );
      tx.sign( elon_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      fund.amount = asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN );
      fund.pair_asset = SYMBOL_CREDIT;

      tx.operations.push_back( fund );
      tx.sign( elon_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      fund.amount = asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_CREDIT );
      fund.pair_asset = SYMBOL_COIN;

      tx.operations.push_back( fund );
      tx.sign( elon_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      credit_pool_lend_operation lend;

      lend.signatory = "elon";
      lend.account = "elon";
      lend.amount = asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN );
      lend.validate();

      tx.operations.push_back( lend );
      tx.sign( elon_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      lend.signatory = "elon";
      lend.account = "elon";
      lend.amount = asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_USD );

      tx.operations.push_back( lend );
      tx.sign( elon_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      credit_pool_collateral_operation collateral;

      collateral.signatory = "alice";
      collateral.account = "alice";
      collateral.amount = asset( 4000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN );
      collateral.validate();

      tx.operations.push_back( collateral );
      tx.sign( alice_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      collateral.signatory = "bob";
      collateral.account = "bob";

      tx.operations.push_back( collateral );
      tx.sign( bob_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      collateral.signatory = "candice";
      collateral.account = "candice";

      tx.operations.push_back( collateral );
      tx.sign( candice_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      margin_order_operation margin;

      margin.signatory = "alice";
      margin.owner = "alice";
      margin.order_id = "db35cabd-2aee-41b1-84ab-7372c4b6f8e5";
      margin.collateral = asset( 1000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN );
      margin.amount_to_borrow = asset( 100 * BLOCKCHAIN_PRECISION, SYMBOL_USD );
      margin.exchange_rate = price( asset( 1, SYMBOL_USD ), asset( 1, SYMBOL_COIN ) );    // Buying Coin at $1.00
      margin.expiration = now() + fc::days( 30 );
      margin.interface = INIT_ACCOUNT;
      margin.fill_or_kill = false;
      margin.opened = true;
      margin.force_close = false;
      margin.validate();

      tx.operations.push_back( margin );
      tx.sign( alice_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      const margin_order_object& alice_order = db.get_margin_order( account_name_type( "alice" ), string( "db35cabd-2aee-41b1-84ab-7372c4b6f8e5" ) );

      BOOST_REQUIRE( alice_order.owner == margin.owner );
      BOOST_REQUIRE( alice_order.collateral == margin.collateral );
      BOOST_REQUIRE( alice_order.debt == margin.amount_to_borrow );
      BOOST_REQUIRE( alice_order.debt_balance == margin.amount_to_borrow );
      BOOST_REQUIRE( alice_order.sell_price == margin.exchange_rate );
      BOOST_REQUIRE( alice_order.position == margin.amount_to_borrow * margin.exchange_rate );
      BOOST_REQUIRE( alice_order.interface == margin.interface );
      BOOST_REQUIRE( alice_order.position_balance.amount == 0 );
      BOOST_REQUIRE( alice_order.interest.amount == 0 );
      BOOST_REQUIRE( alice_order.expiration == margin.expiration );
      BOOST_REQUIRE( alice_order.amount_for_sale() == margin.amount_to_borrow );
      BOOST_REQUIRE( alice_order.amount_to_receive() == margin.amount_to_borrow * margin.exchange_rate );
      BOOST_REQUIRE( alice_order.liquidating == false );
      BOOST_REQUIRE( !alice_order.filled() );

      const credit_collateral_object& alice_collateral = db.get_collateral( account_name_type( "alice" ), SYMBOL_COIN );

      BOOST_REQUIRE( alice_collateral.collateral == asset( 4000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );

      BOOST_TEST_MESSAGE( "│   ├── Passed: Creation of margin order" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: Failure when account does not have required collateral" );

      margin.order_id = "6bb04e7a-d350-4dba-90c0-08b58712160a";
      margin.collateral = asset( 200000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN );
      
      tx.operations.push_back( margin );
      tx.sign( alice_private_active_key, db.get_chain_id() );
      REQUIRE_THROW( db.push_transaction( tx, 0 ), fc::exception );

      tx.operations.clear();
      tx.signatures.clear();

      BOOST_TEST_MESSAGE( "│   ├── Passed: Failure when account does not have required collateral" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: Failure when amount to borrow is 0" );

      margin.order_id = "6bb04e7a-d350-4dba-90c0-08b58712160a";
      margin.collateral = asset( 4000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN );
      margin.amount_to_borrow = asset( 0, SYMBOL_USD );
      
      tx.operations.push_back( margin );
      tx.sign( alice_private_active_key, db.get_chain_id() );
      REQUIRE_THROW( db.push_transaction( tx, 0 ), fc::exception );

      tx.operations.clear();
      tx.signatures.clear();

      BOOST_TEST_MESSAGE( "│   ├── Passed: Failure when amount to sell is 0" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: Failure when fill or kill order is not filled" );

      margin.order_id = "6bb04e7a-d350-4dba-90c0-08b58712160a";
      margin.amount_to_borrow = asset( 100 * BLOCKCHAIN_PRECISION, SYMBOL_COIN );
      margin.fill_or_kill = true;
      
      tx.operations.push_back( margin );
      tx.sign( alice_private_active_key, db.get_chain_id() );
      REQUIRE_THROW( db.push_transaction( tx, 0 ), fc::exception );

      tx.operations.clear();
      tx.signatures.clear();

      BOOST_TEST_MESSAGE( "│   ├── Passed: Failure when fill or kill order is not filled" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: Filling existing margin order fully, but the new order partially" );

      margin.signatory = "bob";
      margin.owner = "bob";
      margin.order_id = "10f11157-9460-4505-b346-28e5b9ed77ed";
      margin.amount_to_borrow = asset( 200 * BLOCKCHAIN_PRECISION, SYMBOL_COIN );
      margin.exchange_rate = price( asset( 1, SYMBOL_COIN ), asset( 1, SYMBOL_USD ) );    // Selling Coin at $1.00
      margin.fill_or_kill = false;
      margin.validate();

      tx.operations.push_back( margin );
      tx.sign( bob_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      const margin_order_object& bob_order = db.get_margin_order( account_name_type( "bob" ), string( "10f11157-9460-4505-b346-28e5b9ed77ed" ) );

      BOOST_REQUIRE( bob_order.owner == margin.owner );
      BOOST_REQUIRE( bob_order.collateral == margin.collateral );
      BOOST_REQUIRE( bob_order.debt == asset( 200 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      BOOST_REQUIRE( bob_order.debt_balance == asset( 1000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      BOOST_REQUIRE( bob_order.sell_price == margin.exchange_rate );
      BOOST_REQUIRE( bob_order.position == asset( 200 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      BOOST_REQUIRE( bob_order.interface == margin.interface );
      BOOST_REQUIRE( bob_order.position_balance == asset( 100 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      BOOST_REQUIRE( bob_order.expiration == margin.expiration );
      BOOST_REQUIRE( bob_order.amount_for_sale() == asset( 100 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      BOOST_REQUIRE( bob_order.amount_to_receive() == asset( 100 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      BOOST_REQUIRE( bob_order.liquidating == false );
      BOOST_REQUIRE( !bob_order.filled() );

      BOOST_REQUIRE( alice_order.debt == asset( 100 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      BOOST_REQUIRE( alice_order.debt_balance.amount == 0 );
      BOOST_REQUIRE( alice_order.collateral == margin.collateral );
      BOOST_REQUIRE( alice_order.position == asset( 100 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      BOOST_REQUIRE( alice_order.position_balance == asset( 100 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      BOOST_REQUIRE( alice_order.amount_for_sale().amount == 0 );
      BOOST_REQUIRE( alice_order.amount_to_receive().amount == 0 );
      BOOST_REQUIRE( alice_order.liquidating == false );
      BOOST_REQUIRE( alice_order.filled() );

      const credit_collateral_object& bob_collateral = db.get_collateral( account_name_type( "bob" ), SYMBOL_COIN );

      BOOST_REQUIRE( bob_collateral.collateral == asset( 4000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      BOOST_REQUIRE( alice_collateral.collateral == asset( 4000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );

      BOOST_TEST_MESSAGE( "│   ├── Passed: Filling existing margin order fully, but the new order partially" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: Closing filled order at higher price" );

      margin.signatory = "alice";
      margin.owner = "alice";
      margin.order_id = "db35cabd-2aee-41b1-84ab-7372c4b6f8e5";
      margin.amount_to_borrow = asset( 100 * BLOCKCHAIN_PRECISION, SYMBOL_USD );
      margin.exchange_rate = price( asset( 15, SYMBOL_USD ), asset( 10, SYMBOL_COIN ) );   // Selling Coin at $1.50
      margin.opened = false;
      margin.force_close = false;
      margin.validate();

      tx.operations.push_back( margin );
      tx.sign( alice_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      BOOST_REQUIRE( alice_order.debt == asset( 100 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      BOOST_REQUIRE( alice_order.debt_balance.amount == 0 );
      BOOST_REQUIRE( alice_order.position == asset( 100 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      BOOST_REQUIRE( alice_order.position_balance == asset( 100 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      BOOST_REQUIRE( alice_order.collateral == margin.collateral );
      BOOST_REQUIRE( alice_order.amount_for_sale() == asset( 100 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      BOOST_REQUIRE( alice_order.amount_to_receive() == asset( 150 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      BOOST_REQUIRE( alice_order.liquidating == true );
      BOOST_REQUIRE( alice_order.filled() );

      BOOST_TEST_MESSAGE( "│   ├── Passed: Closing filled order at higher price" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: Filling existing margin orders" );

      margin.signatory = "candice";
      margin.owner = "candice";
      margin.order_id = "60790f19-2046-4b7e-98ef-bab36153a945";
      margin.amount_to_borrow = asset( 1000 * BLOCKCHAIN_PRECISION, SYMBOL_USD );
      margin.exchange_rate = price( asset( 15, SYMBOL_USD ), asset( 10, SYMBOL_COIN ) );   // Buying Coin at $1.50
      margin.opened = true;
      margin.validate();
      
      tx.operations.push_back( margin );
      tx.sign( candice_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      const margin_order_object& candice_order = db.get_margin_order( account_name_type( "candice" ), string( "60790f19-2046-4b7e-98ef-bab36153a945" ) );

      BOOST_REQUIRE( candice_order.debt == asset( 1000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      BOOST_REQUIRE( candice_order.position == candice_order.debt * candice_order.sell_price );
      BOOST_REQUIRE( candice_order.collateral == margin.collateral );
      BOOST_REQUIRE( candice_order.liquidating == false );
      BOOST_REQUIRE( candice_order.filled() );   // Filled from liquidity pool 

      const auto& margin_idx = db.get_index< margin_order_index >().indices().get< by_account >();
      auto margin_itr = margin_idx.find( std::make_tuple( account_name_type( "alice" ), string( "db35cabd-2aee-41b1-84ab-7372c4b6f8e5" ) ) );

      BOOST_REQUIRE( margin_itr == margin_idx.end() );

      margin_itr = margin_idx.find( std::make_tuple( account_name_type( "bob" ), string( "10f11157-9460-4505-b346-28e5b9ed77ed" ) ) );

      BOOST_REQUIRE( margin_itr == margin_idx.end() );

      BOOST_TEST_MESSAGE( "│   ├── Passed: Filling existing margin orders" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: Force closing filled remaining margin order" );

      margin.signatory = "candice";
      margin.owner = "candice";
      margin.order_id = "60790f19-2046-4b7e-98ef-bab36153a945";
      margin.amount_to_borrow = asset( 1000 * BLOCKCHAIN_PRECISION, SYMBOL_USD );
      margin.exchange_rate = price( asset( 15, SYMBOL_USD ), asset( 10, SYMBOL_COIN ) );
      margin.opened = false;
      margin.force_close = true;   // Liquidate to pools
      margin.validate();

      tx.operations.push_back( margin );
      tx.sign( candice_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      margin_itr = margin_idx.find( std::make_tuple( account_name_type( "candice" ), string( "60790f19-2046-4b7e-98ef-bab36153a945" ) ) );

      BOOST_REQUIRE( margin_itr == margin_idx.end() );
      
      validate_database();

      BOOST_TEST_MESSAGE( "│   ├── Passed: Closing filled order at higher price" );

      BOOST_TEST_MESSAGE( "├── Passed: MARGIN ORDER" );
   }
   FC_LOG_AND_RETHROW()
}



BOOST_AUTO_TEST_CASE( auction_order_operation_test )
{
   try
   {
      BOOST_TEST_MESSAGE( "├── Testing: AUCTION ORDER" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: Creation sequence of auction orderbook" );

      ACTORS( (alice)(bob)(candice)(dan) );

      fund_liquid( "alice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_stake( "alice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_liquid( "alice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );

      fund_liquid( "bob", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_stake( "bob", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_liquid( "bob", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      
      fund_liquid( "candice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_stake( "candice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_liquid( "candice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );

      fund_liquid( "dan", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_stake( "dan", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_liquid( "dan", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      
      signed_transaction tx;

      auction_order_operation auction;

      auction.signatory = "alice";
      auction.owner = "alice";
      auction.order_id = "7390e715-56df-4d70-bd6b-13e3f6867b51";
      auction.amount_to_sell = asset( 100 * BLOCKCHAIN_PRECISION, SYMBOL_COIN );
      auction.limit_close_price = price( asset( BLOCKCHAIN_PRECISION, SYMBOL_COIN ), asset( BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      auction.interface = INIT_ACCOUNT;
      auction.expiration = now() + fc::days(30);
      auction.opened = true;
      auction.validate();

      tx.operations.push_back( auction );
      tx.set_expiration( now() + fc::seconds( MAX_TIME_UNTIL_EXPIRATION ) );
      tx.sign( alice_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      const auction_order_object& alice_order = db.get_auction_order( account_name_type( "alice" ), string( "7390e715-56df-4d70-bd6b-13e3f6867b51" ) );

      BOOST_REQUIRE( alice_order.owner == auction.owner );
      BOOST_REQUIRE( to_string( alice_order.order_id ) == auction.order_id );
      BOOST_REQUIRE( alice_order.amount_to_sell == auction.amount_to_sell );
      BOOST_REQUIRE( alice_order.limit_close_price == auction.limit_close_price );
      BOOST_REQUIRE( alice_order.interface == auction.interface );

      auction.signatory = "bob";
      auction.owner = "bob";
      auction.order_id = "9498ad7d-31bb-4b74-a49e-27f7e6b85620";
      auction.amount_to_sell = asset( 200 * BLOCKCHAIN_PRECISION, SYMBOL_COIN );
      auction.limit_close_price = price( asset( BLOCKCHAIN_PRECISION, SYMBOL_COIN ), asset( BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      auction.validate();

      tx.operations.push_back( auction );
      tx.sign( bob_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      const auction_order_object& bob_order = db.get_auction_order( account_name_type( "bob" ), string( "9498ad7d-31bb-4b74-a49e-27f7e6b85620" ) );

      BOOST_REQUIRE( bob_order.owner == auction.owner );
      BOOST_REQUIRE( to_string( bob_order.order_id ) == auction.order_id );
      BOOST_REQUIRE( bob_order.amount_to_sell == auction.amount_to_sell );
      BOOST_REQUIRE( bob_order.limit_close_price == auction.limit_close_price );
      BOOST_REQUIRE( bob_order.interface == auction.interface );

      auction.signatory = "candice";
      auction.owner = "candice";
      auction.order_id = "cb8d0249-7ebb-4574-8596-2cc2447ccbcc";
      auction.amount_to_sell = asset( 100 * BLOCKCHAIN_PRECISION, SYMBOL_USD );
      auction.limit_close_price = price( asset( BLOCKCHAIN_PRECISION, SYMBOL_USD ), asset( BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      auction.validate();

      tx.operations.push_back( auction );
      tx.sign( candice_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      const auction_order_object& candice_order = db.get_auction_order( account_name_type( "candice" ), string( "cb8d0249-7ebb-4574-8596-2cc2447ccbcc" ) );

      BOOST_REQUIRE( candice_order.owner == auction.owner );
      BOOST_REQUIRE( to_string( candice_order.order_id ) == auction.order_id );
      BOOST_REQUIRE( candice_order.amount_to_sell == auction.amount_to_sell );
      BOOST_REQUIRE( candice_order.limit_close_price == auction.limit_close_price );
      BOOST_REQUIRE( candice_order.interface == auction.interface );

      auction.signatory = "dan";
      auction.owner = "dan";
      auction.order_id = "9743506a-0d99-47e5-9ca2-bad5a95c0055";
      auction.amount_to_sell = asset( 200 * BLOCKCHAIN_PRECISION, SYMBOL_USD );
      auction.limit_close_price = price( asset( BLOCKCHAIN_PRECISION, SYMBOL_USD ), asset( BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      auction.validate();

      tx.operations.push_back( auction );
      tx.sign( dan_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      const auction_order_object& dan_order = db.get_auction_order( account_name_type( "dan" ), string( "9743506a-0d99-47e5-9ca2-bad5a95c0055" ) );

      BOOST_REQUIRE( dan_order.owner == auction.owner );
      BOOST_REQUIRE( to_string( dan_order.order_id ) == auction.order_id );
      BOOST_REQUIRE( dan_order.amount_to_sell == auction.amount_to_sell );
      BOOST_REQUIRE( dan_order.limit_close_price == auction.limit_close_price );
      BOOST_REQUIRE( dan_order.interface == auction.interface );

      BOOST_TEST_MESSAGE( "│   ├── Passed: Creation sequence of auction orderbook" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: Failure when account does not have required underlying funds" );

      auction.signatory = "alice";
      auction.owner = "alice";
      auction.order_id = "7390e715-56df-4d70-bd6b-13e3f6867b51";
      auction.amount_to_sell = asset( 1000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN );
      auction.limit_close_price = price( asset( BLOCKCHAIN_PRECISION, SYMBOL_COIN ), asset( BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      
      tx.operations.push_back( auction );
      tx.sign( alice_private_active_key, db.get_chain_id() );
      REQUIRE_THROW( db.push_transaction( tx, 0 ), fc::exception );

      tx.operations.clear();
      tx.signatures.clear();

      BOOST_TEST_MESSAGE( "│   ├── Passed: Failure when account does not have required underlying funds" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: Auction processing" );

      asset init_alice_liquid_usd = get_liquid_balance( "alice", SYMBOL_USD );
      asset init_bob_liquid_usd = get_liquid_balance( "bob", SYMBOL_USD );
      asset init_candice_liquid_coin = get_liquid_balance( "candice", SYMBOL_COIN );
      asset init_dan_liquid_coin = get_liquid_balance( "dan", SYMBOL_COIN );

      generate_blocks( now() + fc::hours(25) );
      
      asset alice_liquid_usd = get_liquid_balance( "alice", SYMBOL_USD );
      asset bob_liquid_usd = get_liquid_balance( "bob", SYMBOL_USD );
      asset candice_liquid_coin = get_liquid_balance( "candice", SYMBOL_COIN );
      asset dan_liquid_coin = get_liquid_balance( "dan", SYMBOL_COIN );

      BOOST_REQUIRE( alice_liquid_usd == init_alice_liquid_usd + asset( 100 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      BOOST_REQUIRE( bob_liquid_usd == init_bob_liquid_usd + asset( 200 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );

      BOOST_REQUIRE( candice_liquid_coin == init_candice_liquid_coin + asset( 100 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      BOOST_REQUIRE( dan_liquid_coin == init_dan_liquid_coin + asset( 200 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );

      const auto& margin_idx = db.get_index< margin_order_index >().indices().get< by_account >();

      auto alice_margin_itr = margin_idx.find( std::make_tuple( account_name_type( "alice" ), string( "7390e715-56df-4d70-bd6b-13e3f6867b51" ) ) );
      BOOST_REQUIRE( alice_margin_itr == margin_idx.end() );

      auto bob_margin_itr = margin_idx.find( std::make_tuple( account_name_type( "bob" ), string( "9498ad7d-31bb-4b74-a49e-27f7e6b85620" ) ) );
      BOOST_REQUIRE( bob_margin_itr == margin_idx.end() );

      auto candice_margin_itr = margin_idx.find( std::make_tuple( account_name_type( "candice" ), string( "cb8d0249-7ebb-4574-8596-2cc2447ccbcc" ) ) );
      BOOST_REQUIRE( candice_margin_itr == margin_idx.end() );

      auto dan_margin_itr = margin_idx.find( std::make_tuple( account_name_type( "dan" ), string( "9743506a-0d99-47e5-9ca2-bad5a95c0055" ) ) );
      BOOST_REQUIRE( dan_margin_itr == margin_idx.end() );

      validate_database();

      BOOST_TEST_MESSAGE( "│   ├── Passed: Auction processing" );

      BOOST_TEST_MESSAGE( "├── Passed: AUCTION ORDER" );
   }
   FC_LOG_AND_RETHROW()
}


BOOST_AUTO_TEST_CASE( call_order_operation_test )
{
   try
   {
      BOOST_TEST_MESSAGE( "├── Testing: CALL ORDER" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: creation of call order" );

      ACTORS( (alice)(bob)(candice)(dan)(elon)(fred)(george)(haz) );

      fund_liquid( "alice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_stake( "alice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_liquid( "alice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      producer_create( "alice", alice_private_owner_key );
      producer_vote( "alice", alice_private_owner_key );

      fund_liquid( "bob", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_stake( "bob", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_liquid( "bob", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      producer_create( "bob", bob_private_owner_key );
      producer_vote( "bob", bob_private_owner_key );
      
      fund_liquid( "candice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_stake( "candice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_liquid( "candice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      producer_create( "candice", candice_private_owner_key );
      producer_vote( "candice", candice_private_owner_key );

      fund_liquid( "dan", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_stake( "dan", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_liquid( "dan", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      producer_create( "dan", dan_private_owner_key );
      producer_vote( "dan", dan_private_owner_key );

      generate_blocks( TOTAL_PRODUCERS );

      signed_transaction tx;

      asset_publish_feed_operation feed;

      feed.signatory = "alice";
      feed.publisher = "alice";
      feed.symbol = SYMBOL_USD;
      feed.feed.settlement_price = price( asset(1,SYMBOL_USD),asset(1,SYMBOL_COIN) );
      feed.validate();

      tx.operations.push_back( feed );
      tx.set_expiration( now() + fc::seconds( MAX_TIME_UNTIL_EXPIRATION ) );
      tx.sign( alice_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      feed.signatory = "bob";
      feed.publisher = "bob";

      tx.operations.push_back( feed );
      tx.sign( bob_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      feed.signatory = "candice";
      feed.publisher = "candice";

      tx.operations.push_back( feed );
      tx.sign( candice_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      feed.signatory = "dan";
      feed.publisher = "dan";

      tx.operations.push_back( feed );
      tx.sign( dan_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      call_order_operation call;

      call.signatory = "alice";
      call.owner = "alice";
      call.collateral = asset( 1000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN );
      call.debt = asset( 500 * BLOCKCHAIN_PRECISION, SYMBOL_USD );     // 200% Collateralization
      call.interface = INIT_ACCOUNT;
      call.validate();

      tx.operations.push_back( call );
      tx.sign( alice_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      const call_order_object& alice_order = db.get_call_order( account_name_type( "alice" ), SYMBOL_USD );

      BOOST_REQUIRE( alice_order.borrower == call.owner );
      BOOST_REQUIRE( alice_order.collateral == call.collateral );
      BOOST_REQUIRE( alice_order.debt == call.debt );
      BOOST_REQUIRE( alice_order.call_price == price( asset( 1, call.collateral.symbol ), asset( 1, call.debt.symbol ) ) );
      BOOST_REQUIRE( alice_order.interface == call.interface );
      BOOST_REQUIRE( alice_order.created == now() );
      BOOST_REQUIRE( alice_order.last_updated == now() );
      BOOST_REQUIRE( alice_order.amount_for_sale() == call.collateral );
      BOOST_REQUIRE( alice_order.amount_to_receive() == call.debt );
      BOOST_REQUIRE( alice_order.collateralization() == price( call.collateral, call.debt ) );
      
      validate_database();

      BOOST_TEST_MESSAGE( "│   ├── Passed: creation of call order" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: updating call order to cover some of the debt position" );

      call.collateral = asset( 800 * BLOCKCHAIN_PRECISION, SYMBOL_COIN );
      call.debt = asset( 500 * BLOCKCHAIN_PRECISION, SYMBOL_USD );    
      
      tx.operations.push_back( call );
      tx.sign( alice_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      BOOST_REQUIRE( alice_order.borrower == call.owner );
      BOOST_REQUIRE( alice_order.collateral == call.collateral );
      BOOST_REQUIRE( alice_order.debt == call.debt );
      BOOST_REQUIRE( alice_order.call_price == price( asset( 1, call.collateral.symbol ), asset( 1, call.debt.symbol ) ) );
      BOOST_REQUIRE( alice_order.interface == call.interface );
      BOOST_REQUIRE( alice_order.last_updated == now() );
      BOOST_REQUIRE( alice_order.amount_for_sale() == call.collateral );
      BOOST_REQUIRE( alice_order.amount_to_receive() == call.debt );
      BOOST_REQUIRE( alice_order.collateralization() == price( call.collateral, call.debt ) );

      validate_database();

      BOOST_TEST_MESSAGE( "│   ├── Passed: updating call order to cover some of the debt position" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: failure when account does not have required funds" );

      call.collateral = asset( 200000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN );
      
      tx.operations.push_back( call );
      tx.sign( alice_private_active_key, db.get_chain_id() );
      REQUIRE_THROW( db.push_transaction( tx, 0 ), fc::exception );

      tx.operations.clear();
      tx.signatures.clear();

      validate_database();

      BOOST_TEST_MESSAGE( "│   ├── Passed: failure when account does not have required funds" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: failure when account does not claim collateral when closing position" );

      call.collateral = asset( 800 * BLOCKCHAIN_PRECISION, SYMBOL_COIN );
      call.debt = asset( 0, SYMBOL_USD );
      
      tx.operations.push_back( call );
      tx.sign( alice_private_active_key, db.get_chain_id() );
      REQUIRE_THROW( db.push_transaction( tx, 0 ), fc::exception );

      tx.operations.clear();
      tx.signatures.clear();

      validate_database();

      BOOST_TEST_MESSAGE( "│   ├── Passed: failure when account does not claim collateral when closing position" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: failure when account removes collateral without closing debt position" );

      call.collateral = asset( 0, SYMBOL_COIN );
      call.debt = asset( 400, SYMBOL_USD );
      
      tx.operations.push_back( call );
      tx.sign( alice_private_active_key, db.get_chain_id() );
      REQUIRE_THROW( db.push_transaction( tx, 0 ), fc::exception );

      tx.operations.clear();
      tx.signatures.clear();

      validate_database();

      BOOST_TEST_MESSAGE( "│   ├── Passed: failure when account removes collateral without closing debt position" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: failure when debt and collateral amount is 0" );

      call.signatory = "bob";
      call.owner = "bob";
      call.collateral = asset( 0, SYMBOL_COIN );
      call.debt = asset( 0, SYMBOL_USD );
      
      tx.operations.push_back( call );
      tx.sign( bob_private_active_key, db.get_chain_id() );
      REQUIRE_THROW( db.push_transaction( tx, 0 ), fc::exception );

      tx.operations.clear();
      tx.signatures.clear();

      validate_database();

      BOOST_TEST_MESSAGE( "│   ├── Passed: failure when debt and collateral amount is 0" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: failure when insufficient collateral for specified debt" );

      call.collateral = asset( 1000, SYMBOL_COIN );
      call.debt = asset( 1000, SYMBOL_USD );
      
      tx.operations.push_back( call );
      tx.sign( bob_private_active_key, db.get_chain_id() );
      REQUIRE_THROW( db.push_transaction( tx, 0 ), fc::exception );

      tx.operations.clear();
      tx.signatures.clear();

      validate_database();

      BOOST_TEST_MESSAGE( "│   ├── Passed: failure when insufficient collateral for specified debt" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: Margin calling existing order" );

      limit_order_operation limit;

      limit.signatory = "bob";
      limit.owner = "bob";
      limit.order_id = "94b21fb9-5053-4a4d-ba96-b78d312dc054";
      limit.amount_to_sell = asset( 1000 * BLOCKCHAIN_PRECISION, SYMBOL_USD );
      limit.exchange_rate = price( asset( 1, SYMBOL_USD ), asset( 1, SYMBOL_COIN ) );   // Buying Coin at $1.00
      limit.expiration = now() + fc::days( 30 );
      limit.interface = INIT_ACCOUNT;
      limit.fill_or_kill = false;
      limit.opened = true;
      limit.validate();

      tx.operations.push_back( limit );
      tx.sign( bob_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      const limit_order_object& bob_order = db.get_limit_order( account_name_type( "bob" ), string( "94b21fb9-5053-4a4d-ba96-b78d312dc054" ) );

      BOOST_REQUIRE( bob_order.seller == limit.owner );
      BOOST_REQUIRE( bob_order.sell_price == limit.exchange_rate );
      BOOST_REQUIRE( bob_order.interface == limit.interface );
      BOOST_REQUIRE( bob_order.expiration == limit.expiration );
      BOOST_REQUIRE( bob_order.amount_for_sale() == asset( 1000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      BOOST_REQUIRE( bob_order.amount_to_receive() == asset( 1000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );

      tx.operations.push_back( call );
      tx.sign( bob_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      feed.signatory = "alice";
      feed.publisher = "alice";
      feed.symbol = SYMBOL_USD;
      feed.feed.settlement_price = price( asset(75,SYMBOL_USD),asset(100,SYMBOL_COIN) );   // Settlement price of $0.75

      tx.operations.push_back( feed );
      tx.sign( alice_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      feed.signatory = "bob";
      feed.publisher = "bob";

      tx.operations.push_back( feed );
      tx.sign( bob_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      feed.signatory = "candice";
      feed.publisher = "candice";

      tx.operations.push_back( feed );
      tx.sign( candice_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      feed.signatory = "dan";
      feed.publisher = "dan";

      tx.operations.push_back( feed );
      tx.sign( dan_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      const auto& call_idx = db.get_index< call_order_index >().indices().get< by_account >();
      auto call_itr = call_idx.find( std::make_tuple( account_name_type( "alice" ), SYMBOL_USD ) );

      BOOST_REQUIRE( call_itr == call_idx.end() );
      
      validate_database();

      BOOST_TEST_MESSAGE( "│   ├── Passed: Margin calling existing order" );

      BOOST_TEST_MESSAGE( "├── Testing: CALL ORDER" );
   }
   FC_LOG_AND_RETHROW()
}



BOOST_AUTO_TEST_CASE( option_order_operation_test )
{
   try
   {
      BOOST_TEST_MESSAGE( "├── Testing: OPTION ORDER" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: Creation sequence of option order" );

      ACTORS( (alice)(bob)(candice)(dan)(elon)(fred)(george)(haz) );

      fund_liquid( "alice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_stake( "alice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_liquid( "alice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );

      fund_liquid( "bob", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_stake( "bob", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_liquid( "bob", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );

      fund_liquid( "candice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_stake( "candice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_liquid( "candice", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );

      fund_liquid( "dan", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_stake( "dan", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_liquid( "dan", asset( 10000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );

      fund_liquid( "elon", asset( 100000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_stake( "elon", asset( 100000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      fund_liquid( "elon", asset( 100000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      
      signed_transaction tx;

      option_order_operation option;

      date_type today = date_type( now() + fc::days(60) );

      option.signatory = "alice";
      option.owner = "alice";
      option.order_id = "f0ed4510-b852-43ae-a383-3a9940a1f14b";
      option_strike strike = option_strike( price( asset( BLOCKCHAIN_PRECISION, SYMBOL_USD ), asset( BLOCKCHAIN_PRECISION, SYMBOL_COIN ) ), true, 100, date_type( 1, today.month, today.year ) );
      option.options_issued = asset( 10 * BLOCKCHAIN_PRECISION, strike.option_symbol() );
      option.interface = INIT_ACCOUNT;
      option.validate();

      tx.operations.push_back( option );
      tx.set_expiration( now() + fc::seconds( MAX_TIME_UNTIL_EXPIRATION ) );
      tx.sign( alice_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      const option_order_object& alice_order = db.get_option_order( account_name_type( "alice" ), string( "f0ed4510-b852-43ae-a383-3a9940a1f14b" ) );

      BOOST_REQUIRE( alice_order.owner == option.owner );
      BOOST_REQUIRE( to_string( alice_order.order_id ) == option.order_id );
      BOOST_REQUIRE( alice_order.option_position == option.options_issued );
      BOOST_REQUIRE( alice_order.interface == option.interface );
      BOOST_REQUIRE( alice_order.underlying_amount == asset( 1000 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      BOOST_REQUIRE( alice_order.exercise_amount == asset( 1000 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      BOOST_REQUIRE( alice_order.created == now() );

      validate_database();

      BOOST_TEST_MESSAGE( "│   ├── Passed: Creation of option order" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: Failure when account does not have required underlying funds" );

      option.order_id = "f0ed4510-b852-43ae-a383-3a9940a1f14b";
      option.options_issued = asset( 20000 * BLOCKCHAIN_PRECISION, strike.option_symbol() );
      
      tx.operations.push_back( option );
      tx.sign( alice_private_active_key, db.get_chain_id() );
      REQUIRE_THROW( db.push_transaction( tx, 0 ), fc::exception );

      tx.operations.clear();
      tx.signatures.clear();

      validate_database();

      BOOST_TEST_MESSAGE( "│   ├── Passed: Failure when account does not have required underlying funds" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: Exercising Option assets" );

      asset init_alice_liquid_usd = get_liquid_balance( "alice", SYMBOL_USD );
      asset init_alice_liquid_coin = get_liquid_balance( "alice", SYMBOL_COIN );

      asset init_bob_liquid_usd = get_liquid_balance( "bob", SYMBOL_USD );
      asset init_bob_liquid_coin = get_liquid_balance( "bob", SYMBOL_COIN );

      transfer_operation transfer;

      transfer.signatory = "alice";
      transfer.from = "alice";
      transfer.to = "bob";
      transfer.amount = asset( 5 * BLOCKCHAIN_PRECISION, strike.option_symbol() );
      transfer.memo = "Hello";
      transfer.validate();

      tx.operations.push_back( transfer );
      tx.sign( bob_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      asset_option_exercise_operation exercise;

      exercise.signatory = "bob";
      exercise.account = "bob";
      exercise.amount = asset( 5 * BLOCKCHAIN_PRECISION, strike.option_symbol() );
      exercise.validate();

      tx.operations.push_back( exercise );
      tx.sign( bob_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      asset bob_liquid_usd = get_liquid_balance( "bob", SYMBOL_USD );
      asset bob_liquid_coin = get_liquid_balance( "bob", SYMBOL_COIN );
      asset bob_liquid_option = get_liquid_balance( "bob", strike.option_symbol() );

      asset alice_liquid_usd = get_liquid_balance( "alice", SYMBOL_USD );
      asset alice_liquid_coin = get_liquid_balance( "alice", SYMBOL_COIN );
      asset alice_liquid_option = get_liquid_balance( "alice", strike.option_symbol() );

      BOOST_REQUIRE( bob_liquid_usd == init_bob_liquid_usd - asset( 500 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      BOOST_REQUIRE( bob_liquid_coin == init_bob_liquid_coin + asset( 500 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      BOOST_REQUIRE( bob_liquid_option.amount == 0 );

      BOOST_REQUIRE( alice_liquid_usd == init_alice_liquid_usd + asset( 500 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      BOOST_REQUIRE( alice_liquid_option == asset( 5 * BLOCKCHAIN_PRECISION, strike.option_symbol() ) );

      BOOST_REQUIRE( alice_order.option_position == asset( 5 * BLOCKCHAIN_PRECISION, strike.option_symbol() ) );
      BOOST_REQUIRE( alice_order.underlying_amount == asset( 500 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      BOOST_REQUIRE( alice_order.exercise_amount == asset( 500 * BLOCKCHAIN_PRECISION, SYMBOL_USD ) );
      BOOST_REQUIRE( alice_order.created == now() );
      
      validate_database();

      BOOST_TEST_MESSAGE( "│   ├── Passed: Exercising Option assets" );

      BOOST_TEST_MESSAGE( "│   ├── Testing: Close option position" );

      option.order_id = "f0ed4510-b852-43ae-a383-3a9940a1f14b";
      option.options_issued = asset( 0, strike.option_symbol() );
      
      tx.operations.push_back( option );
      tx.sign( alice_private_active_key, db.get_chain_id() );
      db.push_transaction( tx, 0 );

      tx.operations.clear();
      tx.signatures.clear();

      alice_liquid_coin = get_liquid_balance( "alice", SYMBOL_COIN );
      alice_liquid_option = get_liquid_balance( "alice", strike.option_symbol() );

      const option_order_object* alice_order_ptr = db.find_option_order( account_name_type( "alice" ), string( "f0ed4510-b852-43ae-a383-3a9940a1f14b" ) );

      BOOST_REQUIRE( alice_order_ptr == nullptr );
      BOOST_REQUIRE( alice_liquid_coin == init_alice_liquid_coin + asset( 500 * BLOCKCHAIN_PRECISION, SYMBOL_COIN ) );
      BOOST_REQUIRE( alice_liquid_option.amount == 0 );

      validate_database();

      BOOST_TEST_MESSAGE( "│   ├── Passed: Close option position" );

      BOOST_TEST_MESSAGE( "├── Passed: OPTION ORDER" );
   }
   FC_LOG_AND_RETHROW()
}


BOOST_AUTO_TEST_SUITE_END()