/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   DB.h
 * Author: krishna
 *
 * Created on April 9, 2019, 4:16 PM
 */
#pragma once

#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include "AccObj.h"
#include "Pobj.h"
using namespace sql;

class DB
{
private:
    std::string errorMessage;
    std::string db_name, host_id, db_user, db_password;
    int updateStatus = 0;
    int mainCounter = 10000;

    std::map<std::string, std::map<std::string, std::string>> resultSet;

public:
    std::map<std::string, std::map<std::string, double>> feemonth;
    std::map<std::string, bool> scholarhead;
    std::map<std::string, std::string> tempDataS1;
    std::map<std::string, std::map<std::string, std::string>> tempDataS2;
    AccObj accObj;

    DB()
    {
    }

    virtual ~DB()
    {
    }

    void setup(std::string name, std::string host, std::string user, std::string password)
    {
        /* Create a connection */
        //        db_name = name;
        Pobj pobj;
        db_name = pobj.getDb(name);
        host_id = host;
        db_user = user;
        db_password = password;
    };

    void setAccObj(AccObj accObjt)
    {
        accObj = accObjt;
    }

    void setCounter(int counter)
    {
        mainCounter = counter;
    }

    std::map<std::string, std::map<std::string, std::string>> getResult()
    {
        return resultSet;
    }

    bool execute(std::string statement)
    {
        sql::Statement *stmt;
        sql::Driver *driver;
        sql::Connection *con;
        bool flag = true;

        driver = get_driver_instance();
        con = driver->connect("tcp://" + host_id, db_user, db_password);
        /* Connect to the MySQL test database */
        if (db_name.length() > 0)
        {
            con->setSchema(db_name);
        }
        try
        {
            stmt = con->createStatement();
            updateStatus = stmt->executeUpdate(statement);
            stmt->close();
            delete stmt;
            con->close();
            delete con;
            driver->threadEnd();
            //            delete driver;
        }
        catch (sql::SQLException &e)
        {
            errorMessage = e.getSQLState();
            flag = false;
        }
        return flag;
    }

    bool query(std::string statement, std::map<std::string, int> column)
    {
        sql::Statement *stmt;
        sql::ResultSet *res;
        sql::Driver *driver;
        sql::Connection *con;
        bool flag = true;

        driver = get_driver_instance();
        con = driver->connect("tcp://" + host_id, db_user, db_password);
        /* Connect to the MySQL test database */
        if (db_name.length() > 0)
        {
            con->setSchema(db_name);
        }
        try
        {
            stmt = con->createStatement();
            res = stmt->executeQuery(statement);
            resultSet.clear();
            int countrr = mainCounter;
            std::string scount = std::to_string(countrr);
            while (res->next())
            {
                for (auto &iter : column)
                {
                    switch (iter.second)
                    {
                    case 1:
                        resultSet[scount][iter.first] = std::to_string(res->getInt(iter.first));
                        break;
                    case 2:
                        resultSet[scount][iter.first] = std::to_string(res->getDouble(iter.first));
                        break;
                    default:
                        resultSet[scount][iter.first] = res->getString(iter.first);
                    }
                }
                countrr++;
                scount = std::to_string(countrr);
            }
            res->close();
            delete res;
            stmt->close();
            delete stmt;
            con->close();
            delete con;
            driver->threadEnd();
        }
        catch (sql::SQLException &e)
        {
            errorMessage = e.getSQLState();
            flag = false;
        }
        return flag;
    }

    int getUpdateState()
    {
        return updateStatus;
    }

    std::string getError()
    {
        return errorMessage;
    }

