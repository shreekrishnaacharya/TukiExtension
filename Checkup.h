/**
 *  Checkup.h
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

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <fstream>
#include <sstream>
#include <array>
#include <vector>
#include <map>
#include <ctime>
#include "KrishnaSecurity.h"
#include "DB.h"

/**
 *  Class definition
 */
class Checkup : public Php::Base {
public:

    /**
     *  Constructor and destructor
     */

public:
    std::string cmd_drive;
    std::string defaultfolder = "/etc/tkns/res/folds";

    Checkup() {
        cmd_drive = "lsblk --nodeps -no serial | sed -n '/./{p;q;}'";
    }

    virtual ~Checkup() {
    }

    Php::Value mypdo(Php::Parameters &params) {
        std::map<std::string, std::string> result = getdecode();
        std::string db_name = result["db"];
        if (!params.empty()) {
            std::string phpdb = params[0];
            db_name = phpdb;
        }
    //    return Php::Object("PDO", "mysql:host=" + result["host"] + ";dbname=tukidatabase", "root2", "kamal12345");
        return Php::Object("PDO", "mysql:host=" + result["host"] + ";dbname=" + db_name, result["user"], result["password"]);
    }

    Php::Value incbackup(Php::Parameters &params) {
        string result = params[0];
        string fileloc = params[1];
        string filename = params[2];
        std::map<std::string, std::string> client = params[3];
        string filetemp = fileloc + filename + "_temp.sql";

        ofstream myfile(filetemp, ios::app);
        if (myfile.is_open()) {
            myfile << result;
            myfile.close();
        } else {
            return "UNABLE TO WRITE FILE";
        }
        std::string mac = client["college_id"];
        KrishnaSecurity krishnasecurity;
        string key = getPvtkey();
        mac = krishnasecurity.encrypt(mac, key);
        string backupcommand = "cat " + filetemp + " | zip -P " + mac + " > " + fileloc + filename + "_spl.zip";
        exec(backupcommand.c_str());
        backupcommand = "rm " + filetemp;
        exec(backupcommand.c_str());
        return "BACKUP SUCCESS";
    }

    Php::Value fullbackup(Php::Parameters &params) {
        std::map<std::string, std::string> result = getadminlogin();
        string output = "";
        string fileloc = params[0];
        std::string mac = getCode();
        KrishnaSecurity krishnasecurity;
        string key = getPvtkey();
        mac = krishnasecurity.encrypt(mac, key);
        string backupcommand = "mysqldump -u" + result["user"] + " -p" + result["password"] + " -h" + result["host"] + " " + result["db"] + " | zip -P " + mac + " > " + fileloc;
        output += exec(backupcommand.c_str());
        backupcommand = getVersion("/var/www/html/Tuki");
        backupcommand = "echo \"" + backupcommand + "\" | zip -z " + fileloc;
        exec(backupcommand.c_str());
        return output;
    }

    Php::Value userlog(Php::Parameters &params) {
        Php::Array result, temp;
        if (check() > 0) {
            return result;
        }
        std::map<std::string, std::map < std::string, std::string>> message = params[0];
        int counter = 0, status = 0;
        for (auto &iter : message) {
            if (iter.second["category"] == "begin") {
                status = status + 1;
                temp["timestamp"] = iter.second["timestamp"];
                temp["message"] = "START TRANSACTION;";
                result[counter] = temp;
                counter++;
                continue;
            }
            if (iter.second["category"] == "commit") {
                temp["timestamp"] = iter.second["timestamp"];
                temp["message"] = "COMMIT;";
                status--;
                result[counter] = temp;
                counter++;
                continue;
            }
            if (iter.second["category"] == "rollback") {
                temp["timestamp"] = iter.second["timestamp"];
                temp["message"] = "ROLLBACK;";
                status--;
                result[counter] = temp;
                counter++;
                continue;
            }
            temp["timestamp"] = iter.second["timestamp"];
            temp["message"] = iter.second["text"];
            result[counter] = temp;
            counter++;
        }
        if (status > 0) {
            for (int i = status; i > 0; i--) {
                temp["timestamp"] = temp["timestamp"];
                temp["message"] = "ROLLBACK;";
                result[counter] = temp;
                counter++;
            }
        }
        return result;
    }

//    Php::Value myquery(Php::Parameters &params) {
//        std::map<std::string, std::string> result = getdecode();
//        return Php::Object("mysqli", result["host"], result["user"], result["password"], params[0]);
//    }
    
