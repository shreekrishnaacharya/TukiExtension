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
    std::map<std::string, std::map<std::string, std::string>> trackSubject;
    std::map<std::string, std::map<std::string, std::string>> trackStudent;

    /**
     *  Constructor and destructor
     */
    Examrep() {
        die = "die";
        Checkup checkup;
        if (checkup.check() > 0) {
            die("Your application is not registered. Please contact your application provider.");
        }
    }

    virtual ~Examrep() {
    }

    Php::Value gettrackSubject() const {
        return trackSubject;
    }

    void settrackSubject(const Php::Value &value) {
        trackSubject = value;
    }

    Php::Value gettrackStudent() const {
        return trackStudent;
    }

    void settrackStudent(const Php::Value &value) {
        trackStudent = value;
    }

    Php::Value stage1(Php::Parameters &params) {
        std::map<std::string, std::map < std::string, std::string>> queryData = params[0];
        std::map<std::string, std::map < std::string, std::map<std::string, std::map < std::string, int>>>> resultReport;

        float pm, mark;
        for (auto &iter : queryData) {
            pm = std::stof(iter.second["pm"]);
            mark = std::stof(iter.second["mark"]);
            if (trackStudent.find(iter.second["student_id"]) == trackStudent.end()) {
                trackStudent[iter.second["student_id"]]["pg"] = iter.second["pg"];
                trackStudent[iter.second["student_id"]]["name"] = iter.second["student_name"];
                trackStudent[iter.second["student_id"]]["gender"] = iter.second["gender"];
                trackStudent[iter.second["student_id"]]["ethnic"] = iter.second["ethnic"];
            }
            if (trackSubject.find(iter.second["student_id"]) == trackSubject.end()) {
                if (iter.second["att_status"] == "0") {
                    trackSubject[iter.second["student_id"]][iter.second["subject_name"]] = "ABSENT";
                } else if (pm <= mark) {
                    trackSubject[iter.second["student_id"]][iter.second["subject_name"]] = "PASSED";
                } else {
                    trackSubject[iter.second["student_id"]][iter.second["subject_name"]] = "FAILED";
                }
            } else if (trackSubject[iter.second["student_id"]].find(iter.second["subject_name"]) == trackSubject[iter.second["student_id"]].end()) {
                if (iter.second["att_status"] == "0") {
                    trackSubject[iter.second["student_id"]][iter.second["subject_name"]] = "ABSENT";
                } else if (pm <= mark) {
                    trackSubject[iter.second["student_id"]][iter.second["subject_name"]] = "PASSED";
                } else {
                    trackSubject[iter.second["student_id"]][iter.second["subject_name"]] = "FAILED";
                }
            } else {
                if (pm > mark) {
                    trackSubject[iter.second["student_id"]][iter.second["subject_name"]] = "FAILED";
                }
            }
        }

        for (auto &iter : trackSubject) {
            std::string remarkFlag = "PASSED";
            std::string pgid = trackStudent[iter.first]["pg"];
            double gender = std::stod(trackStudent[iter.first]["gender"]);
            std::string ethnic = trackStudent[iter.first]["ethnic"];
            std::map<std::string, std::string> tmpsubj = iter.second;

            for (auto &subt : tmpsubj) {
                if (resultReport[pgid]["subject"].find(subt.first) == resultReport[pgid]["subject"].end()) {
                    resultReport[pgid]["subject"][subt.first]["PASSED"] = 0;
                    resultReport[pgid]["subject"][subt.first]["FAILED"] = 0;
                    resultReport[pgid]["subject"][subt.first]["ABSENT"] = 0;
                }
                if (subt.second == "ABSENT") {
                    resultReport[pgid]["subject"][subt.first]["ABSENT"] += 1;
                } else if (subt.second == "PASSED") {
                    resultReport[pgid]["subject"][subt.first]["PASSED"] += 1;
                } else {
                    resultReport[pgid]["subject"][subt.first]["FAILED"] += 1;
                }
                if (remarkFlag == "ABSENT" || remarkFlag == "PASSED") {
                    remarkFlag = subt.second;
                }
            }

            if (resultReport[pgid]["ethnic"].find(ethnic) == resultReport[pgid]["ethnic"].end()) {
                resultReport[pgid]["ethnic"][ethnic]["PASSED"] = 0;
                resultReport[pgid]["ethnic"][ethnic]["FAILED"] = 0;
                resultReport[pgid]["ethnic"][ethnic]["ABSENT"] = 0;
            }

            if (resultReport[pgid].find("gender") == resultReport[pgid].end()) {
                resultReport[pgid]["gender"]["PASSED"]["boys"] = 0;
                resultReport[pgid]["gender"]["FAILED"]["boys"] = 0;
                resultReport[pgid]["gender"]["ABSENT"]["boys"] = 0;
                resultReport[pgid]["gender"]["PASSED"]["girls"] = 0;
                resultReport[pgid]["gender"]["FAILED"]["girls"] = 0;
                resultReport[pgid]["gender"]["ABSENT"]["girls"] = 0;
                resultReport[pgid]["gender"]["PASSED"]["other"] = 0;
                resultReport[pgid]["gender"]["FAILED"]["other"] = 0;
                resultReport[pgid]["gender"]["ABSENT"]["other"] = 0;
            }
            if (remarkFlag == "ABSENT") {
                if (gender == 0) {
                    resultReport[pgid]["gender"]["ABSENT"]["girls"] += 1;
                } else if (gender == 1) {
                    resultReport[pgid]["gender"]["ABSENT"]["boys"] += 1;
                } else {
                    resultReport[pgid]["gender"]["ABSENT"]["other"] += 1;
                }
                resultReport[pgid]["ethnic"][ethnic]["ABSENT"] += 1;
            } else if (remarkFlag == "PASSED") {
                if (gender == 0) {
                    resultReport[pgid]["gender"]["PASSED"]["girls"] += 1;
                } else if (gender == 1) {
                    resultReport[pgid]["gender"]["PASSED"]["boys"] += 1;
                } else {
                    resultReport[pgid]["gender"]["PASSED"]["other"] += 1;
                }
                resultReport[pgid]["ethnic"][ethnic]["PASSED"] += 1;
            } else {
                if (gender == 0) {
                    resultReport[pgid]["gender"]["FAILED"]["girls"] += 1;
                } else if (gender == 1) {
                    resultReport[pgid]["gender"]["FAILED"]["boys"] += 1;
                } else {
                    resultReport[pgid]["gender"]["FAILED"]["other"] += 1;
                }
                resultReport[pgid]["ethnic"][ethnic]["FAILED"] += 1;
            }
        }
        return resultReport;
    }

};