    Php::Value processFeeReport(std::map<std::string, std::string> stdid, bool flag)
    {
        sql::Statement *stmt;
        sql::ResultSet *ledgerDetail;
        sql::Driver *driver;
        sql::Connection *con;
        std::map<std::string, std::map<std::string, double>> billParticularBig;
        std::map<std::string, double> billParticular;
        std::string query;
        double totalpaid = 0, amount = 0;
        int fee_month_i;
        std::string ledger_id, fee_month_s, studentid;
        std::map<int, double> feemonthtemp;

        driver = get_driver_instance();
        con = driver->connect("tcp://" + host_id, db_user, db_password);
        /* Connect to the MySQL test database */
        if (db_name.length() > 0)
        {
            con->setSchema(db_name);
        }
        try
        {
            stmt = con->createStatement();
            for (auto &iter : stdid)
            {
                studentid = iter.second;
                query = getBilling(studentid, accObj, 1);
                ledgerDetail = stmt->executeQuery(query);
                std::map<std::string, std::string> paidVoucher;
                while (ledgerDetail->next())
                {
                    if (ledgerDetail->getString("ledger_id") == "15" && ledgerDetail->getInt("dr_cr") == 1)
                    {
                        continue;
                    }
                    fee_month_s = ledgerDetail->getString("fee_month");
                    fee_month_i = ledgerDetail->getInt("fee_month");
                    amount = ledgerDetail->getDouble("amount");
                    ledger_id = ledgerDetail->getString("ledger_id");
                    if (ledgerDetail->getInt("dr_cr") == 1)
                    {
                        if (billParticular.find(ledger_id) == billParticular.end())
                        {
                            billParticular[ledger_id] = amount;
                        }
                        else
                        {
                            billParticular[ledger_id] += amount;
                        }
                        if (feemonthtemp.find(fee_month_i) == feemonthtemp.end())
                        {
                            feemonth[studentid][fee_month_s] = amount;
                            feemonthtemp[fee_month_i] = amount;
                        }
                        else
                        {
                            feemonth[studentid][fee_month_s] += amount;
                            feemonthtemp[fee_month_i] += amount;
                        }
                    }
                    else
                    {
                        totalpaid += amount;
                        paidVoucher[ledgerDetail->getString("voucher_id")] = ledgerDetail->getString("voucher_id");
                    }
                }
                if (billParticular.size() > 0 && paidVoucher.size() > 0)
                {
                    query = getVoucher(paidVoucher);
                    ledgerDetail = stmt->executeQuery(query);
                    while (ledgerDetail->next())
                    {
                        amount = ledgerDetail->getDouble("item_amount");
                        ledger_id = ledgerDetail->getString("fk_ledger_id");
                        if (billParticular.find(ledger_id) != billParticular.end())
                        {
                            billParticular[ledger_id] -= amount;
                            if (billParticular[ledger_id] <= 0 && flag)
                            {
                                billParticular.erase(ledger_id);
                            }
                        }
                        else if (!flag)
                        {
                            billParticular[ledger_id] = amount * -1;
                        }
                    }
                }
                for (auto &ite : feemonthtemp)
                {
                    if ((ite.second - totalpaid) < 0)
                    {
                        fee_month_s = std::to_string(ite.first);
                        // feemonthtemp.erase(ite.first);
                        feemonth[studentid].erase(fee_month_s);
                        totalpaid = totalpaid - ite.second;
                    }
                }
                billParticularBig[studentid] = billParticular;
                feemonthtemp.clear();
                billParticular.clear();
                totalpaid = 0;
            }

            ledgerDetail->close();
            delete ledgerDetail;
            stmt->close();
            delete stmt;
            con->close();
            delete con;
            driver->threadEnd();
        }
        catch (sql::SQLException &e)
        {
            errorMessage = e.getSQLState();
            flag = false;
        }

        return billParticularBig;
    }

