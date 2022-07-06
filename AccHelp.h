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
#include "DB.h"

/**
 *  Class definition
 */
class AccHelp : public Php::Base
{
private:
    /**
     *  @var Php::Value
     */
    Php::Value die;
    DB db;
    Checkup checkup;

public:
    Php::Value from_date;
    Php::Value to_date;
    Php::Value year_id;
    Php::Value ledger;
    std::string temp;
    std::map<std::string, std::string> studentStatus;
    std::map<std::string, std::string> tempDataS1;
    std::map<std::string, std::string> tempDataS2;

    /**
     *  Constructor and destructor
     */
    AccHelp()
    {
        die = "die";
        //        from_date = "";
        //        to_date = "";
        //        year_id = "";
        studentStatus["0"] = "0";
        studentStatus["1"] = "1";
        if (checkup.check() > 0)
        {
            die();
        }
    }

    virtual ~AccHelp()
    {
    }

    void setFrom(Php::Parameters &params)
    {
        from_date = params[0];
    }

    void setTo(Php::Parameters &params)
    {
        to_date = params[0];
    }

    void setYear(Php::Parameters &params)
    {
        year_id = params[0];
    }

    void setLedger(Php::Parameters &params)
    {
        ledger = params[0];
    }

    void setStudentStatus(Php::Parameters &params)
    {
        studentStatus = params[0];
    }

