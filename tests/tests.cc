#ifndef CATCH_CONFIG_MAIN
#  define CATCH_CONFIG_MAIN
#endif


#include "atm.hpp"
#include "catch.hpp"


/////////////////////////////////////////////////////////////////////////////////////////////
//                             Helper Definitions //
/////////////////////////////////////////////////////////////////////////////////////////////


bool CompareFiles(const std::string& p1, const std::string& p2) {
 std::ifstream f1(p1);
 std::ifstream f2(p2);


 if (f1.fail() || f2.fail()) {
   return false;  // file problem
 }


 std::string f1_read;
 std::string f2_read;
 while (f1.good() || f2.good()) {
   f1 >> f1_read;
   f2 >> f2_read;
   if (f1_read != f2_read || (f1.good() && !f2.good()) ||
       (!f1.good() && f2.good()))
     return false;
 }
 return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Test Cases
/////////////////////////////////////////////////////////////////////////////////////////////


TEST_CASE("Example: Create a new account", "[ex-1]") {
 Atm atm;
 atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
 auto accounts = atm.GetAccounts();
 REQUIRE(accounts.contains({12345678, 1234}));
 REQUIRE(accounts.size() == 1);


 Account sam_account = accounts[{12345678, 1234}];
 REQUIRE(sam_account.owner_name == "Sam Sepiol");
 REQUIRE(sam_account.balance == 300.30);


 auto transactions = atm.GetTransactions();
 REQUIRE(accounts.contains({12345678, 1234}));
 REQUIRE(accounts.size() == 1);
 std::vector<std::string> empty;
 REQUIRE(transactions[{12345678, 1234}] == empty);
}


TEST_CASE("Example: Simple widthdraw", "[ex-2]") {
 Atm atm;
 atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
 atm.WithdrawCash(12345678, 1234, 20);
 auto accounts = atm.GetAccounts();
 Account sam_account = accounts[{12345678, 1234}];


 REQUIRE(sam_account.balance == 280.30);
}


TEST_CASE("Example: Print Prompt Ledger", "[ex-3]") {
 Atm atm;
 atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
 auto& transactions = atm.GetTransactions();
 transactions[{12345678, 1234}].push_back(
     "Withdrawal - Amount: $200.40, Updated Balance: $99.90");
 transactions[{12345678, 1234}].push_back(
     "Deposit - Amount: $40000.00, Updated Balance: $40099.90");
 transactions[{12345678, 1234}].push_back(
     "Deposit - Amount: $32000.00, Updated Balance: $72099.90");
 atm.PrintLedger("./prompt.txt", 12345678, 1234);
 REQUIRE(CompareFiles("./ex-1.txt", "./prompt.txt"));
}


TEST_CASE("RegisterAccount: duplicate account should throw", "[register]") {
 Atm atm;
 atm.RegisterAccount(1111, 2222, "Alice", 100.0);
 REQUIRE_THROWS_AS(atm.RegisterAccount(1111, 2222, "Alice Again", 200.0), std::invalid_argument);
}


TEST_CASE("RegisterAccount: edge cases", "[register]") {
 Atm atm;
 REQUIRE_NOTHROW(atm.RegisterAccount(3333, 4444, "Bob", 0.0));
 REQUIRE_NOTHROW(atm.RegisterAccount(99999999, 9999, "Charlie", 50.0));
}


TEST_CASE("WithdrawCash: negative withdrawal should throw", "[withdraw]") {
 Atm atm;
 atm.RegisterAccount(1234, 5678, "Eve", 100.0);
 REQUIRE_THROWS_AS(atm.WithdrawCash(1234, 5678, -10.0), std::invalid_argument);
}


TEST_CASE("WithdrawCash: overdraft should throw runtime_error", "[withdraw]") {
 Atm atm;
 atm.RegisterAccount(2345, 6789, "Mallory", 50.0);
 REQUIRE_THROWS_AS(atm.WithdrawCash(2345, 6789, 100.0), std::runtime_error);
}


TEST_CASE("WithdrawCash: invalid account should throw", "[withdraw]") {
 Atm atm;
 REQUIRE_THROWS_AS(atm.WithdrawCash(9999, 8888, 10.0), std::invalid_argument);
}


TEST_CASE("WithdrawCash: exact balance withdrawal leaves zero", "[withdraw]") {
 Atm atm;
 atm.RegisterAccount(3456, 7890, "Oscar", 25.0);
 atm.WithdrawCash(3456, 7890, 25.0);
 REQUIRE(atm.CheckBalance(3456, 7890) == Approx(0.0));
}


TEST_CASE("DepositCash: valid deposit increases balance", "[deposit]") {
 Atm atm;
 atm.RegisterAccount(4567, 8901, "Peggy", 10.0);
 atm.DepositCash(4567, 8901, 40.0);
 REQUIRE(atm.CheckBalance(4567, 8901) == Approx(50.0));
}


TEST_CASE("DepositCash: negative deposit should throw", "[deposit]") {
 Atm atm;
 atm.RegisterAccount(5678, 9012, "Trent", 10.0);
 REQUIRE_THROWS_AS(atm.DepositCash(5678, 9012, -100.0), std::invalid_argument);
}


TEST_CASE("DepositCash: deposit to nonexistent account should throw", "[deposit]") {
 Atm atm;
 REQUIRE_THROWS_AS(atm.DepositCash(9999, 9999, 100.0), std::invalid_argument);
}


TEST_CASE("CheckBalance: valid and invalid queries", "[check]") {
 Atm atm;
 atm.RegisterAccount(6789, 1234, "Victor", 75.5);
 REQUIRE(atm.CheckBalance(6789, 1234) == Approx(75.5));
 REQUIRE_THROWS_AS(atm.CheckBalance(6789, 9999), std::invalid_argument);
}


TEST_CASE("PrintLedger: invalid account should throw", "[ledger]") {
 Atm atm;
 REQUIRE_THROWS_AS(atm.PrintLedger("fake.txt", 1111, 2222), std::invalid_argument);
}


TEST_CASE("PrintLedger: no transactions should still print header", "[ledger]") {
 Atm atm;
 atm.RegisterAccount(7890, 1357, "Walter", 200.0);


 std::string outfile = "./ledger_test.txt";
 atm.PrintLedger(outfile, 7890, 1357);


 std::ifstream in(outfile);
 REQUIRE(in.good());


 std::string content;
 std::getline(in, content);
 REQUIRE(content.find("Walter") != std::string::npos);
}



