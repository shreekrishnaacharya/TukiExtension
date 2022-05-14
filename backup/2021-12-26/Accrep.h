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
#include "AccHelp.h"
#include "AccObj.h"

/**
 *  Class definition
 */
class Accrep : public Php::Base
{
private:
    /**
     *  @var Php::Value
     */
    Php::Value die;
    DB db;
    AccHelp accHelp;
    Checkup checkup;

public:
    AccObj accObj;
    std::map<std::string, std::map<std::string, std::string>> plheading;
    std::map<std::string, std::map<std::string, std::string>> tempDataL1;
    std::map<std::string, std::map<std::string, std::string>> tempDataL2;
    std::map<std::string, std::string> tempDataS1;

    /**
     *  Constructor and destructor
     */
    Accrep()
    {
        die = "die";
        if (checkup.check() > 0)
        {
            die("Your application is not registered. Please contact your application provider.");
        }
    }

    virtual ~Accrep()
    {
    }

    void setFrom(Php::Parameters &params)
    {
        accHelp.from_date = params[0];
        accObj.from_date = params[0];
    }

    void setTo(Php::Parameters &params)
    {
        accHelp.to_date = params[0];
        accObj.to_date = params[0];
    }

    void setYear(Php::Parameters &params)
    {
        accHelp.year_id = params[0];
        accObj.year_id = params[0];
    }

    void setLedger(Php::Parameters &params)
    {
        accHelp.ledger = params[0];
    }

    void setStudentStatus(Php::Parameters &params)
    {
        accObj.studentStatus = params[0];
    }

    Php::Value getPlHead()
    {
        return plheading;
    }

    Php::Value trial(Php::Parameters &params)
    {
        std::map<std::string, std::map<std::string, std::string>> traildata = accHelp.getTrial();
        std::map<std::string, std::map<std::string, std::string>> trialReport;
        double amount;
        for (auto &iter : traildata)
        {
            trialReport[iter.second["ledger_id"]]["name"] = iter.second["ledger"];
            trialReport[iter.second["ledger_id"]]["code"] = iter.second["ledger_code"];
            if (iter.second["bs_head"] == "1" || iter.second["bs_head"] == "3")
            {
                amount = std::stod(iter.second["dr"]) - std::stod(iter.second["cr"]);
                trialReport[iter.second["ledger_id"]]["amount"] = std::to_string(amount);
                trialReport[iter.second["ledger_id"]]["dr_cr"] = "0";
            }
            else
            {
                amount = std::stod(iter.second["cr"]) - std::stod(iter.second["dr"]);
                trialReport[iter.second["ledger_id"]]["amount"] = std::to_string(amount);
                trialReport[iter.second["ledger_id"]]["dr_cr"] = "1";
            }
        }
        return trialReport;
    }

    Php::Value plAccount()
    {
        std::map<std::string, std::map<std::string, std::string>> pldata = accHelp.getPL();
        std::map<std::string, std::map<std::string, std::map<std::string, std::string>>> plReport;
        std::map<std::string, double> pltotal;
        double amount;
        plheading.clear();
        for (auto &iter : pldata)
        {
            plReport[iter.second["sub_head_id"]][iter.second["ledger_id"]]["name"] = iter.second["ledger"];
            plReport[iter.second["sub_head_id"]][iter.second["ledger_id"]]["code"] = iter.second["ledger_code"];
            if (iter.second["sub_head_id"] == "6" || iter.second["sub_head_id"] == "7")
            {
                amount = std::stod(iter.second["cr"]) - std::stod(iter.second["dr"]);
                plReport[iter.second["sub_head_id"]][iter.second["ledger_id"]]["amount"] = std::to_string(amount);
                plReport[iter.second["sub_head_id"]][iter.second["ledger_id"]]["dr_cr"] = "1";
            }
            else
            {
                amount = std::stod(iter.second["dr"]) - std::stod(iter.second["cr"]);
                plReport[iter.second["sub_head_id"]][iter.second["ledger_id"]]["amount"] = std::to_string(amount);
                plReport[iter.second["sub_head_id"]][iter.second["ledger_id"]]["dr_cr"] = "0";
            }
            if (plheading.find(iter.second["sub_head_id"]) == plheading.end())
            {
                plheading[iter.second["sub_head_id"]]["name"] = iter.second["group_name"];
                plheading[iter.second["sub_head_id"]]["amount"] = "0";
                pltotal[iter.second["sub_head_id"]] = 0;
            }
            pltotal[iter.second["sub_head_id"]] += amount;
        }
        for (auto &ite : pltotal)
        {
            plheading[ite.first]["amount"] = std::to_string(ite.second);
        }
        return plReport;
    }