    Php::Value processFeeFullReport(std::map<std::string, std::string> stdid, int vst)
    {

        sql::Statement *stmt;
        sql::ResultSet *ledgerDetail;
        sql::Driver *driver;
        sql::Connection *con;
        std::map<std::string, std::map<std::string, std::map<std::string, double>>> billParticularBig;
        std::map<std::string, std::map<std::string, double>> billParticular;
        std::string query;
        double amount = 0;
        std::string ledger_id, studentid;
        driver = get_driver_instance();
        con = driver->connect("tcp://" + host_id, db_user, db_password);
        /* Connect to the MySQL test database */
        if (db_name.length() > 0)
        {
            con->setSchema(db_name);
        }
        try
        {
            stmt = con->createStatement();
            for (auto &iter : stdid)
            {
                studentid = iter.second;
                query = getBilling(studentid, accObj, vst);
                ledgerDetail = stmt->executeQuery(query);
                std::map<std::string, std::string> paidVoucher;
                while (ledgerDetail->next())
                {
                    if (ledgerDetail->getString("ledger_id") == "15" && ledgerDetail->getInt("dr_cr") == 1)
                    {
                        continue;
                    }
                    amount = ledgerDetail->getDouble("amount");
                    ledger_id = ledgerDetail->getString("ledger_id");
                    if (ledgerDetail->getInt("dr_cr") == 1)
                    {
                        if (billParticular.find(ledger_id) == billParticular.end())
                        {
                            billParticular[ledger_id]["post"] = amount;
                            billParticular[ledger_id]["paid"] = 0;
                            billParticular[ledger_id]["scholar"] = 0;
                        }
                        else
                        {
                            billParticular[ledger_id]["post"] += amount;
                        }
                        scholarhead[ledger_id] = false;
                    }
                    else
                    {
                        paidVoucher[ledgerDetail->getString("voucher_id")] = ledgerDetail->getString("voucher_id");
                    }
                }
                if (billParticular.size() > 0 && paidVoucher.size() > 0)
                {
                    query = getVoucher(paidVoucher);
                    ledgerDetail = stmt->executeQuery(query);
                    while (ledgerDetail->next())
                    {
                        amount = ledgerDetail->getDouble("item_amount");
                        ledger_id = ledgerDetail->getString("fk_ledger_id");
                        if (billParticular.find(ledger_id) == billParticular.end())
                        {
                            billParticular[ledger_id]["post"] = 0;
                            billParticular[ledger_id]["paid"] = 0;
                            billParticular[ledger_id]["scholar"] = 0;
                            if (ledgerDetail->getString("type") == "scholar")
                            {
                                scholarhead[ledger_id] = true;
                                billParticular[ledger_id]["scholar"] = ledgerDetail->getDouble("item_amount");
                            }
                            else
                            {
                                billParticular[ledger_id]["paid"] = ledgerDetail->getDouble("item_amount");
                            }
                        }
                        else
                        {
                            if (ledgerDetail->getString("type") == "scholar")
                            {
                                scholarhead[ledger_id] = true;
                                billParticular[ledger_id]["scholar"] += ledgerDetail->getDouble("item_amount");
                            }
                            else
                            {
                                billParticular[ledger_id]["paid"] += ledgerDetail->getDouble("item_amount");
                            }
                        }
                    }
                }
                billParticularBig[studentid] = billParticular;
                billParticular.clear();
            }
            ledgerDetail->close();
            delete ledgerDetail;
            stmt->close();
            delete stmt;
            con->close();
            delete con;
            driver->threadEnd();
        }
        catch (sql::SQLException &e)
        {
            errorMessage = e.getSQLState();
        }

        return billParticularBig;
    }

