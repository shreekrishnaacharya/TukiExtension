/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#pragma once
#include <map>
#include <algorithm>
#include <string>

/* 
 * File:   Navigation.h
 * Author: krishna
 *
 * Created on March 21, 2019, 12:09 PM
 */

class Navigate : public Php::Base {
private:
    Php::Value die;
public:
    Php::Value module;
    Php::Value controller;
    Php::Value action;
    std::map<std::string, std::map<std::string, std::string>> navlink;
    Php::Value image;
    Php::Value mainurl;
    std::map<std::string, std::string> role;

    Navigate() {
        Checkup checkup;
        if (checkup.check() > 0) {
            die("Your application is not registered. Please contact your application provider.");
        }
    }

    virtual ~Navigate() {
    }

    Php::Value getmodule() const {
        return module;
    }

    void setmodule(const Php::Value &value) {
        module = value;
    }

    Php::Value getcontroller() const {
        return controller;
    }

    void setcontroller(const Php::Value &value) {
        controller = value;
    }

    Php::Value getaction() const {
        return action;
    }

    void setaction(const Php::Value &value) {
        action = value;
    }

    Php::Value getnavlink() const {
        return navlink;
    }

    void setnavlink(const Php::Value &value) {
        navlink = value;
    }

    Php::Value getimage() const {
        return image;
    }

    void setimage(const Php::Value &value) {
        image = value;
    }

    Php::Value getmainurl() const {
        return mainurl;
    }

    void setmainurl(const Php::Value &value) {
        mainurl = value;
    }

    Php::Value getrole() const {
        return role;
    }

    void setrole(const Php::Value &value) {
        role = value;
    }

    Php::Value looper(Php::Parameters &params) {
        int count = params[1];
        return looperinner(params[0], count);
    }

    std::string looperinner(Php::Value paramm, int count) {
        std::string content = "";
        std::string tag = "";
        Php::Value param = sortmap(paramm);
        for (auto &iter : param) {
            if (iter.second.contains("child")) {
                std::string urlcheck = module;
                if (checkNav(urlcheck, iter.second["controller"], false) == false) {
                    continue;
                }
            } else {
                std::string urlcheck = iter.second["url"][0];
                if (checkNav(urlcheck, "", true) == false) {
                    continue;
                }
            }
            std::string name = iter.second["name"];
            if (!iter.second.contains("icon")) {
                tag = "<span class='title'>" + name + "</span>";
            } else {
                std::string icon = iter.second["icon"];
                std::string imgg = image;
                tag = "<div class='gui-icon' data-url='" + makeurl(iter.second["url"]) + "'><img style='height:30px;width:30px;margin:5px;' src='" + imgg + icon + "'/></div>";
                tag += "<span class='title'> " + name + "</span>";
            }

            std::string lclass = "";
            std::string aclass = "";
            std::string childview;

            if (iter.second.contains("child")) {
                lclass = "gui-folder";
                count++;
                std::string childvie = looperinner(iter.second["child"], count);
                childview = childvie;
                if (childvie.size() == 0) {
                    continue;
                }
            } else {
                std::string modl = module;
                std::string contrl = controller;
                std::string actn = action;
                std::string flurl = iter.second["url"][0];
                aclass = lclass = ("/" + modl + "/" + contrl + "/" + actn) == flurl ? "active" : "";
            }
            if (iter.second.contains("child")) {
                content += "<li class='" + lclass + "'><a class='" + aclass + "'>" + tag + "</a>";
            } else {
                content += "<li class='" + lclass + "'><a href='" + makeurl(iter.second["url"]) + "' class='" + aclass + "'>" + tag + "</a>";
            }
            if (iter.second.contains("child")) {
                content += "<ul>" + childview + "</ul>";
            }
            content += "</li>";
        }
        return content;
    }

    Php::Value urlLooper(Php::Parameters &params) {
        return urlLooperinner(params[0]);
    }

    Php::Value urlLooperinner(Php::Value param) {
        Php::Value content = "";
        if (param.contains("child")) {
            Php::Value childend = sortmap(param["child"]);
            for (auto &iter : childend) {
                content = urlLooperinner(iter.second);
                if (content.size() > 0) {
                    break;
                }
            }
        } else {
            std::string urlcheck = param["url"][0];
            if (checkNav(urlcheck, "", true) == false) {
                content = "";
            } else {
                content = param["url"];
            }
        }
        return content;
    }

    bool checkNav(std::string url, std::string control, bool flag) {
        bool output = false;
        if (role.find("1") != role.end()) {
            return true;
        }
        if (flag) {
            std::map<int, std::string> controls = explode(url, '/');
            for (auto &iter : navlink) {
                if (iter.second["module"] == controls[0] && iter.second["controller"] == controls[1] && iter.second["action"] == controls[2]) {
                    output = true;
                    break;
                }
            }
        } else {
            for (auto &iter : navlink) {
                if (iter.second["module"] == url && iter.second["controller"] == control) {
                    output = true;
                    break;
                }
            }
        }
        return output;
    }

    std::string makeurl(Php::Value url) {
        std::string result = "";
        std::string mnurl = mainurl;
        for (auto &iter : url) {
            std::string firstitr = iter.first;
            std::string seconditr = iter.second;
            if (firstitr == "0") {
                result = mnurl + "?r=" + seconditr;
                continue;
            }
            result += "&" + firstitr + "=" + seconditr;
        }
        return result;
    }

    Php::Value sortmap(Php::Value map) {
        std::map<std::string, Php::Value> sorted;
        for (auto &iter : map) {
            std::string key = iter.second["order"];
            if (key.size() == 1) {
                key = "0" + key;
            }
            sorted[key] = iter.second;
        }
        return sorted;
    }

    std::map<int, std::string> explode(string text, char key) {
        std::map<int, std::string> output;
        string word;
        stringstream stream(text);
        int i = 0;
        while (getline(stream, word, key)) {
            if (word != "") {
                output[i] = word;
                i++;
            }
        }
        return output;
    }
};

