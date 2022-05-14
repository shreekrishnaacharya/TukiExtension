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
#include <inttypes.h>
#include "DB.h"
#pragma once

/**
 *  Class definition
 */
class GraceCheck {
public:
    double subject;
    std::string flag;
    bool practical = true;

    GraceCheck() {
        subject = 0;
        flag = "OK";
    };

    virtual ~GraceCheck() {
    }
};

class Exam : public Php::Base {
private:
    /**
     *  @
     */
    std::map<std::string, std::map<std::string, std::map<std::string, std::map<std::string, std::string>>>> sheetResult;
    Php::Value die;
    //    std::map<std::string, std::map<std::string, std::map<std::string, std::string>>> graceTrack;

    std::map<std::string, GraceCheck> graceCheck;
    //    Php::Value graceCheck;
    int graceSubject;
    int graceMarks;
    Php::Value graceStatus;
    Php::Value round;
    std::map<std::string, std::map<std::string, std::string>>finalresult;
    Checkup checkup;
public:

    std::map<std::string, std::map<std::string, std::string>> subjectData;
    std::map<std::string, std::map<std::string, int>> failSubjectCount;
    Php::Array studentToAca;
    std::map<std::string, std::map<std::string, std::string>> studentRemark;
    std::map<std::string, std::map<std::string, std::map<std::string, double>>> subjectTypee;
    std::map<std::string, double> subjectHigh;
    Php::Array markSheet;
    Php::Array gradeSheet;
    Php::Array graceTrack;