    Php::Value processFeeMonthlyReport(std::map<std::string, std::string> stdList, std::string type)
    {

        sql::Statement *stmt;
        sql::ResultSet *ledgerDetail;
        sql::Driver *driver;
        sql::Connection *con;
        std::map<std::string, std::map<std::string, std::map<std::string, double>>> billParticularBig;
        std::string query;
        driver = get_driver_instance();
        con = driver->connect("tcp://" + host_id, db_user, db_password);
        /* Connect to the MySQL test database */
        if (db_name.length() > 0)
        {
            con->setSchema(db_name);
        }
        try
        {
            stmt = con->createStatement();
            query = getMonthly(stdList, accObj);
            ledgerDetail = stmt->executeQuery(query);
            std::string month, stdid, ledgerid;
            if (type == "1")
            {
                while (ledgerDetail->next())
                {
                    stdid = ledgerDetail->getString("stdid");
                    month = ledgerDetail->getString("month");
                    ledgerid = ledgerDetail->getString("ledgerid");

                    if (billParticularBig.find(stdid) == billParticularBig.end())
                    {
                        billParticularBig[stdid][ledgerid][month] = 0;
                    }
                    else if (billParticularBig[stdid].find(ledgerid) == billParticularBig[stdid].end())
                    {
                        billParticularBig[stdid][ledgerid][month] = 0;
                    }
                    else if (billParticularBig[stdid][ledgerid].find(month) == billParticularBig[stdid][ledgerid].end())
                    {
                        billParticularBig[stdid][ledgerid][month] = 0;
                    }
                    tempDataS1[ledgerid] = ledgerid;
                    tempDataS2[month][ledgerid] = ledgerid;
                    billParticularBig[stdid][ledgerid][month] += ledgerDetail->getDouble("amount");
                }
            }
            else
            {
                stdid = "0";
                while (ledgerDetail->next())
                {
                    month = ledgerDetail->getString("month");
                    ledgerid = ledgerDetail->getString("ledgerid");
                    if (billParticularBig[stdid].find(ledgerid) == billParticularBig[stdid].end())
                    {
                        billParticularBig[stdid][ledgerid][month] = 0;
                    }
                    else if (billParticularBig[stdid][ledgerid].find(month) == billParticularBig[stdid][ledgerid].end())
                    {
                        billParticularBig[stdid][ledgerid][month] = 0;
                    }
                    tempDataS1[ledgerid] = ledgerid;
                    tempDataS2[month][ledgerid] = ledgerid;
                    billParticularBig[stdid][ledgerid][month] += ledgerDetail->getDouble("amount");
                }
            }

            ledgerDetail->close();
            delete ledgerDetail;
            stmt->close();
            delete stmt;
            con->close();
            delete con;
            driver->threadEnd();
        }
        catch (sql::SQLException &e)
        {
            errorMessage = e.getSQLState();
        }

        return billParticularBig;
    }

    Php::Value processFeeHead(std::map<std::string, std::string> stdid, std::string headid)
    {
        sql::Statement *stmt;
        sql::ResultSet *ledgerDetail;
        sql::Driver *driver;
        sql::Connection *con;
        std::map<std::string, double> billParticular;
        std::string query;
        double amount = 0;
        int fee_month_i;
        std::string fee_month_s, studentid;
        std::map<std::string, std::map<int, double>> feemonthtemp;
        std::map<int, double> feemonthclone;
        std::map<std::string, double> totalpaid;

        driver = get_driver_instance();
        con = driver->connect("tcp://" + host_id, db_user, db_password);
        /* Connect to the MySQL test database */
        if (db_name.length() > 0)
        {
            con->setSchema(db_name);
        }
        try
        {
            stmt = con->createStatement();
            query = getDueHeads(stdid, headid, accObj);
            ledgerDetail = stmt->executeQuery(query);
            std::map<std::string, std::string> paidVoucher, paidVouchertrack;
            while (ledgerDetail->next())
            {
                studentid = ledgerDetail->getString("student_id");
                amount = ledgerDetail->getDouble("amount");
                if (ledgerDetail->getInt("dr_cr") == 1 || ledgerDetail->getInt("typ") == 1)
                {
                    fee_month_i = ledgerDetail->getInt("fee_month");
                    fee_month_s = ledgerDetail->getString("fee_month");
                    if (billParticular.find(studentid) == billParticular.end())
                    {
                        billParticular[studentid] = amount;
                    }
                    else
                    {
                        billParticular[studentid] += amount;
                    }
                    if (feemonth.find(studentid) == feemonth.end())
                    {
                        feemonth[studentid][fee_month_s] = amount;
                        feemonthtemp[studentid][fee_month_i] = amount;
                    }
                    else if (feemonth[studentid].find(studentid) == feemonth[studentid].end())
                    {
                        feemonth[studentid][fee_month_s] = amount;
                        feemonthtemp[studentid][fee_month_i] = amount;
                    }
                    else
                    {
                        feemonth[studentid][fee_month_s] += amount;
                        feemonthtemp[studentid][fee_month_i] += amount;
                    }
                }
                else
                {
                    if (totalpaid.find(studentid) == totalpaid.end())
                    {
                        totalpaid[studentid] = 0;
                    }
                    totalpaid[studentid] += amount;
                    paidVouchertrack[ledgerDetail->getString("voucher_id")] = studentid;
                    paidVoucher[ledgerDetail->getString("voucher_id")] = ledgerDetail->getString("voucher_id");
                }
            }
            if (billParticular.size() > 0 && paidVoucher.size() > 0)
            {
                query = getVoucher(paidVoucher);
                ledgerDetail = stmt->executeQuery(query);
                while (ledgerDetail->next())
                {
                    studentid = paidVouchertrack[ledgerDetail->getString("fk_voucher_id")];
                    if (billParticular.find(studentid) != billParticular.end())
                    {
                        billParticular[studentid] -= ledgerDetail->getDouble("item_amount");
                    }
                }
            }
            for (auto &ite : feemonthtemp)
            {
                feemonthclone = ite.second;
                for (auto &ites : feemonthclone)
                {
                    if ((ites.second - totalpaid[ite.first]) < 0)
                    {
                        fee_month_s = std::to_string(ites.first);
                        feemonthtemp[ite.first].erase(ites.first);
                        feemonth[ite.first].erase(fee_month_s);
                        totalpaid[ite.first] = totalpaid[ite.first] - ites.second;
                    }
                }
            }

            ledgerDetail->close();
            delete ledgerDetail;
            stmt->close();
            delete stmt;
            con->close();
            delete con;
            driver->threadEnd();
        }
        catch (sql::SQLException &e)
        {
            errorMessage = e.getSQLState();
        }

        return billParticular;
    }

