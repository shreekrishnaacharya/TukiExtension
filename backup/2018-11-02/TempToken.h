/**
 *  TempToken.h
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

/**
 *  Class definition
 */
class TempToken : public Php::Base {
private:
    Php::Value strpos, str_replace, date, explode, die;
public:
    std::map<std::string, std::string> token;
    Php::Value message;

    /**
     *  Constructor and destructor
     */
    TempToken() {
        die = "die";
        Checkup checkup;
        if (checkup.check() > 0) {
            die("Your application is not registered. Please contact your application provider.");
        }
        strpos = "strpos";
        str_replace = "str_replace";
        date = "date";
        explode = "explode";
    }

    virtual ~TempToken() {
    }

    Php::Value gettoken() const {
        return token;
    }

    void settoken(const Php::Value &value) {
        token = explode(",", value);
    }

    Php::Value getmessage() const {
        return message;
    }

    void setmessage(const Php::Value &value) {
        message = value;
    }

    Php::Value filterToken(Php::Parameters &params) {
        Php::Value orgg = params[1];
        std::map<std::string, std::string> org = orgg;
        Php::Value msg = message;
        Php::Array model = params[0];
        model["{{org_logo}}"] = "<img height='50' src='images/'>";
        model["{{org_name}}"] = org["college_name"];
        model["{{org_district}}"] = org["district"];
        model["{{org_address}}"] = org["address"];
        model["{{org_city}}"] = org["city"];
        model["{{org_phone1}}"] = org["phone1"];
        model["{{org_phone2}}"] = org["phone2"];
        model["{{current_date}}"] = date("Y-m-d");
        model["{{current_date_time}}"] = date("Y-m-d g:i a");
        model["{{notice}}"] = "<p class='notice_detail'></p>";
        std::map<std::string, std::string> mapmodel = model;
        std::map<std::string, std::string>::iterator itr, tk;
        for (tk = token.begin(); tk != token.end(); tk++) {
            if (strpos(msg, tk->second) != false) {
                itr = mapmodel.find(tk->second);
                if (itr == mapmodel.end()) {
                    msg = str_replace(tk->second, "", msg);
                } else {
                    msg = str_replace(tk->second, itr->second, msg);
                }
            }
        }
        return msg;
    }

};
