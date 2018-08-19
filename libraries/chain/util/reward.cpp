
#include <eznode/chain/util/reward.hpp>
#include <eznode/chain/util/uint256.hpp>

namespace eznode { namespace chain { namespace util {

uint8_t find_msb( const uint128_t& u )
{
   uint64_t x;
   uint8_t places;
   x      = (u.lo ? u.lo : 1);
   places = (u.hi ?   64 : 0);
   x      = (u.hi ? u.hi : x);
   return uint8_t( boost::multiprecision::detail::find_msb(x) + places );
}

uint64_t approx_sqrt( const uint128_t& x )
{
   if( (x.lo == 0) && (x.hi == 0) )
      return 0;

   uint8_t msb_x = find_msb(x);
   uint8_t msb_z = msb_x >> 1;

   uint128_t msb_x_bit = uint128_t(1) << msb_x;
   uint64_t  msb_z_bit = uint64_t (1) << msb_z;

   uint128_t mantissa_mask = msb_x_bit - 1;
   uint128_t mantissa_x = x & mantissa_mask;
   uint64_t mantissa_z_hi = (msb_x & 1) ? msb_z_bit : 0;
   uint64_t mantissa_z_lo = (mantissa_x >> (msb_x - msb_z)).lo;
   uint64_t mantissa_z = (mantissa_z_hi | mantissa_z_lo) >> 1;
   uint64_t result = msb_z_bit | mantissa_z;

   return result;
}

uint64_t get_ESCOR_reward( const comment_reward_context& ctx )
{
   try
   {
   FC_ASSERT( ctx.ESCORreward > 0 );
   FC_ASSERT( ctx.total_reward_ESCOR2 > 0 );

   u256 rf(ctx.total_reward_fund_ECO.amount.value);
   u256 total_claims = to256( ctx.total_reward_ESCOR2 );

   //idump( (ctx) );

   u256 claim = to256( evaluate_reward_curve( ctx.ESCORreward.value, ctx.reward_curve, ctx.content_constant ) );
   claim = ( claim * ctx.reward_weight ) / PERCENT_100;

   u256 payout_u256 = ( rf * claim ) / total_claims;
   FC_ASSERT( payout_u256 <= u256( uint64_t( std::numeric_limits<int64_t>::max() ) ) );
   uint64_t payout = static_cast< uint64_t >( payout_u256 );

   if( is_comment_payout_dust( ctx.current_ECO_price, payout ) )
      payout = 0;

   asset max_ECO = to_ECO( ctx.current_ECO_price, ctx.max_EUSD );

   payout = std::min( payout, uint64_t( max_ECO.amount.value ) );

   return payout;
   } FC_CAPTURE_AND_RETHROW( (ctx) )
}

uint128_t evaluate_reward_curve( const uint128_t& ESCORreward, const curve_id& curve, const uint128_t& content_constant )
{
   uint128_t result = 0;

   switch( curve )
   {
      case quadratic:
         {
            uint128_t ESCORreward_plus_s = ESCORreward + content_constant;
            result = ESCORreward_plus_s * ESCORreward_plus_s - content_constant * content_constant;
         }
         break;
      case quadratic_curation:
         {
            uint128_t two_alpha = content_constant * 2;
            result = uint128_t( ESCORreward.lo, 0 ) / ( two_alpha + ESCORreward );
         }
         break;
      case linear:
         result = ESCORreward;
         break;
      case square_root:
         result = approx_sqrt( ESCORreward );
         break;
   }

   return result;
}

} } } // eznode::chain::util