    std::string getBilling(std::string studentid, AccObj accObj, int issch)
    {
        std::string year_id = std::to_string(accObj.year_id);
        std::string query = "SELECT ";
        query += "acc_voucher.id as voucher_id,";
        query += "acc_voucher_detail.fk_ledger_id as ledger_id,";
        query += "acc_voucher_detail.dr_cr,";
        query += "acc_voucher_detail.amount,";
        //        query += "acc_fee_head.taxable as taxable,";
        query += "acc_ledger.name as ledger,";
        query += "acc_voucher_to_student.fk_period_id as fee_month,acc_voucher_to_student.typ ";
        query += "FROM acc_voucher_to_student ";
        query += "LEFT JOIN student_to_aca ON student_to_aca.fk_student_id=acc_voucher_to_student.fk_student_id ";
        query += "LEFT JOIN acc_voucher ON acc_voucher.id=acc_voucher_to_student.fk_voucher_id ";
        query += "LEFT JOIN acc_voucher_detail ON acc_voucher.id=acc_voucher_detail.fk_voucher_id ";
        query += "LEFT JOIN acc_ledger ON acc_ledger.id=acc_voucher_detail.fk_ledger_id ";
        //        query += "LEFT JOIN acc_fee_head ON acc_ledger.id=acc_fee_head.fk_ledger_id ";
        query += "WHERE acc_voucher.status=1 ";
        query += "AND acc_voucher.fk_fiscal_year=" + year_id;
        query += " AND student_to_aca.status IN (" + getCsv(accObj.studentStatus) + ") ";
        query += "AND acc_voucher_detail.fk_ledger_id NOT IN (4,1) ";
        query += "AND acc_ledger.fk_ledger_group NOT IN (4,8) ";
        
        if (accObj.from_date > 0)
        {
            query += " AND acc_voucher.v_date>=" + std::to_string(accObj.from_date);
        }
        if (accObj.to_date > 0)
        {
            query += " AND acc_voucher.v_date<" + std::to_string(accObj.to_date);
        }
        query += " AND acc_voucher_to_student.fk_student_id=" + studentid;
        query += " ORDER BY acc_voucher.create_date ASC,acc_voucher_detail.serial_no ASC";
        return query;
    }

