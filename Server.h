/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#pragma once
#include "CrulHelper.h"
#include "Checkup.h"
#include "DB.h"

/* 
 * File:   Server.h
 * Author: krishna
 *
 * Created on March 22, 2019, 12:21 PM
 */

class Server : public Php::Base {
private:
    Php::Value die;
    CurlHelper curlhepler;
    std::string server_url;
    std::string client_code;
    Php::Value json_decode;
    Php::Value json_encode;
    KrishnaSecurity krishnasecurity;
    std::string temppath;
    std::string path;
    std::string backpath;
public:
    Php::Value module;
    Checkup checkup;
    DB db;
    std::map<std::string, std::string>systemfiles;

    Server() {
        systemfiles["filecs"] = checkup.defaultfolder + "/ClientServer.php";
        systemfiles["filesh"] = checkup.defaultfolder + "/.tuki.sh";
        systemfiles["filelc"] = "/usr/share/php/linuxcommand.php";
        systemfiles["filesk"] = "/usr/lib/php/20151012/skeleton.so";
        die = "die";
        json_decode = "json_decode";
        json_encode = "json_encode";
        temppath = "/var/www/html/Tuki";
        path = "/var/www/html/temptukiupdate/";
        backpath = "/var/www/html/temptukiupdatecurr/";
        server_url = "http://103.125.25.11/server_manager/web/index.php";
        if (checkup.check() > 0) {
            die("Your application is not registered. Please contact your application provider.");
        }
        client_code = checkup.getCode();
    }

    virtual ~Server() {
    }

    void secretCode() {
        std::string key1 = checkup.getPvtkey();
        std::string key2 = "KAMALUSScdsJsdcsdc23rwe43sd";
        //        std::string command = exec("dmidecode -t 4 | grep ID");
        std::string command = "THISISPASSWORDFORLOCALDBSERVER";
        std::string local_db_password = krishnasecurity.encrypt(command, key2);
        std::string machineid = exec("cat /etc/machine-id");
        std::string mac = exec("lsblk --nodeps -no serial | head -n 1");
        std::string newcode = mac + "@Krishna@" + machineid + "@db@tukidbuser," + local_db_password + ",localhost,tukidatabase";
        newcode = krishnasecurity.encrypt(newcode, key1);
        exec("echo '" + newcode + "'> /etc/xocf.dat");
    }

    Php::Value connect() {
        std::string comd = "if ping -c1 -w3 103.125.25.11 >/dev/null 2>&1\nthen echo 1\nelse\n    echo 0\nfi";
        if (exec("/sbin/route -n | grep -c '^0\\.0\\.0\\.0'") == "0") {
            return 0;
        }
        std::string filepath = checkup.defaultfolder + "/ClientServer.php";
        return exec("php " + filepath);
    }

    Php::Value currentStatus() {
        std::string address = checkup.getmacaddress();
        std::string id = checkup.getmacid();
        std::string url = server_url + "?r=update/status";
        std::string post = "code=" + client_code + "&macid=" + url_encode(id) + "&macaddress=" + url_encode(address);
        std::string result = curlhepler.request(url + "&" + post);
        if (result.find("@1000123") == std::string::npos) {
            return "";
        }
        Php::Value output = json_decode(result, true);
        return output;
    }

    Php::Value update() {
        std::map<std::string, std::map < std::string, std::string >> sqlresult;
        std::map<std::string, std::string> host = checkup.getdecode();
        db.setup("tukidatabase", host["host"], host["user"], host["password"]);
        std::map<std::string, int>column;
        column["dataValue"] = 0;
        if (db.query("SELECT dataValue FROM settings WHERE dataKey='APPLICATION_DB_VERSION' LIMIT 1;", column)) {
            sqlresult = db.getResult();
        } else {
            return "DATABASE VERSION NOT DETECTED!!!";
        }
        if (sqlresult.empty()) {
            return "DATABASE VERSION NOT DETECTED!!!";
        }
        string dbversion = sqlresult["10000"]["dataValue"];
        string appversion = checkup.getVersion("/var/www/html/Tuki/");
        if (appversion == "0") {
            return "APPLICATION VERSION NOT DETECTED!!!";
        }
        string appv = appversion.substr(0, appversion.size() - 2);
        string dbv = dbversion.substr(0, dbversion.size() - 2);
        if (appv != dbv) {
            return "APPLICATION AND DATABASE VERSION DOES NOT MATCHS!!!\nAPPLICATION : " + appv + "\nDATABASE : " + dbv;
        }

        std::string address = checkup.getmacaddress();
        std::string id = checkup.getmacid();
        std::string url = server_url + "?r=update/version-up";
        std::string post = "code=" + client_code + "&macid=" + url_encode(id) + "&macaddress=" + url_encode(address);
        std::string result = curlhepler.request(url + "&" + post);
        if (result == "Unable to connect to server.") {
            return result + "\n";
        }
        Php::Value output = json_decode(result, true);
        std::string flag = output["flag"];
        if (flag == "false") {
            return output["message"];
        }
        std::string type = output["info"]["type"];
        if (type == "4") {
            return output;
        } else {
            return regularupdate(output["info"], output["data"], post, type);
        }
    }

