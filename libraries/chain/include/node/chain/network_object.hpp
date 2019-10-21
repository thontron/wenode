#pragma once
#include <fc/fixed_string.hpp>

#include <node/protocol/authority.hpp>
#include <node/protocol/node_operations.hpp>

#include <node/chain/node_object_types.hpp>
#include <node/chain/witness_objects.hpp>
#include <node/chain/shared_authority.hpp>

#include <boost/multi_index/composite_key.hpp>

#include <numeric>

namespace node { namespace chain {

   using node::protocol::asset;
   using node::protocol::price;
   using node::protocol::asset_symbol_type;

   struct strcmp_less
   {
      bool operator()( const shared_string& a, const shared_string& b )const
      {
         return less( a.c_str(), b.c_str() );
      }

      bool operator()( const shared_string& a, const string& b )const
      {
         return less( a.c_str(), b.c_str() );
      }

      bool operator()( const string& a, const shared_string& b )const
      {
         return less( a.c_str(), b.c_str() );
      }

      private:
         inline bool less( const char* a, const char* b )const
         {
            return std::strcmp( a, b ) < 0;
         }
   };

   class network_officer_object : public object< network_officer_object_type, network_officer_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         network_officer_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         id_type                        id;

         account_name_type              account;                // The name of the account that owns the network officer.

         bool                           active = true;          // True if the officer is active, set false to deactivate.

         network_officer_types          officer_type;           // The type of network officer that the account serves as. 

         shared_string                  details;                // The officer's details description. 

         shared_string                  url;                    // The officer's reference URL. 

         shared_string                  json;                   // Json metadata of the officer. 
         
         time_point                     created;                // The time the officer was created.

         uint32_t                       vote_count = 0;         // The number of accounts that support the officer.

         share_type                     voting_power = 0;       // The amount of voting power that votes for the officer.
   };


   class network_officer_vote_object : public object< network_officer_vote_object_type, network_officer_vote_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         network_officer_vote_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         id_type                        id;

         account_name_type              account;               // The name of the account voting for the officer.

         account_name_type              network_officer;       // The name of the network officer being voted for.