    std::string getMonthly(std::map<std::string, std::string> studentID, AccObj accObj)
    {
        std::string year_id = std::to_string(accObj.year_id);
        std::string query = "SELECT ";
        query += "acc_invoice_items.item_amount as amount,";
        query += "acc_voucher_to_student.fk_student_id as stdid,";
        query += "acc_invoice_items.fk_ledger_id as ledgerid,";
        query += "acc_voucher_to_student.fk_period_id as month ";
        query += "FROM acc_invoice_items ";
        query += "LEFT JOIN acc_invoice ON acc_invoice.id=acc_invoice_items.invoice_id ";
        query += "LEFT JOIN acc_fee_head ON acc_fee_head.fk_ledger_id=acc_invoice_items.fk_ledger_id ";
        query += "LEFT JOIN acc_voucher_to_student ON acc_voucher_to_student.fk_voucher_id=acc_invoice.fk_voucher_id ";
        query += "LEFT JOIN acc_voucher ON acc_voucher.id=acc_invoice.fk_voucher_id ";
        query += "WHERE acc_voucher.status=1 AND acc_fee_head.type=1 ";
        query += "AND acc_voucher.fk_fiscal_year=" + year_id;
        query += " AND acc_voucher_to_student.fk_student_id IN (" + getCsv(studentID) + ") ";
        return query;
    }

    std::string getVoucher(std::map<std::string, std::string> paidVoucher)
    {
        std::string query = "SELECT *";
        query += "FROM ((SELECT fk_ledger_id,item_qty,item_amount,fk_voucher_id,'bill' as type ";
        query += "FROM acc_invoice ";
        query += "LEFT JOIN acc_invoice_items ON acc_invoice.id=acc_invoice_items.invoice_id ";
        query += "WHERE acc_invoice.fk_voucher_id IN (" + getCsv(paidVoucher) + ")) ";
        query += "UNION ";
        query += "(SELECT acc_fee_head.fk_ledger_id,1 as item_qty,acc_voucher_detail.amount as item_amount,fk_voucher_id,'scholar' as type ";
        query += "FROM acc_voucher_detail ";
        query += "LEFT JOIN acc_ledger ON acc_ledger.id=acc_voucher_detail.fk_ledger_id ";
        query += "LEFT JOIN acc_student_expn ON acc_ledger.id=acc_student_expn.fk_ledger_id ";
        query += "LEFT JOIN acc_fee_head ON acc_fee_head.id=acc_student_expn.fk_fee_head ";
        query += "WHERE acc_ledger.fk_ledger_group=5 ";
        query += "AND acc_ledger.id!=4 ";
        query += "AND acc_voucher_detail.fk_voucher_id IN (" + getCsv(paidVoucher) + "))";
        query += "UNION ";
        query += "(SELECT fk_ledger_id,1 as item_qty,acc_voucher_detail.amount as item_amount,fk_voucher_id,'ledger' as type ";
        query += "FROM acc_voucher_detail ";
        query += "LEFT JOIN acc_ledger ON acc_ledger.id=acc_voucher_detail.fk_ledger_id ";
        query += "WHERE acc_ledger.fk_ledger_group IN (1,11) ";
        query += "AND acc_voucher_detail.fk_voucher_id IN (" + getCsv(paidVoucher) + "))";
        query += ")tbl ";
        query += "UNION ";
        query += "(SELECT fk_ledger_id,1 as item_qty,acc_voucher_detail.amount as item_amount,fk_voucher_id,'scholar' as type ";
        query += "FROM acc_voucher_detail ";
        query += "LEFT JOIN acc_ledger ON acc_ledger.id=acc_voucher_detail.fk_ledger_id ";
        query += "WHERE acc_ledger.fk_ledger_id!=20 ";
        query += "AND acc_voucher_detail.fk_voucher_id IN (" + getCsv(paidVoucher) + "))";
        query += ")tbl ";
        return query;
    }