    Php::Value packageupdate(Php::Value phpinfo, Php::Value phpdetail, std::string post) {
        std::map<std::string, std::string> info = phpinfo;
        std::map<std::string, std::map < std::string, std::string>> param = phpdetail;
        std::string key = checkup.getPvtkey();
        std::string code = "1";
        std::string report = "";
        std::string tmp_sudo = exec("[ -d " + path + " ] && echo 'y' || echo 'n'");
        if (tmp_sudo == "n") {
            exec("mkdir " + path);
        }

        tmp_sudo = exec("[ -d " + backpath + " ] && echo 'y' || echo 'n'");
        if (tmp_sudo == "n") {
            exec("mkdir " + backpath);
        }
        std::string filepath = "/var/www/html/temptukiupdate/packageudate.zip";
        char outfilename[FILENAME_MAX] = "/var/www/html/temptukiupdate/packageudate.zip";
        std::string url = server_url + "?r=update/download&" + post + "&file=" + info["file"];
        tmp_sudo = curlhepler.download(url, outfilename);
        if (tmp_sudo != "SUCCESS" || exec("[ -f " + filepath + " ] && echo 'y' || echo 'n'") == "n") {
            return "Error !!! unable to download update files. Please contact your application provider.";
        }
        exec("mkdir " + path + info["version"]);
        //        std::string passkey = krishnasecurity.decrypt(info["enc"], checkup.getPvtkey());

        report += "\n====================== WORKING NEW PACKAGE ======================";
        std::string passwd = info["enc"];
        std::string keyqq = info["end"];
        if (keyqq != "false") {
            key = krishnasecurity.decrypt(keyqq, key);
        }
        passwd = krishnasecurity.decrypt(passwd, key);
        tmp_sudo = "unzip -P " + passwd + " " + filepath + " -d " + path + info["version"];
        exec(tmp_sudo);
        report += "\n Backing old packages...";
        exec("cp -R " + temppath + "/web/card/image " + backpath + "card");
        exec("cp -R " + temppath + "/web/resource " + backpath + "resource");
        exec("mv " + temppath + " /var/www/html/Tuki_" + checkup.getVersion(path) + "_" + checkup.toString(checkup.getTime()));
        report += "\n Extracting new packages...";
        exec("mv " + path + info["version"] + " " + temppath);
        exec("mv " + backpath + "card" + " " + temppath + "/web/card/image");
        exec("mv " + backpath + "resource" + " " + temppath + "/web/resource");
        report += "\n Updating new packages...";
        tmp_sudo = "echo 'yes' || php /var/www/html/Tuki/yii migrate";
        exec(tmp_sudo);
        report += "\n====================== ADDITIONAL CHANGES UPDATING ======================";
        for (auto &iter : param) {
            if (iter.second["type"] == "1") {
                tmp_sudo = exec(iter.second["data"]);
                report += "\nCOMMAND : " + iter.second["data"];
                report += "\nRESULT : " + tmp_sudo;
                continue;
            }
        }
        checkup.setVersion(info["version"], temppath);
        exec("rm -rf " + path);
        exec("rm -rf " + backpath);
        url = server_url + "?r=update/report";
        post += "&version=" + url_encode(info["version"]) + "&status=" + code + "&message=" + url_encode(report);
        report += "\n";
        tmp_sudo = url + "&" + post;
        report += "\n====================== REPORTING STATUS TO SERVER ======================\n";
        report += curlhepler.request(tmp_sudo);
        report += "\n";
        return report;

    }

