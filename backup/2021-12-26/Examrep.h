/**
 *  ExamRep.h
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

#include <utility>
#include <map>

/**
 *  Class definition
 */
class Examrep : public Php::Base {
private:
    /**
     *  @var Php::Value
     */
    Php::Value die;
public:
    std::map<std::string, std::map<std::string, std::map<std::string, std::map<std::string, int>>>> resultReport;
    std::map<std::string, std::map<std::string, std::map<std::string, int>>> excelReport;
    std::map<std::string, std::map<std::string, std::map<std::string, std::map<std::string, std::string>>>> resultSheet;

    /**
     *  Constructor and destructor
     */
    Examrep() {
        die = "die";
        Checkup checkup;
        if (checkup.check() > 0) {
            throw Php::Exception("Your application is not registered. Please contact your application provider.");
        }
    }

    virtual ~Examrep() {
    }

    Php::Value getresultReport() const {
        return resultReport;
    }

    void setresultReport(const Php::Value &value) {
        resultReport = value;
    }

    Php::Value getexcelReport() const {
        return excelReport;
    }

    void setexcelReport(const Php::Value &value) {
        excelReport = value;
    }

    Php::Value getresultSheet() const {
        return resultSheet;
    }

    void setresultSheet(const Php::Value &value) {
        resultSheet = value;
    }



    /*
     * @var
     * var1 subject list of student
     * var2 gender of student
     * var2 ethnic of student
     * var4 program group id of student
     * var5 remark (PASSED/FAILED) of student
     * 
     */
    void stage1(Php::Parameters &params) {
        std::map<std::string, std::map < std::string, std::string>> subjects = params[0];
        std::map<std::string, std::map<std::string, std::map < std::string, int>>> resultR;
        double gender = std::stod(params[1]);
        std::string ethnic = params[2];
        std::string pgid = params[3];
        std::string remarkFlag = params[4];
        int absentFlag = 0;
        resultR = resultReport[pgid];
        for (auto &subt : subjects) {
            if (resultR["subject"].find(subt.first) == resultR["subject"].end()) {
                resultR["subject"][subt.first]["PASSED"] = 0;
                resultR["subject"][subt.first]["FAILED"] = 0;
                resultR["subject"][subt.first]["ABSENT"] = 0;
            }
            if (absentFlag == 0 && subt.second["att_status"] == "ABS") {
                absentFlag = 1;
            }
            if (subt.second["att_status"] == "F") {
                absentFlag = 2;
            }
            if (subt.second["att_status"] == "ABS") {
                resultR["subject"][subt.first]["ABSENT"] += 1;
            } else if (subt.second["att_status"] == "") {
                resultR["subject"][subt.first]["PASSED"] += 1;
            } else {
                resultR["subject"][subt.first]["FAILED"] += 1;
            }
        }

        if (resultR["ethnic"].find(ethnic) == resultR["ethnic"].end()) {
            resultR["ethnic"][ethnic]["PASSED"] = 0;
            resultR["ethnic"][ethnic]["FAILED"] = 0;
            resultR["ethnic"][ethnic]["ABSENT"] = 0;
        }

        if (resultR.find("gender") == resultR.end()) {
            resultR["gender"]["BOYS"]["PASSED"] = 0;
            resultR["gender"]["BOYS"]["FAILED"] = 0;
            resultR["gender"]["BOYS"]["ABSENT"] = 0;
            resultR["gender"]["GIRLS"]["PASSED"] = 0;
            resultR["gender"]["GIRLS"]["FAILED"] = 0;
            resultR["gender"]["GIRLS"]["ABSENT"] = 0;
            resultR["gender"]["OTHER"]["PASSED"] = 0;
            resultR["gender"]["OTHER"]["FAILED"] = 0;
            resultR["gender"]["OTHER"]["ABSENT"] = 0;
        }
        if (absentFlag == 1) {
            if (gender == 0) {
                resultR["gender"]["GIRLS"]["ABSENT"] += 1;
            } else if (gender == 1) {
                resultR["gender"]["BOYS"]["ABSENT"] += 1;
            } else {
                resultR["gender"]["OTHER"]["ABSENT"] += 1;
            }
            resultR["ethnic"][ethnic]["ABSENT"] += 1;
        } else if (remarkFlag == "PASSED") {
            if (gender == 0) {
                resultR["gender"]["GIRLS"]["PASSED"] += 1;
            } else if (gender == 1) {
                resultR["gender"]["BOYS"]["PASSED"] += 1;
            } else {
                resultR["gender"]["OTHER"]["PASSED"] += 1;
            }
            resultR["ethnic"][ethnic]["PASSED"] += 1;
        } else {
            if (gender == 0) {
                resultR["gender"]["GIRLS"]["FAILED"] += 1;
            } else if (gender == 1) {
                resultR["gender"]["BOYS"]["FAILED"] += 1;
            } else {
                resultR["gender"]["OTHER"]["FAILED"] += 1;
            }
            resultR["ethnic"][ethnic]["FAILED"] += 1;
        }
        resultReport[pgid] = resultR;
    }

