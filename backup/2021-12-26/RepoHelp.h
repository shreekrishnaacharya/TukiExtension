/**
 *  AccObj.h
 * 
 *  Class that is exported to PHP space
 * 
 *  @author Krishna Acharya 
 *  @copyright 2020
 */

/**
 *  Include guard
 */
#pragma once

/**
 *  Class definition
 */
class RepoHelp : public Php::Base {
private:
    /**
     *  @var Php::Value
     */
public:

    /**
     *  Constructor and destructor
     */
    RepoHelp() {
    }

    virtual ~RepoHelp() {
    }

    Php::Value getKey(Php::Parameters &params) {
        int catType = params[0];
        std::map<std::string, std::string> keylist;
        if (params.size() > 1) {
            std::map<std::string, std::string> keytmp = params[1];
            keylist = keytmp;
        }
        std::string keys = "";

        for (auto &iter : keylist) {
            keys += iter.second + ", ";
        }
        switch (catType) {
            case 0:
                keys += "general "
                break;
            case 1:
                keys += "homework, assignment, question collection, questions, hw, todo, problems";
                break;
            case 2:
                keys += "homework, assignment, answer collection, answers, solutions"
                break;
            case 3:
                keys += "course, study material, ebook, elearning, exercise, solutions, class material"
                break;
            case 4:
                keys += "school documents, resources, school gallery, college gallery, organization"
                break;
            case 5:
                keys += "departments, resources, documents, gallery"
                break;
        }
        return keys;
    }

};