    Php::Value balancesheet()
    {
        std::map<std::string, std::map<std::string, std::string>> bsdata = accHelp.getBalance();
        std::map<std::string, std::map<std::string, std::string>> bsReport;
        std::map<std::string, double> amount;
        double plamount = 0;
        for (auto &iter : bsdata)
        {
            if (iter.second["bs_sub_head_type"] == "2")
            {
                plamount = plamount + std::stod(iter.second["cr"]) - std::stod(iter.second["dr"]);
            }
            else
            {
                if (bsReport.find(iter.second["ledger_group_id"]) == bsReport.end())
                {
                    bsReport[iter.second["ledger_group_id"]]["ledger_group_id"] = iter.second["ledger_group_id"];
                    bsReport[iter.second["ledger_group_id"]]["name"] = iter.second["ledger_group_name"];
                    bsReport[iter.second["ledger_group_id"]]["bs_sub_id"] = iter.second["bs_sub_id"];
                    bsReport[iter.second["ledger_group_id"]]["bs_sub_head"] = iter.second["bs_sub_head"];
                    bsReport[iter.second["ledger_group_id"]]["bs_dr_cr"] = iter.second["bs_dr_cr"];
                    amount[iter.second["ledger_group_id"]] = 0;
                }
                if (iter.second["bs_dr_cr"] == "1")
                {
                    amount[iter.second["ledger_group_id"]] = amount[iter.second["ledger_group_id"]] - std::stod(iter.second["cr"]) + std::stod(iter.second["dr"]);
                }
                else
                {
                    amount[iter.second["ledger_group_id"]] = amount[iter.second["ledger_group_id"]] + std::stod(iter.second["cr"]) - std::stod(iter.second["dr"]);
                }
            }
        }
        for (auto &ite : bsReport)
        {
            bsReport[ite.first]["amount"] = std::to_string(amount[ite.first]);
        }
        bsReport["placcount"]["amount"] = std::to_string(plamount);
        return bsReport;
    }

    Php::Value processFeeReport(Php::Parameters &params)
    {
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setAccObj(accObj);
        if (params.size() == 1)
        {
            std::map<std::string, std::string> stdid = params[0];
            return db.processFeeFullReport(stdid, 0);
        }
        else if (params.size() == 2)
        {
            std::map<std::string, std::string> stdid = params[0];
            bool flag = params[1];
            return db.processFeeReport(stdid, flag);
        }
        else if (params[2] == "1")
        {
            std::map<std::string, std::string> stdid = params[0];
            return db.processFeeFullReport(stdid, 1);
        }
        else if (params[2] == "2")
        {
            std::map<std::string, std::string> stdid = params[0];
            std::string feehead = params[3];
            return db.processFeeHead(stdid, feehead);
        }
        else if (params[2] == "3")
        {
            std::map<std::string, std::string> stdid = params[0];
            std::string feehead = params[3];
            Php::Value result = db.processFeeMonthlyReport(stdid, feehead);
            tempDataS1 = db.tempDataS1;
            tempDataL2 = db.tempDataS2;
            return result;
        }
        return "NULL";
    }

    Php::Value getEmployeeLd(Php::Parameters &params)
    {
        accHelp.ledger = params[0];
        std::string emp_ledger = params[1];
        std::map<std::string, std::string> tracklg;
        std::string ledgergpdetil;
        std::map<std::string, std::map<std::string, std::string>> ledgerDetail;
        std::map<std::string, std::map<std::string, std::string>> ledgerPost = accHelp.getEmployeeLedger();
        for (auto &iter : ledgerPost)
        {
            ledgergpdetil = iter.second["post_group"] + "_" + iter.second["post_type"];
            if ((iter.second["ledger"] == emp_ledger) && tracklg.find(ledgergpdetil) == tracklg.end())
            {
                ledgerDetail[iter.second["jno"]] = iter.second;
                tracklg[ledgergpdetil] = ledgergpdetil;
            }
        }
        return ledgerDetail;
    }

    Php::Value getEmployeeVou(Php::Parameters &params)
    {
        std::string parLedger = params[0];
        std::string emp_ledger = params[1];
        std::string accyear = accHelp.year_id;
        return empVoucher(parLedger, emp_ledger, accyear);
    }