    Php::Value myquery() {
        std::map<std::string, std::string> result = getdecode();
        return Php::Object("mysqli", result["host"], result["user"], result["password"], result["db"]);
    }

    Php::Value getid() {
        return getcodein();
    }

    Php::Value check() {
        std::time_t now = time(0);
        if (now > 1712962909) {
            return "1";
        }
        //        std::string filedata1 = getcodein();
        //        if (filedata1.size() == 0) {
        //            getcode();
        //        }
        std::map<std::string, std::string> filedata = getdecode();
        if (filedata["flag"] != "0") {
            return filedata["flag"];
        }
        DB *db = new DB();
        db->setup(filedata["db"], filedata["host"], filedata["user"], filedata["password"]);
        std::map<std::string, std::map < std::string, std::string >> sqlresult;
        std::map<std::string, int>column;

        column["Tables_in_" + filedata["db"]] = 0;
        if (db->query("show tables from " + filedata["db"], column)) {
            sqlresult = db->getResult();
        }
        if (!sqlresult.empty()) {
            column.clear();
            sqlresult.clear();
            column["Field"] = 0;
            if (db->query("SHOW COLUMNS FROM `aca_program` LIKE 'program_id';", column)) {
                sqlresult = db->getResult();
            }
            if (!sqlresult.empty()) {
                return "10";
            }
            column.clear();
            sqlresult.clear();
            column["tables"] = 0;
            if (db->query("SELECT SUM(TABLE_ROWS)as tables FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = 'tukidatabase';", column)) {
                sqlresult = db->getResult();
            }
            bool fourthflag = false;
            if (!sqlresult.empty()) {
                int tabl_count = std::stoi(sqlresult["10000"]["tables"]);
                if (tabl_count > 100) {
                    column.clear();
                    sqlresult.clear();
                    column["kamal"] = 0;
                    if (db->query("SELECT 'kamal' FROM acc_ledger LIMIT 1;", column)) {
                        sqlresult = db->getResult();
                    }
                    if (sqlresult.empty()) {
                        fourthflag = true;
                    }
                }
            }
            if (fourthflag) {
                return "11";
            }
        }
        string lastupdate = exec("cat /var/www/html/Tuki/web/material/fonts/roboto/Roboto-Bolder.eot");
        int lastint = std::stoi(lastupdate) + 103328000;
        string nowtime = toString(getTime());
        int nowtimeint = std::stoi(nowtime);
        if (lastint < nowtimeint) {
            return "12";
        }
        delete db;
        return 0;
    }

    std::time_t getTime() {
        std::time_t now = time(0);
        return now;
    }

    std::string getVersion(std::string path) {
        std::string comd = "[ -f " + path + "/runtime/mail/versod.emd ] && echo 'y' || echo 'n'";
        if (exec(comd.c_str()) == "n") {
            return "0";
        } else {
            comd = "cat " + path + "/runtime/mail/versod.emd";
            return exec(comd.c_str());
        }
    }

    Php::Value getVer() {
        std::string comd = "[ -f /var/www/html/Tuki/runtime/mail/versod.emd ] && echo 'y' || echo 'n'";
        if (exec(comd.c_str()) == "n") {
            return "UNKNOWN";
        } else {
            comd = "cat /var/www/html/Tuki/runtime/mail/versod.emd";
            return exec(comd.c_str());
        }
    }

    Php::Value getFileVer() {
        std::string comd = "[ -f /var/www/html/Tuki/runtime/mail/fando.emd ] && echo 'y' || echo 'n'";
        if (exec(comd.c_str()) == "n") {
            return "UNKNOWN";
        } else {
            comd = "cat /var/www/html/Tuki/runtime/mail/fando.emd";
            return exec(comd.c_str());
        }
    }

    void setFileVer(Php::Parameters &params) {
        std::string version = params[0];
        std::string comd = "echo '" + version + "' > /var/www/html/Tuki/runtime/mail/fando.emd";
        exec(comd.c_str());
    }

    Php::Value getDbVer() {
        DB db;
        std::map<std::string, std::map < std::string, std::string >> sqlresult;
        std::map<std::string, int>column;
        std::map<std::string, std::string> host = getdecode();
        db.setup("tukidatabase", host["host"], host["user"], host["password"]);
        column["version"] = 0;
        if (db.query("SELECT dataValue as version FROM `settings` WHERE dataKey='APPLICATION_DB_VERSION'", column)) {
            sqlresult = db.getResult();
        }
        if (sqlresult.empty()) {
            return "UNKNOWN";
        } else {
            return sqlresult["10000"]["version"];
        }
    }

