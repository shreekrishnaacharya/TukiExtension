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
#include "DB.h"

/**
 *  Class definition
 */
class SystemAi : public Php::Base {
private:
    /**
     *  @var Php::Value
     */
    std::map<std::string, std::map<std::string, int>> actionList;
    Php::Value die;
    DB db;
    Checkup checkup;
    std::map<std::string, std::map<std::string, std::string>> messageList;
    int counter;
    std::string current_location;

public:

    /**
     *  Constructor and destructor
     */
    SystemAi() {
        die = "die";
        if (checkup.check() > 0) {
            die("Your application is not registered. Please contact your application provider.");
        }
        actionList["accounts.fee-head.index"]["is_mf"] = 1;
        actionList["accounts.setting.fiscalyear"]["is_mf"] = 1;
        actionList["accounts.default.index"]["closeYear"] = 2;
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        counter = 0;
        current_location = "";
    }

    virtual ~SystemAi() {
    }

    void init(Php::Parameters &params) {
        std::string module = params[0];
        std::string controller = params[1];
        std::string view = params[2];
        current_location = module + "." + controller + "." + view;
    }

    Php::Value process() {
        std::map<std::string, int> actionMethod;
        for (auto &iter : actionList) {
            if (iter.first == current_location) {
                actionMethod.clear();
                actionMethod = iter.second;
                for (auto &ite : actionMethod) {
                    callFunction(ite.second);
                }
            }
        }
        return messageList;
    }

    void callFunction(int fid) {
        switch (fid) {
            case 1:
                ismf();
                break;
            case 2:
                closeYear();
                break;
        }
    }

    void ismf() {
        std::map<std::string, int>column;
        std::map<std::string, std::map < std::string, std::string >> sqlresult;
        column["id"] = 0;
        db.setCounter(100000);
        std::string query = "SELECT COUNT(id) as id FROM acc_fee_head WHERE is_mf=1";
        if (db.query(query, column)) {
            sqlresult = db.getResult();
        }
        if (sqlresult.size() == 0) {
            counter++;
            messageList[std::to_string(counter)]["type"] = "2";
            messageList[std::to_string(counter)]["msg"] = "Monthly fee head type not defined";
            return;
        }
        if (sqlresult["100000"]["id"] == "0") {
            counter++;
            messageList[std::to_string(counter)]["type"] = "2";
            messageList[std::to_string(counter)]["msg"] = "Monthly fee head type not defined";
            return;
        }
        if (sqlresult["100000"]["id"] != "1") {
            counter++;
            messageList[std::to_string(counter)]["type"] = "2";
            messageList[std::to_string(counter)]["msg"] = "Cannot have more than 1 default monthly fee head";
            return;
        }
    }

    void closeYear() {
        std::map<std::string, int>column;
        std::map<std::string, std::map < std::string, std::string >> sqlresult;
        column["id"] = 0;
        db.setCounter(100000);
        std::string query = "SELECT id FROM acc_bs_sub_head WHERE id=0";
        if (db.query(query, column)) {
            sqlresult = db.getResult();
        }
        if (sqlresult.size() == 0) {
            return;
        }
        db.closeYear();
    }
};