    /*
     * @var
     * var1 subject list of student
     * var2 student id
     * var3 gender of student
     * var4 ethnic of student
     * var5 remark (PASSED/FAILED) of student
     * 
     */
    void stage2(Php::Parameters &params) {
        std::map<std::string, std::map < std::string, std::string>> subjects = params[0];
        std::string studentid = params[1];
        double gender = std::stod(params[2]);
        std::string ethnic = params[3];
        std::string remarkFlag = params[4];
        int absentFlag = 0;
        for (auto &subt : subjects) {
            if (excelReport["subject"].find(subt.first) == excelReport["subject"].end()) {
                excelReport["subject"][subt.first]["PASSED"] = 0;
                excelReport["subject"][subt.first]["FAILED"] = 0;
                excelReport["subject"][subt.first]["ABSENT"] = 0;
            }
            if (absentFlag == 0 && subt.second["att_status"] == "ABS") {
                absentFlag = 1;
            }
            if (subt.second["att_status"] == "F") {
                absentFlag = 2;
            }
            if (subt.second["att_status"] == "ABS") {
                excelReport["subject"][subt.first]["ABSENT"] += 1;
                resultSheet["subject"][subt.first]["ABSENT"][studentid] = subt.second["AM"];
            } else if (subt.second["att_status"] == "") {
                excelReport["subject"][subt.first]["PASSED"] += 1;
                resultSheet["subject"][subt.first]["PASSED"][studentid] = subt.second["AM"];
            } else {
                excelReport["subject"][subt.first]["FAILED"] += 1;
                resultSheet["subject"][subt.first]["FAILED"][studentid] = subt.second["AM"];
            }
        }

        if (excelReport["ethnic"].find(ethnic) == excelReport["ethnic"].end()) {
            excelReport["ethnic"][ethnic]["PASSED"] = 0;
            excelReport["ethnic"][ethnic]["FAILED"] = 0;
            excelReport["ethnic"][ethnic]["ABSENT"] = 0;
        }

        if (excelReport.find("gender") == excelReport.end()) {
            excelReport["gender"]["BOYS"]["PASSED"] = 0;
            excelReport["gender"]["BOYS"]["FAILED"] = 0;
            excelReport["gender"]["BOYS"]["ABSENT"] = 0;
            excelReport["gender"]["GIRLS"]["PASSED"] = 0;
            excelReport["gender"]["GIRLS"]["FAILED"] = 0;
            excelReport["gender"]["GIRLS"]["ABSENT"] = 0;
            excelReport["gender"]["OTHER"]["PASSED"] = 0;
            excelReport["gender"]["OTHER"]["FAILED"] = 0;
            excelReport["gender"]["OTHER"]["ABSENT"] = 0;
        }
        if (absentFlag == 1) {
            if (gender == 0) {
                excelReport["gender"]["GIRLS"]["ABSENT"] += 1;
                resultSheet["gender"]["GIRLS"]["ABSENT"][studentid] = studentid;
            } else if (gender == 1) {
                excelReport["gender"]["BOYS"]["ABSENT"] += 1;
                resultSheet["gender"]["BOYS"]["ABSENT"][studentid] = studentid;
            } else {
                excelReport["gender"]["OTHER"]["ABSENT"] += 1;
                resultSheet["gender"]["OTHER"]["ABSENT"][studentid] = studentid;
            }
            excelReport["ethnic"][ethnic]["ABSENT"] += 1;
            resultSheet["ethnic"][ethnic]["ABSENT"][studentid] = studentid;
        } else if (remarkFlag == "PASSED") {
            if (gender == 0) {
                excelReport["gender"]["GIRLS"]["PASSED"] += 1;
                resultSheet["gender"]["GIRLS"]["PASSED"][studentid] = studentid;
            } else if (gender == 1) {
                excelReport["gender"]["BOYS"]["PASSED"] += 1;
                resultSheet["gender"]["BOYS"]["PASSED"][studentid] = studentid;
            } else {
                excelReport["gender"]["OTHER"]["PASSED"] += 1;
                resultSheet["gender"]["OTHER"]["PASSED"][studentid] = studentid;
            }
            excelReport["ethnic"][ethnic]["PASSED"] += 1;
            resultSheet["ethnic"][ethnic]["PASSED"][studentid] = studentid;
        } else {
            if (gender == 0) {
                excelReport["gender"]["GIRLS"]["FAILED"] += 1;
                resultSheet["gender"]["GIRLS"]["FAILED"][studentid] = studentid;
            } else if (gender == 1) {
                excelReport["gender"]["BOYS"]["FAILED"] += 1;
                resultSheet["gender"]["BOYS"]["FAILED"][studentid] = studentid;
            } else {
                excelReport["gender"]["OTHER"]["FAILED"] += 1;
                resultSheet["gender"]["OTHER"]["FAILED"][studentid] = studentid;
            }
            excelReport["ethnic"][ethnic]["FAILED"] += 1;
            resultSheet["ethnic"][ethnic]["FAILED"][studentid] = studentid;
        }
    }

};