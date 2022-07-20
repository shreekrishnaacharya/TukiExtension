/**
 *  AccObj.h
 *
 *  Class that is exported to PHP space
 *
 *  @author Krishna Acharya
 *  @copyright 2020
 */

/**
 *  Include guard
 */
#pragma once
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include "Accrep.h"

/**
 *  Class definition
 */
class AccountAudit : public Php::Base
{
private:
    /**
     *  @var Php::Value
     */
    Php::Value die;
    Checkup checkup;
    std::string db_name, host_id, db_user, db_password;
    std::string message;
    std::map<int, std::map<std::string, double>> payrollPostDetail;
    std::map<int, std::map<std::string, double>> payrollAdvanceDetail;
    std::map<std::string, std::string> advLedgerDetail;
    int voucherid = 0;
    std::map<int, std::map<std::string, std::string>> staffDetail;
    std::map<std::string, int> oldYear, newYear;

    sql::Statement *stmtDB;
    sql::Driver *driverDB;
    sql::Connection *conDB;
    sql::ResultSet *resDB;

public:
    /**
     *  Constructor and destructor
     */
    AccountAudit()
    {
        die = "die";
        if (checkup.check() > 0)
        {
            die("Your application is not registered. Please contact your application provider.");
        }
        std::map<std::string, std::string> result = checkup.getdecode();
        Pobj pobj;
        db_name = pobj.getDb("tukidatabase");
        host_id = result["host"];
        db_user = result["user"];
        db_password = result["password"];

        // db connection statement
        driverDB = get_driver_instance();
        conDB = driverDB->connect("tcp://" + host_id, db_user, db_password);
        /* Connect to the MySQL test database */
        conDB->setSchema(db_name);
        message = "";
    }

    virtual ~AccountAudit()
    {
        stmtDB->close();
        delete stmtDB;
        conDB->close();
        delete conDB;
        driverDB->threadEnd();
    }

