#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>

using namespace eosio;

class [[eosio::contract]] eostrader : public eosio::contract {
  public:
    using contract::contract;

    eostrader(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}

    [[eosio::action]]
    void sell(name user, asset amount) {
      require_auth( name("payment"));
      payment_index payments(name(_code), _code.value);
      auto iterator = payments.find(user.value);
      
      if (iterator == payments.end()) {
        // no more sell order
      }
      else {
        pick_exch_rate_and_sell(user, amount);
      }
    }

  private:
    struct [[eosio::table]] counter {
      name key;
      uint64_t amount_to_sell;
      float exch_rate;
      uint64_t primary_key() const { return key.value; }
    };

    void pick_exch_rate_and_sell(name user, asset amount) {
      // user Coinbase API to sell EOS / Bitcoin / Ethers
      // once sold, determine exchange rate and pay vendor in realEOS
      float exch_rate = 5.313;
      pay_vendor_in_tokens(user, amount, exch_rate);
    }

    void pay_vendor_in_tokens(name user, asset amount, float exch_rate) {
      
      action payInRealEOS = action(
        permission_level{get_self(),"active"_n},
        "payment"_n,
        "payinrealeos"_n,
        std::make_tuple(user, amount * exch_rate)
      );

      payInRealEOS.send();
    }

    using payment_index = eosio::multi_index<"payments"_n, counter>;
};

EOSIO_DISPATCH( eostrader, (sell));
