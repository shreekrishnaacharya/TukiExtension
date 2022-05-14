/**
 *  Exam.h
 * 
 *  Class that is exported to PHP space
 * 
 *  @author Krishna Acharya 
 *  @copyright 2018
 */

/**
 *  Include guard
 */
#include <math.h>
#include <utility>
#include <memory>
#include <map>
#include <iomanip>
#pragma once

/**
 *  Class definition
 */
class Exam : public Php::Base {
private:
    /**
     *  @var Php::Value
     */
    std::map<std::string, std::map<std::string, std::map<std::string, std::map<std::string, std::string>>>>sheetResult;
    Php::Value die;
    Php::Value graceTrack;
    Php::Value graceMark;
    Php::Value round;
public:
    std::map<std::string, std::map<std::string, std::string>> subjectData;
    Php::Array studentToAca;
    std::map<std::string, std::map<std::string, std::string>> studentRemark;
    std::map<std::string, std::map<std::string, std::map<std::string, double>>> subjectType;
    std::map<std::string, double> subjectHigh;
    Php::Array markSheet;
    Php::Array gradeSheet;

    /**
     *  Constructor and destructor
     */
    Exam() {
        die = "die";
        Checkup checkup;
        if (checkup.check() > 0) {
            die("Your application is not registered. Please contact your application provider.");
        }
        round = "round";
    }

    virtual ~Exam() {
    }

    Php::Value getgradeSheet() const {
        return gradeSheet;
    }

    void setgradeSheet(const Php::Value &value) {
        gradeSheet = value;
    }

    Php::Value getmarkSheet() const {
        return markSheet;
    }

    void setmarkSheet(const Php::Value &value) {
        markSheet = value;
    }

    Php::Value getstudentToAca() const {
        return studentToAca;
    }

    void setstudentToAca(const Php::Value &value) {
        studentToAca = value;
    }

    Php::Value getstudentRemark() const {
        return studentRemark;
    }

    void setstudentRemark(const Php::Value &value) {
        studentRemark = value;
    }

    Php::Value getsubjectType() const {
        return subjectType;
    }

    void setsubjectType(const Php::Value &value) {
        subjectType = value;
    }

    Php::Value getsubjectData() const {
        return subjectData;
    }

    void setsubjectData(const Php::Value &value) {
        subjectData = value;
    }

    Php::Value getsubjectHigh() const {
        return subjectHigh;
    }

    void setsubjectHigh(const Php::Value &value) {
        subjectHigh = value;
    }

    Php::Value getsheetResult() const {
        return sheetResult;
    }

    void setsheetResult(const Php::Value &value) {
        sheetResult = value;
    }

    Php::Value getgraceTrack() const {
        return graceTrack;
    }

    void setgraceTrack(const Php::Value &value) {
        graceTrack = value;
    }

    Php::Value getgraceMark() const {
        return graceMark;
    }

    void setgraceMark(const Php::Value &value) {
        graceMark = value;
    }

    void calculate(Php::Parameters &params) {
        std::map<std::string, std::map < std::string, std::string>> markBasket = params[0];
        double fm, pm, mark;
        for (auto &iter : markBasket) {
            fm = std::stod(iter.second["fm"]);
            pm = std::stod(iter.second["pm"]);
            mark = std::stod(iter.second["mark"]);
            if (fm < 1) {
                throw Php::Exception("Full marks cannot be zero");
            }
            // array structure for subjects
            subjectData[iter.second["subject_code"]]["credit"] = iter.second["credit"];
            subjectData[iter.second["subject_code"]]["s_name"] = iter.second["subject_sname"];
            subjectData[iter.second["subject_code"]]["name"] = iter.second["subject_name"];
            subjectData[iter.second["subject_code"]]["code"] = iter.second["subject_code"];

            subjectType[iter.second["subject_code"]][iter.second["subject_type"]]["fm"] = std::stod(iter.second["fm"]);
            subjectType[iter.second["subject_code"]][iter.second["subject_type"]]["pm"] = std::stod(iter.second["pm"]);


            //array structure for students
            studentToAca[iter.second["student_aca"]] = iter.second["student_aca"];

            sheetResult[iter.second["student_id"]][iter.second["subject_code"]][iter.second["subject_type"]]["mark"] = iter.second["mark"];
            sheetResult[iter.second["student_id"]][iter.second["subject_code"]][iter.second["subject_type"]]["fm"] = iter.second["fm"];
            sheetResult[iter.second["student_id"]][iter.second["subject_code"]][iter.second["subject_type"]]["pm"] = iter.second["pm"];
            sheetResult[iter.second["student_id"]][iter.second["subject_code"]][iter.second["subject_type"]]["att_status"] = iter.second["att_status"] == "1" ? "" : "ABS";

            if (studentRemark.find(iter.second["student_id"]) == studentRemark.end()) {
                studentRemark[iter.second["student_id"]]["remark"] = "PASSED";
            }
            studentRemark[iter.second["student_id"]][iter.second["subject_code"] + iter.second["subject_type"]] = "PASSED";
            if (pm > mark) {
                sheetResult[iter.second["student_id"]][iter.second["subject_code"]][iter.second["subject_type"]]["att_status"] = iter.second["att_status"] == "0" ? "ABS" : "F";
                studentRemark[iter.second["student_id"]]["remark"] = "FAILED";
                studentRemark[iter.second["student_id"]][iter.second["subject_code"] + iter.second["subject_type"]] = "FAILED";
            }
        }
    }