    Php::Value closeAccount()
    {

        std::string monthlyFee = "";
        message += "Initiating account close process ...\n";
        string query = "SELECT * FROM acc_year WHERE is_closed=0 LIMIT 1";
        stmtDB = conDB->createStatement();
        resDB = stmtDB->executeQuery(query);
        while (resDB->next())
        {
            oldYear["id"] = resDB->getInt("id");
            oldYear["fk_batch_id"] = resDB->getInt("fk_batch_id");
            oldYear["start_from"] = resDB->getInt("start_from");
            oldYear["end_at"] = resDB->getInt("end_at");
        }
        if (oldYear.size() == 0)
        {
            message += "\nProcess failed!!!\nERROR CODE: 10001\n";
            return message;
        }
        conDB->setAutoCommit(false);
        delete resDB;
        resDB = stmtDB->executeQuery("SELECT MAX(id) as id FROM acc_voucher");
        resDB->next();
        voucherid = resDB->getInt("id");
        int key = 0, vKey = 0, serialNo = 0, ledgerid = 0;
        double balanceAmount = 0;
        std::string DR = "1", CR = "0", narrationDr = "", narrationCr = "";
        std::map<int, std::map<std::string, std::string>> voucherArr;
        std::map<int, std::map<std::string, std::string>> voucherDArr;
        std::map<int, std::map<std::string, std::string>> studentArr;

        delete resDB;
        resDB = stmtDB->executeQuery("SELECT count(id) as id FROM acc_fee_head WHERE is_mf=1");
        resDB->next();
        if (resDB->getInt("id") != 1)
        {
            message += "\nProcess failed!!!\nERROR CODE: 10010\n";
            return message;
        }
        delete resDB;
        resDB = stmtDB->executeQuery("SELECT fk_ledger_id FROM acc_fee_head WHERE is_mf=1 LIMIT 1");
        resDB->next();
        monthlyFee = resDB->getString("fk_ledger_id");

        if (payrollClose() == false)
        {
            conDB->rollback();
            return message;
        }
        query = "SELECT fk_student_id,fk_program_group as pg_id,acc_voucher_detail.dr_cr,";
        query += "acc_voucher_detail.amount,";
        query += "CONCAT(first_name,' ',middle_name,' ',last_name)as student_name ";
        query += "FROM acc_voucher_to_student ";
        query += "LEFT JOIN acc_voucher ON acc_voucher.id=acc_voucher_to_student.fk_voucher_id ";
        query += "LEFT JOIN acc_voucher_detail ON acc_voucher.id=acc_voucher_detail.fk_voucher_id ";
        query += "LEFT JOIN student ON student.id=acc_voucher_to_student.fk_student_id ";
        query += "LEFT JOIN acc_ledger ON acc_ledger.id=acc_voucher_detail.fk_ledger_id ";
        query += "WHERE acc_voucher.fk_fiscal_year=" + std::to_string(oldYear["id"]);
        query += " AND acc_voucher.status=1 ";
        query += "AND acc_voucher_detail.fk_ledger_id!=1 AND (acc_voucher_detail.fk_ledger_id!=15 OR acc_voucher_detail.dr_cr!=1)";

        //        delete stmtDB;
        delete resDB;
        resDB = stmtDB->executeQuery(query);
        std::map<int, std::map<std::string, std::string>> studentData;
        std::map<int, std::map<std::string, double>> studentDc;
        // std::map<std::string, std::map<std::string, std::string>> studentdcc;
        while (resDB->next())
        {
            if (studentData.find(resDB->getInt("fk_student_id")) == studentData.end())
            {
                studentData[resDB->getInt("fk_student_id")]["pg_id"] = resDB->getString("pg_id");
                studentData[resDB->getInt("fk_student_id")]["name"] = resDB->getString("student_name");
                studentDc[resDB->getInt("fk_student_id")]["dr"] = 0;
                studentDc[resDB->getInt("fk_student_id")]["cr"] = 0;
            }
            if (resDB->getInt("dr_cr") == 1)
            {
                studentDc[resDB->getInt("fk_student_id")]["dr"] += resDB->getDouble("amount");
            }
            else
            {
                studentDc[resDB->getInt("fk_student_id")]["cr"] += resDB->getDouble("amount");
            }
            // studentdcc[resDB->getString("fk_student_id")]["dr"] = std::to_string(studentDc[resDB->getInt("fk_student_id")]["dr"]);
            // studentdcc[resDB->getString("fk_student_id")]["cr"] = std::to_string(studentDc[resDB->getInt("fk_student_id")]["cr"]);
        }
        // return studentDc;
        // return studentdcc;
        if (studentData.size() == 0)
        {
            message += "\nProcess failed!!!\nERROR CODE: 10002\n";
            return message;
        }

        query = "SELECT ";
        query += "fk_ledger_id,";
        query += "acc_bs_sub_head.type as bs_sub_head_type,";
        query += "acc_voucher_detail.dr_cr,";
        query += "acc_voucher_detail.amount ";
        query += "FROM acc_voucher ";
        query += "LEFT JOIN acc_voucher_detail ON acc_voucher.id=acc_voucher_detail.fk_voucher_id ";
        query += "LEFT JOIN acc_ledger ON acc_ledger.id=acc_voucher_detail.fk_ledger_id ";
        query += "LEFT JOIN acc_ledger_group ON acc_ledger_group.id=acc_ledger.fk_ledger_group ";
        query += "LEFT JOIN acc_bs_sub_head ON acc_bs_sub_head.id=acc_ledger_group.fk_bs_sub_head ";
        query += "WHERE acc_voucher.fk_fiscal_year=" + std::to_string(oldYear["id"]);
        query += " AND acc_voucher.status=1";
        //        delete stmtDB;
        delete resDB;
        resDB = stmtDB->executeQuery(query);
        double plAccount = 0;

        std::map<int, std::map<std::string, double>> ledgerDc;
        while (resDB->next())
        {
            if (resDB->getInt("bs_sub_head_type") == 2)
            {
                if (resDB->getInt("dr_cr") == 1)
                {
                    plAccount += resDB->getDouble("amount");
                }
                else
                {
                    plAccount -= resDB->getDouble("amount");
                }
                continue;
            }
            ledgerid = resDB->getInt("fk_ledger_id");
            if (resDB->getInt("fk_ledger_id") == 1)
            {
                ledgerid = 7;
            }
            if (ledgerDc.find(ledgerid) == ledgerDc.end())
            {
                ledgerDc[ledgerid]["dr"] = 0;
                ledgerDc[ledgerid]["cr"] = 0;
            }
            if (resDB->getInt("dr_cr") == 1)
            {
                ledgerDc[ledgerid]["dr"] += resDB->getDouble("amount");
            }
            else
            {
                ledgerDc[ledgerid]["cr"] += resDB->getDouble("amount");
            }
        }
        if (ledgerDc.find(8) == ledgerDc.end())
        {
            ledgerDc[8]["dr"] = 0;
            ledgerDc[8]["cr"] = 0;
        }
        if (plAccount > 0)
        {
            ledgerDc[8]["dr"] += plAccount;
        }
        else
        {
            ledgerDc[8]["cr"] += (plAccount * (-1));
        }

        newYear["fk_batch_id"] = oldYear["fk_batch_id"];
        newYear["start_from"] = (oldYear["end_at"] + 86400);
        newYear["end_at"] = (newYear["start_from"] + 31536000);
        query = "INSERT INTO acc_year(year_name,fk_batch_id,start_from,end_at)";
        query += "VALUES('NEW'," + std::to_string(oldYear["fk_batch_id"]) + "," + std::to_string(newYear["start_from"]) + "," + std::to_string(newYear["end_at"]) + ")";
        if (stmtDB->executeUpdate(query) == 0)
        {
            conDB->rollback();
            message += "\nProcess failed!!!\nERROR CODE: 10003\n";
            return message;
        }
        delete resDB;
        resDB = stmtDB->executeQuery("SELECT LAST_INSERT_ID() AS id;");
        resDB->next();
        newYear["id"] = resDB->getInt("id");
        query = "UPDATE acc_year SET is_closed=2 WHERE id=" + std::to_string(oldYear["id"]);
        if (stmtDB->executeUpdate(query) == 0)
        {
            conDB->rollback();
            message += "\nProcess failed!!!\nERROR CODE: 10004\n";
            return message;
        }

        serialNo = key = vKey = 0;
        balanceAmount = 0;
        DR = "1";
        CR = "0";
        narrationDr = "";
        narrationCr = "";
        voucherArr.clear();
        voucherDArr.clear();
        studentArr.clear();

        for (auto &iter : studentDc)
        {
            balanceAmount = 0;
            DR = "1";
            CR = "0";
            if (iter.second["cr"] == iter.second["dr"])
            {
                continue;
            }
            if (iter.second["cr"] > iter.second["dr"])
            {
                
                DR = "15";
                CR = "1";
                balanceAmount = iter.second["cr"] - iter.second["dr"];
                narrationDr = "Closing Student";
                narrationCr = "Fee - " + studentData[iter.first]["name"];
            }
            else
            {
                DR = "7";
                CR = "1";
                balanceAmount = iter.second["dr"] - iter.second["cr"];
                narrationDr = "Fee - " + studentData[iter.first]["name"];
                narrationCr = "Closing Student";
            }
            voucherid++;
            voucherArr[key]["id"] = std::to_string(voucherid);
            voucherArr[key]["v_date"] = std::to_string(oldYear["end_at"]);
            voucherArr[key]["fk_fiscal_year"] = std::to_string(oldYear["id"]);
            voucherArr[key]["fk_posted_by"] = "1";
            voucherArr[key]["narration"] = "Being closing balance recorded.";
            voucherArr[key]["create_date"] = checkup.toString(checkup.getTime());
            voucherArr[key]["type"] = "7";

            voucherDArr[vKey]["fk_voucher_id"] = std::to_string(voucherid);
            voucherDArr[vKey]["fk_ledger_id"] = DR;
            voucherDArr[vKey]["serial_no"] = "1";
            voucherDArr[vKey]["dr_cr"] = "0";
            voucherDArr[vKey]["amount"] = std::to_string(balanceAmount);
            voucherDArr[vKey]["narration"] = narrationDr;
            vKey++;
            voucherDArr[vKey]["fk_voucher_id"] = std::to_string(voucherid);
            voucherDArr[vKey]["fk_ledger_id"] = CR;
            voucherDArr[vKey]["serial_no"] = "2";
            voucherDArr[vKey]["dr_cr"] = "1";
            voucherDArr[vKey]["amount"] = std::to_string(balanceAmount);
            voucherDArr[vKey]["narration"] = narrationCr;
            vKey++;
            studentArr[key]["fk_voucher_id"] = std::to_string(voucherid);
            studentArr[key]["fk_student_id"] = std::to_string(iter.first);
            studentArr[key]["fk_program_group"] = studentData[iter.first]["pg_id"];
            key++;
        }
        for (auto &iter : studentDc)
        {
            balanceAmount = 0;
            DR = "1";
            CR = "0";
            if (iter.second["cr"] == iter.second["dr"])
            {
                continue;
            }
            if (iter.second["cr"] > iter.second["dr"])
            {
                DR = "1";
                CR = "15";
                balanceAmount = iter.second["cr"] - iter.second["dr"];
                narrationDr = "Opening Student";
                narrationCr = "Fee - " + studentData[iter.first]["name"];
            }
            else
            {
                DR = "1";
                CR = "7";
                balanceAmount = iter.second["dr"] - iter.second["cr"];
                narrationDr = "Fee - " + studentData[iter.first]["name"];
                narrationCr = "Opening Student";
            }
            voucherid++;
            voucherArr[key]["id"] = std::to_string(voucherid);
            voucherArr[key]["v_date"] = std::to_string(newYear["start_from"]);
            voucherArr[key]["fk_fiscal_year"] = std::to_string(newYear["id"]);
            voucherArr[key]["fk_posted_by"] = "1";
            voucherArr[key]["narration"] = "Being opening balance recorded.";
            voucherArr[key]["create_date"] = checkup.toString(checkup.getTime());
            voucherArr[key]["type"] = "6";

            voucherDArr[vKey]["fk_voucher_id"] = std::to_string(voucherid);
            voucherDArr[vKey]["fk_ledger_id"] = DR;
            voucherDArr[vKey]["serial_no"] = "1";
            voucherDArr[vKey]["dr_cr"] = "0";
            voucherDArr[vKey]["amount"] = std::to_string(balanceAmount);
            voucherDArr[vKey]["narration"] = narrationDr;
            vKey++;
            voucherDArr[vKey]["fk_voucher_id"] = std::to_string(voucherid);
            voucherDArr[vKey]["fk_ledger_id"] = CR;
            voucherDArr[vKey]["serial_no"] = "2";
            voucherDArr[vKey]["dr_cr"] = "1";
            voucherDArr[vKey]["amount"] = std::to_string(balanceAmount);
            voucherDArr[vKey]["narration"] = narrationCr;
            vKey++;
            studentArr[key]["fk_voucher_id"] = std::to_string(voucherid);
            studentArr[key]["fk_student_id"] = std::to_string(iter.first);
            studentArr[key]["fk_program_group"] = studentData[iter.first]["pg_id"];
            key++;
        }

        if (ledgerDc.size() > 0)
        {
            voucherid++;
            voucherArr[key]["id"] = std::to_string(voucherid);
            voucherArr[key]["v_date"] = std::to_string(newYear["start_from"]);
            voucherArr[key]["fk_fiscal_year"] = std::to_string(newYear["id"]);
            voucherArr[key]["fk_posted_by"] = "1";
            voucherArr[key]["narration"] = "Being opening balance recorded.";
            voucherArr[key]["create_date"] = checkup.toString(checkup.getTime());
            voucherArr[key]["type"] = "6";
            serialNo = 1;
            for (auto &iter : ledgerDc)
            {
                if (iter.second["cr"] == iter.second["dr"])
                {
                    continue;
                }
                voucherDArr[vKey]["fk_voucher_id"] = std::to_string(voucherid);
                voucherDArr[vKey]["fk_ledger_id"] = std::to_string(iter.first);
                voucherDArr[vKey]["serial_no"] = std::to_string(serialNo);
                voucherDArr[vKey]["dr_cr"] = "0";
                voucherDArr[vKey]["amount"] = std::to_string(balanceAmount);
                voucherDArr[vKey]["narration"] = narrationDr;
                if (iter.second["cr"] > iter.second["dr"])
                {
                    voucherDArr[vKey]["dr_cr"] = "0";
                    voucherDArr[vKey]["amount"] = std::to_string((iter.second["cr"] - iter.second["dr"]));
                }
                else
                {
                    voucherDArr[vKey]["dr_cr"] = "1";
                    voucherDArr[vKey]["amount"] = std::to_string((iter.second["dr"] - iter.second["cr"]));
                }
                voucherDArr[vKey]["narration"] = "Opening Balance";
                serialNo++;
                vKey++;
            }
        }
        std::string voucherQuery = "INSERT INTO acc_voucher(id,v_date,fk_fiscal_year,fk_posted_by,narration,create_date,type) VALUES";
        for (auto &iter : voucherArr)
        {
            voucherQuery += "(" + iter.second["id"] + ",";
            voucherQuery += iter.second["v_date"] + ",";
            voucherQuery += iter.second["fk_fiscal_year"] + ",";
            voucherQuery += iter.second["fk_posted_by"] + ",'";
            voucherQuery += iter.second["narration"] + "',";
            voucherQuery += iter.second["create_date"] + ",";
            voucherQuery += iter.second["type"] + "),";
        }
        voucherQuery = voucherQuery.substr(0, voucherQuery.size() - 1);
        std::string voucherDQuery = "INSERT INTO acc_voucher_detail(fk_voucher_id,fk_ledger_id,serial_no,dr_cr,amount,narration) VALUES";
        for (auto &iter : voucherDArr)
        {
            voucherDQuery += "(" + iter.second["fk_voucher_id"] + ",";
            voucherDQuery += iter.second["fk_ledger_id"] + ",";
            voucherDQuery += iter.second["serial_no"] + ",";
            voucherDQuery += iter.second["dr_cr"] + ",";
            voucherDQuery += iter.second["amount"] + ",'";
            voucherDQuery += iter.second["narration"] + "'),";
        }
        voucherDQuery = voucherDQuery.substr(0, voucherDQuery.size() - 1);
        std::string studentQuery = "INSERT INTO acc_voucher_to_student(fk_voucher_id,fk_period_id,fk_student_id,fk_program_group) VALUES";
        for (auto &iter : studentArr)
        {
            studentQuery += "(" + iter.second["fk_voucher_id"] + ",3,";
            studentQuery += iter.second["fk_student_id"] + ",";
            studentQuery += iter.second["fk_program_group"] + "),";
        }
        studentQuery = studentQuery.substr(0, studentQuery.size() - 1);
        try
        {
            if (voucherArr.size() > 0)
            {
                if (stmtDB->executeUpdate(voucherQuery) == 0)
                {
                    conDB->rollback();
                    message += "\nProcess failed!!!\nERROR CODE: 10007\n";
                    return message;
                }
                if (stmtDB->executeUpdate(voucherDQuery) == 0)
                {
                    conDB->rollback();
                    message += "\nProcess failed!!!\nERROR CODE: 10008\n";
                    return message;
                }
                if (studentArr.size() > 0)
                {
                    if (stmtDB->executeUpdate(studentQuery) == 0)
                    {
                        conDB->rollback();
                        message += "\nProcess failed!!!\nERROR CODE: 10009\n";
                        return message;
                    }
                }
                query = "UPDATE acc_year SET is_closed=1 WHERE id=" + std::to_string(oldYear["id"]);
                if (stmtDB->executeUpdate(query) == 0)
                {
                    conDB->rollback();
                    message += "\nProcess failed!!!\nERROR CODE: 10010\n";
                    return message;
                }
                message += "\n----- PHASE 2 COMPLETED -----\n";
            }
            else
            {
                conDB->rollback();
                message += "\nProcess failed!!!\nERROR CODE: 10006\n";
                return message;
            }
        }
        catch (sql::SQLException &e)
        {
            message += e.getSQLState();
            message += "\nProcess failed!!!\nERROR CODE: 10012\n";
            conDB->rollback();
            return message;
        }
        if (payrollOpen() == false)
        {
            conDB->rollback();
        }
        else
        {
            conDB->commit();
        }
        return message;
    }