    std::string getDueHeads(std::map<std::string, std::string> stdid, std::string headid, AccObj accObj)
    {
        std::string year_id = std::to_string(accObj.year_id);
        std::string query = "SELECT ";
        query += "acc_voucher_to_student.fk_student_id as student_id,";
        query += "acc_voucher.id as voucher_id,";
        query += "acc_voucher_detail.dr_cr,";
        query += "acc_voucher_detail.amount,";
        query += "acc_ledger.name as ledger, ";
        query += "acc_voucher_to_student.fk_period_id as fee_month ";
        query += "FROM acc_voucher_to_student ";
        query += "LEFT JOIN acc_voucher ON acc_voucher.id=acc_voucher_to_student.fk_voucher_id ";
        query += "LEFT JOIN acc_voucher_detail ON acc_voucher.id=acc_voucher_detail.fk_voucher_id ";
        query += "LEFT JOIN acc_ledger ON acc_ledger.id=acc_voucher_detail.fk_ledger_id ";
        query += "WHERE acc_voucher.status=1 ";
        query += "AND acc_voucher.fk_fiscal_year=" + year_id;
        query += " AND acc_voucher_detail.fk_ledger_id=" + headid;
        query += " AND acc_voucher_to_student.fk_student_id IN (" + getCsv(stdid) + ") ";
        if (accObj.to_date > 0)
        {
            query += " AND acc_voucher.v_date<" + std::to_string(accObj.to_date);
        }
        query += " ORDER BY acc_voucher.create_date ASC,acc_voucher_detail.serial_no ASC";
        return query;
    }

    int closeYearAdj()
    {
        int counter = 0;
        std::map<int, std::string> queryList;
        queryList[counter] = "UPDATE acc_scholar_track SET fk_ledger_id=7 WHERE fk_ledger_id=8;";
        counter++;
        queryList[counter] = "UPDATE acc_scholar_track SET fk_ledger_id=8 WHERE fk_ledger_id=9;";
        counter++;
        queryList[counter] = "UPDATE acc_student_expn SET fk_ledger_id=7 WHERE fk_ledger_id=8;";
        counter++;
        queryList[counter] = "UPDATE acc_student_expn SET fk_ledger_id=8 WHERE fk_ledger_id=9;";
        counter++;
        queryList[counter] = "UPDATE acc_student_expn_detail SET fk_ledger_id=7 WHERE fk_ledger_id=8;";
        counter++;
        queryList[counter] = "UPDATE acc_student_expn_detail SET fk_ledger_id=8 WHERE fk_ledger_id=9;";
        counter++;
        queryList[counter] = "UPDATE acc_student_expn_percent SET fk_ledger_id=7 WHERE fk_ledger_id=8;";
        counter++;
        queryList[counter] = "UPDATE acc_student_expn_percent SET fk_ledger_id=8 WHERE fk_ledger_id=9;";
        counter++;
        queryList[counter] = "UPDATE acc_voucher_detail SET fk_ledger_id=7 WHERE fk_ledger_id=8;";
        counter++;
        queryList[counter] = "UPDATE acc_voucher_detail SET fk_ledger_id=8 WHERE fk_ledger_id=9;";
        counter++;
        queryList[counter] = "UPDATE acc_ledger SET id=7 WHERE id=8;";
        counter++;
        queryList[counter] = "UPDATE acc_ledger SET id=8 WHERE id=9;";
        counter++;
        queryList[counter] = "UPDATE `acc_ledger_group` SET `fk_bs_sub_head`=`fk_bs_sub_head`+100;";
        counter++;
        queryList[counter] = "UPDATE `acc_ledger_group` SET `fk_bs_sub_head`=`fk_bs_sub_head`-99;";
        counter++;
        queryList[counter] = "UPDATE `acc_bs_sub_head` SET `id`=id+1000;";
        counter++;
        queryList[counter] = "UPDATE `acc_bs_sub_head` SET `id`=id-999;";

        sql::Statement *stmt;
        sql::Driver *driver;
        sql::Connection *con;
        bool flag = true;
        driver = get_driver_instance();
        con = driver->connect("tcp://" + host_id, db_user, db_password);
        /* Connect to the MySQL test database */
        if (db_name.length() > 0)
        {
            con->setSchema(db_name);
        }
        std::string query = "";
        try
        {
            con->setAutoCommit(false);
            stmt = con->createStatement();
            for (auto &ite : queryList)
            {
                query = ite.second;
                stmt->executeUpdate(query);
            }
            if (flag)
            {
                con->commit();
            }
            else
            {
                con->rollback();
            }
            stmt->close();
            delete stmt;
            con->close();
            delete con;
            driver->threadEnd();
        }
        catch (sql::SQLException &e)
        {
            con->rollback();
        }
        return flag;
    }

