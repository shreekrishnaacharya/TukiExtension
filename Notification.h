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
#include <map>
#pragma once

/**
 *  Class definition
 */
class Notification : public Php::Base {
private:
    Php::Value die;
    std::map<std::string, std::map<std::string, std::string>> msgtype;

public:

    /**
     *  Constructor and destructor
     */
    Notification() {
        die = "die";
        Checkup checkup;
        if (checkup.check() > 0) {
            die("Your application is not registered. Please contact your application provider.");
        }
        msgtype["0"]["0"] = "success";
        msgtype["0"]["1"] = "glyphicon glyphicon-ok-sign";
        msgtype["0"]["2"] = "Success";
        msgtype["1"]["0"] = "error";
        msgtype["1"]["1"] = "glyphicon glyphicon-remove-sign";
        msgtype["1"]["2"] = "Danger";
        msgtype["2"]["0"] = "warning";
        msgtype["2"]["1"] = "glyphicon glyphicon-warning-sign";
        msgtype["2"]["2"] = "Warning";
        msgtype["3"]["0"] = "info";
        msgtype["3"]["1"] = "glyphicon glyphicon-ok-sign";
        msgtype["3"]["2"] = "Info";
    }

    virtual ~Notification() {
    }

    Php::Value output(Php::Parameters &params) {
        std::map<std::string, std::map < std::string, std::string>> messages = params[0];
        std::string msg = "";
        std::string type = params[1];
        for (auto &iter : messages) {
            if (type == "alert") {
                msgtype["1"]["0"] = "danger";
                msg += "<div class='alert alert-" + msgtype[iter.second["type"]]["0"] + " alert-dismissable w3-margin'><a href='' class='close' data-dismiss='alert' aria-label='close'>&times;</a><strong><i class='" + msgtype[iter.second["type"]]["1"] + "'></i>&nbsp;" + msgtype[iter.second["type"]]["2"] + "</strong> " + iter.second["message"] + "</div>";
            } else {
                msg += "toastr['" + msgtype[iter.second["type"]]["0"] + "']('" + iter.second["message"] + "');";
            }
        }
        if (type == "alert") {
            return msg;
        } else {
            return "<script>" + msg + "</script>";
        }
    }

    Php::Value ajax(Php::Parameters &params) {
        std::string msg = params[0];
        std::string type = params[1];
        return "<script>toastr['" + msgtype[type]["0"] + "']('" + msg + "');</script>";
    }

};