    bool payrollClose()
    {
        int key = 0, vKey = 0;
        double balanceAmount = 0;
        int ledgerid = 0;
        bool payrolFlag = false;

        std::string DR = "1", CR = "0", narrationDr = "", narrationCr = "";
        std::map<int, std::map<std::string, std::string>> voucherArr;
        std::map<int, std::map<std::string, std::string>> voucherDArr;
        std::map<int, std::map<std::string, std::string>> studentArr;
        std::string query = "SELECT ";
        query += "acc_voucher.id as jno,";
        query += "acc_voucher_detail.fk_ledger_id,";
        query += "hrm_employee.id as emp_id,";
        query += "acc_ledger.name as ledger_name,";
        query += "acc_ledger.fk_ledger_group as lg,";
        query += "acc_voucher_detail.dr_cr,";
        query += "acc_voucher_detail.amount ";
        query += "FROM acc_voucher_detail ";
        query += "LEFT JOIN acc_voucher ON acc_voucher.id=acc_voucher_detail.fk_voucher_id ";
        query += "LEFT JOIN acc_ledger ON acc_ledger.id=acc_voucher_detail.fk_ledger_id ";
        query += "LEFT JOIN hrm_employee ON hrm_employee.fk_ledger_id=acc_ledger.id ";
        query += "WHERE acc_voucher.fk_fiscal_year=" + std::to_string(oldYear["id"]);
        query += " AND acc_voucher.status=1 AND acc_ledger.fk_ledger_group=13 AND hrm_employee.fk_ledger_id=acc_ledger.id";

        delete resDB;
        resDB = stmtDB->executeQuery(query);

        while (resDB->next())
        {
            ledgerid = resDB->getInt("fk_ledger_id");
            if (payrollPostDetail.find(ledgerid) == payrollPostDetail.end())
            {
                payrollPostDetail[ledgerid]["dr"] = 0;
                payrollPostDetail[ledgerid]["cr"] = 0;
            }
            if (resDB->getInt("dr_cr") == 1)
            {
                payrollPostDetail[ledgerid]["dr"] += resDB->getDouble("amount");
            }
            else
            {
                payrollPostDetail[ledgerid]["cr"] += resDB->getDouble("amount");
            }
        }
        query = "SELECT ";
        query += "acc_ledger.id,";
        query += "hrm_employee.id as emp_id,";
        query += "acc_ledger.name as ledger_name,";
        query += "acc_ledger.fk_ledger_group as lg ";
        query += "FROM acc_ledger ";
        query += "LEFT JOIN hrm_employee ON hrm_employee.fk_ledger_id=acc_ledger.id ";
        query += "WHERE acc_ledger.fk_ledger_group=13 AND hrm_employee.fk_ledger_id=acc_ledger.id";

        delete resDB;
        resDB = stmtDB->executeQuery(query);
        Accrep accrep;
        std::string oldyearid = std::to_string(oldYear["id"]);
        std::string advledgers = "";
        while (resDB->next())
        {
            if (resDB->getString("emp_id") == "")
            {
                continue;
            }
            staffDetail[resDB->getInt("id")]["name"] = resDB->getString("ledger_name");
            staffDetail[resDB->getInt("id")]["id"] = resDB->getString("emp_id");
            payrollAdvanceDetail[resDB->getInt("id")] = accrep.empVoucher2(resDB->getString("emp_id"), resDB->getString("id"), oldyearid);
            for (auto &iter : payrollAdvanceDetail[resDB->getInt("id")])
            {
                advledgers += iter.first + ",";
            }
        }
        if (advledgers.length() > 0)
        {
            advledgers = advledgers.substr(0, advledgers.size() - 1);
            query = "SELECT id,name FROM acc_ledger WHERE id IN (" + advledgers + ")";
            delete resDB;
            resDB = stmtDB->executeQuery(query);
            while (resDB->next())
            {
                advLedgerDetail[resDB->getString("id")] = resDB->getString("name");
            }
        }

        std::string post_group = "PCLOSING_" + checkup.toString(checkup.getTime());
        for (auto &iter : payrollPostDetail)
        {
            balanceAmount = 0;
            DR = "1";
            CR = "0";
            if (iter.second["cr"] == iter.second["dr"])
            {
                continue;
            }
            if (iter.second["dr"] > iter.second["cr"])
            {
                DR = std::to_string(iter.first);
                CR = "17";
                balanceAmount = iter.second["dr"] - iter.second["cr"];
                narrationDr = "Closing Salary";
                narrationCr = staffDetail[iter.first]["name"];
            }
            else
            {
                DR = "17";
                CR = std::to_string(iter.first);
                balanceAmount = iter.second["cr"] - iter.second["dr"];
                narrationDr = staffDetail[iter.first]["name"];
                narrationCr = "Closing Salary";
            }
            voucherid++;
            voucherArr[key]["id"] = std::to_string(voucherid);
            voucherArr[key]["v_date"] = std::to_string(oldYear["end_at"]);
            voucherArr[key]["fk_fiscal_year"] = std::to_string(oldYear["id"]);
            voucherArr[key]["fk_posted_by"] = "1";
            voucherArr[key]["narration"] = "Being closing balance recorded.";
            voucherArr[key]["create_date"] = checkup.toString(checkup.getTime());
            voucherArr[key]["type"] = "7";

            voucherDArr[vKey]["fk_voucher_id"] = std::to_string(voucherid);
            voucherDArr[vKey]["fk_ledger_id"] = DR;
            voucherDArr[vKey]["serial_no"] = "1";
            voucherDArr[vKey]["dr_cr"] = "0";
            voucherDArr[vKey]["amount"] = std::to_string(balanceAmount);
            voucherDArr[vKey]["narration"] = narrationDr;
            vKey++;
            voucherDArr[vKey]["fk_voucher_id"] = std::to_string(voucherid);
            voucherDArr[vKey]["fk_ledger_id"] = CR;
            voucherDArr[vKey]["serial_no"] = "2";
            voucherDArr[vKey]["dr_cr"] = "1";
            voucherDArr[vKey]["amount"] = std::to_string(balanceAmount);
            voucherDArr[vKey]["narration"] = narrationCr;
            vKey++;
            studentArr[key]["fk_emp_id"] = staffDetail[iter.first]["id"];
            studentArr[key]["fk_voucher_id"] = std::to_string(voucherid);
            studentArr[key]["type"] = "5";
            studentArr[key]["group_id"] = post_group;
            key++;
        }

        post_group = "ACLOSING_" + checkup.toString(checkup.getTime());
        for (auto &advitr : payrollAdvanceDetail)
        {
            for (auto &iter : advitr.second)
            {
                if (iter.second == 0)
                {
                    continue;
                }
                balanceAmount = 0;
                DR = "1";
                CR = "0";
                if (iter.second > 0)
                {
                    DR = "18";
                    CR = iter.first;
                    balanceAmount = iter.second;
                    narrationDr = "Employee Loan - " + staffDetail[advitr.first]["name"];
                    narrationCr = advLedgerDetail[iter.first];
                }
                else
                {
                    DR = iter.first;
                    CR = "18";
                    balanceAmount = (iter.second * (-1));
                    narrationDr = advLedgerDetail[iter.first];
                    narrationCr = "Employee Loan - " + staffDetail[advitr.first]["name"];
                }

                voucherid++;
                voucherArr[key]["id"] = std::to_string(voucherid);
                voucherArr[key]["v_date"] = std::to_string(oldYear["end_at"]);
                voucherArr[key]["fk_fiscal_year"] = std::to_string(oldYear["id"]);
                voucherArr[key]["fk_posted_by"] = "1";
                voucherArr[key]["narration"] = "Being closing balance recorded.";
                voucherArr[key]["create_date"] = checkup.toString(checkup.getTime());
                voucherArr[key]["type"] = "7";

                voucherDArr[vKey]["fk_voucher_id"] = std::to_string(voucherid);
                voucherDArr[vKey]["fk_ledger_id"] = DR;
                voucherDArr[vKey]["serial_no"] = "1";
                voucherDArr[vKey]["dr_cr"] = "0";
                voucherDArr[vKey]["amount"] = std::to_string(balanceAmount);
                voucherDArr[vKey]["narration"] = narrationDr;
                vKey++;
                voucherDArr[vKey]["fk_voucher_id"] = std::to_string(voucherid);
                voucherDArr[vKey]["fk_ledger_id"] = CR;
                voucherDArr[vKey]["serial_no"] = "2";
                voucherDArr[vKey]["dr_cr"] = "1";
                voucherDArr[vKey]["amount"] = std::to_string(balanceAmount);
                voucherDArr[vKey]["narration"] = narrationCr;
                vKey++;
                studentArr[key]["fk_emp_id"] = staffDetail[advitr.first]["id"];
                studentArr[key]["fk_voucher_id"] = std::to_string(voucherid);
                studentArr[key]["type"] = "5";
                studentArr[key]["group_id"] = post_group;
                key++;
            }
        }

        std::string voucherQuery = "INSERT INTO acc_voucher(id,v_date,fk_fiscal_year,fk_posted_by,narration,create_date,type) VALUES";
        for (auto &iter : voucherArr)
        {
            voucherQuery += "(" + iter.second["id"] + ",";
            voucherQuery += iter.second["v_date"] + ",";
            voucherQuery += iter.second["fk_fiscal_year"] + ",";
            voucherQuery += iter.second["fk_posted_by"] + ",'";
            voucherQuery += iter.second["narration"] + "',";
            voucherQuery += iter.second["create_date"] + ",";
            voucherQuery += iter.second["type"] + "),";
        }
        voucherQuery = voucherQuery.substr(0, voucherQuery.size() - 1);

        std::string voucherDQuery = "INSERT INTO acc_voucher_detail(fk_voucher_id,fk_ledger_id,serial_no,dr_cr,amount,narration) VALUES";
        for (auto &iter : voucherDArr)
        {
            voucherDQuery += "(" + iter.second["fk_voucher_id"] + ",";
            voucherDQuery += iter.second["fk_ledger_id"] + ",";
            voucherDQuery += iter.second["serial_no"] + ",";
            voucherDQuery += iter.second["dr_cr"] + ",";
            voucherDQuery += iter.second["amount"] + ",'";
            voucherDQuery += iter.second["narration"] + "'),";
        }
        voucherDQuery = voucherDQuery.substr(0, voucherDQuery.size() - 1);

        std::string payrollQuery = "INSERT INTO acc_payroll_post(fk_emp_id,fk_period_id,fk_voucher_id,type,group_id) VALUES";
        for (auto &iter : studentArr)
        {
            payrollQuery += "(" + iter.second["fk_emp_id"] + ",3,";
            payrollQuery += iter.second["fk_voucher_id"] + ",";
            payrollQuery += iter.second["type"] + ",'";
            payrollQuery += iter.second["group_id"] + "'),";
        }
        payrollQuery = payrollQuery.substr(0, payrollQuery.size() - 1);
        try
        {
            if (voucherArr.size() > 0)
            {
                if (stmtDB->executeUpdate(voucherQuery) == 0)
                {
                    conDB->rollback();
                    message += "\nProcess failed!!!\nERROR CODE: 20007\n";
                    return payrolFlag;
                }

                if (stmtDB->executeUpdate(voucherDQuery) == 0)
                {
                    conDB->rollback();
                    message += "\nProcess failed!!!\nERROR CODE: 20008\n";
                    return payrolFlag;
                }
                if (studentArr.size() > 0)
                {
                    if (stmtDB->executeUpdate(payrollQuery) == 0)
                    {
                        conDB->rollback();
                        message += "\nProcess failed!!!\nERROR CODE: 20009\n";
                        return payrolFlag;
                    }
                }
                message += "\n----- PHASE 1 COMPLETED -----\n";
                payrolFlag = true;
            }
            else
            {
                message += "\n----- PHASE 1 COMPLETED! -----\n";
                payrolFlag = true;
            }
        }
        catch (sql::SQLException &e)
        {
            message += e.getSQLState();
            message += "\nProcess failed!!!\nERROR CODE: 20010\n";
            conDB->rollback();
        }
        return payrolFlag;
    }