         uint16_t                       vote_rank;
   };


   class executive_board_object : public object< executive_board_object_type, executive_board_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         executive_board_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         id_type                        id;

         account_name_type              account;                              // The name of the governance account that created the executive team.

         bool                           active = true;                        // True if the executive team is active, set false to deactivate.

         asset                          budget;                               // Total amount of Credit asset requested for team compensation and funding.
         
         shared_string                  details;                              // The executive team's details description. 

         shared_string                  url;                                  // The executive team's reference URL. 

         shared_string                  json;                                 // Json metadata of the executive team. 
         
         time_point                     created;                              // The time the executive team was created.

         uint32_t                       vote_count = 0;                       // The number of accounts that support the executive team.

         share_type                     voting_power = 0;                     // The amount of voting power that votes for the executive team. 
   };


   class executive_board_vote_object : public object< executive_board_vote_object_type, executive_board_vote_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         executive_board_vote_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         id_type                        id;

         account_name_type              account;               // The name of the account that voting for the executive board.

         account_name_type              executive_board;       // The name of the executive board being voted for.

         uint16_t                       vote_rank;
   };


   class governance_account_object : public object< governance_account_object_type, governance_account_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         governance_account_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         id_type                        id;

         account_name_type              account;                // The name of the account that owns the governance account.

         bool                           active = true;          // True if the governance account is active, set false to deactivate.

         shared_string                  details;                // The governance account's details description. 

         shared_string                  url;                    // The governance account's reference URL. 

         shared_string                  json;                   // Json metadata of the governance account. 
         
         time_point                     created;                // The time the governance account was created.

         uint32_t                       subscriber_count = 0;   // The number of accounts that subscribe to the governance account.

         share_type                     subscriber_power = 0;   // The amount of voting power the subscribes to the governance account.
   };


   class governance_subscription_object : public object< governance_subscription_object_type, governance_subscription_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         governance_subscription_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         id_type                        id;

         account_name_type              account;               // The name of the account that subscribes to the governance account.

         account_name_type              governance_account;    // The name of the governance account being subscribed to.
   };


   class supernode_object : public object< supernode_object_type, supernode_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         supernode_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         id_type                        id;

         account_name_type              account;                     // The name of the account that owns the supernode.

         bool                           active = true;               // True if the supernode is active, set false to deactivate.

         shared_string                  details;                     // The supernode's details description. 

         shared_string                  url;                         // The supernode's reference URL. 

         shared_string                  node_api_endpoint;           // The Full Archive node public API endpoint of the supernode.

         shared_string                  notification_api_endpoint;   // The Notification API endpoint of the Supernode. 

         shared_string                  auth_api_endpoint;           // The Transaction signing authentication API endpoint of the supernode.

         shared_string                  ipfs_endpoint;               // The IPFS file storage API endpoint of the supernode.

         shared_string                  bittorrent_endpoint;         // The Bittorrent Seed Box endpoint URL of the Supernode. 

         shared_string                  json;                        // Json metadata of the supernode, including additonal outside of consensus APIs and services. 
         
         time_point                     created;                     // The time the supernode was created.
         
         share_type                     storage_rewards = 0;         // Amount of core asset earned from storage.

         uint64_t                       daily_active_users = 0;      // The average number of accounts (X percent 100) that have used files from the node in the prior 24h.

         uint64_t                       monthly_active_users = 0;    // The average number of accounts (X percent 100) that have used files from the node in the prior 30 days.

         share_type                     recent_view_weight = 0;      // The rolling 7 day average of daily accumulated voting power of viewers. 

         time_point                     last_update_time;            // The time the file weight and active users was last decayed.

         time_point                     last_activation_time;        // The time the Supernode was last reactivated, must be at least 24h ago to claim rewards.
   };


   class interface_object : public object< interface_object_type, interface_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         interface_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         id_type                        id;

         account_name_type              account;                      // The name of the account that owns the interface.

         bool                           active = true;                // True if the interface is active, set false to deactivate.

         shared_string                  details;                      // The interface's details description. 

         shared_string                  url;                          // The interface's reference URL. 

         shared_string                  json;                         // Json metadata of the interface. 
         
         time_point                     created;                      // The time the interface was created.

         uint64_t                       daily_active_users = 0;       // The average number of accounts (X percent 100) that have signed a transaction from the interface in the prior 24h.

         uint64_t                       monthly_active_users = 0;     // The average number of accounts (X percent 100) that have signed a transaction from the interface in the prior 30 days.

         time_point                     last_user_update;             // The time the user counts were last updated.
   };


   class community_enterprise_object : public object< community_enterprise_object_type, community_enterprise_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         community_enterprise_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         id_type                        id;

         account_name_type              creator;                                    // The name of the governance account that created the community enterprise proposal.

         shared_string                  enterprise_id;                              // UUIDv4 for referring to the proposal.

         bool                           active = true;                              // True if the project is active, set false to deactivate.

         proposal_types                 proposal_type;                              // The type of proposal, determines release schedule.

         flat_map< account_name_type, uint16_t > beneficiaries;                     // Map of account names and percentages of budget value.

         vector< pair < shared_string, uint16_t > > milestones;                     // Ordered vector of milestone descriptions and percentages of budget value.

         vector< shared_string >        milestone_history;                          // Ordered vector of the details of every claimed milestone.

         int16_t                        approved_milestones = -1;                   // Number of the last approved milestone by the community.

         int16_t                        claimed_milestones = 0;                     // Number of milestones claimed for release.  

         fc::optional < asset_symbol_type > investment;                             // Symbol of the asset to be purchased with the funding if the proposal is investment type. 

         shared_string                  details;                                    // The proposals's details description. 

         shared_string                  url;                                        // The proposals's reference URL. 

         shared_string                  json;                                       // Json metadata of the proposal. 

         time_point                     begin;                                      // Enterprise proposal start time. If the proposal is not approved by the start time, it is rejected. 

         time_point                     end;                                        // Enterprise proposal end time. Determined by start plus remaining interval number of days.

         time_point                     expiration;                                 // Time that the proposal expires, and transfers all remaining pending budget back to the community fund. 

         asset                          daily_budget;                               // Daily amount of Core asset requested for project compensation and funding.

         uint16_t                       duration;                                   // Number of days that the proposal lasts for. 

         asset                          pending_budget = asset( 0, SYMBOL_COIN );   // Funds held in the proposal for release. 

         asset                          total_distributed = asset( 0, SYMBOL_COIN ); // Total amount of funds distributed for the proposal. 

         uint16_t                       days_paid = 0;                              // Number of days that the proposal has been paid for. 

         uint32_t                       total_approvals = 0;                        // The overall number of accounts that support the enterprise proposal.

         share_type                     total_voting_power = 0;                     // The oveall amount of voting power that supports the enterprise proposal.

         uint32_t                       total_witness_approvals = 0;                // The overall number of top 50 witnesses that support the enterprise proposal.

         share_type                     total_witness_voting_power = 0;             // The overall amount of witness voting power that supports the enterprise proposal.

         uint32_t                       current_approvals = 0;                      // The number of accounts that support the latest claimed milestone.

         share_type                     current_voting_power = 0;                   // The amount of voting power that supports the latest claimed milestone.

         uint32_t                       current_witness_approvals = 0;              // The number of top 50 witnesses that support the latest claimed milestone.

         share_type                     current_witness_voting_power = 0;           // The amount of witness voting power that supports the latest claimed milestone.

         time_point                     created;                                    // The time the proposal was created.

         asset                          total_budget()const                         // Returns the total amount of funding requested by the proposal.
         {
            return duration * daily_budget;
         }

         asset                          remaining_budget()const                     // Returns the total amount of funding remaining for payment.
         {
            return ( duration - days_paid ) * daily_budget;
         }

         bool                           is_beneficiary( const account_name_type& beneficiary )    // Finds if a given account name is in the beneficiarys set. 
         {
            if( beneficiaries[ beneficiary ] > 0 )
            {
               return true;
            }
            else
            {
               return false;
            }
         }

         void adjust_pending_budget(const asset& delta)
         {
            assert(delta.symbol == SYMBOL_COIN);
            pending_budget += delta;
         }
   };

   class enterprise_approval_object : public object< enterprise_approval_object_type, enterprise_approval_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         enterprise_approval_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         id_type                        id;

         account_name_type              account;              // Account approving the enterprise Milestone. 

         account_name_type              creator;              // The name of the account that created the community enterprise proposal.

         shared_string                  enterprise_id;        // UUIDv4 referring to the proposal being claimed.

         int16_t                        milestone;            // Number of the milestone being approved for release.    

         time_point                     created;              // The time the claim request was created.
   };

   struct by_type_voting_power;
   struct by_type_vote_count;

   typedef multi_index_container <
      network_officer_object,
      indexed_by <
         ordered_unique< tag< by_id >,
            member< network_officer_object, network_officer_id_type, &network_officer_object::id > >,
         ordered_unique< tag< by_account >,
            member< network_officer_object, account_name_type, &network_officer_object::account > >,
         ordered_unique< tag< by_type_vote_count >,
            composite_key< network_officer_object,
               member< network_officer_object, network_officer_types, &network_officer_object::officer_type >,
               member< network_officer_object, uint32_t, &network_officer_object::vote_count >,
               member< network_officer_object, network_officer_id_type, &network_officer_object::id >
            >,
            composite_key_compare< std::less< network_officer_types >, std::greater< uint32_t >, std::less< network_officer_id_type > >
         >,
         ordered_unique< tag< by_type_voting_power >,
            composite_key< network_officer_object,
               member< network_officer_object, network_officer_types, &network_officer_object::officer_type >,
               member< network_officer_object, share_type, &network_officer_object::voting_power >,
               member< network_officer_object, network_officer_id_type, &network_officer_object::id >
            >,
            composite_key_compare< std::less< network_officer_types >, std::greater< share_type >, std::less< network_officer_id_type > >
         >
      >,
      allocator< network_officer_object >
   > network_officer_index;

   struct by_account_officer;
   struct by_officer_account;

   typedef multi_index_container<
      network_officer_vote_object,
      indexed_by<
         ordered_unique< tag<by_id>, member< network_officer_vote_object, network_officer_vote_id_type, &network_officer_vote_object::id > >,
         ordered_unique< tag<by_account_officer>,
            composite_key< network_officer_vote_object,
               member<network_officer_vote_object, account_name_type, &network_officer_vote_object::account >,
               member<network_officer_vote_object, uint16_t, &network_officer_vote_object::vote_rank >,
               member<network_officer_vote_object, account_name_type, &network_officer_vote_object::network_officer >
            >
         >,
         ordered_unique< tag<by_officer_account>,
            composite_key< network_officer_vote_object,
               member<network_officer_vote_object, account_name_type, &network_officer_vote_object::network_officer >,
               member<network_officer_vote_object, account_name_type, &network_officer_vote_object::account >,
               member<network_officer_vote_object, uint16_t, &network_officer_vote_object::vote_rank >
            >
         >
      >,
      allocator< network_officer_vote_object >
   > network_officer_vote_index;

   struct by_name;
   struct by_account;
   struct by_subscribers;
   struct by_subscriber_power;
   struct by_voting_power;
   struct by_vote_count;
   struct by_budget;

   typedef multi_index_container <
      executive_board_object,
      indexed_by <
         ordered_unique< tag< by_id >,
            member< executive_board_object, executive_board_id_type, &executive_board_object::id > >,
         ordered_unique< tag< by_account >,
            member< executive_board_object, account_name_type, &executive_board_object::account > >,
         ordered_unique< tag< by_budget >,
            composite_key< executive_board_object,
               member< executive_board_object, asset, &executive_board_object::budget >,
               member< executive_board_object, executive_board_id_type, &executive_board_object::id >
            >,
            composite_key_compare< std::greater< asset >, std::less< executive_board_id_type > >
         >,
         ordered_unique< tag< by_vote_count >,
            composite_key< executive_board_object,
               member< executive_board_object, uint32_t, &executive_board_object::vote_count >,
               member< executive_board_object, executive_board_id_type, &executive_board_object::id >
            >,
            composite_key_compare< std::greater< uint32_t >, std::less< executive_board_id_type > >
         >,
         ordered_unique< tag< by_voting_power >,
            composite_key< executive_board_object,
               member< executive_board_object, share_type, &executive_board_object::voting_power >,
               member< executive_board_object, executive_board_id_type, &executive_board_object::id >
            >,
            composite_key_compare< std::greater< share_type >, std::less< executive_board_id_type > >
         >
      >,
      allocator< executive_board_object >
   > executive_board_index;

   struct by_account_executive;
   struct by_executive_account;

   typedef multi_index_container<
      executive_board_vote_object,
      indexed_by<
         ordered_unique< tag<by_id>, member< executive_board_vote_object, executive_board_vote_id_type, &executive_board_vote_object::id > >,
         ordered_unique< tag<by_account_executive >,
            composite_key< executive_board_vote_object,
               member<executive_board_vote_object, account_name_type, &executive_board_vote_object::account >,
               member<executive_board_vote_object, uint16_t, &executive_board_vote_object::vote_rank >,
               member<executive_board_vote_object, account_name_type, &executive_board_vote_object::executive_board >
            >
         >,
         ordered_unique< tag<by_executive_account >,
            composite_key< executive_board_vote_object,
               member<executive_board_vote_object, account_name_type, &executive_board_vote_object::executive_board >,
               member<executive_board_vote_object, account_name_type, &executive_board_vote_object::account >,
               member<executive_board_vote_object, uint16_t, &executive_board_vote_object::vote_rank >
            >
         >
      >,
      allocator< executive_board_vote_object >
   > executive_board_vote_index;


   typedef multi_index_container <
      governance_account_object,
      indexed_by <
         ordered_unique< tag< by_id >,
            member< governance_account_object, governance_account_id_type, &governance_account_object::id > >,
         ordered_unique< tag< by_account >,
            member< governance_account_object, account_name_type, &governance_account_object::account > >,
         ordered_unique< tag< by_subscribers >,
            composite_key< governance_account_object,
               member< governance_account_object, uint32_t, &governance_account_object::subscriber_count >,
               member< governance_account_object, governance_account_id_type, &governance_account_object::id >
            >,
            composite_key_compare< std::greater< uint32_t >, std::less< governance_account_id_type > >
         >,
         ordered_unique< tag< by_subscriber_power >,
            composite_key< governance_account_object,
               member< governance_account_object, share_type, &governance_account_object::subscriber_power >,
               member< governance_account_object, governance_account_id_type, &governance_account_object::id >
            >,
            composite_key_compare< std::greater< share_type >, std::less< governance_account_id_type > >
         >
      >,
      allocator< governance_account_object >
   > governance_account_index;

   struct by_account_governance;
   struct by_governance_account;

   typedef multi_index_container<
      governance_subscription_object,
      indexed_by<
         ordered_unique< tag<by_id>, member< governance_subscription_object, governance_subscription_id_type, &governance_subscription_object::id > >,
         ordered_unique< tag<by_account_governance>,
            composite_key< governance_subscription_object,
               member<governance_subscription_object, account_name_type, &governance_subscription_object::account >,
               member<governance_subscription_object, account_name_type, &governance_subscription_object::governance_account >
            >
         >,
         ordered_unique< tag<by_governance_account>,
            composite_key< governance_subscription_object,
               member<governance_subscription_object, account_name_type, &governance_subscription_object::governance_account >,
               member<governance_subscription_object, account_name_type, &governance_subscription_object::account >
            >
         >
      >,
      allocator< governance_subscription_object >
   > governance_subscription_index;

   struct by_view_weight;
   struct by_daily_active_users;
   struct by_monthly_active_users;

   typedef multi_index_container <
      supernode_object,
      indexed_by <
         ordered_unique< tag< by_id >,
            member< supernode_object, supernode_id_type, &supernode_object::id > >,
         ordered_unique< tag< by_account >,
            member< supernode_object, account_name_type, &supernode_object::account > >,
         ordered_unique< tag< by_daily_active_users >,
            composite_key< supernode_object,
               member< supernode_object, uint64_t, &supernode_object::daily_active_users >,
               member< supernode_object, supernode_id_type, &supernode_object::id >
            >,
            composite_key_compare< std::greater< uint64_t >, std::less< supernode_id_type > >
         >,
         ordered_unique< tag< by_monthly_active_users >,
            composite_key< supernode_object,
               member< supernode_object, uint64_t, &supernode_object::monthly_active_users >,
               member< supernode_object, supernode_id_type, &supernode_object::id >
            >,
            composite_key_compare< std::greater< uint64_t >, std::less< supernode_id_type > >
         >,
         ordered_unique< tag< by_view_weight >,
            composite_key< supernode_object,
               member< supernode_object, share_type, &supernode_object::recent_view_weight >,
               member< supernode_object, supernode_id_type, &supernode_object::id >
            >,
            composite_key_compare< std::greater< share_type >, std::less< supernode_id_type > >
         >
      >,
      allocator< supernode_object >
   > supernode_index;

   

   typedef multi_index_container <
      interface_object,
      indexed_by <
         ordered_unique< tag< by_id >,
            member< interface_object, interface_id_type, &interface_object::id > >,
         ordered_unique< tag< by_account >,
            member< interface_object, account_name_type, &interface_object::account > >,
         ordered_unique< tag< by_daily_active_users >,
            composite_key< interface_object,
               member< interface_object, uint64_t, &interface_object::daily_active_users >,
               member< interface_object, interface_id_type, &interface_object::id >
            >,
            composite_key_compare< std::greater< uint64_t >, std::less< interface_id_type > >
         >,
         ordered_unique< tag< by_monthly_active_users >,
            composite_key< interface_object,
               member< interface_object, uint64_t, &interface_object::monthly_active_users >,
               member< interface_object, interface_id_type, &interface_object::id >
            >,
            composite_key_compare< std::greater< uint64_t >, std::less< interface_id_type > >
         >
      >,
      allocator< interface_object >
   > interface_index;

   struct by_creator;
   struct by_enterprise_id;
   struct by_daily_budget;
   struct by_pending_budget;
   struct by_total_budget;
   struct by_begin_time;
   struct by_end_time;
   struct by_expiration_time;
   struct by_next_payment;
   struct by_total_voting_power;
   struct by_total_approvals;
   struct by_total_witness_voting_power;
   struct by_total_witness_approvals;

   typedef multi_index_container <
      community_enterprise_object,
      indexed_by <
         ordered_unique< tag< by_id >,
            member< community_enterprise_object, community_enterprise_id_type, &community_enterprise_object::id > >,
         ordered_non_unique< tag< by_begin_time >,
            member< community_enterprise_object, time_point, &community_enterprise_object::begin > >,
         ordered_non_unique< tag< by_end_time >,
            member< community_enterprise_object, time_point, &community_enterprise_object::end > >,
         ordered_non_unique< tag< by_expiration_time >,
            member< community_enterprise_object, time_point, &community_enterprise_object::expiration > >,
         ordered_unique< tag< by_creator >,
            composite_key< community_enterprise_object,
               member< community_enterprise_object, account_name_type, &community_enterprise_object::creator >,
               member< community_enterprise_object, community_enterprise_id_type, &community_enterprise_object::id >
            >,
            composite_key_compare< std::less< account_name_type >, std::less< community_enterprise_id_type > >
         >,
         ordered_unique< tag< by_enterprise_id >,
            composite_key< community_enterprise_object,
               member< community_enterprise_object, account_name_type, &community_enterprise_object::creator >,
               member< community_enterprise_object, shared_string, &community_enterprise_object::enterprise_id >
            >,
            composite_key_compare< std::less< account_name_type >, strcmp_less >
         >,
         ordered_unique< tag< by_daily_budget >,
            composite_key< community_enterprise_object,
               member< community_enterprise_object, asset, &community_enterprise_object::daily_budget >,
               member< community_enterprise_object, community_enterprise_id_type, &community_enterprise_object::id >
            >,
            composite_key_compare< std::greater< asset >, std::less< community_enterprise_id_type > >
         >,
         ordered_unique< tag< by_pending_budget >,
            composite_key< community_enterprise_object,
               member< community_enterprise_object, asset, &community_enterprise_object::pending_budget >,
               member< community_enterprise_object, community_enterprise_id_type, &community_enterprise_object::id >
            >,
            composite_key_compare< std::greater< asset >, std::less< community_enterprise_id_type > >
         >,
         ordered_unique< tag< by_total_voting_power >,
            composite_key< community_enterprise_object,
               member< community_enterprise_object, share_type, &community_enterprise_object::total_voting_power >,
               member< community_enterprise_object, community_enterprise_id_type, &community_enterprise_object::id >
            >,
            composite_key_compare< std::greater< share_type >, std::less< community_enterprise_id_type > >
         >,
         ordered_unique< tag< by_total_witness_voting_power >,
            composite_key< community_enterprise_object,
               member< community_enterprise_object, share_type, &community_enterprise_object::total_witness_voting_power >,
               member< community_enterprise_object, community_enterprise_id_type, &community_enterprise_object::id >
            >,
            composite_key_compare< std::greater< share_type >, std::less< community_enterprise_id_type > >
         >,
         ordered_unique< tag< by_total_budget >,
            composite_key< community_enterprise_object,
               const_mem_fun< community_enterprise_object, asset, &community_enterprise_object::total_budget >,
               member< community_enterprise_object, community_enterprise_id_type, &community_enterprise_object::id >
            >,
            composite_key_compare< std::greater< asset >, std::less< community_enterprise_id_type > >
         >
      >,
      allocator< community_enterprise_object >
   > community_enterprise_index;


   typedef multi_index_container <
      enterprise_approval_object,
      indexed_by <
         ordered_unique< tag< by_id >,
            member< enterprise_approval_object, enterprise_approval_id_type, &enterprise_approval_object::id > >,
         ordered_unique< tag< by_creator >,
            composite_key< enterprise_approval_object,
               member< enterprise_approval_object, account_name_type, &enterprise_approval_object::creator >,
               member< enterprise_approval_object, enterprise_approval_id_type, &enterprise_approval_object::id >
            >,
            composite_key_compare< std::less< account_name_type >, std::less< enterprise_approval_id_type > >
         >,
         ordered_unique< tag< by_enterprise_id >,
            composite_key< enterprise_approval_object,
               member< enterprise_approval_object, account_name_type, &enterprise_approval_object::account >,
               member< enterprise_approval_object, account_name_type, &enterprise_approval_object::creator >,
               member< enterprise_approval_object, shared_string, &enterprise_approval_object::enterprise_id >
            >,
            composite_key_compare< std::less< account_name_type >, strcmp_less, std::less< account_name_type > >
         >
      >,
      allocator< enterprise_approval_object >
   > enterprise_approval_index;

} }    // node::chain

