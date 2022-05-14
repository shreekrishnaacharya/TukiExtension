
#include <phpcpp.h>
#include "Checkup.h"
#include "Exam.h"
#include "Examrep.h"
#include "Accrep.h"
#include "TempToken.h"



extern "C" {

    PHPCPP_EXPORT void *get_module() {
        static Php::Extension extension("skeleton", "1.0");
        Php::Class<Exam> exam("exam");

        // the master class has one method - to return a child
        exam.method<&Exam::calculate>("calculate");
        exam.method<&Exam::markCalc>("markCalc");
        exam.method<&Exam::gradeCalc>("gradeCalc");

        //        exam.property("studentTrack", &Exam::getstudentTrack, &Exam::setstudentTrack);
        exam.property("subjectData", &Exam::getsubjectData, &Exam::setsubjectData);
        exam.property("subjectType", &Exam::getsubjectType, &Exam::setsubjectType);
        exam.property("subjectHigh", &Exam::getsubjectHigh, &Exam::setsubjectHigh);
        exam.property("sheetResult", &Exam::getsheetResult, &Exam::setsheetResult);
        exam.property("graceTrack", &Exam::getgraceTrack, &Exam::setgraceTrack);
        exam.property("graceMark", &Exam::getgraceMark, &Exam::setgraceMark);
        exam.property("markSheet", &Exam::getmarkSheet, &Exam::setmarkSheet);
        exam.property("gradeSheet", &Exam::getgradeSheet, &Exam::setgradeSheet);

        exam.property("studentToAca", &Exam::getstudentToAca, &Exam::setstudentToAca);
        exam.property("studentRemark", &Exam::getstudentRemark, &Exam::setstudentRemark);

        Php::Class<Examrep> examrep("examrep");

        // the master class has one method - to return a child
        examrep.method<&Examrep::stage1>("stage1");
        examrep.property("trackStudent", &Examrep::gettrackStudent, &Examrep::settrackStudent);
        examrep.property("trackSubject", &Examrep::gettrackSubject, &Examrep::settrackSubject);
        //        examrep.property("resultReport", &Examrep::getresultReport, &Examrep::setresultReport);

        //extension definition for TempToken
        Php::Class<TempToken> temptoken("temptoken");

        // the master class has one method - to return a child
        temptoken.method<&TempToken::filterToken>("filterToken");

        temptoken.property("token", &TempToken::gettoken, &TempToken::settoken);
        temptoken.property("message", &TempToken::getmessage, &TempToken::setmessage);

        //extension definition for Account Report
        Php::Class<Accrep> accrep("accrep");

        // the master class has one method - to return a child
        accrep.method<&Accrep::trial>("trial");
        accrep.method<&Accrep::balancesheet>("balancesheet");

        //extension definition for Checkup
        Php::Class<Checkup> checkup("checkup");
        checkup.method<&Checkup::check>("check");
//        checkup.method<&Checkup::mypdo>("mypdo");
//        checkup.method<&Checkup::mysql>("mysql");
//        checkup.method<&Checkup::setdb>("setdb");

        //extension definition for KrishnaSecurity
        //        Php::Class<KrishnaSecurity> krishnasecurity("krishnasecurity");
        //        krishnasecurity.method<&KrishnaSecurity::encrypt>("encrypt");
        //        krishnasecurity.method<&KrishnaSecurity::decrypt>("decrypt");
        //        extension.add(krishnasecurity);

        extension.add(temptoken);
        extension.add(exam);
        extension.add(examrep);
        extension.add(accrep);
        extension.add(checkup);
        extension.add(Php::Constant("lockva", 1595951302));
        return extension;
    }
}