    std::map<std::string, std::map<std::string, std::string>> getTrial()
    {
        std::string yeardata = year_id;
        std::string todata = to_date;
        std::string fromdata = from_date;
        std::string query = "SELECT ";
        query += "ledger_bl.dr,";
        query += "ledger_bl.cr,";
        query += "acc_ledger.name as ledger,";
        query += "acc_ledger.code as ledger_code,";
        query += "acc_ledger.id as ledger_id, ";
        query += "acc_bs_sub_head.fk_bs_head as bs_head ";
        query += "FROM (SELECT fk_ledger_id,SUM(IF(dr_cr=0,amount,0))as dr,SUM(IF(dr_cr=1,amount,0))as cr ";
        query += "FROM acc_voucher_detail ";
        query += "LEFT JOIN acc_voucher ON acc_voucher.id=acc_voucher_detail.fk_voucher_id ";
        query += "WHERE acc_voucher.status=1";
        query += " AND acc_voucher.fk_fiscal_year=" + yeardata;
        query += " AND acc_voucher.v_date>=" + fromdata;
        query += " AND acc_voucher.v_date<" + todata;
        query += " GROUP BY acc_voucher_detail.fk_ledger_id)ledger_bl ";
        query += " LEFT JOIN acc_ledger ON acc_ledger.id=ledger_bl.fk_ledger_id ";
        query += " LEFT JOIN acc_ledger_group ON acc_ledger_group.id=acc_ledger.fk_ledger_group ";
        query += " LEFT JOIN acc_bs_sub_head ON acc_bs_sub_head.id=acc_ledger_group.fk_bs_sub_head; ";
        std::map<std::string, int> column;
        std::map<std::string, std::map<std::string, std::string>> sqlresult;
        column["dr"] = 2;
        column["cr"] = 2;
        column["bs_head"] = 0;
        column["ledger"] = 0;
        column["ledger_code"] = 0;
        column["ledger_id"] = 0;
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column))
        {
            sqlresult = db.getResult();
        }
        return sqlresult;
    }

    std::map<std::string, std::map<std::string, std::string>> getPL()
    {
        //    Php::Value getPL() {
        std::string yeardata = year_id;
        std::string todata = to_date;
        std::string fromdata = from_date;
        std::string query = "SELECT ";
        query += "ledger_bl.dr,";
        query += "ledger_bl.cr,";
        query += "acc_ledger.name as ledger,";
        query += "acc_ledger.code as ledger_code,";
        query += "acc_ledger.id as ledger_id,";
        query += "acc_bs_sub_head.name as group_name,";
        query += "acc_ledger_group.fk_bs_sub_head as sub_head_id ";
        query += "FROM (SELECT fk_ledger_id,SUM(IF(dr_cr=0,amount,0))as dr,SUM(IF(dr_cr=1,amount,0))as cr ";
        query += "FROM acc_voucher_detail ";
        query += "LEFT JOIN acc_voucher ON acc_voucher.id=acc_voucher_detail.fk_voucher_id ";
        query += "WHERE acc_voucher.status=1";
        query += " AND acc_voucher.fk_fiscal_year=" + yeardata;
        query += " AND acc_voucher.v_date>=" + fromdata;
        query += " AND acc_voucher.v_date<" + todata;
        query += " GROUP BY acc_voucher_detail.fk_ledger_id)ledger_bl ";
        query += " LEFT JOIN acc_ledger ON acc_ledger.id=ledger_bl.fk_ledger_id";
        query += " LEFT JOIN acc_ledger_group ON acc_ledger_group.id=acc_ledger.fk_ledger_group ";
        query += " LEFT JOIN acc_bs_sub_head ON acc_bs_sub_head.id=acc_ledger_group.fk_bs_sub_head ";
        query += " WHERE acc_bs_sub_head.type=2";
        std::map<std::string, int> column;
        std::map<std::string, std::map<std::string, std::string>> sqlresult;
        column["dr"] = 2;
        column["cr"] = 2;
        column["ledger"] = 0;
        column["ledger_code"] = 0;
        column["ledger_id"] = 0;
        column["group_name"] = 0;
        column["sub_head_id"] = 0;
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column))
        {
            sqlresult = db.getResult();
        }
        return sqlresult;
    }

    //    std::map<std::string, std::map < std::string, std::string>> getBalance() {

    std::map<std::string, std::map<std::string, std::string>> getBalance()
    {
        std::string yeardata = year_id;
        std::string todata = to_date;
        std::string fromdata = from_date;
        std::string query = "SELECT ";
        query += "ledger_bl.dr,";
        query += "ledger_bl.cr,";
        query += "acc_ledger_group.id as ledger_group_id,";
        query += "acc_ledger_group.name as ledger_group_name,";
        query += "acc_bs_sub_head.id as bs_sub_id,";
        query += "acc_bs_sub_head.name as bs_sub_head,";
        query += "acc_bs_sub_head.type as bs_sub_head_type,";
        query += "acc_bs_head.dr_cr as bs_dr_cr ";
        query += "FROM (SELECT fk_ledger_id,SUM(IF(dr_cr=0,amount,0))as dr,SUM(IF(dr_cr=1,amount,0))as cr ";
        query += "FROM acc_voucher_detail ";
        query += "LEFT JOIN acc_voucher ON acc_voucher.id=acc_voucher_detail.fk_voucher_id ";
        query += "WHERE acc_voucher.status=1";
        query += " AND acc_voucher.fk_fiscal_year=" + yeardata;
        query += " AND acc_voucher.v_date>=" + fromdata;
        query += " AND acc_voucher.v_date<" + todata;
        query += " GROUP BY acc_voucher_detail.fk_ledger_id)ledger_bl ";
        query += " LEFT JOIN acc_ledger ON acc_ledger.id=ledger_bl.fk_ledger_id";
        query += " LEFT JOIN acc_ledger_group ON acc_ledger_group.id=acc_ledger.fk_ledger_group ";
        query += " LEFT JOIN acc_bs_sub_head ON acc_bs_sub_head.id=acc_ledger_group.fk_bs_sub_head ";
        query += " LEFT JOIN acc_bs_head ON acc_bs_head.id=acc_bs_sub_head.fk_bs_head ";
        std::map<std::string, int> column;
        std::map<std::string, std::map<std::string, std::string>> sqlresult;
        column["dr"] = 2;
        column["cr"] = 2;
        column["ledger_group_id"] = 0;
        column["ledger_group_name"] = 0;
        column["bs_sub_id"] = 0;
        column["bs_sub_head"] = 0;
        column["bs_sub_head_type"] = 0;
        column["bs_dr_cr"] = 0;
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column))
        {
            sqlresult = db.getResult();
        }
        return sqlresult;
    }

    Php::Value getOpeningLedger()
    {
        std::string yeardata = year_id;
        std::string fromdata = from_date;
        std::string ledgerdata = ledger;
        std::string query = "SELECT SUM(IF(dr_cr=0,amount,0))as dr,SUM(IF(dr_cr=1,amount,0))as cr ";
        query += "FROM acc_voucher_detail ";
        query += "LEFT JOIN acc_voucher ON acc_voucher.id=acc_voucher_detail.fk_voucher_id ";
        query += "WHERE acc_voucher.status=1";
        query += " AND acc_voucher_detail.fk_ledger_id=" + ledgerdata;
        query += " AND acc_voucher.fk_fiscal_year=" + yeardata;
        query += " AND acc_voucher.v_date<" + fromdata;
        query += " GROUP BY acc_voucher_detail.fk_ledger_id";
        std::map<std::string, int> column;
        std::map<std::string, std::map<std::string, std::string>> sqlresult;
        column["dr"] = 2;
        column["cr"] = 2;
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column))
        {
            sqlresult = db.getResult();
        }
        if (sqlresult.size() > 0)
        {
            return std::stod(sqlresult["100000"]["dr"]) - std::stod(sqlresult["100000"]["cr"]);
        }
        return 0;
    }

    Php::Value getLedger()
    {
        std::string yeardata = year_id;
        std::string todata = to_date;
        std::string fromdata = from_date;
        std::string ledgerdata = ledger;
        std::string query = "SELECT ";
        query += "acc_voucher.id as jno,";
        query += "acc_voucher_detail.dr_cr,";
        query += "acc_voucher_detail.amount,";
        query += "acc_voucher.v_date,";
        query += "acc_voucher_detail.narration as ledger,";
        query += "acc_voucher.narration as narration,";
        query += "acc_ledger.id as ledger_id,";
        query += "acc_voucher_to_student.fk_student_id as student_id ";
        query += "FROM acc_voucher_detail ";
        query += "LEFT JOIN acc_voucher ON acc_voucher.id=acc_voucher_detail.fk_voucher_id ";
        query += "LEFT JOIN acc_voucher_to_student ON acc_voucher_to_student.fk_voucher_id=acc_voucher.id ";
        query += "LEFT JOIN acc_ledger ON acc_ledger.id=acc_voucher_detail.fk_ledger_id ";
        query += "WHERE acc_voucher.fk_fiscal_year=" + yeardata;
        query += " AND acc_voucher.status=1";
        query += " AND acc_voucher_detail.fk_ledger_id=" + ledgerdata;
        query += " AND acc_voucher.v_date>=" + fromdata;
        query += " AND acc_voucher.v_date<" + todata;
        query += " ORDER BY v_date ASC,acc_voucher_detail.fk_voucher_id,acc_voucher_detail.serial_no ASC";
        //        return query;
        std::map<std::string, int> column;
        std::map<std::string, std::map<std::string, std::string>> sqlresult;
        column["jno"] = 0;
        column["dr_cr"] = 0;
        column["amount"] = 2;
        column["v_date"] = 0;
        column["ledger"] = 0;
        column["narration"] = 0;
        column["ledger_id"] = 0;
        column["student_id"] = 0;
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column))
        {
            sqlresult = db.getResult();
        }
        return sqlresult;
    }

    Php::Value getScholarTrack(Php::Parameters &params)
    {
        std::map<std::string, std::string> ladgers = params[0];
        std::map<std::string, std::string> typelist = params[1];
        std::string yeardata = year_id;
        std::string todata = to_date;
        std::string fromdata = from_date;

        std::string query = "SELECT ";
        query += "acc_voucher_to_student.fk_student_id as student_id,";
        query += "acc_fee_period.name as month,";
        query += "aca_period.code as class,";
        query += "acc_voucher.id as voucher_id,";
        query += "acc_voucher_detail.dr_cr,";
        query += "acc_voucher_detail.fk_ledger_id,";
        query += "acc_voucher_detail.amount,";
        query += "acc_voucher.v_date,";
        query += "acc_voucher.narration,";
        query += "student.first_name,student.middle_name,student.last_name,";
        query += "student.regid,acc_scholar_track.type as sc_type,acc_voucher_to_student.typ as vst ";
        query += "FROM acc_scholar_track ";
        query += "LEFT JOIN acc_fee_period ON acc_fee_period.id=acc_scholar_track.fk_month_id ";
        query += "LEFT JOIN acc_voucher ON acc_voucher.id=acc_scholar_track.fk_voucher_id ";
        query += "LEFT JOIN acc_voucher_detail ON acc_voucher.id=acc_voucher_detail.fk_voucher_id ";
        query += "LEFT JOIN acc_voucher_to_student ON acc_voucher.id=acc_voucher_to_student.fk_voucher_id ";
        query += "LEFT JOIN aca_program_group ON aca_program_group.id=acc_voucher_to_student.fk_program_group ";
        query += "LEFT JOIN aca_period ON aca_period.id=aca_program_group.fk_period_id ";
        query += "LEFT JOIN student ON student.id=acc_scholar_track.fk_student_id ";
        query += "LEFT JOIN acc_ledger ON acc_ledger.id=acc_voucher_detail.fk_ledger_id ";
        query += "WHERE acc_voucher.status=1";
        query += " AND acc_voucher_detail.fk_ledger_id=acc_scholar_track.fk_ledger_id";
        query += " AND acc_scholar_track.fk_ledger_id IN (" + getCsv(ladgers) + ")";
        query += " AND acc_voucher.v_date>=" + fromdata;
        query += " AND acc_voucher.v_date<" + todata;
        if (typelist.size() > 0)
        {
            query += " AND acc_scholar_track.type IN (" + db.getCsv(typelist) + ") ";
        }
        query += " ORDER BY acc_voucher.v_date ASC";
        std::map<std::string, int> column;
        std::map<std::string, std::map<std::string, std::string>> sqlresult;
        column["student_id"] = 0;
        column["month"] = 0;
        column["amount"] = 2;
        column["class"] = 0;
        column["voucher_id"] = 0;
        column["fk_ledger_id"] = 0;
        column["dr_cr"] = 0;
        column["v_date"] = 0;
        column["narration"] = 0;
        column["first_name"] = 0;
        column["middle_name"] = 0;
        column["last_name"] = 0;
        column["regid"] = 0;
        column["sc_type"] = 0;
        column["vst"] = 0;
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column))
        {
            sqlresult = db.getResult();
        }
        return sqlresult;
    }
    Php::Value getScholarReport(Php::Parameters &params)
    {
        std::map<std::string, std::string> ladgers = params[0];
        std::string yeardata = year_id;
        std::string todata = to_date;
        std::string fromdata = from_date;

        std::string query = "SELECT ";
        query += "acc_voucher_to_student.fk_student_id as student_id,";
        query += "acc_fee_period.name as month,";
        query += "aca_period.code as class,";
        query += "acc_voucher.id as voucher_id,";
        query += "acc_voucher_detail.dr_cr,";
        query += "acc_voucher_detail.fk_ledger_id,";
        query += "acc_voucher_detail.amount,";
        query += "acc_voucher.v_date,";
        query += "acc_voucher.narration,";
        query += "student.first_name,student.middle_name,student.last_name,";
        query += "student.regid,acc_voucher_to_student.typ as vst ";
        query += "FROM acc_voucher_to_student ";
        query += "LEFT JOIN acc_fee_period ON acc_fee_period.id=acc_voucher_to_student.fk_period_id ";
        query += "LEFT JOIN aca_program_group ON aca_program_group.id=acc_voucher_to_student.fk_program_group ";
        query += "LEFT JOIN aca_period ON aca_period.id=aca_program_group.fk_period_id ";
        query += "LEFT JOIN student ON student.id=acc_voucher_to_student.fk_student_id ";
        query += "LEFT JOIN acc_voucher ON acc_voucher.id=acc_voucher_to_student.fk_voucher_id ";
        query += "LEFT JOIN acc_voucher_detail ON acc_voucher.id=acc_voucher_detail.fk_voucher_id ";
        query += "RIGHT JOIN acc_ledger ON acc_ledger.id=acc_voucher_detail.fk_ledger_id ";
        query += "WHERE acc_voucher.status=1";
        query += " AND acc_voucher_detail.dr_cr=0";
        query += " AND acc_voucher_detail.fk_ledger_id IN (" + getCsv(ladgers) + ")";
        query += " AND acc_voucher.v_date>=" + fromdata;
        query += " AND acc_voucher.v_date<" + todata;
        query += " ORDER BY acc_voucher.v_date ASC";
        std::map<std::string, int> column;
        std::map<std::string, std::map<std::string, std::string>> sqlresult;
        column["student_id"] = 0;
        column["month"] = 0;
        column["amount"] = 2;
        column["class"] = 0;
        column["voucher_id"] = 0;
        column["fk_ledger_id"] = 0;
        column["dr_cr"] = 0;
        column["v_date"] = 0;
        column["narration"] = 0;
        column["first_name"] = 0;
        column["middle_name"] = 0;
        column["last_name"] = 0;
        column["regid"] = 0;
        column["vst"] = 0;
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column))
        {
            sqlresult = db.getResult();
        }
        return sqlresult;
    }

    Php::Value getBillCollectReport()
    {
        std::string yeardata = year_id;
        std::string todata = to_date;
        std::string fromdata = from_date;

        std::string query = "SELECT ";
        query += "hrm_employee.name as emp_name,";
        query += "aca_period.code as period_code,";
        query += "acc_voucher.v_date,";
        query += "IF(student.id IS NULL,acc_invoice.customer_name,IF(student.middle_name IS NULL OR student.middle_name='',CONCAT(student.first_name,' ',student.last_name),CONCAT(student.first_name,' ',student.middle_name,' ',student.last_name))) as student_name,";
        query += "student.regid,";
        query += "aca_section.name as std_sec,";
        query += "IF(student.id IS NULL,CONCAT('bill',acc_invoice.id),student.id) as std_id,";
        query += "acc_invoice_items.fk_ledger_id as fk_ledger_id,";
        query += "acc_invoice_items.item_amount as amount,";
        query += "acc_invoice_items.item as item,";
        query += "acc_invoice.id as invoice_id,";
        query += "acc_invoice.p_type as pt,";
        query += "acc_invoice.bill_no as bill_no,";
        query += "acc_invoice.tax_amount as tax_amount,";
        query += "acc_invoice.discount as discount,";
        query += "acc_invoice.total_amount as total_amount ";
        query += "FROM acc_invoice ";
        query += "LEFT JOIN acc_invoice_items ON acc_invoice.id=acc_invoice_items.invoice_id ";
        query += "LEFT JOIN hrm_employee ON hrm_employee.id=acc_invoice.fk_emp_id ";
        query += "LEFT JOIN acc_voucher ON acc_voucher.id=acc_invoice.fk_voucher_id ";
        query += "LEFT JOIN acc_voucher_to_student ON acc_voucher_to_student.fk_voucher_id=acc_invoice.fk_voucher_id ";
        query += "LEFT JOIN student_to_aca ON (student_to_aca.fk_student_id=acc_voucher_to_student.fk_student_id ";
        query += "AND student_to_aca.status IN (" + getCsv(studentStatus) + ") ";
        query += "AND student_to_aca.fk_program_group_id=acc_voucher_to_student.fk_program_group) ";
        query += "LEFT JOIN aca_period ON aca_period.id=student_to_aca.fk_period_id ";
        query += "LEFT JOIN aca_section ON aca_section.id=student_to_aca.fk_section_id ";
        query += "LEFT JOIN student ON student.id=acc_voucher_to_student.fk_student_id ";
        query += "WHERE acc_voucher.fk_fiscal_year=" + yeardata;
        query += " AND acc_voucher.status=1 ";
        query += " AND acc_voucher.v_date>=" + fromdata;
        query += " AND acc_voucher.v_date<" + todata;
        query += " ORDER BY student_name ASC,fk_ledger_id ASC";

        std::map<std::string, int> column;
        std::map<std::string, std::map<std::string, std::string>> sqlresult;
        column["emp_name"] = 0;
        column["period_code"] = 0;
        column["v_date"] = 1;
        column["student_name"] = 0;
        column["regid"] = 0;
        column["std_sec"] = 0;
        column["std_id"] = 0;
        column["fk_ledger_id"] = 0;
        column["amount"] = 2;
        column["item"] = 0;
        column["invoice_id"] = 0;
        column["pt"] = 0;
        column["bill_no"] = 0;
        column["tax_amount"] = 2;
        column["discount"] = 2;
        column["total_amount"] = 2;

        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column))
        {
            sqlresult = db.getResult();
        }
        return sqlresult;
    }

    Php::Value checkDep(Php::Parameters &params)
    {
        std::map<std::string, std::string> studentid = params[0];
        std::string programg = params[1];
        std::string ledger = params[2];
        std::string period = params[3];
        std::string starttime = params[4];
        std::string feetype = params[5];
        std::string query = "SELECT acc_voucher_to_student.fk_student_id,";
        query += "IF(student.middle_name='',CONCAT(student.first_name,' ',student.last_name),CONCAT(student.first_name,' ',student.middle_name,' ',student.last_name)) as student_name,";
        query += "student.regid,";
        query += "acc_voucher_detail.amount,";
        query += "acc_voucher.narration,";
        query += "acc_voucher.create_date,";
        query += "acc_voucher.fk_posted_by,";
        query += "acc_voucher.id as lfno,acc_voucher_to_student.typ as vst ";
        query += "FROM acc_voucher_to_student ";
        query += "LEFT JOIN acc_voucher ON acc_voucher.id=acc_voucher_to_student.fk_voucher_id ";
        query += "LEFT JOIN acc_voucher_detail ON acc_voucher.id=acc_voucher_detail.fk_voucher_id ";
        query += "LEFT JOIN student ON student.id=acc_voucher_to_student.fk_student_id ";
        query += "WHERE acc_voucher.status=1 ";
        query += "AND acc_voucher_to_student.fk_student_id IN (" + db.getCsv(studentid) + ") ";
        query += " AND acc_voucher_to_student.fk_program_group=" + programg;
        query += " AND acc_voucher_detail.fk_ledger_id=" + ledger;
        if (feetype != "0")
        {
            query += " AND acc_voucher_to_student.fk_period_id=" + period;
        }
        if (starttime.length() > 0)
        {
            query += " AND acc_voucher.v_date>" + starttime;
        }
        query += " ORDER BY regid";
        std::map<std::string, int> column;
        std::map<std::string, std::map<std::string, std::string>> sqlresult;
        column["fk_student_id"] = 0;
        column["student_name"] = 0;
        column["regid"] = 0;
        column["amount"] = 2;
        column["narration"] = 0;
        column["fk_posted_by"] = 0;
        column["create_date"] = 0;
        column["lfno"] = 0;
        column["vst"] = 0;

        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column))
        {
            sqlresult = db.getResult();
        }
        if (params.size() == 6)
        {
            return sqlresult;
        }
        std::map<std::string, std::string> voucherToDelete;
        std::map<std::string, std::map<std::string, std::string>> voucherList;
        std::map<std::string, std::string> voucherTmp;
        for (auto &iter : sqlresult)
        {
            voucherList[iter.second["fk_posted_by"]][iter.second["create_date"]] = iter.second["fk_posted_by"];
        }
        column.clear();
        sqlresult.clear();
        column["id"] = 0;
        std::string counterSt = "0";
        int counterIn = 0;
        for (auto &pby : voucherList)
        {
            voucherTmp = pby.second;
            for (auto &crdate : voucherTmp)
            {
                query = "SELECT id FROM acc_voucher WHERE status=1 AND create_date=" + crdate.first + " AND fk_posted_by=" + pby.first;
                if (db.query(query, column))
                {
                    sqlresult = db.getResult();
                }
                for (auto &iter : sqlresult)
                {
                    voucherToDelete[counterSt] = iter.second["id"];
                    counterIn++;
                    counterSt = std::to_string(counterIn);
                }
                sqlresult.clear();
            }
        }
        return voucherToDelete;
    }

    std::map<std::string, std::map<std::string, std::string>> getBsReport()
    {
        std::string yeardata = year_id;
        std::string todata = to_date;
        std::string fromdata = from_date;
        std::string ledger_id = ledger;
        std::string query = "SELECT ";
        query += "acc_voucher_detail.dr_cr,";
        query += "acc_voucher_detail.amount,";
        query += "acc_voucher.v_date,";
        query += "acc_ledger.name as ledger,";
        query += "acc_ledger.code as ledger_code,";
        query += "acc_ledger.id as ledger_id,";
        query += "acc_ledger_group.id as ledger_group_id,";
        query += "acc_ledger_group.name as ledger_group_name,";
        query += "acc_bs_sub_head.id as bs_sub_id,";
        query += "acc_bs_sub_head.name as bs_sub_head,";
        query += "acc_bs_head.dr_cr as bs_dr_cr ";
        query += "FROM acc_voucher ";
        query += "LEFT JOIN acc_voucher_detail ON acc_voucher.id=acc_voucher_detail.fk_voucher_id ";
        query += "LEFT JOIN acc_ledger ON acc_ledger.id=acc_voucher_detail.fk_ledger_id ";
        query += "LEFT JOIN acc_ledger_group ON acc_ledger_group.id=acc_ledger.fk_ledger_group ";
        query += "LEFT JOIN acc_bs_sub_head ON acc_bs_sub_head.id=acc_ledger_group.fk_bs_sub_head ";
        query += "LEFT JOIN acc_bs_head ON acc_bs_head.id=acc_bs_sub_head.fk_bs_head ";
        query += "WHERE acc_voucher.fk_fiscal_year=" + yeardata;
        query += " AND acc_voucher.status=1 ";
        query += " AND acc_ledger_group.id=" + ledger_id;
        query += " AND acc_voucher.v_date>=" + fromdata;
        query += " AND acc_voucher.v_date<" + todata;
        query += " ORDER BY acc_ledger_group.fk_bs_sub_head,acc_voucher_detail.id,v_date ASC,acc_voucher_detail.fk_voucher_id,acc_voucher_detail.serial_no ASC";
        std::map<std::string, int> column;
        std::map<std::string, std::map<std::string, std::string>> sqlresult;
        column["dr_cr"] = 1;
        column["amount"] = 2;
        column["ledger"] = 0;
        column["v_date"] = 0;
        column["ledger_code"] = 0;
        column["ledger_id"] = 0;
        column["ledger_group_id"] = 0;
        column["ledger_group_name"] = 0;
        column["bs_sub_id"] = 0;
        column["bs_sub_head"] = 0;
        column["bs_dr_cr"] = 0;

        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column))
        {
            sqlresult = db.getResult();
        }
        return sqlresult;
    }

    Php::Value getClassReport(Php::Parameters &params)
    {
        std::string yeardata = year_id;
        std::string todata = to_date;
        std::map<std::string, std::string> studentid = params[0];
        std::string query = "SELECT ";
        query += "aca_period.code as period_code,";
        query += "acc_voucher_detail.id as v_id,";
        query += "acc_voucher_detail.dr_cr,";
        query += "acc_voucher_detail.fk_ledger_id,";
        query += "acc_voucher_detail.amount,";
        query += "acc_voucher.v_date,";
        query += "student.first_name,student.middle_name,student.last_name,";
        query += "student.regid,aca_batch.name as batch,";
        query += "student.id as std_id,";
        query += "aca_section.name as std_sec,";
        query += "student_to_aca.id as stu_aca_id,";
        query += "student_to_aca.status as student_status,";
        query += "acc_ledger.name as ledger_name,acc_voucher_to_student.typ as vst ";
        query += "FROM acc_voucher_to_student ";
        query += "LEFT JOIN student_to_aca ON (student_to_aca.fk_student_id=acc_voucher_to_student.fk_student_id ";
        query += "AND student_to_aca.status IN (" + db.getCsv(studentStatus) + ")) ";
        query += "LEFT JOIN aca_period ON aca_period.id=student_to_aca.fk_period_id ";
        query += "LEFT JOIN aca_section ON aca_section.id=student_to_aca.fk_section_id ";
        query += "LEFT JOIN student ON student.id=acc_voucher_to_student.fk_student_id ";
        query += "LEFT JOIN acc_voucher ON acc_voucher.id=acc_voucher_to_student.fk_voucher_id ";
        query += "LEFT JOIN acc_voucher_detail ON acc_voucher.id=acc_voucher_detail.fk_voucher_id ";
        query += "LEFT JOIN acc_ledger ON acc_ledger.id=acc_voucher_detail.fk_ledger_id ";
        query += "LEFT JOIN aca_batch ON aca_batch.id=student_to_aca.fk_batch_id ";
        query += "WHERE acc_voucher_to_student.fk_student_id IN (" + db.getCsv(studentid) + ") ";
        query += "AND acc_voucher.status=1 ";
        query += "AND acc_voucher.fk_fiscal_year=" + yeardata;
        query += " AND acc_voucher_detail.fk_ledger_id!=1 ";
        query += "AND acc_voucher.v_date<" + todata;
        query += " ORDER BY aca_period.serial_no,student.id,v_date ASC,acc_voucher_detail.serial_no ASC";
        // return query;
        std::map<std::string, int> column;
        std::map<std::string, std::map<std::string, std::string>> sqlresult;
        column["period_code"] = 0;
        column["v_id"] = 0;
        column["dr_cr"] = 1;
        column["amount"] = 2;
        column["fk_ledger_id"] = 0;
        column["v_date"] = 0;
        column["first_name"] = 0;
        column["middle_name"] = 0;
        column["last_name"] = 0;
        column["regid"] = 0;
        column["std_id"] = 0;
        column["std_sec"] = 0;
        column["stu_aca_id"] = 0;
        column["batch"] = 0;
        column["student_status"] = 0;
        column["ledger_name"] = 0;
        column["vst"] = 0;

        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column))
        {
            sqlresult = db.getResult();
        }
        std::map<std::string, std::map<std::string, std::string>> pssresult;
        std::map<std::string, std::string> ite;
        int counter = 100000;
        for (auto &sl : sqlresult)
        {
            if (sl.second["fk_ledger_id"] == "15" && sl.second["dr_cr"] == "0")
            {
                continue;
            }
            ite = sl.second;
            if (sl.second["fk_ledger_id"] == "15" && sl.second["dr_cr"] == "1")
            {
                ite["dr_cr"] = "0";
            }
            pssresult[std::to_string(counter)] = ite;
            counter++;
        }
        return pssresult;
    }

    Php::Value searchLedger(Php::Parameters &params)
    {
        std::string key = params[0];
        std::map<std::string, std::string> tylelist = params[1];
        std::string query = "SELECT ";
        query += "acc_ledger.id as id,";
        query += "acc_ledger.name as ledger,";
        query += "acc_ledger.code as ledger_code,";
        query += "acc_ledger_group.name as ledger_group,";
        query += "acc_bs_sub_head.name as bs_sub_head,";
        query += "acc_bs_head.name as bs_head ";
        query += "FROM acc_ledger ";
        query += "LEFT JOIN acc_ledger_group ON acc_ledger_group.id=acc_ledger.fk_ledger_group ";
        query += "LEFT JOIN acc_bs_sub_head ON acc_bs_sub_head.id=acc_ledger_group.fk_bs_sub_head ";
        query += "LEFT JOIN acc_bs_head ON acc_bs_head.id=acc_bs_sub_head.fk_bs_head ";
        query += "WHERE acc_bs_sub_head.type IN (" + db.getCsv(tylelist) + ") ";
        query += "AND (acc_ledger.name LIKE '%" + key + "%' ";
        query += "OR acc_ledger.code LIKE '%" + key + "%')";
        std::map<std::string, int> column;
        std::map<std::string, std::map<std::string, std::string>> sqlresult;
        column["id"] = 0;
        column["ledger"] = 0;
        column["ledger_code"] = 1;
        column["ledger_group"] = 1;
        column["bs_sub_head"] = 0;
        column["bs_head"] = 0;

        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column))
        {
            sqlresult = db.getResult();
        }
        return sqlresult;
    }

    Php::Value getStudentLedger(Php::Parameters &params)
    {
        std::string yeardata = year_id;
        std::string key = params[0];
        std::string query = "SELECT ";
        query += "acc_voucher.id as jno,";
        query += "acc_fee_period.name as period_name,";
        query += "acc_voucher_detail.dr_cr,";
        query += "acc_voucher_detail.amount,";
        query += "acc_voucher.v_date,";
        query += "acc_ledger.name as ledger,";
        query += "acc_ledger.id as ledger_id,";
        query += "acc_invoice.id as invoice_id,acc_voucher_to_student.typ as vst ";
        query += "FROM acc_voucher_to_student ";
        query += "LEFT JOIN acc_fee_period ON acc_fee_period.id=acc_voucher_to_student.fk_period_id ";
        query += "LEFT JOIN acc_voucher ON acc_voucher.id=acc_voucher_to_student.fk_voucher_id ";
        query += "LEFT JOIN acc_invoice ON acc_voucher.id=acc_invoice.fk_voucher_id ";
        query += "LEFT JOIN acc_voucher_detail ON acc_voucher.id=acc_voucher_detail.fk_voucher_id ";
        query += "LEFT JOIN acc_ledger ON acc_ledger.id=acc_voucher_detail.fk_ledger_id ";
        query += "WHERE acc_voucher.fk_fiscal_year=" + yeardata;
        query += " AND acc_voucher.status=1";
        query += " AND acc_voucher_detail.fk_ledger_id!=1";
        query += " AND acc_voucher_to_student.fk_student_id=" + key;
        query += " ORDER BY acc_voucher.v_date ASC,acc_voucher.id ASC,acc_voucher_detail.serial_no ASC";
        std::map<std::string, int> column;
        std::map<std::string, std::map<std::string, std::string>> sqlresult;
        column["jno"] = 0;
        column["period_name"] = 0;
        column["dr_cr"] = 0;
        column["amount"] = 0;
        column["v_date"] = 0;
        column["ledger"] = 0;
        column["ledger_id"] = 0;
        column["invoice_id"] = 0;
        column["vst"] = 0;

        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column))
        {
            sqlresult = db.getResult();
        }
        std::map<std::string, std::map<std::string, std::string>> pssresult;
        std::map<std::string, std::string> ite;
        std::map<std::string, std::map<std::string, std::string>> sqlresult2;
        column.clear();
        column["is_closed"] = 0;
        query = "SELECT is_closed FROM acc_year WHERE id=" + yeardata;
        if (db.query(query, column))
        {
            sqlresult2 = db.getResult();
        }
        int counter = 100000;
        std::string currentYear = sqlresult2["100000"]["is_closed"];
        for (auto &sl : sqlresult)
        {
            ite = sl.second;
            if (sl.second["ledger_id"] == "15" && sl.second["dr_cr"] == "0")
            {
                continue;
            }

            // if (sl.second["ledger_id"] == "15" && sl.second["dr_cr"] == "1" && currentYear == "0")
            if (sl.second["ledger_id"] == "15" && sl.second["dr_cr"] == "1")
            {
                ite["dr_cr"] = "0";
            }

            pssresult[std::to_string(counter)] = ite;
            counter++;
        }
        return pssresult;
    }

    std::map<std::string, std::map<std::string, std::string>> getEmployeeLedger()
    {
        std::string yeardata = year_id;
        std::string empid = ledger;
        std::string query = "SELECT ";
        query += "acc_payroll_post.type as post_type,";
        query += "acc_payroll_post.group_id as post_group,";
        query += "acc_fee_period.name as month,";
        query += "acc_voucher.v_date as voucher_date,";
        query += "acc_voucher.id as jno,";
        query += "acc_voucher_detail.fk_ledger_id as ledger,";
        query += "acc_ledger.name as ledger_name,";
        query += "acc_ledger.code as ledger_code,";
        query += "acc_voucher_detail.dr_cr,";
        query += "acc_voucher_detail.amount,";
        query += "acc_voucher_detail.narration,";
        query += "acc_voucher.v_date ";
        query += "FROM acc_payroll_post ";
        query += "LEFT JOIN acc_fee_period ON acc_fee_period.id=acc_payroll_post.fk_period_id ";
        query += "LEFT JOIN acc_voucher ON acc_voucher.id=acc_payroll_post.fk_voucher_id ";
        query += "LEFT JOIN acc_voucher_detail ON acc_voucher.id=acc_voucher_detail.fk_voucher_id ";
        query += "LEFT JOIN acc_ledger ON acc_ledger.id=acc_voucher_detail.fk_ledger_id ";
        query += "WHERE acc_voucher.fk_fiscal_year=" + yeardata;
        query += " AND acc_payroll_post.fk_emp_id=" + empid;
        query += " AND acc_voucher.status=1 ";
        query += " ORDER BY acc_voucher.v_date ASC,acc_voucher.id ASC";
        std::map<std::string, int> column;
        std::map<std::string, std::map<std::string, std::string>> sqlresult;
        column["post_type"] = 0;
        column["post_group"] = 0;
        column["month"] = 0;
        column["voucher_date"] = 0;
        column["jno"] = 0;
        column["ledger"] = 0;
        column["ledger_name"] = 0;
        column["ledger_code"] = 0;
        column["dr_cr"] = 0;
        column["amount"] = 2;
        column["narration"] = 0;
        column["v_date"] = 0;

        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column))
        {
            sqlresult = db.getResult();
        }
        return sqlresult;
    }

    std::map<std::string, std::map<std::string, std::string>> getEmployeeAdvanceLedger()
    {
        std::string yeardata = year_id;
        std::string query = "SELECT ";
        query += "acc_payroll_post.fk_emp_id as emp_id,";
        query += "acc_payroll_post.type as post_type,";
        query += "acc_payroll_post.group_id as post_group,";
        query += "acc_fee_period.name as month,";
        query += "acc_voucher.id as jno,";
        query += "acc_voucher_detail.fk_ledger_id as ledger,";
        query += "acc_ledger.name as ledger_name,";
        query += "acc_ledger.code as ledger_code,";
        query += "acc_voucher_detail.dr_cr,";
        query += "acc_voucher_detail.amount,";
        query += "acc_voucher_detail.narration,";
        query += "acc_voucher.v_date ";
        query += "FROM acc_payroll_post ";
        query += "LEFT JOIN acc_fee_period ON acc_fee_period.id=acc_payroll_post.fk_period_id ";
        query += "LEFT JOIN acc_voucher ON acc_voucher.id=acc_payroll_post.fk_voucher_id ";
        query += "LEFT JOIN acc_voucher_detail ON acc_voucher.id=acc_voucher_detail.fk_voucher_id ";
        query += "LEFT JOIN acc_ledger ON acc_ledger.id=acc_voucher_detail.fk_ledger_id ";
        query += "WHERE acc_payroll_post.type=3 ";
        query += " AND acc_voucher.fk_fiscal_year=" + yeardata;
        query += " AND acc_payroll_post.fk_emp_id IN (" + db.getCsv(tempDataS1) + ") ";
        query += "AND acc_voucher.status=1 ";
        query += "ORDER BY acc_voucher.v_date ASC,acc_voucher.id ASC";
        std::map<std::string, int> column;
        std::map<std::string, std::map<std::string, std::string>> sqlresult;
        column["emp_id"] = 0;
        column["post_type"] = 0;
        column["post_group"] = 0;
        column["month"] = 0;
        column["jno"] = 0;
        column["ledger"] = 0;
        column["ledger_name"] = 0;
        column["ledger_code"] = 0;
        column["dr_cr"] = 0;
        column["amount"] = 2;
        column["narration"] = 0;
        column["v_date"] = 0;
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column))
        {
            sqlresult = db.getResult();
        }
        return sqlresult;
    }

    std::map<std::string, std::map<std::string, std::string>> getEmployeeLedgerTable()
    {
        std::string yeardata = year_id;
        std::string query = "SELECT ";
        query += "acc_payroll_post.fk_emp_id as emp_id,";
        query += "acc_payroll_post.type as post_type,";
        query += "acc_payroll_post.group_id as post_group,";
        query += "acc_fee_period.name as month,";
        query += "acc_payroll_post.fk_period_id as month_id,";
        //        query += "acc_voucher.v_date as voucher_date,";
        query += "acc_voucher.id as jno,";
        query += "acc_voucher_detail.fk_ledger_id as ledger,";
        query += "acc_ledger.name as ledger_name,";
        query += "acc_ledger.code as ledger_code,";
        query += "acc_voucher_detail.dr_cr,";
        query += "acc_voucher_detail.amount,";
        query += "acc_voucher_detail.narration,";
        query += "acc_voucher.v_date,";
        query += "acc_voucher.type as v_type ";
        query += "FROM acc_payroll_post ";
        query += "LEFT JOIN acc_fee_period ON acc_fee_period.id=acc_payroll_post.fk_period_id ";
        query += "LEFT JOIN acc_voucher ON acc_voucher.id=acc_payroll_post.fk_voucher_id ";
        query += "LEFT JOIN acc_voucher_detail ON acc_voucher.id=acc_voucher_detail.fk_voucher_id ";
        query += "LEFT JOIN acc_ledger ON acc_ledger.id=acc_voucher_detail.fk_ledger_id ";
        query += "WHERE acc_voucher.fk_fiscal_year=" + yeardata;
        if (tempDataS2.size() > 0)
        {
            query += " AND acc_payroll_post.fk_period_id IN (" + db.getCsv(tempDataS2) + ") ";
        }
        query += " AND acc_payroll_post.fk_emp_id IN (" + db.getCsv(tempDataS1) + ") ";
        query += "AND acc_voucher.status=1 ";
        query += "ORDER BY acc_voucher.v_date ASC,acc_voucher.id ASC";
        std::map<std::string, int> column;
        std::map<std::string, std::map<std::string, std::string>> sqlresult;
        column["emp_id"] = 0;
        column["post_type"] = 0;
        column["post_group"] = 0;
        column["month"] = 0;
        column["jno"] = 0;
        column["ledger"] = 0;
        column["ledger_name"] = 0;
        column["ledger_code"] = 0;
        column["dr_cr"] = 0;
        column["amount"] = 2;
        column["narration"] = 0;
        column["v_date"] = 0;
        column["v_type"] = 0;
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column))
        {
            sqlresult = db.getResult();
        }
        return sqlresult;
    }

    std::map<std::string, std::map<std::string, std::string>> getStdAdvance(std::map<std::string, std::string> stdid)
    {
        std::string yeardata = year_id;
        std::string query = "SELECT ";
        query += "acc_voucher_to_student.fk_student_id,";
        query += "acc_voucher_detail.dr_cr,";
        query += "acc_voucher_detail.amount ";
        query += " FROM acc_voucher_detail";
        query += " LEFT JOIN acc_voucher_to_student ON acc_voucher_detail.fk_voucher_id=acc_voucher_to_student.fk_voucher_id ";
        query += " LEFT JOIN acc_voucher ON acc_voucher.id=acc_voucher_detail.fk_voucher_id ";
        query += " WHERE acc_voucher.status=1 ";
        query += " AND acc_voucher.fk_fiscal_year=" + yeardata;
        query += " AND acc_voucher_to_student.fk_student_id IN (" + db.getCsv(stdid) + ")";
        query += " AND acc_voucher_detail.fk_ledger_id=15";
        std::map<std::string, int> column;
        std::map<std::string, std::map<std::string, std::string>> sqlresult;
        column["fk_student_id"] = 0;
        column["dr_cr"] = 0;
        column["amount"] = 2;
        column["vst"] = 0;
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column))
        {
            sqlresult = db.getResult();
        }
        return sqlresult;
    }

    Php::Value getPayrollFee(Php::Parameters &params)
    {
        std::string yeardata = year_id;
        std::string monthdat = params[0];
        std::string query = "SELECT ";
        query += "tbps.fk_voucher_id,";
        query += "student.id as student_id,";
        query += "student.regid,";
        query += "IF(student.middle_name='',CONCAT(student.first_name,' ',student.last_name),CONCAT(student.first_name,' ',student.middle_name,' ',student.last_name)) as student_name,";
        query += "hrm_employee.name as emp_name,";
        query += "hrm_employee.id as emp_id,";
        query += "amount,acc_voucher_to_student.typ as vst ";
        query += "FROM hrm_student ";
        query += "LEFT JOIN student ON student.id=hrm_student.fk_student_id ";
        query += "LEFT JOIN hrm_employee ON hrm_employee.id=hrm_student.fk_emp_id ";
        query += "LEFT JOIN (";
        query += "SELECT acc_payroll_post.fk_voucher_id,fk_emp_id,amount,fk_fiscal_year ";
        query += "FROM acc_payroll_post ";
        query += "LEFT JOIN acc_voucher_to_student ";
        query += "ON acc_voucher_to_student.fk_voucher_id=acc_payroll_post.fk_voucher_id ";
        query += "LEFT JOIN acc_voucher ON acc_voucher.id=acc_payroll_post.fk_voucher_id ";
        query += "LEFT JOIN acc_voucher_detail ON acc_voucher.id=acc_voucher_detail.fk_voucher_id ";
        query += "WHERE acc_voucher_to_student.fk_voucher_id=acc_payroll_post.fk_voucher_id ";
        query += "AND acc_voucher_detail.serial_no=1 ";
        query += "AND acc_voucher.status=1 ";
        query += "AND acc_payroll_post.fk_period_id=" + monthdat;
        query += "  AND fk_fiscal_year=" + yeardata;
        query += ")tbps ON tbps.fk_emp_id=hrm_student.fk_emp_id ";
        query += "LEFT JOIN acc_voucher_to_student ON tbps.fk_voucher_id=acc_voucher_to_student.fk_voucher_id ";
        query += "WHERE acc_voucher_to_student.fk_student_id=hrm_student.fk_student_id";
        std::map<std::string, int> column;
        column["fk_voucher_id"] = 0;
        column["student_id"] = 0;
        column["regid"] = 0;
        column["student_name"] = 0;
        column["emp_name"] = 0;
        column["emp_id"] = 0;
        column["amount"] = 0;
        column["vst"] = 0;
        std::map<std::string, std::map<std::string, std::string>> sqlresult;
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column))
        {
            sqlresult = db.getResult();
        }
        return sqlresult;
    }

    std::map<std::string, std::map<std::string, std::string>> getPayrollPostData()
    {
        std::string yeardata = year_id;
        std::string query = "SELECT ";
        query += "fk_ledger_id as ledger_id,acc_ledger.fk_ledger_group,dr_cr,amount,code,name ";
        query += "FROM (SELECT fk_ledger_id,dr_cr,SUM(amount)as amount FROM acc_payroll_post ";
        query += "LEFT JOIN acc_voucher ON acc_voucher.id=acc_payroll_post.fk_voucher_id ";
        query += "LEFT JOIN acc_voucher_detail ON acc_voucher_detail.fk_voucher_id=acc_voucher.id ";
        query += "WHERE acc_voucher.status=1 ";
        query += " AND acc_payroll_post.fk_period_id IN (" + db.getCsv(tempDataS1) + ")";
        query += " AND acc_payroll_post.type IN (" + db.getCsv(tempDataS2) + ")";
        query += " AND acc_voucher.fk_fiscal_year=" + yeardata;
        query += " GROUP BY fk_ledger_id,dr_cr)tbledg ";
        query += "LEFT JOIN acc_ledger ON acc_ledger.id=tbledg.fk_ledger_id ORDER BY dr_cr,fk_ledger_group";
        temp = query;
        std::map<std::string, int> column;
        std::map<std::string, std::map<std::string, std::string>> sqlresult;
        column["ledger_id"] = 0;
        column["dr_cr"] = 0;
        column["fk_ledger_group"] = 0;
        column["amount"] = 0;
        column["code"] = 0;
        column["name"] = 0;
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column))
        {
            sqlresult = db.getResult();
        }
        return sqlresult;
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

    std::string getCsv(std::map<int, int> arrData)
    {
        std::string myString = "";
        for (auto &iter : arrData)
        {
            myString += std::to_string(iter.second) + ",";
        }
        return myString.substr(0, myString.size() - 1);
    }
};