FC_REFLECT( node::chain::network_officer_object,
         (id)
         (account)
         (active)
         (officer_type)
         (details)
         (url)
         (json)
         (created)
         (vote_count)
         (voting_power)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::network_officer_object, node::chain::network_officer_index );

FC_REFLECT( node::chain::network_officer_vote_object,
         (id)
         (account)
         (network_officer)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::network_officer_vote_object, node::chain::network_officer_vote_index );

FC_REFLECT( node::chain::executive_board_object,
         (id)
         (account)
         (active)
         (officers)
         (members)
         (budget)
         (details)
         (url)
         (json)
         (created)
         (vote_count)
         (voting_power)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::executive_board_object, node::chain::executive_board_index );

FC_REFLECT( node::chain::executive_board_vote_object,
         (id)
         (account)
         (executive_board)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::executive_board_vote_object, node::chain::executive_board_vote_index );

FC_REFLECT( node::chain::governance_account_object,
         (id)
         (account)
         (active)
         (details)
         (url)
         (json)
         (created)
         (subscriber_count)
         (subscriber_power)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::governance_account_object, node::chain::governance_account_index );

FC_REFLECT( node::chain::governance_subscription_object,
         (id)
         (account)
         (governance_account)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::governance_subscription_object, node::chain::governance_subscription_index );

