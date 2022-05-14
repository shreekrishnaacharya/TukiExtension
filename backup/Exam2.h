
#include <math.h>
#include <utility>
#include <memory>
#include <map>
#pragma once

class Exmsubject : public Php::Base {
public:
    std::string name;
    std::string sname;
    std::string code;
    double credit;
    double fm = 0;
    double pm = 0;
    double highest = 0;
    std::map<std::string, std::map<std::string, double>>type;

    Php::Value getname() const {
        return name;
    }

    void setname(const Php::Value &value) {
        name = value;
    }

    Php::Value getsname() const {
        return sname;
    }

    void setsname(const Php::Value &value) {
        sname = value;
    }

    Php::Value getcode() const {
        return code;
    }

    void setcode(const Php::Value &value) {
        code = value;
    }

    Php::Value getcredit() const {
        return credit;
    }

    void setcredit(const Php::Value &value) {
        credit = value;
    }

    Php::Value getfm() const {
        return fm;
    }

    void setfm(const Php::Value &value) {
        fm = value;
    }

    Php::Value getpm() const {
        return pm;
    }

    void setpm(const Php::Value &value) {
        pm = value;
    }

    Php::Value gethighest() const {
        return highest;
    }

    void sethighest(const Php::Value &value) {
        highest = value;
    }
};

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
    std::map<std::string, Exmsubject> subjectData;
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
            if (subjectData.find(iter.second["subject_code"]) == subjectData.end()) {
                Exmsubject subobj;
                subobj.code = iter.second["subject_code"];
                subobj.name = iter.second["subject_name"];
                subobj.sname = iter.second["subject_sname"];
                subobj.credit = std::stod(iter.second["credit"]);
                subjectData[iter.second["subject_code"]] = subobj;
            } else if (subjectData[iter.second["subject_code"]].type.find(iter.second["subject_type"]) == subjectData[iter.second["subject_code"]].type.end()) {
                subjectData[iter.second["subject_code"]].type[iter.second["subject_type"]]["fm"] = std::stod(iter.second["fm"]);
                subjectData[iter.second["subject_code"]].type[iter.second["subject_type"]]["pm"] = std::stod(iter.second["pm"]);
            }

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
        Php::Array subject, temp2, temp3;

        double grade, gradeCount, pmcount, countOM, countFM, total_fm, fm, pm, mark;
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
                    fm = std::stod(stype.second["fm"]);
                    pm = std::stod(stype.second["pm"]);
                    mark = std::stod(stype.second["mark"]);
                    countFM = countFM + (double) fm;
                    countOM = countOM + (double) mark;
                    pmcount = pmcount + (double) pm;
                    gradeCount = round((countOM / countFM) * (double) formula[0]["base"], 2);
                    temp2[stype.first] = (double) mark;
                    temp2["AM"] += (double) mark;
                    temp2["FG"] += (double) mark;
                    temp3["total_marks"] += (double) mark;
                }
                subject[ssub.first] = temp2;
                temp3["subject"] = subject;
                total_fm = total_fm + countFM;
                temp3["total_points"] += gradeCount;
                if (subjectData[ssub.first].fm == 0) {
                    subjectData[ssub.first].fm = countFM;
                    subjectData[ssub.first].pm = pmcount;
                }
                if (subjectData[ssub.first].highest < (double) temp2["AM"]) {
                    subjectData[ssub.first].highest = (double) temp2["AM"];
                }
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
        Php::Value looper1, looper2, finalGrade, formula = params[0];
        Php::Array subject, temp2, temp3;
        double grade, gradePoint, gradeCount, pmcount, countOM, countFM, fm, pm, mark;
        for (auto &sstud : sheetResult) {
            temp3["total_points"] = 0;
            temp3["total_credit"] = 0;
            looper1 = sstud.second;
            for (auto &ssub : looper1) {
                grade = gradeCount = pmcount = countOM = countFM = gradePoint = 0;
                finalGrade = "N";
                looper2 = ssub.second;
                for (auto &stype : looper2) {
                    fm = std::stod(stype.second["fm"]);
                    pm = std::stod(stype.second["pm"]);
                    mark = std::stod(stype.second["mark"]);
                    grade = round((mark / fm) * (double) formula[0]["base"], 2);
                    countFM = countFM + fm;
                    countOM = countOM + mark;
                    pmcount = pmcount + pm;
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
                temp3["total_credit"] = (double) temp3["total_credit"] + subjectData[ssub.first].credit;
                temp3["total_points"] = (double) temp3["total_points"] + (gradePoint * subjectData[ssub.first].credit);
                if (subjectData[ssub.first].fm == 0) {
                    subjectData[ssub.first].fm = countFM;
                    subjectData[ssub.first].pm = pmcount;
                }
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
