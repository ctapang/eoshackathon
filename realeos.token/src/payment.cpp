#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/asset.hpp>

using namespace eosio;


class [[eosio::contract]] payment : public eosio::contract {

public:
  using contract::contract;
  
  // constructor
  payment(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}

  [[eosio::action]]
  void payineos(name vendor, name customer, asset amount) {
    require_auth(customer);
    emit_payment_event(vendor, customer, amount);
    print("payment in EOS: %n, %n, %n", vendor.value, customer.value, amount.amount);
  }

  // pay in our tokens
  [[eosio::action]]
  void payinrealeos3(name vendor, name customer, asset amount) {
    payinrealeos(vendor, amount);
  }

  // pay in our tokens
  // we assume that the amount communicated by Payment Mobile App already includes
  // transaction fee of 1% and tax
  [[eosio::action]]
  void payinrealeos(name vendor, asset amount) {
    // pay vendor the original amount
    print("payment in tokens: %n, %n", vendor.value, amount.amount);
  }

  [[eosio::action]]
  void settaxrate(float rate) {
    require_auth(get_self());
  }

private:

  void emit_payment_event(name vendor, name customer, asset amount) {
    
    action sellEOS = action(
      permission_level{get_self(),"active"_n},
      "eostrader"_n,
      "sell"_n,
      std::make_tuple(vendor, amount)
    );

    sellEOS.send();
  }

};

EOSIO_DISPATCH( payment, (payineos)(payinrealeos3)(payinrealeos)(settaxrate) )