    int closeYear()
    {
        int counter = 0;
        std::map<int, std::string> queryList;
        queryList[counter] = "UPDATE acc_scholar_track SET fk_ledger_id=7 WHERE fk_ledger_id=8;";
        counter++;
        queryList[counter] = "UPDATE acc_scholar_track SET fk_ledger_id=8 WHERE fk_ledger_id=9;";
        counter++;
        queryList[counter] = "UPDATE acc_student_expn SET fk_ledger_id=7 WHERE fk_ledger_id=8;";
        counter++;
        queryList[counter] = "UPDATE acc_student_expn SET fk_ledger_id=8 WHERE fk_ledger_id=9;";
        counter++;
        queryList[counter] = "UPDATE acc_student_expn_detail SET fk_ledger_id=7 WHERE fk_ledger_id=8;";
        counter++;
        queryList[counter] = "UPDATE acc_student_expn_detail SET fk_ledger_id=8 WHERE fk_ledger_id=9;";
        counter++;
        queryList[counter] = "UPDATE acc_student_expn_percent SET fk_ledger_id=7 WHERE fk_ledger_id=8;";
        counter++;
        queryList[counter] = "UPDATE acc_student_expn_percent SET fk_ledger_id=8 WHERE fk_ledger_id=9;";
        counter++;
        queryList[counter] = "UPDATE acc_voucher_detail SET fk_ledger_id=7 WHERE fk_ledger_id=8;";
        counter++;
        queryList[counter] = "UPDATE acc_voucher_detail SET fk_ledger_id=8 WHERE fk_ledger_id=9;";
        counter++;
        queryList[counter] = "UPDATE acc_ledger SET id=7 WHERE id=8;";
        counter++;
        queryList[counter] = "UPDATE acc_ledger SET id=8 WHERE id=9;";
        counter++;
        queryList[counter] = "UPDATE `acc_ledger_group` SET `fk_bs_sub_head`=`fk_bs_sub_head`+100;";
        counter++;
        queryList[counter] = "UPDATE `acc_ledger_group` SET `fk_bs_sub_head`=`fk_bs_sub_head`-99;";
        counter++;
        queryList[counter] = "UPDATE `acc_bs_sub_head` SET `id`=id+1000;";
        counter++;
        queryList[counter] = "UPDATE `acc_bs_sub_head` SET `id`=id-999;";

        sql::Statement *stmt;
        sql::Driver *driver;
        sql::Connection *con;
        bool flag = true;
        driver = get_driver_instance();
        con = driver->connect("tcp://" + host_id, db_user, db_password);
        /* Connect to the MySQL test database */
        if (db_name.length() > 0)
        {
            con->setSchema(db_name);
        }
        std::string query = "";
        try
        {
            con->setAutoCommit(false);
            stmt = con->createStatement();
            for (auto &ite : queryList)
            {
                query = ite.second;
                stmt->executeUpdate(query);
            }
            if (flag)
            {
                con->commit();
            }
            else
            {
                con->rollback();
            }
            stmt->close();
            delete stmt;
            con->close();
            delete con;
            driver->threadEnd();
        }
        catch (sql::SQLException &e)
        {
            con->rollback();
        }
        return flag;
    }

    std::string getCsv(std::map<std::string, std::string> arrData)
    {
        std::string myString = "";
        for (auto &iter : arrData)
        {
            myString += iter.second + ",";
        }
        return myString.substr(0, myString.size() - 1);
    }
};