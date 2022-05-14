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
#pragma once

/**
 *  Class definition
 */
class Exam : public Php::Base {
protected:
    /**
     *  @var Php::Value
     */
    Php::Value die;
    Php::Array subjectData;
    Php::Array sheetResult;
    Php::Array graceTrack;
    Php::Array graceMark;
    Php::Value round;

public:
    Php::Array studentToAca;
    Php::Array studentRemark;
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

    Php::Value getsubjectData() const {
        return subjectData;
    }

    void setsubjectData(const Php::Value &value) {
        subjectData = value;
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
        Php::Array markBasket = params[0];
        Php::Array SubjectType, sheet_result1, sheet_result2;
        for (auto &iter : markBasket) {
            if (iter.second["fm"] < 1) {
                throw Php::Exception("Full marks cannot be zero");
            }
            // array structure for subjects
            //            if (Php::empty(subjectData[iter.second["subject_code"]])) {
            subjectData[iter.second["subject_code"]]["name"] = iter.second["subject_name"];
            subjectData[iter.second["subject_code"]]["code"] = iter.second["subject_code"];
            subjectData[iter.second["subject_code"]]["s_name"] = iter.second["subject_sname"];
            subjectData[iter.second["subject_code"]]["credit"] = iter.second["credit"];
            SubjectType[iter.second["subject_type"]]["type"] = iter.second["subject_type"];
            SubjectType[iter.second["subject_type"]]["fm"] = iter.second["fm"];
            SubjectType[iter.second["subject_type"]]["pm"] = iter.second["pm"];
            subjectData[iter.second["subject_code"]]["type"] = SubjectType;

            //array structure for students
            if (Php::empty(studentToAca[iter.second["student_aca"]])) {
                studentToAca[iter.second["student_aca"]] = iter.second["student_aca"];
            }
            //            if (Php::empty(sheetResult[iter.second["student_id"]])) {
            sheet_result1[iter.second["subject_type"]]["mark"] = iter.second["mark"];
            sheet_result1[iter.second["subject_type"]]["fm"] = iter.second["fm"];
            sheet_result1[iter.second["subject_type"]]["pm"] = iter.second["pm"];
            sheet_result1[iter.second["subject_type"]]["att_status"] = iter.second["att_status"] == 1 ? "" : "ABS";
            sheet_result2[iter.second["subject_code"]] = sheet_result1;
            sheetResult[iter.second["student_id"]] = sheet_result2;

            //grace mark filter
            if (!Php::empty(graceMark)) {
                if (!Php::array_key_exists(iter.second["student_id"], graceTrack)) {
                    graceTrack[iter.second["student_id"]]["fail"] = 0;
                    graceTrack[iter.second["student_id"]]["grace"] = true;
                }
            }
            if (!Php::isset(studentRemark[iter.second["student_id"]]["remark"])) {
                studentRemark[iter.second["student_id"]]["remark"] = "PASSED";
            }

            if (iter.second["pm"] > iter.second["mark"]) {
                sheet_result1[iter.second["subject_type"]]["att_status"] = iter.second["att_status"] == 0 ? "ABS" : "F";
                sheet_result2[iter.second["subject_code"]] = sheet_result1;
                sheetResult[iter.second["student_id"]] = sheet_result2;
                studentRemark[iter.second["student_id"]]["remark"] = "FAILED";
                if (!Php::empty(graceMark)) {
                    Php::Value graceRatio = iter.second["fm"] * (graceMark["min_mark"] / 100);
                    if (iter.second["subject_type"] == "PR" || graceTrack[iter.second["student_id"]]["fail"] > graceMark["max_subjects"] || iter.second["mark"] < graceRatio) {
                        graceTrack[iter.second["student_id"]]["grace"] = false;
                    }
                    graceTrack[iter.second["student_id"]]["fail"] += 1; //..grace 
                }
            }
        }
    }

    void markCalc(Php::Parameters &params) {
        Php::Value formula = params[0];
        Php::Array subject, temp2, temp3, looper1, looper2;
        double grade, gradeCount, pmcount, countOM, countFM, total_fm, high_score = 0;
        for (auto &sstud : sheetResult) {
            total_fm = 0;
            temp3["total_points"] = 0;
            temp3["total_marks"] = 0;
            looper1 = sstud.second;
            for (auto &ssub : looper1) {
                grade = gradeCount = pmcount = countOM = countFM = 0;
                temp2["AM"] = 0;
                temp2["FG"] = 0;
                looper2 = ssub.second;
                for (auto &stype : looper2) {
                    countFM = countFM + (double) stype.second["fm"];
                    countOM = countOM + (double) stype.second["mark"];
                    pmcount = pmcount + (double) stype.second["pm"];
                    gradeCount = round((countOM / countFM) * (double) formula[0]["base"], 2);
                    temp2[stype.first] = (double) stype.second["mark"];
                    temp2["AM"] += (double) stype.second["mark"];
                    temp2["FG"] += (double) stype.second["mark"];
                    temp3["total_marks"] += (double) stype.second["mark"];
                }
                subject[ssub.first] = temp2;
                temp3["subject"] = subject;
                total_fm = total_fm + countFM;
                temp3["total_points"] += gradeCount;
                subjectData[ssub.first]["fm"] = countFM;
                subjectData[ssub.first]["pm"] = pmcount;
                if (high_score < (double) temp2["AM"]) {
                    high_score = temp2["AM"];
                }
                subjectData[ssub.first]["HIGNSCORE"] = high_score;
            }
            temp3["GPA"] = "N";
            temp3["AG"] = "N";
            grade = round((temp3["total_marks"] / total_fm) *100, 2);
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
        Php::Value finalGrade, formula = params[0];
        Php::Array subject, temp2, temp3, looper1, looper2;
        double grade, gradePoint, gradeCount, pmcount, countOM, countFM;
        for (auto &sstud : sheetResult) {
            temp3["total_points"] = 0;
            temp3["total_credit"] = 0;
            looper1 = sstud.second;
            for (auto &ssub : looper1) {
                grade = gradeCount = pmcount = countOM = countFM = gradePoint = 0;
                finalGrade = "N";
                looper2 = ssub.second;
                for (auto &stype : looper2) {
                    grade = round(((double) stype.second["mark"] / (double) stype.second["mark"]) * (double) formula[0]["base"], 2);
                    countFM = countFM + (double) stype.second["fm"];
                    countOM = countOM + (double) stype.second["mark"];
                    pmcount = pmcount + (double) stype.second["pm"];
                    gradeCount = round((countOM / countFM) * (double) formula[0]["base"]);
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
                temp3["total_credit"] = (double) temp3["total_credit"] + (double) subjectData[ssub.first]["credit"];
                temp3["total_points"] = (double) temp3["total_points"] + (double) (gradePoint * (double) subjectData[ssub.first]["credit"]);
                subjectData[ssub.first]["fm"] = countFM;
                subjectData[ssub.first]["pm"] = pmcount;
            }
            temp3["GPA"] = "N";
            temp3["AG"] = "N";
            grade = round((double) temp3["total_points"] / (double) temp3["total_credit"], 2);
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