    std::map<std::string, double> empVoucher(std::string parLedger, std::string parEmp, std::string parYear)
    {
        accHelp.ledger = parLedger;
        accHelp.year_id = parYear;
        std::string emp_ledger = parEmp;
        tempDataL1.clear();
        tempDataL2.clear();
        std::map<std::string, std::map<std::string, double>> payment;
        std::map<std::string, double> ledgerAdvance;
        std::map<std::string, std::string> tracklg;
        std::map<std::string, std::map<std::string, std::string>> ledgerPost = accHelp.getEmployeeLedger();
        std::map<std::string, std::map<std::string, std::string>> tmpLedger;
        std::string ledgergpdetil;
        double finbalance = 0;
        for (auto &iter : ledgerPost)
        {
            if (iter.second["post_type"] == "3")
            {
                if (iter.second["dr_cr"] == "0")
                {
                    if (ledgerAdvance.find(iter.second["ledger"]) == ledgerAdvance.end())
                    {
                        ledgerAdvance[iter.second["ledger"]] = 0;
                        tempDataL1[iter.second["ledger"]] = iter.second;
                    }
                    ledgerAdvance[iter.second["ledger"]] += std::stod(iter.second["amount"]);
                }
            }
            if (iter.second["post_type"] == "2")
            {
                if (iter.second["dr_cr"] == "1")
                {
                    if (payment.find(iter.second["month"]) == payment.end())
                    {
                        payment[iter.second["month"]][iter.second["ledger"]] = 0;
                    }
                    if (payment[iter.second["month"]].find(iter.second["ledger"]) == payment[iter.second["month"]].end())
                    {
                        payment[iter.second["month"]][iter.second["ledger"]] = 0;
                    }
                    tmpLedger[iter.second["ledger"]]["name"] = iter.second["ledger_name"];
                    tmpLedger[iter.second["ledger"]]["code"] = iter.second["ledger_code"];
                    payment[iter.second["month"]][iter.second["ledger"]] += std::stod(iter.second["amount"]);
                }
            }

            ledgergpdetil = iter.second["post_group"] + "_" + iter.second["post_type"];
            if ((iter.second["ledger"] == emp_ledger) && tracklg.find(ledgergpdetil) == tracklg.end())
            {
                if (iter.second["dr_cr"] == "1")
                {
                    finbalance += std::stod(iter.second["amount"]);
                }
                else
                {
                    finbalance -= std::stod(iter.second["amount"]);
                }
                tracklg[ledgergpdetil] = ledgergpdetil;
            }
        }
        for (auto &iter : ledgerPost)
        {
            if (iter.second["post_type"] != "3")
            {
                if (iter.second["dr_cr"] == "1")
                {
                    if (ledgerAdvance.find(iter.second["ledger"]) == ledgerAdvance.end())
                    {
                        continue;
                    }
                    ledgerAdvance[iter.second["ledger"]] -= std::stod(iter.second["amount"]);
                    if (payment.find(iter.second["month"]) == payment.end())
                    {
                        payment[iter.second["month"]][iter.second["ledger"]] = 0;
                    }
                    if (payment[iter.second["month"]].find(iter.second["ledger"]) == payment[iter.second["month"]].end())
                    {
                        payment[iter.second["month"]][iter.second["ledger"]] = 0;
                    }
                    tmpLedger[iter.second["ledger"]]["name"] = iter.second["ledger_name"];
                    tmpLedger[iter.second["ledger"]]["code"] = iter.second["ledger_code"];
                    payment[iter.second["month"]][iter.second["ledger"]] += std::stod(iter.second["amount"]);
                }
            }
        }
        std::string scounter = "0";
        int icounter = 0;
        for (auto &iter1 : payment)
        {
            for (auto &iter2 : iter1.second)
            {
                tempDataL2[scounter]["ledger_name"] = tmpLedger[iter2.first]["name"];
                tempDataL2[scounter]["ledger_code"] = tmpLedger[iter2.first]["code"];
                tempDataL2[scounter]["month"] = iter1.first;
                tempDataL2[scounter]["amount"] = std::to_string(iter2.second);
                icounter++;
                scounter = std::to_string(icounter);
            }
        }
        tempDataL2["final_balance"]["amount"] = std::to_string(finbalance);
        return ledgerAdvance;
    }

