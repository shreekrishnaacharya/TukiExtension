/**
 *  Serverup.h
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

#include "Checkup.h"
#include "DB.h"

/**
 *  Class definition
 */


class Serverup : public Php::Base {
public:
    Php::Value die;

    std::map<std::string, std::map < std::string, std::string>> newPackages, newPackageDetail;
    std::map<std::string, std::string> currentVersion, newPackageversion, newPackage;
    std::map<std::string, std::string> outputmessage;
    Checkup checkup;
    KrishnaSecurity krishnasecurity;

    /**
     *  Constructor and destructor
     */
    Serverup() {
        die = "die";

        if (checkup.check() > 0) {
            die("Your application is not registered. Please contact your application provider.");
        }
    }

    virtual ~Serverup() {
    }

    Php::Value getCurrentVersion() {
        return currentVersion;
    }

    Php::Value getNewPackages() {
        return newPackages;
    }

    Php::Value getNewPackage() {
        return newPackage;
    }

    Php::Value getNewPackageDetail() {
        return newPackageDetail;
    }

    Php::Value getData() {
        std::map<std::string, std::string> host = checkup.getdecode();
        DB db;
        db.setup(host["db"], host["host"], host["user"], host["password"]);
        std::map<std::string, int>column;
        column["id"] = 1;
        column["first_name"] = 0;
        column["middle_name"] = 0;
        column["last_name"] = 0;
        //        return db.query("SELECT * FROM student LIMIT 1", column);
        if (db.query("SELECT * FROM student LIMIT 1", column)) {
            return db.getResult();
        } else {
            return db.getError();
        }
    }

    Php::Value getResultMessage() {
        return outputmessage;
    }

    Php::Value update(Php::Parameters &params) {
        std::map<std::string, std::map < std::string, std::string >> allowedversion, newversion, lastversion, packagefile;
        outputmessage["flag"] = "true";
        outputmessage["message"] = "SUCCESS";
        std::map<std::string, std::string> newversion2, client = params[0];
        std::map<std::string, std::string> host = checkup.getdecode();
        std::string sqlquery = "";
        DB db;
        db.setup("server_script", host["host"], host["user"], host["password"]);
        std::map<std::string, int>column;
        column["id"] = 1;
        sqlquery = "SELECT id FROM versions WHERE id>=" + client["current_version"] + " AND id<=" + client["allowed_version"] + ";";
        if (db.query(sqlquery, column)) {
            allowedversion = db.getResult();
        } else {
            outputmessage["flag"] = "false";
            outputmessage["message"] = db.getError();
            outputmessage["sql"] = sqlquery;
            return false;
        }
        std::string allowedids = "";
        for (auto &iter : allowedversion) {
            allowedids += iter.second["id"] + ",";
        }
        allowedids = allowedids.substr(0, allowedids.size() - 1);
        sqlquery = "SELECT ";
        sqlquery += "package_update.id,package_update.update_date,package_update.type,";
        sqlquery += "package_update.sub_version,package_update.detail,versions.name as version ";
        sqlquery += "FROM package_update LEFT JOIN versions ON versions.id=package_update.fk_version_id ";
        sqlquery += "WHERE fk_version_id IN (" + allowedids + ") ";
        sqlquery += "AND (clients='*' OR clients LIKE '" + client["client_code"] + ",%' ";
        sqlquery += "OR clients LIKE '%," + client["client_code"] + ",%' ";
        sqlquery += "OR clients LIKE '%," + client["client_code"] + "') ";
        sqlquery += "ORDER BY sub_version ASC";
        column.clear();
        column["id"] = 1;
        column["update_date"] = 1;
        column["type"] = 1;
        column["sub_version"] = 0;
        column["version"] = 0;
        column["detail"] = 0;
        if (db.query(sqlquery, column)) {
            newversion = db.getResult();
        } else {
            outputmessage["flag"] = "false";
            return false;
        }
        sqlquery = "SELECT ";
        sqlquery += "package_update.id as packageid,package_update.sub_version,client_update.date as lastupdate,package_update.detail ";
        sqlquery += "FROM client_update LEFT JOIN package_update ON client_update.fk_version_detail=package_update.id ";
        sqlquery += "WHERE client_update.fk_client_id='" + client["id"] + "' AND client_update.status=1 ORDER BY client_update.id ASC LIMIT 1";
        column.clear();
        column["packageid"] = 1;
        column["lastupdate"] = 1;
        column["sub_version"] = 0;
        column["detail"] = 0;
        if (db.query(sqlquery, column)) {
            lastversion = db.getResult();
        } else {
            outputmessage["flag"] = "false";
            return false;
        }
        bool flag = false;
        std::string lastversionid = lastversion["10000"]["packageid"];
        for (auto &iter : newversion) {
            if (flag) {
                newversion2 = newversion[iter.first];
                flag = false;
                break;
            }
            if (iter.second["id"] == lastversionid) {
                flag = true;
            }
        }
        if (newversion2.size() == 0) {
            outputmessage["flag"] = "true";
            outputmessage["message"] = "ALREADY LATEST VERSION";
            return false;
        }
        std::string key = checkup.getPvtkey();
        newPackage["version"] = newversion2["sub_version"];
        newPackage["type"] = newversion2["type"];
        newPackage["enc"] = checkup.randomString(20);
        std::string randstg = checkup.randomString(20);
        newPackage["file"] = checkup.toString(checkup.getTime()) + "_" + krishnasecurity.encrypt(randstg, key);

        sqlquery = "SELECT * FROM package_file WHERE fk_pu_id=" + newversion2["id"] + " ORDER BY id ASC";
        column.clear();
        column["type"] = 1;
        column["file_name"] = 0;
        column["file_location"] = 0;
        if (db.query(sqlquery, column)) {
            packagefile = db.getResult();
        } else {
            outputmessage["flag"] = "false";
            return false;
        }
        std::string temp_path = params[1];
        std::string temp_package = params[2];
        std::string fileaddress = "fileinfo";
        std::string filetype = "";
        std::string filedestination = "";
        std::string passwd = newPackage["enc"];
        std::string command = "cd " + temp_package + "/" + newversion2["version"] + "/" + newversion2["sub_version"] + " && zip -P " + krishnasecurity.encrypt(passwd, key);
        command += " -r " + temp_path + "/" + newPackage["file"] + ".zip ./*";
        checkup.exec(command.c_str());
        for (auto &iter : packagefile) {
            filedestination = iter.second["file_name"];
            filedestination = krishnasecurity.encrypt(filedestination, key);
            filetype = iter.second["type"];
            filetype = krishnasecurity.encrypt(filetype, key);
            newPackageDetail[iter.first]["vac"] = filetype;
            if (iter.second["type"] == "1") {
                newPackageDetail[iter.first]["data"] = filedestination;
                continue;
            }
            fileaddress = "fileinfo" + iter.first;
            fileaddress = krishnasecurity.encrypt(fileaddress, key);
            newPackageDetail[iter.first]["path1"] = fileaddress;
            newPackageDetail[iter.first]["path2"] = filedestination;
            command = "printf \"@ " + iter.second["file_location"] + "\n@=fileinfo" + iter.first + "\n\" | zipnote -w " + temp_path + "/" + newPackage["file"] + ".zip";
            checkup.exec(command.c_str());
        }
        outputmessage["flag"] = "true";
        outputmessage["message"] = "NEW UPDATE(S) AVAILABLE";
        return true;
    }

