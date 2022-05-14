/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Importbackup.h
 * Author: krishna
 *
 * Created on May 4, 2019, 7:28 PM
 */
#include <phpcpp.h>
#include <iostream>

#include "Checkup.h"
#include "DB.h"
#include "Server.h"

Php::Value importbackup(Php::Parameters &params) {
    Checkup checkup;

    string output = "";
    if (params.size() == 0) {
        Php::out << "PLEASE SUBMIT TUKI BACKUP FILE LOCATION !!!\n" << std::endl;
        output = "PLEASE SUBMIT TUKI BACKUP FILE LOCATION !!!<br>";
        return output;
    }

    std::map<std::string, std::string> result = checkup.getdecode();
    if (result["host"] == "@") {
        Php::out << "Your application is not registered. Please contact your application provider. !!!\n" << std::endl;
        output = "Your application is not registered. Please contact your application provider. !!!<br>";
        return output;
    }
    string backupcommand;
    string fileloc = params[0];

    KrishnaSecurity krishnasecurity;
    string key = checkup.getPvtkey();
    string client_code = checkup.getCode();
    string password = krishnasecurity.encrypt(client_code, key);
    backupcommand = "[ -f " + fileloc + " ] && echo 'y' || echo 'n'";
    backupcommand = checkup.exec(backupcommand.c_str());
    if (backupcommand == "n") {
        Php::out << "FILE DOES NOT EXIST!!! \n" << std::endl;
        Php::out << "FILE :- " << fileloc << std::endl;
        output = "FILE DOES NOT EXIST!!!<br>";
        output += "FILE :- " + fileloc;
        return output;
    }
    backupcommand = "zipinfo -1 " + fileloc;
    backupcommand = checkup.exec(backupcommand.c_str());
    if (backupcommand.find("-") == std::string::npos) {
        Php::out << "NOT A VALID TUKI BACKUP FILE!!! \n" << std::endl;
        output = "NOT A VALID TUKI BACKUP FILE !!!<br>";
        return output;
    }
    backupcommand = "unzip -t -P " + password + " " + fileloc;
    backupcommand = checkup.exec(backupcommand.c_str());
    if (backupcommand.find("incorrect password") != std::string::npos) {
        Php::out << "FILE IS NOT SUPPORTED !!! \n" << std::endl;
        output = "FILE IS NOT SUPPORTED !!!<br>";
        return output;
    }
    if (backupcommand.find("No errors detected") == std::string::npos) {
        Php::out << "FILE IS CORROPTED OR NOT SUPPORTED !!! \n" << std::endl;
        output = "FILE IS CORROPTED OR NOT SUPPORTED !!!<br>";
        return output;
    }
    backupcommand = "zipnote " + fileloc;
    backupcommand = checkup.exec(backupcommand.c_str());
    backupcommand = backupcommand.substr(backupcommand.find("(zip file comment below this line)") + 34);
    string dbversion = backupcommand;
    string appversion = checkup.getVersion("/var/www/html/Tuki/");
    if (appversion == "0") {
        Php::out << "APPLICATION VERSION NOT DETECTED \n" << std::endl;
        output = "APPLICATION VERSION NOT DETECTED";
        return output;
    }
    if (dbversion == "") {
        Php::out << "DATABASE VERSION NOT DETECTED \n" << std::endl;
        output = "DATABASE VERSION NOT DETECTED";
        return output;
    }
    string appv = appversion.substr(0, appversion.size() - 2);
    string dbv = dbversion.substr(0, dbversion.size() - 2);
    if (appv != dbv) {
        Php::out << "APPLICATION AND DATABASE VERSION NOT DOES NOT MATCH!!!" << std::endl;
        Php::out << "APPLICATION : " << appv << std::endl;
        Php::out << "DATABASE : " << dbv << std::endl;
        output = "APPLICATION AND DATABASE VERSION DOES NOT MATCHS!!! <br>";
        output += "APPLICATION : " + appv + "<br/>DATABASE : " + dbv;
        return output;
    }

    Php::out << "LOADING FILE TO PROCESS \n" << std::endl;
    Php::out << "PLEASE WAIT ..." << std::endl;
    output += "LOADING FILE TO PROCESS <br>";
    output += "PLEASE WAIT ... <br>";
    string oldpackage = checkup.toString(checkup.getTime());
    Php::out << "****************  Step 1 started  **************** \n" << std::endl;
    Php::out << "   Processing ..." << std::endl;
    output += "****************  Step 1 started  **************** <br>";
    output += "   Processing ...<br>";
    string backupfileloc = "/tuki/" + client_code + "_tukibackup_on_import_" + oldpackage + ".zip";
    backupcommand = "mysqldump -u" + result["user"] + " -p" + result["password"] + " -h" + result["host"] + " tukidatabase | zip -P " + password + " > " + backupfileloc;
    backupcommand = checkup.exec(backupcommand.c_str());
    string versi = checkup.getVersion("/var/www/html/Tuki");
    backupcommand = "echo \"" + versi + "\" | zip -z " + backupfileloc;
    checkup.exec(backupcommand.c_str());
    Php::out << "**************** Step 1 completed ****************  \n" << std::endl;

    Php::out << "****************  Step 2 started  ****************  " << std::endl;
    Php::out << "   Processing ..." << std::endl;
    output += "****************  Step 1 completed  **************** <br>";
    output += "****************  Step 2 started  **************** <br>";
    output += "   Processing ...<br>";
    backupcommand = "mysql -u" + result["user"] + " -p" + result["password"] + " -h" + result["host"] + " -e 'DROP DATABASE IF EXISTS tukidatabase;'";
    backupcommand = checkup.exec(backupcommand.c_str());
    Php::out << "**************** Step 2 completed ****************  \n" << std::endl;
    Php::out << "****************  Step 3 started  **************** \n" << std::endl;
    Php::out << "   Processing ..." << std::endl;
    output += "****************  Step 2 completed  **************** <br>";
    output += "****************  Step 3 started  **************** <br>";
    output += "   Processing ...<br>";
    backupcommand = "mysql -u" + result["user"] + " -p" + result["password"] + " -h" + result["host"] + " -e 'CREATE DATABASE IF NOT EXISTS tukidatabase;'";
    backupcommand = checkup.exec(backupcommand.c_str());
    Php::out << "**************** Step 3 completed ****************  \n" << std::endl;
    Php::out << "****************  Step 4 started  **************** \n" << std::endl;
    Php::out << "   Processing ..." << std::endl;
    output += "****************  Step 3 completed  **************** <br>";
    output += "****************  Step 4 started  **************** <br>";
    output += "   Processing ...<br>";
    backupcommand = "unzip -q -P " + password + " -p " + fileloc + " | mysql -u" + result["user"] + " -p" + result["password"] + " -h" + result["host"] + " tukidatabase  ";
    backupcommand = checkup.exec(backupcommand.c_str());
    Php::out << "**************** Step 4 completed **************** \n" << std::endl;
    Php::out << "TUKI BACKUP IMPORT COMPLETED SUCCESSFULLY" << std::endl;
    output += "****************  Step 4 completed  **************** <br>";
    Php::out << "****************  Step 5 started  **************** \n" << std::endl;
    output += "UPDATING NEW BACKUP TO SERVER<br>";
    Php::out << "UPDATING NEW BACKUP TO SERVER\n" << std::endl;
    Php::out << "   Processing ..." << std::endl;
    backupcommand = "php /var/www/html/Tuki/yii offlinejob/backup";
    backupcommand = checkup.exec(backupcommand.c_str());
    Php::out << "**************** Step 5 completed **************** \n" << std::endl;
    output += "****************  Step 5 completed  **************** <br>";
    output += "TUKI BACKUP IMPORT COMPLETED SUCCESSFULLY<br>";
    return output;
}

