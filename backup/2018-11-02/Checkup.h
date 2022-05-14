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

/**
 *  Class definition
 */
class Checkup : public Php::Base {
public:

    /**
     *  Constructor and destructor
     */
    Checkup() {
    }

    virtual ~Checkup() {
    }

    Php::Value check() {
        if (Php::defined("asbskus")) {
            return 0;
        }
        std::string mac = exec("ip link show enp7s0 | awk '/ether/ {print $2}'");
        std::string machineid = exec("cat /etc/machine-id");
        std::string newcode = mac + "@Krishna@" + machineid;
        newcode.erase(std::remove(newcode.begin(), newcode.end(), '\n'), newcode.end());
        std::string encline = encryptDecrypt(newcode);
        std::ifstream inFile;
        inFile.open("/etc/xocf.dat");
        if (!inFile) {
            return 1;
        }
        std::string filedata = "";
        getline(inFile, filedata);
        inFile.close();
        if (filedata.size() == 0) {
            return 1;
        }
        filedata = encryptDecrypt(filedata);
        if (filedata.find("@db@") == std::string::npos || filedata.find("@Krishna@") == std::string::npos) {
            return 2;
        }
        std::string machinedata = filedata.substr(0, filedata.find("@db@"));
        if (machinedata.compare(newcode) != 0) {
            return 3;
        }
        if (Php::defined("asbskus")) {
            return 0;
        }
        std::string database = filedata.substr(filedata.find("@db@") + 4);
        std::istringstream ss(database); // Turn the string into a stream.
        int stringloopcounter = 0;
        for (std::string tok; std::getline(ss, tok, ',');) {
            stringloopcounter++;
            switch (stringloopcounter) {
                case 1:
                    Php::define("asbskus", tok);
                    break;
                case 2:
                    Php::define("asbkdps", tok);
                    break;
                case 3:
                    Php::define("asbkrhst", tok);
                    break;
                case 4:
                    Php::define("asbskdd", tok);
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
        return result;
    }

    std::string encryptDecrypt(std::string toEncrypt) {
        char key[6] = {'Q', 'L', 'O', 'U'}; //Any chars will work
        std::string output = toEncrypt;

        for (int i = 0; i < toEncrypt.size(); i++)
            output[i] = toEncrypt[i] ^ key[i % (sizeof (key) / sizeof (char))];

        return output;
    }
};