    Php::Value status(Php::Parameters &params) {
        std::map<std::string, std::map < std::string, std::string >> allowedversion, newversion, newversion2, lastversion;
        outputmessage["flag"] = "true";
        outputmessage["message"] = "SUCCESS";
        std::map<std::string, std::string> client = params[0];
        std::map<std::string, std::string> host = checkup.getdecode();
        std::string sqlquery = "";
        DB db;
        db.setup("server_script", host["host"], host["user"], host["password"]);
        std::map<std::string, int>column;
        column["id"] = 1;
        sqlquery = "SELECT id FROM versions WHERE id>=" + client["current_version"] + " AND id<=" + client["allowed_version"] + ";";
        if (db.query(sqlquery, column)) {
            allowedversion = db.getResult();
        } else {
            outputmessage["flag"] = "false";
            return false;
        }
        std::string allowedids = "";
        for (auto &iter : allowedversion) {
            allowedids += iter.second["id"] + ",";
        }
        allowedids = allowedids.substr(0, allowedids.size() - 1);
        sqlquery = "SELECT * FROM package_update WHERE fk_version_id IN (" + allowedids + ") ";
        sqlquery += "AND (clients='*' OR clients LIKE '" + client["client_code"] + ",%'";
        sqlquery += "OR clients LIKE '%," + client["client_code"] + ",%'";
        sqlquery += "OR clients LIKE '%," + client["client_code"] + "') ";
        sqlquery += "ORDER BY sub_version ASC";
        column.clear();
        column["id"] = 1;
        column["update_date"] = 1;
        column["sub_version"] = 0;
        column["detail"] = 0;
        if (db.query(sqlquery, column)) {
            newversion = db.getResult();
        } else {
            outputmessage["flag"] = "false";
            return false;
        }
        sqlquery = "SELECT ";
        sqlquery += "package_update.id as packageid,package_update.sub_version,client_update.date as lastupdate,package_update.detail ";
        sqlquery += "FROM client_update LEFT JOIN package_update ON client_update.fk_version_detail=package_update.id ";
        sqlquery += "WHERE client_update.fk_client_id='" + client["id"] + "' AND client_update.status=1 ORDER BY client_update.id ASC LIMIT 1";
        column.clear();
        column["packageid"] = 1;
        column["lastupdate"] = 1;
        column["sub_version"] = 0;
        column["detail"] = 0;
        if (db.query(sqlquery, column)) {
            lastversion = db.getResult();
        } else {
            outputmessage["flag"] = "false";
            return false;
        }

        bool flag = false, nextversion = false;
        int counter = 0;
        std::string lastversionid = lastversion["10000"]["packageid"];
        for (auto &iter : newversion) {
            if (nextversion) {
                newPackage = newversion[iter.first];
                nextversion = false;
            }
            if (flag) {
                newversion2[std::to_string(counter)] = newversion[iter.first];
                counter++;
            }

            if (iter.second["id"] == lastversionid) {
                flag = true;
                nextversion = true;
            }
        }
        std::string subversion = lastversion["10000"]["sub_version"];
        std::string dateversion = lastversion["10000"]["lastupdate"];
        std::string detailversion = lastversion["10000"]["detail"];
        currentVersion["version"] = subversion;
        currentVersion["updated_on"] = dateversion;
        currentVersion["detail"] = detailversion;
        if (newversion2.size() == 0) {
            outputmessage["flag"] = "true";
            outputmessage["message"] = "ALREADY LATEST VERSION";
            return true;
        }
        counter = 0;
        std::map<std::string, std::string> newpack;
        for (auto &iter : newversion2) {
            newpack["version"] = iter.second["sub_version"];
            newpack["detail"] = iter.second["detail"];
            newpack["updated_on"] = iter.second["update_date"];
            newPackages[std::to_string(counter)] = newpack;
            counter++;
        }
        outputmessage["flag"] = "true";
        outputmessage["message"] = "NEW UPDATE(S) AVAILABLE";
        return true;
    }

};