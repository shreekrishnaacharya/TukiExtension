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
#include "KrishnaSecurity.h"

/**
 *  Class definition
 */
class Checkup : public Php::Base {
public:

    static std::string host, user, password, db_name;

    /**
     *  Constructor and destructor
     */


    Checkup() {

    }

    virtual ~Checkup() {
    }

    void setdb(Php::Parameters &params) {
        std::string dbnam = params[0];
        Checkup::db_name = dbnam;
    }

    Php::Value mypdo() {
        return Php::Object("PDO", "mysql:host=" + Checkup::host + ";dbname=" + Checkup::db_name, Checkup::user, Checkup::password);
    }

    Php::Value mysql() {
        return Php::Object("mysqli", Checkup::host, Checkup::user, Checkup::password, Checkup::db_name);
    }

    Php::Value check() {
        if (Checkup::user.length() > 0) {
            return 0;
        }
        KrishnaSecurity krishnasecurity;
        std::string mac = exec("lsblk --nodeps -no serial /dev/sda");
        std::string machineid = exec("cat /etc/machine-id");
        std::string newcode = mac + "@Krishna@" + machineid;
        std::ifstream inFile;
        if (mac.length() == 0) {
            host = "localhost";
            user = "brainser";
            password = "brainser";
            db_name = "tukiclientdb";
            return 0;
        }

        inFile.open("/etc/xocf.dat");
        if (!inFile) {
            return 1;
        }
        std::string filedata = "", filedata1 = "";
        getline(inFile, filedata1);
        inFile.close();
        if (filedata1.size() == 0) {
            return 1;
        }
        std::string key = "SKA219$KP978$APA480$GDA626$TPA978";
        filedata = krishnasecurity.decrypt(filedata1, key);
        if (filedata.find("@db@") == std::string::npos || filedata.find("@Krishna@") == std::string::npos) {
            return 1;
        }

        std::string machinedata = filedata.substr(0, filedata.find("@db@"));
        if (machinedata.compare(newcode) != 0) {
            return 2;
        }


        std::string database = filedata.substr(filedata.find("@db@") + 4);
        std::istringstream ss(database); // Turn the string into a stream.
        int stringloopcounter = 0;
        for (std::string tok; std::getline(ss, tok, ',');) {
            stringloopcounter++;
            switch (stringloopcounter) {
                case 1:
                    Checkup::user = tok;
                    break;
                case 2:
                    Checkup::password = tok;
                    break;
                case 3:
                    Checkup::host = tok;
                    break;
                case 4:
                    Checkup::db_name = tok;
                    break;
            }
        }

        return 0;
    }

private:

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


};