FC_REFLECT( node::chain::interface_object,
         (id)
         (account)
         (active)
         (details)
         (url)
         (json)
         (created)
         (daily_active_users)
         (monthly_active_users)
         (last_user_update)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::interface_object, node::chain::interface_index );


FC_REFLECT( node::chain::supernode_object,
         (id)
         (account)
         (active)
         (details)
         (url)
         (node_api_endpoint)
         (auth_api_endpoint)
         (ipfs_endpoint)
         (bittorrent_endpoint)
         (json)
         (created)
         (daily_active_users)
         (monthly_active_users)
         (recent_view_weight)
         (last_view_weight_update)
         (last_activation_time)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::supernode_object, node::chain::supernode_index );

FC_REFLECT( node::chain::community_enterprise_object,
         (id)
         (creator)
         (enterprise_id)
         (active)
         (beneficiaries)
         (milestones)
         (claimed_milestones)
         (approved_milestones)
         (investment)
         (details)
         (url)
         (json)
         (begin)
         (end)
         (daily_budget)
         (pending_budget)
         (next_payment_time)
         (total_approvals)
         (total_voting_power)
         (total_witness_approvals)
         (total_witness_voting_power)
         (current_approvals)
         (current_voting_power)
         (current_witness_approvals)
         (current_witness_voting_power)
         (created)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::community_enterprise_object , node::chain::community_enterprise_index );

FC_REFLECT( node::chain::enterprise_approval_object,
         (id)
         (account)
         (creator)
         (enterprise_id)
         (milestone)
         (created)
         );

CHAINBASE_SET_INDEX_TYPE( node::chain::enterprise_approval_object , node::chain::enterprise_approval_index );