    std::map<std::string, double> empVoucher2(std::string parLedger, std::string parEmp, std::string parYear)
    {
        accHelp.ledger = parLedger;
        accHelp.year_id = parYear;
        std::map<std::string, double> ledgerAdvance;
        std::map<std::string, std::map<std::string, std::string>> ledgerPost = accHelp.getEmployeeLedger();
        for (auto &iter : ledgerPost)
        {
            if (iter.second["post_type"] == "3")
            {
                if (iter.second["dr_cr"] == "0")
                {
                    if (ledgerAdvance.find(iter.second["ledger"]) == ledgerAdvance.end())
                    {
                        ledgerAdvance[iter.second["ledger"]] = 0;
                    }
                    ledgerAdvance[iter.second["ledger"]] += std::stod(iter.second["amount"]);
                }
            }
        }
        for (auto &iter : ledgerPost)
        {
            if (iter.second["post_type"] != "3")
            {
                if (iter.second["dr_cr"] == "1")
                {
                    if (ledgerAdvance.find(iter.second["ledger"]) == ledgerAdvance.end())
                    {
                        continue;
                    }
                    ledgerAdvance[iter.second["ledger"]] -= std::stod(iter.second["amount"]);
                }
            }
        }
        return ledgerAdvance;
    }

    Php::Value bsDetailModel(Php::Parameters &params)
    {
        std::map<std::string, std::map<std::string, std::string>> ledgerPost = accHelp.getBsReport();
        std::map<std::string, std::map<std::string, std::string>> ledgerData;
        std::map<std::string, double> amountdata;
        for (auto &lp : ledgerPost)
        {
            if (ledgerData.find(lp.second["ledger_id"]) == ledgerData.end())
            {
                ledgerData[lp.second["ledger_id"]]["ledger"] = lp.second["ledger"];
                ledgerData[lp.second["ledger_id"]]["amount"] = "0";
                amountdata[lp.second["ledger_id"]] = 0;
            }
            if (lp.second["bs_dr_cr"] == "1")
            {
                if (lp.second["dr_cr"] == "1")
                {
                    amountdata[lp.second["ledger_id"]] -= std::stod(lp.second["amount"]);
                }
                else
                {
                    amountdata[lp.second["ledger_id"]] += std::stod(lp.second["amount"]);
                }
            }
            else
            {
                if (lp.second["dr_cr"] == "1")
                {
                    amountdata[lp.second["ledger_id"]] += std::stod(lp.second["amount"]);
                }
                else
                {
                    amountdata[lp.second["ledger_id"]] -= std::stod(lp.second["amount"]);
                }
            }
        }
        for (auto &lp : ledgerData)
        {
            ledgerData[lp.first]["amount"] = std::to_string(amountdata[lp.first]);
        }
        return ledgerData;
    }

    Php::Value advanceReport(Php::Parameters &params)
    {
        accHelp.tempDataS1 = params[0];
        std::string groupinfo = "";
        std::map<std::string, std::string> tracklg;
        std::map<std::string, std::string> trackAdv;
        std::map<std::string, std::map<std::string, std::map<std::string, std::string>>> ledgerDetail;
        std::map<std::string, std::map<std::string, std::string>> ledgerPost = accHelp.getEmployeeLedgerTable();
        std::map<std::string, std::map<std::string, std::string>> ledgerAdvance = accHelp.getEmployeeAdvanceLedger();
        for (auto &lp : ledgerAdvance)
        {
            groupinfo = lp.second["post_group"] + "_" + lp.second["post_type"];
            if (tracklg.find(groupinfo) == tracklg.end())
            {
                ledgerDetail[lp.second["emp_id"]][lp.second["v_date"] + lp.second["jno"]] = lp.second;
                tracklg[groupinfo] = groupinfo;
                trackAdv[lp.second["ledger"] + "_" + lp.second["emp_id"]] = lp.second["ledger"];
            }
        }
        for (auto &lp : ledgerPost)
        {
            if (lp.second["post_type"] != "3" && trackAdv.find(lp.second["ledger"] + "_" + lp.second["emp_id"]) != trackAdv.end())
            {
                ledgerDetail[lp.second["emp_id"]][lp.second["v_date"] + lp.second["jno"]] = lp.second;
                ledgerDetail[lp.second["emp_id"]][lp.second["v_date"] + lp.second["jno"]]["narration"] = lp.second["ledger_name"];
            }
        }
        return ledgerDetail;
    }

    Php::Value ledgerReport(Php::Parameters &params)
    {
        accHelp.tempDataS1 = params[0];
        std::string groupinfo = "";
        std::map<std::string, std::string> tracklg;
        std::map<std::string, std::map<std::string, std::map<std::string, std::string>>> ledgerDetail;
        std::map<std::string, std::map<std::string, std::string>> ledgerPost = accHelp.getEmployeeLedgerTable();
        for (auto &empi : accHelp.tempDataS1)
        {
            for (auto &lp : ledgerPost)
            {
                if (lp.second["emp_id"] != empi.second || lp.second["ledger"] != empi.first)
                {
                    continue;
                }
                groupinfo = lp.second["post_group"] + "_" + lp.second["post_type"];
                if (tracklg.find(groupinfo) == tracklg.end())
                {
                    ledgerDetail[empi.second][lp.second["jno"]] = lp.second;
                    tracklg[groupinfo] = groupinfo;
                }
            }
        }
        return ledgerDetail;
    }