    void setVersion(std::string version, std::string path) {
        std::string comd = "echo '" + version + "' > " + path + "/runtime/mail/versod.emd";
        exec(comd.c_str());
    }

    std::string getPvtkey() {
        return "SKA219$KP978$APA480$GDA626$TPA978";
    }

    std::string randomString(int len) {
        std::srand(time(0));
        std::string str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        std::string newstr;
        int pos;
        while (newstr.size() != len) {
            pos = ((std::rand() % (str.size() - 1)));
            newstr += str.substr(pos, 1);
        }
        return newstr;
    }

    void phpmyadmin(Php::Parameters &params) {
        int openflag = params[0];
        std::string temp_cmd;
        if (openflag == 1) {
            temp_cmd = "cp /etc/phpmyadmin/conf.d/apache.conf /etc/phpmyadmin/";
            exec(temp_cmd.c_str());
        } else {
            temp_cmd = "[ -f /etc/phpmyadmin/apache.conf ] && echo 'y' || echo 'n'";
            if (exec(temp_cmd.c_str()) == "y") {
                temp_cmd = "rm /etc/phpmyadmin/apache.conf";
                exec(temp_cmd.c_str());
            }
        }
        restartApache();
    }

    Php::Value enablemysqluser() {
        DB db;
        std::map<std::string, std::string> host = getadminlogin();
        db.setup("", host["host"], host["user"], host["password"]);
        std::string password = randomString(10);
        std::string temp = "CREATE USER IF NOT EXISTS 'temp_user'@'localhost' IDENTIFIED BY '" + password + "';";
        if (db.execute(temp) && db.getUpdateState() > 0) {
            temp = "GRANT SELECT, INSERT, UPDATE, DELETE, TRIGGER, LOCK TABLES ON tukidatabase.* TO 'temp_user'@'localhost';";
            if (db.execute(temp) && db.getUpdateState() > 0) {
                return "USER : temp_user --- PASSWORD : " + password;
            }
            return "UNABLE TO SET USER - 2 : " + db.getUpdateState();
        }
        return "UNABLE TO SET USER - 1 : " + db.getUpdateState();
    }

    Php::Value disablemysqluser() {
        DB db;
        std::map<std::string, std::string> host = getadminlogin();
        db.setup("", host["host"], host["user"], host["password"]);
        std::string temp = "DROP USER IF EXISTS 'temp_user'@'localhost';";
        if (db.execute(temp) && db.getUpdateState() > 0) {
            return "USER DISABLED";
        }
        return "UNABLE TO DISABLE USER : " + db.getUpdateState();
    }

    Php::Value resetsuper() {
        DB db;
        std::map<std::string, std::string> host = getadminlogin();
        db.setup(host["db"], host["host"], host["user"], host["password"]);
        string query = "INSERT INTO `hrm_employee`(id,name,email,password,phone,fk_district_id,address1,dob,date_of_join,fk_married_id,fk_emp_type,fk_post_id,create_date,status) ";
        query += "VALUES(1,'Super Admin','superadmin@tuki.com','tuki@12345','1234567890',100,'Nepalgunj','-19800','1262283300',1,1,1,1262283300,1) ";
        query += "ON DUPLICATE KEY UPDATE `email`='superadmin@tuki.com', `password`='tuki@12345',status=1";
        if (db.execute(query) && db.getUpdateState() > 0) {
            return "SUPER USER RESET";
        } else {
            return "SUPER USER NOT RESET";
        }
    }

    Php::Value systemstate(Php::Parameters &params) {
        DB db;
        string state = params[0];
        string message = params[1];
        string statuval = "0";
        if (state == "1") {
            statuval = "1";
        }
        if (message.empty()) {
            message = "System is currently under maintenance. Please come back later.";
        }
        std::map<std::string, std::string> host = getadminlogin();
        db.setup("tukidatabase", host["host"], host["user"], host["password"]);
        string query = "UPDATE `settings` SET dataValue='" + statuval + "',constrants='" + message + "' WHERE dataKey='APPLICATION_STATUS';";
        if (db.execute(query) && db.getUpdateState() > 0) {
            return "SYSTEM STATE CHANGED : " + statuval;
        }
        return "SYSTEM STATE NOT CHANGED : " + statuval;
    }

    void restartApache() {
        std::string temp_cmd = "/etc/init.d/apache2 restart";
        exec(temp_cmd.c_str());
    }

