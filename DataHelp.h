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
class DataHelp : public Php::Base {
private:
    /**
     *  @var Php::Value
     */
    Php::Value die;
    DB db;
    Checkup checkup;
public:
    std::string template1 = "0";
    std::string template2 = "0";

    /**
     *  Constructor and destructor
     */
    DataHelp() {
        die = "die";
        if (checkup.check() > 0) {
            die("Your application is not registered. Please contact your application provider.");
        }
    }

    virtual ~DataHelp() {
    }

    Php::Value getAccFeeDue(Php::Parameters &params) {
        std::string studentid = params[0];
        std::string type = params[1];
        std::string query = "SELECT fk_ledger_id,SUM(IF(type=1,amount,0))as post,SUM(IF(type=2,amount,0))as rec ";
        query += "FROM acc_fines WHERE status=1 AND user_type=" + type + " AND fk_guilty_id=" + studentid;
        query += " GROUP BY fk_ledger_id ";
        std::map<std::string, int>column;
        std::map<std::string, std::map < std::string, std::string >> sqlresult;
        column["fk_ledger_id"] = 0;
        column["post"] = 2;
        column["rec"] = 2;
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column)) {
            sqlresult = db.getResult();
        }
        std::map<std::string, double> idlst;
        for (auto &iter : sqlresult) {
            if (idlst.find(iter.second["fk_ledger_id"]) == idlst.end()) {
                idlst[iter.second["fk_ledger_id"]] = 0;
            }
            idlst[iter.second["fk_ledger_id"]] += std::stod(iter.second["post"]) - std::stod(iter.second["rec"]);
        }
        return idlst;
    }

    Php::Value getMyNotice(Php::Parameters &params) {
        std::string type = params[0];
        std::string offset = params[1];
        std::string studentid = params[2];
        std::string query = "SELECT id FROM sms_notice ";
        if (type == "0") {
            std::string peogramid = params[3];
            query += "WHERE (FIND_IN_SET('" + peogramid + "', fk_pg_id)>0) OR (FIND_IN_SET('" + studentid + "', student_id)>0) OR all_flag=1 ";
        } else {
            query += "WHERE (FIND_IN_SET('" + studentid + "', emp_group)>0) OR all_flag=1 ";
        }
        query += "ORDER BY `sms_notice`.`create_date` DESC LIMIT 10000 OFFSET " + offset;
        std::map<std::string, int>column;
        std::map<std::string, std::map < std::string, std::string >> sqlresult;
        column["id"] = 0;
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column)) {
            sqlresult = db.getResult();
        }
        std::map<std::string, std::string> idlst;
        for (auto &iter : sqlresult) {
            idlst[iter.second["id"]] = iter.second["id"];
        }
        return idlst;
    }

    Php::Value getEvents(Php::Parameters &params) {
        std::string from_date = params[0];
        std::string to_date = params[1];
        std::string query = "SELECT * FROM aca_calendar ";
        query += "WHERE ((`from_date`>='" + from_date + "' AND `from_date`<='" + to_date + "') ";
        query += "OR (`to_date`>='" + from_date + "' AND `to_date`<='" + to_date + "')) AND (`status`=1)";
        std::map<std::string, int>column;
        std::map<std::string, std::map < std::string, std::string >> sqlresult;
        column["id"] = 0;
        column["title"] = 0;
        column["description"] = 0;
        column["from_date"] = 0;
        column["to_date"] = 0;
        column["type"] = 0;
        column["at_flag"] = 0;

        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column)) {
            sqlresult = db.getResult();
        }
        return sqlresult;
    }

    Php::Value getNoticeGroup(Php::Parameters &params) {
        std::string key = params[0];
        std::string sortColumn = params[1];
        std::string sortType = params[2];
        std::string page = params[3];
        std::string pagelimit = params[4];

        std::string query = "SELECT MAX(sms_notice.id) as id FROM sms_notice ";
        if (key.length() > 0) {
            query += "LEFT JOIN hrm_employee ON hrm_employee.id=sms_notice.fk_emp_id ";
            query += "WHERE sms_notice.title LIKE '%" + key + "%' OR hrm_employee.name LIKE '%" + key + "%' ";
        }
        query += "GROUP BY code";
        std::map<std::string, int>column;
        std::map<std::string, std::map < std::string, std::string >> sqlresult;
        column["id"] = 0;
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column)) {
            sqlresult = db.getResult();
        }
        std::map<std::string, std::string> idlst;
        for (auto &iter : sqlresult) {
            idlst[iter.second["id"]] = iter.second["id"];
        }
        if (idlst.size() > 0) {
            template2 = std::to_string(idlst.size());
            sqlresult.clear();
            db.setCounter(100000);
            column.clear();
            column["allcount"];
            query = "SELECT COUNT(code) as allcount FROM (SELECT code FROM sms_notice GROUP BY code)tl";
            if (db.query(query, column)) {
                sqlresult = db.getResult();
            }

            template1 = sqlresult["100000"]["allcount"];
            query = "SELECT sms_notice.id as sms_id,sms_notice.title,sms_notice.code,hrm_employee.name as employee,sms_notice.create_date FROM sms_notice ";
            query += "LEFT JOIN hrm_employee ON hrm_employee.id=sms_notice.fk_emp_id ";
            query += "WHERE sms_notice.id IN (" + db.getCsv(idlst) + ")";
            if (sortColumn.length() > 0) {
                query += " order by " + sortColumn + " " + sortType;
            } else {
                query += " order by sms_notice.create_date DESC";
            }
            query += " LIMIT " + page + "," + pagelimit;
            column.clear();
            column["sms_id"] = 0;
            column["title"] = 0;
            column["code"] = 0;
            column["employee"] = 0;
            column["create_date"] = 0;
            sqlresult.clear();
            db.setCounter(100000);
            if (db.query(query, column)) {
                sqlresult = db.getResult();
            }
        } else {
            sqlresult.clear();
        }
        return sqlresult;
    }

    Php::Value getPgAttendance(Php::Parameters &params) {
        std::map<std::string, std::string> dateLimit = params[0];
        std::string pgid = params[1];
        bool count = params[2];
        std::string query = "";
        std::map<std::string, int>column;
        if (count) {
            query = "SELECT SUM(case when att_status=1 then 1 else 0 end) as att_status,att_date FROM student_attendance ";
            query += "WHERE fk_program_group_id=" + pgid + " AND student_attendance.fk_period_id=1 ";
            if (dateLimit.find("from") != dateLimit.end()) {
                query += " AND (att_date BETWEEN " + dateLimit["from"] + " AND " + dateLimit["to"] + ") ";
            }
            query += " GROUP BY att_date";
            column["att_status"] = 0;
            column["att_date"] = 0;
            column["student_id"] = 0;
        } else {
            query = "SELECT SUM(case when att_status=1 then 1 else 0 end) as att_status,COUNT(id) as total_days,fk_stu_aca_id as student_id FROM student_attendance ";
            query += "WHERE fk_program_group_id=" + pgid + " AND student_attendance.fk_period_id=1 ";
            if (dateLimit.find("from") != dateLimit.end()) {
                query += " AND (att_date BETWEEN " + dateLimit["from"] + " AND " + dateLimit["to"] + ") ";
            }
            query += " GROUP BY fk_stu_aca_id";
            column["att_status"] = 0;
            column["total_days"] = 0;
            column["student_id"] = 0;
        }

        std::map<std::string, std::map < std::string, std::string >> sqlresult;
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column)) {
            sqlresult = db.getResult();
        }
        return sqlresult;
    }

    Php::Value getStudentAttendance(Php::Parameters &params) {
        std::map<std::string, std::string> dateLimit = params[0];
        std::string andwhere = " ";
        std::string query = "SELECT ";
        query += "SUM(case when att_status=1 then 1 else 0 end) as att_present,";
        query += "SUM(case when att_status=0 then 1 else 0 end) as att_absent, ";
        query += "SUM(case when att_status=2 then 1 else 0 end) as att_leave,";
        query += "COUNT(id) as total_days,fk_stu_aca_id as student_id FROM student_attendance WHERE student_attendance.fk_period_id=1 ";
        if (params.size() > 1) {
            std::map<std::string, std::string> studentid = params[1];
            query += " AND fk_stu_aca_id IN (" + db.getCsv(studentid) + ") ";
            andwhere = " AND ";
        }
        if (dateLimit.find("from") != dateLimit.end()) {
            query += andwhere + " (att_date BETWEEN " + dateLimit["from"] + " AND " + dateLimit["to"] + ") ";
        }
        query += " GROUP BY fk_stu_aca_id";
        std::map<std::string, int>column;
        std::map<std::string, std::map < std::string, std::string >> sqlresult;
        column["att_present"] = 0;
        column["att_absent"] = 0;
        column["att_leave"] = 0;
        column["total_days"] = 0;
        column["student_id"] = 0;
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column)) {
            sqlresult = db.getResult();
        }
        return sqlresult;
    }

    Php::Value getDbAttendance(Php::Parameters &params) {
        std::map<std::string, std::string> dateLimit = params[0];
        std::string query = "SELECT ";
        query += "COUNT(id)as total_student,";
        query += "SUM(case when att_status is null then 1 else 0 end) as not_taken,";
        query += "SUM(case when att_status=1 then 1 else 0 end) as att_present,";
        query += "SUM(case when att_status=0 then 1 else 0 end) as att_absent, ";
        query += "SUM(case when att_status=2 then 1 else 0 end) as att_leave ";
        query += "FROM `student_to_aca` ";
        query += "LEFT JOIN (SELECT fk_stu_aca_id,att_status FROM student_attendance WHERE student_attendance.fk_period_id=1";
        if (dateLimit.find("from") != dateLimit.end()) {
            query += " AND (att_date BETWEEN '" + dateLimit["from"] + "' AND '" + dateLimit["to"] + "') ";
        }
        query += ")tab_att ON tab_att.fk_stu_aca_id=student_to_aca.id ";
        query += "WHERE student_to_aca.status=1";

        std::map<std::string, int>column;
        std::map<std::string, std::map < std::string, std::string >> sqlresult;
        column["not_taken"] = 0;
        column["att_present"] = 0;
        column["att_absent"] = 0;
        column["att_leave"] = 0;
        column["total_student"] = 0;
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column)) {
            sqlresult = db.getResult();
        }
        return sqlresult;
    }

    Php::Value getTemplate1() {
        return template1;
    }

    Php::Value getTemplate2() {
        return template2;
    }


};