    /**
     *  Constructor and destructor
     */
    Exam() {
        die = "die";

        if (checkup.check() > 0) {
            die("Your application is not registered. Please contact your application provider.");
        }
        round = "round";
        graceStatus = "NO";
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

    Php::Value getsubjectTypee() const {
        return subjectTypee;
    }

    void setsubjectTypee(const Php::Value &value) {
        subjectTypee = value;
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

    Php::Value getgraceTrack() const {
        return graceTrack;
    }

    void setgraceTrack(const Php::Value &value) {
        graceTrack = value;
    }

    Php::Value calculate(Php::Parameters &params) {
        Php::Value routinedata = params[0];
        std::map<std::string, std::map < std::string, std::string>> markBasket;
        if (Php::is_array(routinedata)) {
            std::map<std::string, std::string> routineid = routinedata;
            markBasket = getMarkLedger(routineid);
        } else {
            std::string routineid = routinedata;
            markBasket = getMarkLedger(routineid);
        }
        if (markBasket.size() == 0) {
            return false;
        }
        double fm, pm, mark;
        for (auto &iter : markBasket) {
            fm = std::stod(iter.second["fm"]);
            pm = std::stod(iter.second["pm"]);
            mark = std::stod(iter.second["mark"]);
            if (fm < 1) {
                throw new Php::Exception("Full marks cannot be zero");
            }
            // array structure for subjects
            if (subjectData.find(iter.second["subject_code"]) == subjectData.end()) {
                subjectData[iter.second["subject_code"]]["credit"] = iter.second["credit"];
                subjectData[iter.second["subject_code"]]["s_name"] = iter.second["subject_sname"];
                subjectData[iter.second["subject_code"]]["name"] = iter.second["subject_name"];
                subjectData[iter.second["subject_code"]]["code"] = iter.second["subject_code"];
                subjectData[iter.second["subject_code"]]["sno"] = iter.second["subject_number"];
            }

            subjectTypee[iter.second["subject_code"]][iter.second["subject_type"]]["fm"] = fm;
            subjectTypee[iter.second["subject_code"]][iter.second["subject_type"]]["pm"] = pm;

            //array structure for students
            sheetResult[iter.second["student_id"]][iter.second["subject_code"]][iter.second["subject_type"]]["mark"] = iter.second["mark"];
            sheetResult[iter.second["student_id"]][iter.second["subject_code"]][iter.second["subject_type"]]["fm"] = iter.second["fm"];
            sheetResult[iter.second["student_id"]][iter.second["subject_code"]][iter.second["subject_type"]]["pm"] = iter.second["pm"];
            sheetResult[iter.second["student_id"]][iter.second["subject_code"]][iter.second["subject_type"]]["att_status"] = iter.second["att_status"] == "1" ? "" : "ABS";
            sheetResult[iter.second["student_id"]][iter.second["subject_code"]][iter.second["subject_type"]]["remark"] = iter.second["remark"];

            if (studentRemark.find(iter.second["student_id"]) == studentRemark.end()) {
                GraceCheck gcheck;
                graceCheck[iter.second["student_id"]] = gcheck;
                studentRemark[iter.second["student_id"]]["remark"] = "PASSED";
                studentToAca[iter.second["student_aca"]] = iter.second["student_aca"];
                failSubjectCount[iter.second["student_id"]][iter.second["subject_code"]] = 0;
                studentRemark[iter.second["student_id"]]["failsub"] = "0";
            }
            studentRemark[iter.second["student_id"]][iter.second["subject_code"] + iter.second["subject_type"]] = "PASSED";
            if (pm > mark) {
                if (iter.second["subject_type"] == "PR") {
                    graceCheck[iter.second["student_id"]].practical = false;
                }
                sheetResult[iter.second["student_id"]][iter.second["subject_code"]][iter.second["subject_type"]]["att_status"] = iter.second["att_status"] == "0" ? "ABS" : "F";
                studentRemark[iter.second["student_id"]]["remark"] = "FAILED";
                failSubjectCount[iter.second["student_id"]][iter.second["subject_code"]] = 1;
                studentRemark[iter.second["student_id"]][iter.second["subject_code"] + iter.second["subject_type"]] = "FAILED";
            }
            if (graceStatus == "YES" && graceCheck[iter.second["student_id"]].flag != "NO" && iter.second["subject_type"] == "TH" && pm > mark) {
                if (((mark / fm)*100) < graceMarks) {
                    graceCheck[iter.second["student_id"]].flag = "NO";
                    continue;
                }
                graceCheck[iter.second["student_id"]].subject++;
                if (graceCheck[iter.second["student_id"]].subject > graceSubject) {
                    graceCheck[iter.second["student_id"]].flag = "NO";
                    continue;
                }
                graceCheck[iter.second["student_id"]].flag = "YES";
            }
        }
        graceProcess();
        for (auto &fsc : failSubjectCount) {
            int count = 0;
            for (auto &fs : fsc.second) {
                count += fs.second;
            }
            studentRemark[fsc.first]["failsub"] = std::to_string(count);
        }
        return true;
    }

    void setGrace(Php::Parameters &params) {
        graceStatus = params[0];
        graceSubject = params[1];
        graceMarks = params[2];
    }

    void graceProcess() {
        if (graceStatus == "YES") {
            int pm, mark;
            for (auto &sstud : graceCheck) {
                if (sstud.second.flag != "YES") {
                    continue;
                }
                for (auto &ssub : sheetResult[sstud.first]) {

                    for (auto &ssubt : ssub.second) {
                        if (ssubt.first != "TH") {
                            continue;
                        }
                        pm = std::stoi(ssubt.second["pm"]);
                        mark = std::stoi(ssubt.second["mark"]);
                        if (pm > mark) {
                            if (sstud.second.practical) {
                                failSubjectCount[sstud.first][ssub.first] = 0;
                                studentRemark[sstud.first]["remark"] = "PASSED";
                            }
                            studentRemark[sstud.first][ssub.first + "TH"] = "PASSED";
                            sheetResult[sstud.first][ssub.first]["TH"]["att_status"] = "";
                            sheetResult[sstud.first][ssub.first]["TH"]["mark"] = ssubt.second["pm"];
                            graceTrack[sstud.first][ssub.first] = std::stod(ssubt.second["pm"]) - std::stod(ssubt.second["mark"]);
                        }
                    }
                }
            }
        }
    }

    void compress(Php::Parameters &params) {
        double divisionValue = params[0];
        string type = params[1];
        Php::Value looper1, looper2;
        double fm, pm, mark, totfm, calcvalue;
        for (auto &sstud : sheetResult) {
            looper1 = sstud.second;
            for (auto &ssub : looper1) {
                looper2 = ssub.second;
                totfm = 0;
                fm = pm = mark = 0;
                for (auto &sty : looper2) {
                    totfm += std::stod(sty.second["fm"]);
                }
                for (auto &stype : looper2) {
                    fm = std::stod(stype.second["fm"]);
                    if (type == "FULL MARK") {
                        calcvalue = fm * (divisionValue / 100);
                    } else {
                        calcvalue = (fm / totfm) * divisionValue;
                    }
                    pm = (std::stod(stype.second["pm"]) / fm) * calcvalue;
                    mark = (std::stod(stype.second["mark"]) / fm) * calcvalue;
                    sheetResult[sstud.first][ssub.first][stype.first]["fm"] = std::to_string(calcvalue);
                    sheetResult[sstud.first][ssub.first][stype.first]["pm"] = std::to_string(pm);
                    sheetResult[sstud.first][ssub.first][stype.first]["mark"] = std::to_string(mark);
                    subjectTypee[ssub.first][stype.first]["fm"] = calcvalue;
                    subjectTypee[ssub.first][stype.first]["pm"] = pm;
                }
            }
        }
    }

    void markCalc(Php::Parameters &params) {
        Php::Value looper1, looper2, formula = params[0];
        Php::Value subject, temp2, temp3;
        bool passes = true;
        double total_credit, total_points, grade, gradeCount, pmcount, countOM, countFM, total_fm, fm, pm, mark;
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
                temp2["att_status"] = "";
                looper2 = ssub.second;
                for (auto &stype : looper2) {
                    fm = std::stod(stype.second["fm"]);
                    pm = std::stod(stype.second["pm"]);
                    mark = std::stod(stype.second["mark"]);
                    countFM = countFM + fm;
                    countOM = countOM + mark;
                    pmcount = pmcount + pm;
                    temp2[stype.first] = mark;
                    temp2["AM"] += mark;
                    temp2["FG"] += mark;
                    temp2["att_status"] = stype.second["att_status"] != "" ? stype.second["att_status"] : temp2["att_status"];
                    total_points += mark;
                }
                subject[ssub.first] = temp2;
                temp3["subject"] = subject;
                total_fm = total_fm + countFM;
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
            temp3["total_fm"] = total_fm;
            grade = round((total_points / total_fm) *100, 2);
            temp3["GPA"] = grade;
            if (studentRemark[sstud.first]["remark"] == "FAILED") {
                grade = 0;
            }
            for (auto &frm : formula) {
                if ((double) frm.second["value_from"] <= grade && (double) frm.second["value_to"] >= grade) {
                    temp3["AG"] = frm.second["grade"];
                    break;
                }
            }
            markSheet[sstud.first] = temp3;
        }
    }

    Php::Value getGradeReport() {
        return finalresult;
    }

    Php::Value getGrade(Php::Parameters &params) {
        std::map<std::string, std::map < std::string, std::map < std::string, std::map < std::string, std::string>>>> gradedata = params[0];
        Php::Value tmpformula, formula1 = params[1];
        Php::Value formula2 = params[2];
        double fm, grade, total_fm, total_marks, total_points, gradePoint, total_credit, mark = 0, basevalue = (double) formula1[0]["base"];
        for (auto &frm : formula1) {
            tmpformula[frm.second["value_from"]] = frm.second;
        }

        for (auto &sstud : gradedata) {
            finalresult[sstud.first]["MARK"] = "0";
            finalresult[sstud.first]["FM"] = "0";
            finalresult[sstud.first]["PERC"] = "0";
            finalresult[sstud.first]["GPA"] = "0";
            finalresult[sstud.first]["AG"] = "E";
            finalresult[sstud.first]["DIVISION"] = "N";
            finalresult[sstud.first]["REMARK"] = "PASSED";
            total_credit = total_points = total_marks = total_fm = 0;
            for (auto &ssubb : sstud.second) {
                for (auto &ssub : ssubb.second) {
                    fm = std::stod(ssub.second["fm"]);
                    mark = std::stod(ssub.second["mark"]);
                    grade = round((mark / fm) * basevalue, 2);
                    gradePoint = 0;
                    for (auto &frm : formula1) {
                        if ((double) frm.second["value_from"] <= grade && (double) frm.second["value_to"] >= grade) {
                            std::stringstream stream;
                            stream << frm.second["grade"];
                            gradedata[sstud.first][ssubb.first][ssub.first]["grade"] = stream.str();
                            gradePoint = frm.second["code"];
                            break;
                        }
                    }
                    if (ssub.first == "final") {
                        gradedata[sstud.first][ssubb.first][ssub.first]["gpa"] = std::to_string(gradePoint);
                        total_marks += mark;
                        total_fm += fm;
                        total_credit = (double) total_credit + std::stod(subjectData[ssubb.first]["credit"]);
                        total_points = (double) total_points + (double) (gradePoint * std::stod(subjectData[ssubb.first]["credit"]));
                    }
                }
            }
            finalresult[sstud.first]["PERC"] = std::to_string((total_marks / total_fm) *100);
            finalresult[sstud.first]["MARK"] = std::to_string(total_marks);
            finalresult[sstud.first]["FM"] = std::to_string(total_fm);

            grade = round((double) total_points / (double) total_credit, 2);
            finalresult[sstud.first]["GPA"] = std::to_string(grade);
            for (auto &frm : tmpformula) {
                if ((double) frm.second["code"] < grade) {
                    continue;
                }
                std::stringstream stream;
                stream << frm.second["grade"];
                finalresult[sstud.first]["AG"] = stream.str();
                break;
            }
            grade = (total_marks / total_fm) *100;
            for (auto &frm : formula2) {
                if ((double) frm.second["value_from"] <= grade && (double) frm.second["value_to"] >= grade) {
                    std::stringstream stream;
                    stream << frm.second["grade"];
                    finalresult[sstud.first]["DIVISION"] = stream.str();
                    break;
                }
            }
            for (auto &finl : gradedata[sstud.first]) {
                if (std::stoi(finl.second["final"]["pm"]) > std::stoi(finl.second["final"]["mark"])) {
                    finalresult[sstud.first]["REMARK"] = "FAILED";
                    break;
                }
            }
        }
        return gradedata;
    }

    void gradeCalc(Php::Parameters &params) {
        Php::Value looper1, looper2, finalGrade, tmpformula, formula = params[0];
        Php::Value subject, temp2, temp3;
        double total_credit, tempgrade, rmrk, total_points, grade, gradePoint, gradeCount, pmcount, countOM, countFM, fm, pm, mark, basevalue = (double) formula[0]["base"];
        for (auto &frm : formula) {
            tmpformula[frm.second["value_from"]] = frm.second;
        }
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
                rmrk = 0;
                temp2["att_status"] = "";
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
                            gradePoint = frm.second["code"];
                            finalGrade = frm.second["grade"];
                        }
                        if ((double) frm.second["value_from"] <= grade && (double) frm.second["value_to"] >= grade) {
                            temp2[stype.first] = frm.second["grade"];
                        }
                    }
                    if (stype.second["att_status"] == "ABS") {
                        temp2["att_status"] = "*@";
                        rmrk = 3;
                        temp2[stype.first] = "";
                    }
                    if (stype.second["remark"] == "2" && rmrk == 1) {
                        rmrk = 2;
                        temp2["att_status"] = "*TP";
                        temp2[stype.first] = "";
                    } else if (stype.first == "TH" && stype.second["remark"] == "2" && rmrk < 2) {
                        rmrk = 1;
                        temp2["att_status"] = "*T";
                        temp2[stype.first] = "";
                    } else if (stype.first == "PR" && stype.second["remark"] == "2" && rmrk < 2) {
                        rmrk = 1;
                        temp2["att_status"] = "*P";
                        temp2[stype.first] = "";
                    }
                }
                temp2["AM"] = gradePoint;
                temp2["FG"] = finalGrade;
                temp2["FFG"] = "";
                temp2["FAG"] = "";
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
            for (auto &frm : tmpformula) {
                if ((double) frm.second["code"] < grade) {
                    continue;
                }
                temp3["AG"] = frm.second["grade"];
                break;
            }
            gradeSheet[sstud.first] = temp3;
        }
    }

    void gradeNebCalc(Php::Parameters &params) {
        Php::Value looper1, looper2, finalGrade, tmpformula, formula = params[0];
        Php::Value subject, temp2, temp3;
        std::string newcode;
        double total_credit, tempgrade, rmrk, total_points, grade, gradePoint, gradeCount, pmcount, countOM, countFM, fm, pm, mark, basevalue = (double) formula[0]["base"];
        for (auto &frm : formula) {
            tmpformula[frm.second["value_from"]] = frm.second;
        }
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
                rmrk = 0;
                temp2["att_status"] = "";
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
                            gradePoint = frm.second["code"];
                            finalGrade = frm.second["grade"];
                        }
                        if ((double) frm.second["value_from"] <= grade && (double) frm.second["value_to"] >= grade) {
                            temp2[stype.first] = frm.second["grade"];
                        }
                    }
                    if (stype.second["att_status"] == "ABS") {
                        temp2["att_status"] = "*@";
                        rmrk = 3;
                        temp2[stype.first] = "";
                    }
                    if (stype.second["remark"] == "2" && rmrk == 1) {
                        rmrk = 2;
                        temp2["att_status"] = "*TP";
                        temp2[stype.first] = "";
                    } else if (stype.first == "TH" && stype.second["remark"] == "2" && rmrk < 2) {
                        rmrk = 1;
                        temp2["att_status"] = "*T";
                        temp2[stype.first] = "";
                    } else if (stype.first == "PR" && stype.second["remark"] == "2" && rmrk < 2) {
                        rmrk = 1;
                        temp2["att_status"] = "*P";
                        temp2[stype.first] = "";
                    }
                }
                temp2["AM"] = gradePoint;
                temp2["FG"] = finalGrade;
                temp2["FFG"] = "";
                temp2["FAG"] = "";
                subject[ssub.first] = temp2;
    //            temp3["subject"] = subject;
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
            std::map<std::string,std::map<std::string,std::string>>subjecttemp=subject;
            for (auto &sbt : subject) {
                    std::string line=sbt.first;
                    if(!checkCode(line)){
                        continue;
                    }
                    int cod = std::stoi(line);
                    if(cod%2!=0){
                        continue;
                    }
                    cod++;
                    newcode=line.substr(0, line.size()-1)+std::to_string(cod).back();
                    subject[sbt.first]["FFG"] = sbt.second["FG"];
                    subject[sbt.first]["FAG"] = sbt.second["AM"];
                    if(subjecttemp.find(newcode)!=subjecttemp.end()){
                        total_credit=std::stod(subjectData[line]["credit"])+std::stod(subjectData[newcode]["credit"]);                     
                        gradeCount=(std::stod(subjecttemp[line]["AM"])*std::stod(subjectData[line]["credit"]))+(std::stod(subjecttemp[newcode]["AM"])*std::stod(subjectData[newcode]["credit"]));
                        tempgrade=round((double) gradeCount / (double) total_credit, 2);
                        for (auto &frm : tmpformula) {
                            if ((double) frm.second["code"] < tempgrade) {
                                continue;
                            }
                            subject[sbt.first]["FFG"] = (frm.second["grade"]);
                            subject[sbt.first]["FAG"] = (frm.second["code"]);
                          //  subject=subjecttemp;
                            
                            break;
                        }
                    }
            }
            temp3["subject"] = subject;
            for (auto &frm : tmpformula) {
                if ((double) frm.second["code"] < grade) {
                    continue;
                }
                temp3["AG"] = frm.second["grade"];
                break;
            }
            gradeSheet[sstud.first] = temp3;
        }
    }

    std::map<std::string, std::map < std::string, std::string >> getMarkLedger(std::string routineid) {
        std::string query = "SELECT ";
        query += "exa_routine_to_subject.id as rsub_id,";
        query += "aca_subject_to_program.id as subject_id,aca_subject_to_program.credit,";
        query += "aca_subject.name as subject_name,aca_subject.s_name as subject_sname,";
        query += "aca_subject.code as subject_code,aca_subject_to_program.subject_number,";
        query += "ext_subject_type.code as subject_type,";
        query += "exa_routine_to_subject.fm,exa_routine_to_subject.pm,exa_routine_to_subject.post_date,";
        query += "student_to_aca.fk_student_id as student_id,";
        query += "exa_marksheet.fk_stu_aca_id as student_aca,";
        query += "exa_marksheet.mark,exa_marksheet.att_status,ext_exam_grade.name as remark ";
        query += "FROM exa_routine_to_subject ";
        query += "LEFT JOIN ext_subject_type ON ext_subject_type.id=exa_routine_to_subject.fk_subject_type ";
        query += "LEFT JOIN aca_subject_to_program ON aca_subject_to_program.id=exa_routine_to_subject.fk_sp_id ";
        query += "LEFT JOIN aca_subject ON aca_subject.id=aca_subject_to_program.fk_subject_id ";
        query += "LEFT JOIN exa_marksheet ON exa_marksheet.fk_routine_sub_id=exa_routine_to_subject.id ";
        query += "LEFT JOIN ext_exam_grade ON ext_exam_grade.id=exa_marksheet.remark ";
        query += "LEFT JOIN student_to_aca ON student_to_aca.id=exa_marksheet.fk_stu_aca_id ";
        query += "WHERE exa_routine_to_subject.post_status=1 AND exa_marksheet.status=1 ";
        query += "AND exa_routine_to_subject.fk_routine_id=" + routineid;
        DB db;
        std::map<std::string, int>column;
        std::map<std::string, std::map < std::string, std::string >> sqlresult;
        column["rsub_id"] = 0;
        column["subject_id"] = 0;
        column["credit"] = 0;
        column["subject_name"] = 0;
        column["subject_sname"] = 0;
        column["subject_code"] = 0;
        column["subject_number"] = 0;
        column["subject_type"] = 0;
        column["fm"] = 0;
        column["pm"] = 0;
        column["post_date"] = 0;
        column["student_id"] = 0;
        column["student_aca"] = 0;
        column["mark"] = 0;
        column["att_status"] = 0;
        column["remark"] = 0;
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column)) {
            sqlresult = db.getResult();
        }
        return sqlresult;
    }

    std::map<std::string, std::map < std::string, std::string >> getMarkLedger(std::map < std::string, std::string > routineid) {
        DB db;
        std::string query = "SELECT ";
        query += "exa_routine_to_subject.id as rsub_id,";
        query += "aca_subject_to_program.id as subject_id,aca_subject_to_program.credit,";
        query += "aca_subject.name as subject_name,aca_subject.s_name as subject_sname,";
        query += "aca_subject.code as subject_code,aca_subject_to_program.subject_number,";
        query += "ext_subject_type.code as subject_type,";
        query += "exa_routine_to_subject.fm,exa_routine_to_subject.pm,exa_routine_to_subject.post_date,";
        query += "student_to_aca.fk_student_id as student_id,";
        query += "exa_marksheet.fk_stu_aca_id as student_aca,";
        query += "exa_marksheet.mark,exa_marksheet.att_status,ext_exam_grade.name as remark ";
        query += "FROM exa_routine_to_subject ";
        query += "LEFT JOIN ext_subject_type ON ext_subject_type.id=exa_routine_to_subject.fk_subject_type ";
        query += "LEFT JOIN aca_subject_to_program ON aca_subject_to_program.id=exa_routine_to_subject.fk_sp_id ";
        query += "LEFT JOIN aca_subject ON aca_subject.id=aca_subject_to_program.fk_subject_id ";
        query += "LEFT JOIN exa_marksheet ON exa_marksheet.fk_routine_sub_id=exa_routine_to_subject.id ";
        query += "LEFT JOIN ext_exam_grade ON ext_exam_grade.id=exa_marksheet.remark ";
        query += "LEFT JOIN student_to_aca ON student_to_aca.id=exa_marksheet.fk_stu_aca_id ";
        query += "WHERE exa_routine_to_subject.post_status=1 AND exa_marksheet.status=1 ";
        query += "AND exa_routine_to_subject.fk_routine_id IN (" + db.getCsv(routineid) + ")";
        std::map<std::string, int>column;
        std::map<std::string, std::map < std::string, std::string >> sqlresult;
        column["rsub_id"] = 0;
        column["subject_id"] = 0;
        column["credit"] = 0;
        column["subject_name"] = 0;
        column["subject_sname"] = 0;
        column["subject_code"] = 0;
        column["subject_number"] = 0;
        column["subject_type"] = 0;
        column["fm"] = 0;
        column["pm"] = 0;
        column["post_date"] = 0;
        column["student_id"] = 0;
        column["student_aca"] = 0;
        column["mark"] = 0;
        column["att_status"] = 0;
        column["remark"] = 0;
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column)) {
            sqlresult = db.getResult();
        }
        return sqlresult;
    }

    bool checkCode(std::string line)
    {
        char* p;
        strtol(line.c_str(), &p, 10);
        return *p == 0;
    }
};