    void refresh() {
        std::string tmp_sudo = "mv /var/www/html/Tuki/mail/libphpcpp.so /usr/lib/ && ";
        tmp_sudo += "mv /var/www/html/Tuki/mail/libphpcpp.so.2.0 /usr/lib/ && ";
        tmp_sudo += "mv /var/www/html/Tuki/mail/libphpcpp.so.2.0.0 /usr/lib/ && ";
        tmp_sudo += "mv /var/www/html/Tuki/mail/libphpcpp.so.2.1 /usr/lib/ && ";
        tmp_sudo += "mv /var/www/html/Tuki/mail/libphpcpp.so.2.1.1 /usr/lib/ && ";
        tmp_sudo += "/etc/init.d/apache2 restart";
        system(tmp_sudo.c_str());
    }

    Php::Value regularupdate(Php::Value phpinfo, Php::Value phpdetail, std::string post, std::string updatetype) {
        std::map<std::string, std::string> info = phpinfo;
        std::map<std::string, std::map < std::string, std::string>> param = phpdetail;
        std::string report = "", serverreport = "";
        std::string key = checkup.getPvtkey();
        std::string tmp_sudo = exec("[ -d " + path + " ] && echo 'y' || echo 'n'");
        if (tmp_sudo == "n") {
            exec("mkdir " + path);
        }
        tmp_sudo = exec("[ -d " + backpath + " ] && echo 'y' || echo 'n'");
        if (tmp_sudo == "n") {
            exec("mkdir " + backpath);
        }
        std::string filepath = "/var/www/html/temptukiupdate/packageudate.zip";
        char outfilename[FILENAME_MAX] = "/var/www/html/temptukiupdate/packageudate.zip";
        std::string url = server_url + "?r=update/download&" + post + "&file=" + info["file"];
        report += "\n====================== DOWNLOADING UPDATES ======================";
        curlhepler.progress = false;
        tmp_sudo = curlhepler.download(url, outfilename);
        if (tmp_sudo.find("@1000") != std::string::npos || exec("[ -f " + filepath + " ] && echo 'y' || echo 'n'") == "n") {
            serverreport += report + "\nUNABLE TO DOWNLOAD UPDATES | DOWNLOAD CODE : " + tmp_sudo + " | CODE @100001!!!\n";
            report += "\nFAIL TO UPGRADE | CODE @100001";
            url = server_url + "?r=update/report";
            post += "&version=" + url_encode(info["version"]) + "&status=0&message=" + url_encode(serverreport);
            report += "\n";
            tmp_sudo = url + "&" + post;
            report += "\n====================== REPORTING STATUS TO SERVER ======================\n";
            report += curlhepler.request(tmp_sudo);
            return report += "\n";
        }
        report += "\n  UPDATE DOWNLOADED\n";
        tmp_sudo = exec("[ -d " + path + info["version"] + " ] && echo 'y' || echo 'n'");
        if (tmp_sudo == "n") {
            exec("mkdir " + path + info["version"] + " 2>/tmp/logcore.txt");
        }
        std::string passwd = info["enc"];
        std::string keyqq = info["end"];
        if (keyqq != "false") {
            key = krishnasecurity.decrypt(keyqq, key);
        }
        passwd = krishnasecurity.encrypt(passwd, key);
        tmp_sudo = "unzip -t -P " + passwd + " " + filepath;
        tmp_sudo = exec(tmp_sudo + " 2>/tmp/logcore.txt");
        if (tmp_sudo.find("incorrect password") != std::string::npos) {
            serverreport = report + "\nENCRYPTION DOES NOT MATCHES | CODE @100002!!!";
            serverreport += "\n\tLOG : " + exec("cat /tmp/logcore.txt");
            report += "\nFAIL TO UPGRADE | CODE @100002";
            url = server_url + "?r=update/report";
            post += "&version=" + url_encode(info["version"]) + "&status=0&message=" + url_encode(serverreport);
            report += "\n";
            tmp_sudo = url + "&" + post;
            report += "\n====================== REPORTING STATUS TO SERVER ======================\n";
            report += curlhepler.request(tmp_sudo);
            return report += "\n";
        }
        if (tmp_sudo.find("No errors detected") == std::string::npos) {
            serverreport = report + "\nFILE IS CORROPTED OR NOT SUPPORTED | CODE @100003!!!";
            report += "\nFAIL TO UPGRADE | CODE @100003";
            serverreport += "\n\tLOG : " + exec("cat /tmp/logcore.txt");
            url = server_url + "?r=update/report";
            post += "&version=" + url_encode(info["version"]) + "&status=0&message=" + url_encode(serverreport);
            report += "\n";
            tmp_sudo = url + "&" + post;
            report += "\n====================== REPORTING STATUS TO SERVER ======================\n";
            report += curlhepler.request(tmp_sudo);
            return report += "\n";
        }

        tmp_sudo = "unzip -P " + passwd + " " + filepath + " -d " + path + info["version"] + " 2>/tmp/logcore.txt";
        exec(tmp_sudo);
        tmp_sudo = exec("cat /tmp/logcore.txt");
        serverreport += "\n\tLOG : " + tmp_sudo; //check the file for conflict
        report += "\nLOADING NEW MODULES";
        std::map<int, std::map < std::string, std::string>> tempfiles;

        int loop = 0;

        bool flag = true;
        std::string code = "1";
        report += "\n====================== UPDATING MODULES ======================";
        std::string fileaddress = "fileinfo";
        std::string filetype = "";
        std::string filedestination = "";
        serverreport = report;
        for (auto &iter : param) {
            filetype = iter.second["vac"];
            filetype = krishnasecurity.decrypt(filetype, key);
            if (filetype == "1") {
                fileaddress = iter.second["data"];
                fileaddress = krishnasecurity.decrypt(fileaddress, key);
                tmp_sudo = exec(fileaddress + " 2>/tmp/logcore.txt");
                serverreport += "\n\tLOG : " + exec("cat /tmp/logcore.txt");
                serverreport += "\n\tCOMMAND : " + fileaddress;
                serverreport += "\n\tRESULT : " + tmp_sudo;
                continue;
            }
            fileaddress = iter.second["path1"];
            fileaddress = krishnasecurity.decrypt(fileaddress, key);
            filedestination = iter.second["path2"];
            filedestination = krishnasecurity.decrypt(filedestination, key);

            if (filetype == "2") {
                tmp_sudo = exec("[ ! -e " + temppath + systemfiles[filedestination] + " ]; echo $?");
                if (tmp_sudo == "1") {
                    tempfiles[loop]["destination"] = systemfiles[filedestination];
                    tempfiles[loop]["source"] = backpath + fileaddress;
                    exec("cp " + tempfiles[loop]["destination"] + " " + backpath + "file" + std::to_string(loop));
                    loop++;
                }
                tmp_sudo = exec("mv -f -v " + path + info["version"] + "/" + fileaddress + " " + systemfiles[filedestination] + " 2>/tmp/logcore.txt");
                serverreport += "\n\tLOG : " + exec("cat /tmp/logcore.txt");
                serverreport += "\n  SYSTEM : " + filedestination;
            } else {
                report += "\nPROCESSING ON UPDATE : " + iter.first;
                tmp_sudo = exec("[ ! -e " + temppath + filedestination + " ]; echo $?");
                if (tmp_sudo == "1") {
                    tempfiles[loop]["destination"] = temppath + filedestination;
                    tempfiles[loop]["source"] = backpath + fileaddress;
                    exec("cp " + tempfiles[loop]["destination"] + " " + backpath + "file" + std::to_string(loop));
                    loop++;
                }
                tmp_sudo = exec("mv -f -v " + path + info["version"] + "/" + fileaddress + " " + temppath + filedestination + " 2>/tmp/logcore.txt");
                serverreport += "\n\tLOG : " + exec("cat /tmp/logcore.txt");
                serverreport += "\n  UPDATING : " + filedestination;
                serverreport += "\n  RESULT : " + tmp_sudo;
            }

        }
        report += "\n====================== VERIFYING UPDATES ======================";
        serverreport += "\n====================== VERIFYING UPDATES ======================";
        for (auto &iter : tempfiles) {
            tmp_sudo = "cmp --silent " + iter.second["source"] + " " + iter.second["destination"] + " || echo 'different'";
            tmp_sudo = exec(tmp_sudo);
            serverreport += "\n\tCHECKING : " + iter.second["destination"];
            serverreport += "\n\tRESULT : " + tmp_sudo;
            report += "\n\tCHECKING : " + std::to_string(iter.first);
            if (tmp_sudo != "different") {
                serverreport += "\n\tUnchanged file : " + iter.second["destination"];
                report += "\n\tFAILED : " + std::to_string(iter.first);
                flag = false;
            } else {
                serverreport += "\n\tSUCCESS : " + iter.second["destination"];
                report += "\n\tSUCCESS : " + std::to_string(iter.first);
            }
        }
        if (!flag) {
            code = "0";
            report += "\n\n====================== ROLLBACK UPDATES ======================";
            serverreport += "\n\n====================== ROLLBACK UPDATES ======================";
            for (auto &iter : tempfiles) {
                tmp_sudo = exec("mv -f -v " + iter.second["source"] + " " + iter.second["destination"]);
                serverreport += tmp_sudo;
                serverreport += "\n";
            }
            report += "\n\n====================== FAILED ======================";
            serverreport += "\n\n====================== FAILED ======================";
        } else {
            checkup.setVersion(info["version"], temppath);
            db.execute("UPDATE `settings` SET dataValue='" + info["version"] + "' WHERE dataKey='APPLICATION_DB_VERSION'");
            report += "\n\n====================== SUCCESS ======================";
            serverreport += "\n\n====================== SUCCESS ======================";
            checkup.restartApache();
        }
        if (updatetype != "1") {
            report += "\n\n====================== DB CHANGED UPDATED ======================\n";
            serverreport += "\n\n====================== DB CHANGED UPDATED ======================\n";
            tmp_sudo = exec("php /var/www/html/Tuki/yii offlinejob/backup");
            report += "\n DB BACKUP JOB : " + tmp_sudo;
            serverreport += "\n DB BACKUP JOB : " + tmp_sudo;
            tmp_sudo = exec("echo 'yes'| php /var/www/html/Tuki/yii migrate");
            serverreport += "\n DB UPDATE JOB : " + tmp_sudo;
        }
        exec("rm -rf " + path);
        exec("rm -rf " + backpath);
        serverreport += "\n====================== REPORTING STATUS TO SERVER ======================\n";
        url = server_url + "?r=update/report";
        post += "&version=" + url_encode(info["version"]) + "&status=" + code;
        report += "\n";
        tmp_sudo = url + "&" + post;
        report += "\n====================== REPORTING STATUS TO SERVER ======================\n";

        std::string messag = "message=" + url_encode(serverreport);
        tmp_sudo = curlhepler.request_post(tmp_sudo, messag);
        if (tmp_sudo.find("@100005") == std::string::npos) {
            report += "REPORTING TO SERVER FAILED\n";
            if (tmp_sudo == "Unable to connect to server.") {
                report += tmp_sudo + "\n";
            } else {
                string file_name = checkup.toString(checkup.getTime());
                exec("echo '" + tmp_sudo + "'> /var/www/html/Tuki/runtime/logs/app/application_update_error_" + file_name + ".log");
            }
        } else {
            report += tmp_sudo + "\n";
        }
        report += "\n";
        return report;
    }

    std::string reporttoserver(std::string url) {
        curlhepler.request(url);
        return url;
    }

    string url_encode(const string & value) {
        ostringstream escaped;
        escaped.fill('0');
        escaped << hex;
        for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
            string::value_type c = (*i);

            // Keep alphanumeric and other accepted characters intact
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                escaped << c;

                continue;
            }
            // Any other characters are percent-encoded
            escaped << uppercase;
            escaped << '%' << setw(2) << int((unsigned char) c);
            escaped << nouppercase;
        }

        return escaped.str();
    }

    std::string exec(std::string command) {
        const char* cmd = command.c_str();
        char buffer[128];
        std::string result = "";
        FILE* pipe = popen(cmd, "r");
        if (!pipe) throw std::runtime_error("popen() failed!");
        try {
            while (!feof(pipe)) {
                if (fgets(buffer, 128, pipe) != NULL)
                    result += buffer;
            }
        } catch (const char* msg) {
            pclose(pipe);
            throw;
        }
        pclose(pipe);
        result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
        return result;
    }


};
