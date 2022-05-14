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
#include "DB.h"

/**
 *  Class definition
 */
class ExamHelp : public Php::Base
{
private:
    /**
     *  @var Php::Value
     */
    Php::Value die;
    Checkup checkup;

public:
    /**
     *  Constructor and destructor
     */
    ExamHelp()
    {
        die = "die";
        if (checkup.check() > 0)
        {
            die();
        }
    }

    virtual ~ExamHelp()
    {
    }

    Php::Value getFullRoutine(Php::Parameters &params)
    {
        std::map<std::string, std::string> routineid = params[0];
        DB db;
        std::string query = "SELECT ";
        query += "exa_routine_to_subject.start_time as start_time,";
        query += "exa_routine_to_subject.end_time as end_time,";
        query += "aca_subject.name as subject_name,";
        query += "aca_subject.code as subject_code,";
        query += "ext_subject_type.code as subject_type,";
        query += "aca_program_group.id as program_group,";
        query += "aca_program.code as program,";
        query += "aca_period.code as period,";
        query += "aca_lang_medium.code as lang,";
        query += "aca_shift.code as shift ";
        query += "FROM exa_routine ";
        query += "RIGHT JOIN exa_exam_type ON exa_exam_type.id=exa_routine.fk_exam_type_id ";
        query += "RIGHT JOIN aca_program_group ON aca_program_group.id=exa_routine.fk_program_group_id ";
        query += "RIGHT JOIN aca_program ON aca_program.id=aca_program_group.fk_program_id ";
        query += "RIGHT JOIN aca_period ON aca_period.id=aca_program_group.fk_period_id ";
        query += "RIGHT JOIN aca_lang_medium ON aca_lang_medium.id=aca_program_group.fk_lang_medium ";
        query += "RIGHT JOIN aca_shift ON aca_shift.id=aca_program_group.fk_shift_id ";
        query += "LEFT JOIN exa_routine_to_subject ON exa_routine.id=exa_routine_to_subject.fk_routine_id ";
        query += "RIGHT JOIN ext_subject_type ON ext_subject_type.id=exa_routine_to_subject.fk_subject_type ";
        query += "RIGHT JOIN aca_subject_to_program ON aca_subject_to_program.id=exa_routine_to_subject.fk_sp_id ";
        query += "RIGHT JOIN aca_subject ON aca_subject.id=aca_subject_to_program.fk_subject_id ";
        query += "WHERE exa_routine.id IN (" + db.getCsv(routineid) + ") AND exa_routine_to_subject.r_state=1 ";
        query += "ORDER BY exa_routine_to_subject.start_time,aca_period.serial_no";
        //        return query;
        std::map<std::string, int> column;
        std::map<std::string, std::map<std::string, std::string>> sqlresult;
        column["start_time"] = 0;
        column["end_time"] = 0;
        column["subject_name"] = 0;
        column["subject_code"] = 0;
        column["subject_type"] = 0;
        column["program_group"] = 0;
        column["program"] = 0;
        column["period"] = 0;
        column["lang"] = 0;
        column["shift"] = 0;
        std::map<std::string, std::string> result = checkup.getdecode();
        db.setup("tukidatabase", result["host"], result["user"], result["password"]);
        db.setCounter(100000);
        if (db.query(query, column))
        {
            sqlresult = db.getResult();
        }
        return sqlresult;
    }
};