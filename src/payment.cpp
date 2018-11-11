#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/asset.hpp>

using namespace eosio;


class [[eosio::contract]] payment : public eosio::contract {

public:
  using contract::contract;

  float _divider;
  float _feeRate;
  float _taxRate;
  name _ourAccount;
  name _govtAccount;
  
  // constructor
  payment(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {
    this->_divider = 1.0 + this->_feeRate + this->_taxRate;

  }

  // pay in EOS
  // the customer already has EOS, and wants to use her EOS to pay
  [[eosio::action]]
  void payineos(name vendor, name customer, asset amount) {
    require_auth(customer);
    emit_payment_event(vendor, customer, amount);
    print("payment in EOS: %n, %n, %n", vendor.value, customer.value, amount.amount);
  }

  // pay in our tokens (3 params)
  // RealEOS is our own token - a stabletoken that exchanges for a US dollar
  [[eosio::action]]
  void payinrealeos3(name vendor, name customer, asset token) {
    asset vpay = token;
    vpay.amount = token.amount / this->_divider;
    asset opay = vpay;
    opay.amount = vpay.amount * this->_feeRate;
    asset gpay = vpay;
    gpay.amount = vpay.amount * this->_taxRate;
    transfer_tokens(customer, vendor, vpay);
    transfer_tokens(customer, this->_ourAccount, opay);
    transfer_tokens(customer, this->_govtAccount, gpay);
  }

  // pay in our tokens
  // we assume that the amount communicated by Payment Mobile App already includes
  // transaction fee of 1% and tax
  [[eosio::action]]
  void payinrealeos(name vendor, asset token) {
    require_auth( name("payment"));
    // pay vendor the original amount
    print("payment in tokens: %n, %n", vendor.value, token.amount);
  }

  [[eosio::action]]
  void settaxrate(float rate) {
    this->_taxRate = rate;
  }

  [[eosio::action]]
  void setxfeerate(float feerate) {
    this->_feeRate = feerate;
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

  void transfer_tokens(name from, name to, asset amount) {

    action transfer = action(
      permission_level{get_self(),"active"_n},
      "token"_n,
      "transfer"_n,
      std::make_tuple(from, to, amount, "")
    );

    transfer.send();

  }

};

EOSIO_DISPATCH( payment, (payineos)(payinrealeos3)(payinrealeos)(settaxrate) )