    Php::Value salarySheetEmp(Php::Parameters &params)
    {
        accHelp.tempDataS1 = params[0];
        accHelp.tempDataS2 = params[1];
        std::string counter = "0";
        std::map<std::string, std::map<std::string, std::map<std::string, std::map<std::string, std::string>>>> ledgerDetail;
        std::map<std::string, std::map<std::string, std::string>> ledgerPost = accHelp.getEmployeeLedgerTable();
        int advcount, crcount, drcount;
        advcount = crcount = drcount = 0;
        for (auto &empi : accHelp.tempDataS1)
        {
            ledgerDetail[empi.second]["dr"] = ledgerDetail[empi.second]["cr"] = ledgerDetail[empi.second]["adv"] = {};
            for (auto &lp : ledgerPost)
            {
                //                if (lp.second["emp_id"] != empi.second || lp.second["ledger"] == empi.first || lp.second["v_type"] == "6" || lp.second["v_type"] == "7") {
                if (lp.second["emp_id"] != empi.second || lp.second["ledger"] == empi.first)
                {
                    continue;
                }
                if (lp.second["post_type"] == "2" || lp.second["post_type"] == "3")
                {
                    counter = std::to_string(advcount);
                    ledgerDetail[empi.second]["adv"][counter] = lp.second;
                    advcount++;
                    continue;
                }
                if (lp.second["dr_cr"] == "1")
                {
                    counter = std::to_string(crcount);
                    ledgerDetail[empi.second]["cr"][counter] = lp.second;
                    crcount++;
                }
                else
                {
                    counter = std::to_string(drcount);
                    ledgerDetail[empi.second]["dr"][counter] = lp.second;
                    drcount++;
                }
            }
        }
        return ledgerDetail;
    }

    Php::Value advanceBalance(Php::Parameters &params)
    {
        std::map<std::string, std::string> stdid = params[0];
        std::map<std::string, std::map<std::string, std::string>> advanceData = accHelp.getStdAdvance(stdid);
        std::map<std::string, double> adAmount;
        for (auto &adv : advanceData)
        {
            if (adAmount.find(adv.second["fk_student_id"]) == adAmount.end())
            {
                adAmount[adv.second["fk_student_id"]] = 0;
            }
            if (adv.second["dr_cr"] == "1")
            {
                adAmount[adv.second["fk_student_id"]] += std::stod(adv.second["amount"]);
            }
            else
            {
                adAmount[adv.second["fk_student_id"]] -= std::stod(adv.second["amount"]);
            }
        }
        std::map<std::string, std::string> adtdAmount;
        for (auto &adm : adAmount)
        {
            adtdAmount[adm.first] = std::to_string(adm.second);
        }
        return adtdAmount;
    }

    Php::Value getPayrollPostData(Php::Parameters &params)
    {
        std::map<std::string, std::string> months = params[0];
        std::map<std::string, std::string> types = params[1];
        accHelp.tempDataS1 = months;
        accHelp.tempDataS2 = types;
        double amount1, amount2, balance;
        std::map<std::string, std::map<std::string, std::string>> payrollData = accHelp.getPayrollPostData();
        std::map<std::string, std::map<std::string, std::string>> resultData;
        for (auto &pd1 : payrollData)
        {
            if (resultData.find(pd1.second["ledger_id"]) != resultData.end())
            {
                amount1 = std::stod(resultData[pd1.second["ledger_id"]]["amount"]);
                amount2 = std::stod(pd1.second["amount"]);
                if (amount1 < amount2)
                {
                    resultData[pd1.second["ledger_id"]] = pd1.second;
                    balance = amount2 - amount1;
                    resultData[pd1.second["ledger_id"]]["amount"] = std::to_string(balance);
                }
                else
                {
                    balance = amount1 - amount2;
                    resultData[pd1.second["ledger_id"]]["amount"] = std::to_string(balance);
                }
            }
            else
            {
                resultData[pd1.second["ledger_id"]] = pd1.second;
            }
        }
        return resultData;
    }

    Php::Value getTempDataL1()
    {
        return tempDataL1;
    }

    Php::Value getTempDataL2()
    {
        return tempDataL2;
    }

    Php::Value getTempDataS1()
    {
        return tempDataS1;
    }

    Php::Value getFeemonth()
    {
        return db.feemonth;
    }

    Php::Value getScholarHead()
    {
        return db.scholarhead;
    }
};