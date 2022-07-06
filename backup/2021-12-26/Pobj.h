/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <string>

/* 
 * File:   Pobj.h
 * Author: krishna
 *
 * Created on April 5, 2020, 5:52 PM
 */

class Pobj : public Php::Base {
private:
    /**
     *  @var Php::Value
     */
public:
    std::string dbname;

    std::string getDb(std::string name) {
        if (Php::GLOBALS["useparam"].length() > 0) {
            std::string dbnametmp = Php::GLOBALS["useparam"];
            dbname = dbnametmp;
        } else {
            dbname = name;
        }
        return dbname;
    }

};