    bool payrollOpen()
    {
        int key = 0, vKey = 0;
        double balanceAmount = 0;
        bool payrolFlag = false;

        std::string DR = "1", CR = "0", narrationDr = "", narrationCr = "";
        std::map<int, std::map<std::string, std::string>> voucherArr;
        std::map<int, std::map<std::string, std::string>> voucherDArr;
        std::map<int, std::map<std::string, std::string>> studentArr;

        std::string post_group = "POPENING_" + checkup.toString(checkup.getTime());
        for (auto &iter : payrollPostDetail)
        {
            balanceAmount = 0;
            DR = "1";
            CR = "0";
            if (iter.second["cr"] == iter.second["dr"])
            {
                continue;
            }
            if (iter.second["cr"] > iter.second["dr"])
            {
                DR = std::to_string(iter.first);
                CR = "17";
                balanceAmount = iter.second["cr"] - iter.second["dr"];
                narrationDr = "Opening Salary";
                narrationCr = staffDetail[iter.first]["name"];
            }
            else
            {
                DR = "17";
                CR = std::to_string(iter.first);
                balanceAmount = iter.second["dr"] - iter.second["cr"];
                narrationDr = staffDetail[iter.first]["name"];
                narrationCr = "Opening Salary";
            }
            voucherid++;
            voucherArr[key]["id"] = std::to_string(voucherid);
            voucherArr[key]["v_date"] = std::to_string(newYear["start_from"]);
            voucherArr[key]["fk_fiscal_year"] = std::to_string(newYear["id"]);
            voucherArr[key]["fk_posted_by"] = "1";
            voucherArr[key]["narration"] = "Being opening balance recorded.";
            voucherArr[key]["create_date"] = checkup.toString(checkup.getTime());
            voucherArr[key]["type"] = "6";

            voucherDArr[vKey]["fk_voucher_id"] = std::to_string(voucherid);
            voucherDArr[vKey]["fk_ledger_id"] = DR;
            voucherDArr[vKey]["serial_no"] = "1";
            voucherDArr[vKey]["dr_cr"] = "0";
            voucherDArr[vKey]["amount"] = std::to_string(balanceAmount);
            voucherDArr[vKey]["narration"] = narrationDr;
            vKey++;
            voucherDArr[vKey]["fk_voucher_id"] = std::to_string(voucherid);
            voucherDArr[vKey]["fk_ledger_id"] = CR;
            voucherDArr[vKey]["serial_no"] = "2";
            voucherDArr[vKey]["dr_cr"] = "1";
            voucherDArr[vKey]["amount"] = std::to_string(balanceAmount);
            voucherDArr[vKey]["narration"] = narrationCr;
            vKey++;
            studentArr[key]["fk_emp_id"] = staffDetail[iter.first]["id"];
            studentArr[key]["fk_voucher_id"] = std::to_string(voucherid);
            studentArr[key]["type"] = "4";
            studentArr[key]["group_id"] = post_group;
            key++;
        }

        post_group = "AOPENING_" + checkup.toString(checkup.getTime());
        for (auto &advitr : payrollAdvanceDetail)
        {
            for (auto &iter : advitr.second)
            {
                balanceAmount = 0;
                DR = "1";
                CR = "0";
                if (iter.second > 0)
                {
                    DR = iter.first;
                    CR = "18";
                    balanceAmount = iter.second;
                    narrationDr = "Employee Loan - " + staffDetail[advitr.first]["name"];
                    narrationCr = advLedgerDetail[iter.first];
                }
                else
                {
                    DR = "18";
                    CR = iter.first;

                    balanceAmount = (iter.second * (-1));
                    narrationDr = advLedgerDetail[iter.first];
                    narrationCr = "Employee Loan - " + staffDetail[advitr.first]["name"];
                }

                voucherid++;
                voucherArr[key]["id"] = std::to_string(voucherid);
                voucherArr[key]["v_date"] = std::to_string(newYear["start_from"]);
                voucherArr[key]["fk_fiscal_year"] = std::to_string(newYear["id"]);
                voucherArr[key]["fk_posted_by"] = "1";
                voucherArr[key]["narration"] = "Being opening balance recorded.";
                voucherArr[key]["create_date"] = checkup.toString(checkup.getTime());
                voucherArr[key]["type"] = "6";

                voucherDArr[vKey]["fk_voucher_id"] = std::to_string(voucherid);
                voucherDArr[vKey]["fk_ledger_id"] = DR;
                voucherDArr[vKey]["serial_no"] = "1";
                voucherDArr[vKey]["dr_cr"] = "0";
                voucherDArr[vKey]["amount"] = std::to_string(balanceAmount);
                voucherDArr[vKey]["narration"] = narrationDr;
                vKey++;
                voucherDArr[vKey]["fk_voucher_id"] = std::to_string(voucherid);
                voucherDArr[vKey]["fk_ledger_id"] = CR;
                voucherDArr[vKey]["serial_no"] = "2";
                voucherDArr[vKey]["dr_cr"] = "1";
                voucherDArr[vKey]["amount"] = std::to_string(balanceAmount);
                voucherDArr[vKey]["narration"] = narrationCr;
                vKey++;
                studentArr[key]["fk_emp_id"] = staffDetail[advitr.first]["id"];
                studentArr[key]["fk_voucher_id"] = std::to_string(voucherid);
                studentArr[key]["type"] = "3";
                studentArr[key]["group_id"] = post_group;
                key++;
            }
        }

        std::string voucherQuery = "INSERT INTO acc_voucher(id,v_date,fk_fiscal_year,fk_posted_by,narration,create_date,type) VALUES";
        for (auto &iter : voucherArr)
        {
            voucherQuery += "(" + iter.second["id"] + ",";
            voucherQuery += iter.second["v_date"] + ",";
            voucherQuery += iter.second["fk_fiscal_year"] + ",";
            voucherQuery += iter.second["fk_posted_by"] + ",'";
            voucherQuery += iter.second["narration"] + "',";
            voucherQuery += iter.second["create_date"] + ",";
            voucherQuery += iter.second["type"] + "),";
        }
        voucherQuery = voucherQuery.substr(0, voucherQuery.size() - 1);
        std::string voucherDQuery = "INSERT INTO acc_voucher_detail(fk_voucher_id,fk_ledger_id,serial_no,dr_cr,amount,narration) VALUES";
        for (auto &iter : voucherDArr)
        {
            voucherDQuery += "(" + iter.second["fk_voucher_id"] + ",";
            voucherDQuery += iter.second["fk_ledger_id"] + ",";
            voucherDQuery += iter.second["serial_no"] + ",";
            voucherDQuery += iter.second["dr_cr"] + ",";
            voucherDQuery += iter.second["amount"] + ",'";
            voucherDQuery += iter.second["narration"] + "'),";
        }

        voucherDQuery = voucherDQuery.substr(0, voucherDQuery.size() - 1);
        std::string payrollQuery = "INSERT INTO acc_payroll_post(fk_emp_id,fk_period_id,fk_voucher_id,type,group_id) VALUES";
        for (auto &iter : studentArr)
        {
            payrollQuery += "(" + iter.second["fk_emp_id"] + ",3,";
            payrollQuery += iter.second["fk_voucher_id"] + ",";
            payrollQuery += iter.second["type"] + ",'";
            payrollQuery += iter.second["group_id"] + "'),";
        }
        payrollQuery = payrollQuery.substr(0, payrollQuery.size() - 1);
        try
        {
            if (voucherArr.size() > 0)
            {
                if (stmtDB->executeUpdate(voucherQuery) == 0)
                {
                    conDB->rollback();
                    message += "\nProcess failed!!!\nERROR CODE: 30007\n";
                    return payrolFlag;
                }
                if (stmtDB->executeUpdate(voucherDQuery) == 0)
                {
                    conDB->rollback();
                    message += "\nProcess failed!!!\nERROR CODE: 30008\n";
                    return payrolFlag;
                }
                if (studentArr.size() > 0)
                {
                    if (stmtDB->executeUpdate(payrollQuery) == 0)
                    {
                        conDB->rollback();
                        message += "\nProcess failed!!!\nERROR CODE: 30009\n";
                        return payrolFlag;
                    }
                }
                //                message += "\nPayroll Opening : "+payrollQuery+"\n";
                message += "\n----- PHASE 3 COMPLETED -----\n";
                payrolFlag = true;
            }
            else
            {
                message += "\n----- PHASE 3 COMPLETED! -----\n";
                payrolFlag = true;
            }
        }
        catch (sql::SQLException &e)
        {
            message += e.getSQLState();
            message += "\nProcess failed!!!\nERROR CODE: 30010\n";
            conDB->rollback();
        }
        return payrolFlag;
    }
};