    void markCalc(Php::Parameters &params) {
        Php::Value looper1, looper2, formula = params[0];
        Php::Value subject, temp2, temp3;

        double total_credit, total_points, grade, gradeCount, pmcount, countOM, countFM, total_fm, fm, pm, mark, basevalue = (double) formula[0]["base"];
        for (auto &sstud : sheetResult) {
            subject = nullptr;
            temp2 = nullptr;
            temp3 = nullptr;
            total_fm = 0;
            total_points = 0;
            total_credit = 0;
            looper1 = sstud.second;
            for (auto &ssub : looper1) {
                grade = gradeCount = pmcount = countOM = countFM = 0;
                temp2["AM"] = 0;
                temp2["FG"] = 0;
                looper2 = ssub.second;
                for (auto &stype : looper2) {
                    fm = std::stod(stype.second["fm"]);
                    pm = std::stod(stype.second["pm"]);
                    mark = std::stod(stype.second["mark"]);
                    countFM = countFM + fm;
                    countOM = countOM + mark;
                    pmcount = pmcount + pm;
                    gradeCount = round((countOM / countFM) * basevalue, 2);
                    temp2[stype.first] = mark;
                    temp2["AM"] += mark;
                    temp2["FG"] += mark;
                    total_credit += mark;
                }
                subject[ssub.first] = temp2;
                temp3["subject"] = subject;
                total_fm = total_fm + countFM;
                total_points += gradeCount;
                if (subjectData[ssub.first].find("fm") == subjectData[ssub.first].end()) {
                    std::stringstream stream;
                    stream << std::fixed << setprecision(0) << countFM;
                    subjectData[ssub.first]["fm"] = stream.str();
                    stream.str("");
                    stream << std::fixed << setprecision(0) << pmcount;
                    subjectData[ssub.first]["pm"] = stream.str();
                }

                if (subjectHigh.find(ssub.first) == subjectHigh.end()) {
                    subjectHigh[ssub.first] = temp2["AM"];
                } else {
                    if (subjectHigh[ssub.first] < (double) temp2["AM"]) {
                        subjectHigh[ssub.first] = temp2["AM"];
                    }
                }
                temp2 = nullptr;
            }
            temp3["GPA"] = "N";
            temp3["AG"] = "N";
            temp3["total_credit"] = total_credit;
            temp3["total_marks"] = total_points;
            grade = round((total_credit / total_fm) *100, 2);
            temp3["GPA"] = grade;
            for (auto &frm : formula) {
                if ((double) frm.second["value_from"] <= grade && (double) frm.second["value_to"] >= grade) {
                    temp3["AG"] = frm.second["grade"];
                    break;
                }
            }
            markSheet[sstud.first] = temp3;
        }
    }

    void gradeCalc(Php::Parameters &params) {
        Php::Value looper1, looper2, finalGrade, formula = params[0];
        Php::Value subject, temp2, temp3;
        double total_credit, total_points, grade, gradePoint, gradeCount, pmcount, countOM, countFM, fm, pm, mark, basevalue = (double) formula[0]["base"];
        for (auto &sstud : sheetResult) {
            total_points = 0;
            total_credit = 0;

            subject = nullptr;
            temp2 = nullptr;
            temp3 = nullptr;
            looper1 = sstud.second;
            for (auto &ssub : looper1) {
                grade = gradeCount = pmcount = countOM = countFM = gradePoint = 0;
                finalGrade = "N";
                looper2 = ssub.second;
                for (auto &stype : looper2) {
                    fm = std::stod(stype.second["fm"]);
                    pm = std::stod(stype.second["pm"]);
                    mark = std::stod(stype.second["mark"]);
                    grade = round((mark / fm) * basevalue, 2);
                    countFM = countFM + fm;
                    countOM = countOM + mark;
                    pmcount = pmcount + pm;
                    gradeCount = round((countOM / countFM) * basevalue, 2);
                    for (auto &frm : formula) {
                        if ((double) frm.second["value_from"] <= gradeCount && (double) frm.second["value_to"] >= gradeCount) {
                            gradePoint = frm.second["value_to"];
                            finalGrade = frm.second["code"];
                        }
                        if ((double) frm.second["value_from"] <= grade && (double) frm.second["value_to"] >= grade) {
                            temp2[stype.first] = frm.second["code"];
                        }
                    }
                }
                temp2["AM"] = gradePoint;
                temp2["FG"] = finalGrade;
                subject[ssub.first] = temp2;

                temp3["subject"] = subject;
                total_credit = (double) total_credit + std::stod(subjectData[ssub.first]["credit"]);
                total_points = (double) total_points + (double) (gradePoint * std::stod(subjectData[ssub.first]["credit"]));
                if (subjectData[ssub.first].find("fm") == subjectData[ssub.first].end()) {
                    std::stringstream stream;
                    stream << std::fixed << setprecision(0) << countFM;
                    subjectData[ssub.first]["fm"] = stream.str();
                    stream.str("");
                    stream << std::fixed << setprecision(0) << pmcount;
                    subjectData[ssub.first]["pm"] = stream.str();
                }
                temp2 = nullptr;
            }
            temp3["GPA"] = "N";
            temp3["AG"] = "N";
            temp3["total_credit"] = total_credit;
            temp3["total_points"] = total_points;
            grade = round((double) total_points / (double) total_credit, 2);
            temp3["GPA"] = grade;
            for (auto &frm : formula) {
                if ((double) frm.second["value_from"] <= grade && (double) frm.second["value_to"] >= grade) {
                    temp3["AG"] = frm.second["grade"];
                    break;
                }
            }
            gradeSheet[sstud.first] = temp3;
        }
    }
};