Php::Value getfilebackup(Php::Parameters &params) {
    std::string filebackuppath = params[0];
    Checkup checkup;
    KrishnaSecurity krishnasecurity;
    string key = checkup.getPvtkey();
    string client_code = checkup.getCode();
    string passcode = client_code + "file" + client_code + "backup" + client_code;
    string password = krishnasecurity.encrypt(passcode, key);
    string command = "cd /var/www/html/Tuki/web/ && zip -r -P " + password + " " + filebackuppath + "/Tukifilebackup_" + client_code + ".zip resource/ >/tmp/tmp.txt";
    checkup.exec(command.c_str());
    return "*********** BACKUP SUCCESS ***********";
}

Php::Value setfilebackup(Php::Parameters &params) {
    Checkup checkup;
    std::string filebackuppath = params[0];
    KrishnaSecurity krishnasecurity;
    string command;
    string key = checkup.getPvtkey();
    string client_code = checkup.getCode();
    string passcode = client_code + "file" + client_code + "backup" + client_code;
    string password = krishnasecurity.encrypt(passcode, key);
    string user = checkup.exec("echo $SUDO_USER");
    filebackuppath = filebackuppath + "/Tukifilebackup_" + client_code + ".zip";
    command = "[ -f " + filebackuppath + " ] && echo 'y' || echo 'n'";
    if (checkup.exec(command.c_str()) == "n") {
        return "NO BACKUP FILE FOUND ON USER " + user + "'s Desktop";
    }
    command = "unzip -t -P " + password + " " + filebackuppath;
    command = checkup.exec(command.c_str());
    if (command.find("incorrect password") != std::string::npos) {
        Php::out << "FILE IS NOT SUPPORTED !!! \n" << std::endl;
        command = "FILE IS NOT SUPPORTED !!!<br>";
        return command;
    }
    command = "unzip -o -P " + password + " " + filebackuppath + "/Tukifilebackup_" + client_code + ".zip -d /var/www/html/Tuki/web/ >/tmp/tmp.txt";
    checkup.exec(command.c_str());
    return "*********** BACK FILE LOADED ***********";
}

Php::Value mydecode(Php::Parameters &params) {
    Checkup checkup;
    KrishnaSecurity krishnasecurity;
    std::string machineid = params[0];
    std::string key = checkup.getPvtkey();
    return krishnasecurity.decrypt(machineid, key);
    //        return "decode";
}

Php::Value myencode(Php::Parameters &params) {
    Checkup checkup;
    KrishnaSecurity krishnasecurity;
    std::string machineid = params[0];
    std::string key = checkup.getPvtkey();
    return krishnasecurity.encrypt(machineid, key);
    //        return "encode";
}

Php::Value mydecode2(Php::Parameters &params) {
    KrishnaSecurity krishnasecurity;
    std::string machineid = params[0];
    std::string key = "KAMALUSScdsJsdcsdc23rwe43sd";
    return krishnasecurity.decrypt(machineid, key);
    //        return "decode";
}

Php::Value myencode2(Php::Parameters &params) {
    KrishnaSecurity krishnasecurity;
    std::string machineid = params[0];
    std::string key = "KAMALUSScdsJsdcsdc23rwe43sd";
    return krishnasecurity.encrypt(machineid, key);
    //        return "encode";
}

