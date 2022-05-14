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

/**
 *  Class definition
 */
class AccObj : public Php::Base{
private:
    /**
     *  @var Php::Value
     */
public:
    int from_date;
    int to_date;
    int year_id;
    std::map<std::string, std::string> studentStatus;

    /**
     *  Constructor and destructor
     */
    AccObj() {
        from_date = 0;
        to_date = 0;
        year_id = 0;
        studentStatus["0"] = "0";
        studentStatus["1"] = "1";
    }

    virtual ~AccObj() {
    }

};