    Php::Value getmacid() {
        KrishnaSecurity krishnasecurity;
        std::string machineid = exec("cat /etc/machine-id");
        std::string key = getPvtkey();
        return krishnasecurity.encrypt(machineid, key);
    }

    Php::Value getmacaddress() {
        KrishnaSecurity krishnasecurity;
        std::string machineid = exec(cmd_drive.c_str());
        std::string key = getPvtkey();
        return krishnasecurity.encrypt(machineid, key);
    }

    template<typename T>
    std::string toString(const T &t) {
        std::ostringstream oss;
        oss << t;
        return oss.str();
    }

    std::string getCode() {
        KrishnaSecurity krishnasecurity;
        std::string filedata1 = getcodein();
        std::string key = getPvtkey();
        std::string filedata = krishnasecurity.decrypt(filedata1, key);
        return filedata.substr(filedata.find("@client@") + 8);
    }

    Php::Value getClientCode() {
        KrishnaSecurity krishnasecurity;
        std::string filedata1 = getcodein();
        std::string key = getPvtkey();
        std::string filedata = krishnasecurity.decrypt(filedata1, key);
        return filedata.substr(filedata.find("@client@") + 8);
    }

    std::map<std::string, std::string> getadminlogin() {
        KrishnaSecurity krishnasecurity;
        std::map<std::string, std::string> result;
        result["host"] = "localhost";
        result["user"] = "tukiadminuser";
        result["password"] = "@";
        result["db"] = "tukidatabase";
        std::string command = exec("cat /var/www/html/Tuki/runtime/mail/acee.dat");
        std::string key2 = "KAMALUSScdsJsdcsdc23rwe43sd";
        result["password"] = krishnasecurity.decrypt(command, key2);
        return result;
    }

    std::map<std::string, std::string> getdecode() {
        KrishnaSecurity krishnasecurity;
        std::string mac = exec(cmd_drive.c_str());
//        std::string mac = "WD-WCC4M0JTX7Uj";   //for 10002 client
        std::string machineid = exec("cat /etc/machine-id");
        std::string newcode = mac + "@Krishna@" + machineid;
        std::string filedata1 = getcodein();
        std::map<std::string, std::string> result;
        result["host"] = "@";
        result["user"] = "@";
        result["password"] = "@";
        result["db"] = "@";
        result["flag"] = "0";
        if (filedata1.size() == 0) {
            result["flag"] = "2";
            return result;
        }
        std::string key = getPvtkey();
        std::string filedata = krishnasecurity.decrypt(filedata1, key);

        if (filedata.find("@client@") == std::string::npos || filedata.find("@db@") == std::string::npos || filedata.find("@Krishna@") == std::string::npos) {
            result["flag"] = "3";
            return result;
        }

        std::string machinedata = filedata.substr(0, filedata.find("@db@"));
        if (machinedata.compare(newcode) != 0) {
            result["flag"] = "4";
            return result;
        }

        std::string databasedata = filedata.substr(filedata.find("@db@") + 4);
        std::string database = databasedata.substr(0, databasedata.find("@client@"));
        std::istringstream ss(database); // Turn the string into a stream.
        int stringloopcounter = 0;
        for (std::string tok; std::getline(ss, tok, ',');) {
            stringloopcounter++;
            switch (stringloopcounter) {
                case 1:
                    result["user"] = "tukidbuser";
                    //                    Php::define("asbskus", tok);
                    break;
                case 2:
                    result["password"] = tok;
                    //                    Php::define("asbkdps", tok);
                    break;
                case 3:
                    result["host"] = "localhost";
                    //                    Php::define("asbkrhst", tok);
                    break;
                case 4:
                    result["db"] = "tukidatabase";
                    //                    Php::define("asbskdd", tok);
                    break;
            }
        }
        return result;
    }

    std::string getcode() {
        std::string filedata = exec("cat /etc/xocf.dat");
        Php::GLOBALS["machineid"] = filedata;
        return filedata;
    }

    std::string getcodein() {
        return exec("cat /etc/xocf.dat");
    }

    std::string exec(const char* cmd) {
        std::array<char, 128> buffer;
        std::string result;
        std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
        if (!pipe) throw std::runtime_error("popen() failed!");
        while (!feof(pipe.get())) {
            if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
                result += buffer.data();
        }
        result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
        return result;
    }

    std::string trim(const string& str)
    {
        size_t first = str.find_first_not_of(' ');
        if (string::npos == first)
        {
            return str;
        }
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }

};
