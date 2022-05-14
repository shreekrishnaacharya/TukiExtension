/**
 *  ExamRep.h
 * 
 *  Class that is exported to PHP space
 * 
 *  @author Krishna Acharya 
 *  @copyright 2018
 */

/**
 *  Include guard
 */
#pragma once

/**
 *  Class definition
 */
class Accrep : public Php::Base {
private:
    /**
     *  @var Php::Value
     */
    Php::Value die;
public:
    std::map<std::string, std::map<std::string, std::string>> trackSubject;
    std::map<std::string, std::map<std::string, std::string>> trackStudent;

    /**
     *  Constructor and destructor
     */
    Accrep() {
        die = "die";
        Checkup checkup;
        if (checkup.check() > 0) {
            die("Your application is not registered. Please contact your application provider.");
        }
    }

    virtual ~Accrep() {
    }

    Php::Value trial(Php::Parameters &params) {
        std::map<std::string, std::map < std::string, std::string>>traildata = params[0];
        std::map<std::string, std::map < std::string, std::string>>trialReport;
        std::map < std::string, double> amount;
        for (auto &iter : traildata) {
            if (trialReport.find(iter.second["ledger_id"]) == trialReport.end()) {
                trialReport[iter.second["ledger_id"]]["name"] = iter.second["ledger"];
                trialReport[iter.second["ledger_id"]]["code"] = iter.second["ledger_code"];
                amount[iter.second["ledger_id"]] = 0;
            }
            if (iter.second["dr_cr"] == "1") {
                amount[iter.second["ledger_id"]] += std::stod(iter.second["amount"]);
            } else {
                amount[iter.second["ledger_id"]] -= std::stod(iter.second["amount"]);
            }
        }
        for (auto &iter : traildata) {
            trialReport[iter.second["ledger_id"]]["amount"] = std::to_string(amount[iter.second["ledger_id"]]);
        }
        return trialReport;
    }

    Php::Value balancesheet(Php::Parameters &params) {
        std::map<std::string, std::map < std::string, std::string>>bsdata = params[0];
        std::map<std::string, std::map < std::string, std::string>>bsReport;
        std::map < std::string, double> amount;
        double plamount = 0;
        for (auto &iter : bsdata) {
            if (iter.second["bs_sub_head_type"] == "2") {
                if (iter.second["dr_cr"] == "1") {
                    plamount += std::stod(iter.second["amount"]);
                } else {
                    plamount -= std::stod(iter.second["amount"]);
                }
            } else {
                if (bsReport.find(iter.second["ledger_group_id"]) == bsReport.end()) {
                    bsReport[iter.second["ledger_group_id"]]["ledger_group_id"] = iter.second["ledger_group_id"];
                    bsReport[iter.second["ledger_group_id"]]["name"] = iter.second["ledger_group_name"];
                    bsReport[iter.second["ledger_group_id"]]["bs_sub_id"] = iter.second["bs_sub_id"];
                    bsReport[iter.second["ledger_group_id"]]["bs_sub_head"] = iter.second["bs_sub_head"];
                    bsReport[iter.second["ledger_group_id"]]["bs_dr_cr"] = iter.second["bs_dr_cr"];
                    amount[iter.second["ledger_group_id"]] = 0;
                }
                if (iter.second["bs_dr_cr"] == "1") {
                    if (iter.second["dr_cr"] == "1") {
                        amount[iter.second["ledger_group_id"]] -= std::stod(iter.second["amount"]);
                    } else {
                        amount[iter.second["ledger_group_id"]] += std::stod(iter.second["amount"]);
                    }
                } else {
                    if (iter.second["dr_cr"] == "1") {
                        amount[iter.second["ledger_group_id"]] += std::stod(iter.second["amount"]);
                    } else {
                        amount[iter.second["ledger_group_id"]] -= std::stod(iter.second["amount"]);
                    }
                }
            }
        }
        for (auto &ite : bsReport) {
            bsReport[ite.first]["amount"] = std::to_string(amount[ite.first]);
        }
        bsReport["placcount"]["amount"] = std::to_string(plamount);
        return bsReport;
    }


};