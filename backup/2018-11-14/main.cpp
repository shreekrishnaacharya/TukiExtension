
#include <phpcpp.h>
#include "Checkup.h"
#include "Exam.h"
#include "TempToken.h"



extern "C" {

    PHPCPP_EXPORT void *get_module() {
        static Php::Extension extension("skeleton", "1.0");
        Php::Class<Exam> exam("exam");

        // the master class has one method - to return a child
        exam.method<&Exam::calculate>("calculate");
        exam.method<&Exam::markCalc>("markCalc");
        exam.method<&Exam::gradeCalc>("gradeCalc");

        exam.property("subjectData", &Exam::getsubjectData, &Exam::setsubjectData);
        exam.property("sheetResult", &Exam::getsheetResult, &Exam::setsheetResult);
        exam.property("graceTrack", &Exam::getgraceTrack, &Exam::setgraceTrack);
        exam.property("graceMark", &Exam::getgraceMark, &Exam::setgraceMark);
        exam.property("markSheet", &Exam::getmarkSheet, &Exam::setmarkSheet);
        exam.property("gradeSheet", &Exam::getgradeSheet, &Exam::setgradeSheet);

        exam.property("studentToAca", &Exam::getstudentToAca, &Exam::setstudentToAca);
        exam.property("studentRemark", &Exam::getstudentRemark, &Exam::setstudentRemark);

        //extension definition for TempToken
        Php::Class<TempToken> temptoken("temptoken");

        // the master class has one method - to return a child
        temptoken.method<&TempToken::filterToken>("filterToken");

        temptoken.property("token", &TempToken::gettoken, &TempToken::settoken);
        temptoken.property("message", &TempToken::getmessage, &TempToken::setmessage);

        //extension definition for Checkup
        Php::Class<Checkup> checkup("checkup");
        checkup.method<&Checkup::check>("check");

        //extension definition for KrishnaSecurity
//        Php::Class<KrishnaSecurity> krishnasecurity("krishnasecurity");
//        krishnasecurity.method<&KrishnaSecurity::encrypt>("encrypt");
//        krishnasecurity.method<&KrishnaSecurity::decrypt>("decrypt");
//        extension.add(krishnasecurity);

        extension.add(temptoken);
        extension.add(exam);
        extension.add(checkup);
        extension.add(Php::Constant("lockva", 1595951302));
        return extension;
    }
}
