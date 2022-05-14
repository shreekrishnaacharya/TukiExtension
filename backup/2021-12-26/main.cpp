#include <phpcpp.h>
#include "Checkup.h"
#include "Exam.h"
#include "Examrep.h"
#include "Accrep.h"
#include "TempToken.h"
#include "Cright.h"
#include "Notification.h"
#include "Navigate.h"
#include "Server.h"
#include "CoreCard.h"
#include "AccHelp.h"
#include "Importbackup.h"
#include "ExamHelp.h"
#include "DataHelp.h"
#include "AccountAudit.h"
#include "SystemAi.h"
//#include "Serverup.h"


extern "C" {

    PHPCPP_EXPORT void *get_module() {
        static Php::Extension extension("skeleton", "1.0");


        Php::Class<Exam> exam("exam");

        // the master class has one method - to return a child
        exam.method<&Exam::calculate>("calculate");
        exam.method<&Exam::markCalc>("markCalc");
        exam.method<&Exam::gradeCalc>("gradeCalc");
        exam.method<&Exam::gradeNebCalc>("gradeNebCalc");
        exam.method<&Exam::setGrace>("setGrace");
        exam.method<&Exam::compress>("compress");
        exam.method<&Exam::getGrade>("getGrade");
        exam.method<&Exam::getGradeReport>("getGradeReport");

        exam.property("subjectData", &Exam::getsubjectData, &Exam::setsubjectData);
        exam.property("subjectTypee", &Exam::getsubjectTypee, &Exam::setsubjectTypee);
        exam.property("subjectHigh", &Exam::getsubjectHigh, &Exam::setsubjectHigh);
        exam.property("graceTrack", &Exam::getgraceTrack, &Exam::setgraceTrack);
        exam.property("markSheet", &Exam::getmarkSheet, &Exam::setmarkSheet);
        exam.property("gradeSheet", &Exam::getgradeSheet, &Exam::setgradeSheet);

        exam.property("studentToAca", &Exam::getstudentToAca, &Exam::setstudentToAca);
        exam.property("studentRemark", &Exam::getstudentRemark, &Exam::setstudentRemark);


        Php::Class<Examrep> examrep("examrep");

        // the master class has one method - to return a child
        examrep.method<&Examrep::stage1>("stage1");
        examrep.method<&Examrep::stage2>("stage2");
        examrep.property("resultReport", &Examrep::getresultReport, &Examrep::setresultReport);
        examrep.property("excelReport", &Examrep::getexcelReport, &Examrep::setexcelReport);
        examrep.property("resultSheet", &Examrep::getresultSheet, &Examrep::setresultSheet);
        //        examrep.property("resultReport", &Examrep::getresultReport, &Examrep::setresultReport);
        //        extension definition for TempToken
        Php::Class<TempToken> temptoken("temptoken");

        // the master class has one method - to return a child
        temptoken.method<&TempToken::filterToken>("filterToken");
        temptoken.property("token", &TempToken::gettoken, &TempToken::settoken);
        temptoken.property("message", &TempToken::getmessage, &TempToken::setmessage);


        //        //extension definition for Account Report
        Php::Class<Accrep> accrep("accrep");

        // the master class has one method - to return a child
        accrep.method<&Accrep::trial>("trial");
        accrep.method<&Accrep::plAccount>("plAccount");
        accrep.method<&Accrep::getPlHead>("getPlHead");
        accrep.method<&Accrep::balancesheet>("balancesheet");
        accrep.method<&Accrep::setFrom>("setFrom");
        accrep.method<&Accrep::setTo>("setTo");
        accrep.method<&Accrep::setYear>("setYear");
        accrep.method<&Accrep::setLedger>("setLedger");
        accrep.method<&Accrep::setStudentStatus>("setStudentStatus");
        accrep.method<&Accrep::getFeemonth>("getFeemonth");
        accrep.method<&Accrep::getScholarHead>("getScholarHead");
        accrep.method<&Accrep::processFeeReport>("processFeeReport");
        accrep.method<&Accrep::getEmployeeVou>("getEmployeeVou");
        accrep.method<&Accrep::salarySheetEmp>("salarySheetEmp");
        accrep.method<&Accrep::ledgerReport>("ledgerReport");
        accrep.method<&Accrep::advanceReport>("advanceReport");
        accrep.method<&Accrep::bsDetailModel>("bsDetailModel");
        accrep.method<&Accrep::getEmployeeLd>("getEmployeeLd");
        accrep.method<&Accrep::advanceBalance>("advanceBalance");
        accrep.method<&Accrep::getPayrollPostData>("getPayrollPostData");


        accrep.method<&Accrep::getTempDataL1>("getTempDataL1");
        accrep.method<&Accrep::getTempDataL2>("getTempDataL2");
        accrep.method<&Accrep::getTempDataS1>("getTempDataS1");

        //        //extension definition for Checkup
        Php::Class<Checkup> checkup("checkup");
        checkup.method<&Checkup::getid>("getid");
        checkup.method<&Checkup::getClientCode>("getClientCode");
        checkup.method<&Checkup::incbackup>("incbackup");
        checkup.method<&Checkup::fullbackup>("fullbackup");
        checkup.method<&Checkup::check>("check");
        checkup.method<&Checkup::mypdo>("mypdo");
        checkup.method<&Checkup::myquery>("myquery");
        checkup.method<&Checkup::userlog>("userlog");
        checkup.method<&Checkup::getmacid>("getmacid");
        checkup.method<&Checkup::getmacaddress>("getmacaddress");
        checkup.method<&Checkup::getVer>("getVer");
        checkup.method<&Checkup::getDbVer>("getDbVer");
        checkup.method<&Checkup::getFileVer>("getFileVer");
        checkup.method<&Checkup::setFileVer>("setFileVer");
        checkup.method<&Checkup::phpmyadmin>("phpmyadmin");
        checkup.method<&Checkup::systemstate>("systemstate");
        checkup.method<&Checkup::restartApache>("restartApache");
        checkup.method<&Checkup::resetsuper>("resetsuper");
        checkup.method<&Checkup::enablemysqluser>("enablemysqluser");
        checkup.method<&Checkup::disablemysqluser>("disablemysqluser");


        Php::Class<Cright> cright("cright");
        cright.property("version", &Cright::getversion, &Cright::setversion);
        cright.property("copyright", &Cright::getcopyright, &Cright::setcopyright);
        cright.property("developer", &Cright::getdeveloper, &Cright::setdeveloper);
        cright.property("email", &Cright::getemail, &Cright::setemail);
        cright.property("agreement", &Cright::getagreement, &Cright::setagreement);

        Php::Class<Notification> notification("notification");
        notification.method<&Notification::output>("output");
        notification.method<&Notification::ajax>("ajax");


        Php::Class<Navigate> navigate("navigate");
        navigate.method<&Navigate::looper>("looper");
        navigate.method<&Navigate::urlLooper>("urlLooper");
        navigate.property("module", &Navigate::getmodule, &Navigate::setmodule);
        navigate.property("controller", &Navigate::getcontroller, &Navigate::setcontroller);
        navigate.property("action", &Navigate::getaction, &Navigate::setaction);
        navigate.property("image", &Navigate::getimage, &Navigate::setimage);
        navigate.property("role", &Navigate::getrole, &Navigate::setrole);
        navigate.property("mainurl", &Navigate::getmainurl, &Navigate::setmainurl);
        navigate.property("navlink", &Navigate::getnavlink, &Navigate::setnavlink);



        Php::Class<Server> server("server");
        server.method<&Server::connect>("connect");
        server.method<&Server::currentStatus>("currentStatus");
        server.method<&Server::update>("update");
        server.method<&Server::secretCode>("secretCode");



        Php::Class<CoreCard> corecard("corecard");
        corecard.method<&CoreCard::config>("config");
        corecard.method<&CoreCard::setPhoto>("setPhoto");
        corecard.method<&CoreCard::setGif>("setGif");
        corecard.method<&CoreCard::setText>("setText");
        corecard.method<&CoreCard::setRawText>("setRawText");
        corecard.method<&CoreCard::setHeader>("setHeader");
        corecard.method<&CoreCard::process>("process");
        //                corecard.method<&CoreCard::qrcode>("qrcode");

        Php::Class<AccHelp> acchelp("acchelp");
        acchelp.method<&AccHelp::setFrom>("setFrom");
        acchelp.method<&AccHelp::setTo>("setTo");
        acchelp.method<&AccHelp::setYear>("setYear");
        acchelp.method<&AccHelp::setLedger>("setLedger");
        acchelp.method<&AccHelp::setStudentStatus>("setStudentStatus");
        acchelp.method<&AccHelp::getScholarReport>("getScholarReport");
        acchelp.method<&AccHelp::getBillCollectReport>("getBillCollectReport");
        acchelp.method<&AccHelp::getOpeningLedger>("getOpeningLedger");
        acchelp.method<&AccHelp::getLedger>("getLedger");
        acchelp.method<&AccHelp::checkDep>("checkDep");
        acchelp.method<&AccHelp::getClassReport>("getClassReport");
        acchelp.method<&AccHelp::searchLedger>("searchLedger");
        acchelp.method<&AccHelp::getStudentLedger>("getStudentLedger");
        acchelp.method<&AccHelp::getPayrollFee>("getPayrollFee");
        acchelp.method<&AccHelp::getScholarTrack>("getScholarTrack");
        


        Php::Class<ExamHelp> examhelp("examhelp");
        examhelp.method<&ExamHelp::getFullRoutine>("getFullRoutine");

        Php::Class<DataHelp> datahelp("datahelp");
        datahelp.method<&DataHelp::getNoticeGroup>("getNoticeGroup");
        datahelp.method<&DataHelp::getMyNotice>("getMyNotice");
        datahelp.method<&DataHelp::getPgAttendance>("getPgAttendance");
        datahelp.method<&DataHelp::getStudentAttendance>("getStudentAttendance");
        datahelp.method<&DataHelp::getDbAttendance>("getDbAttendance");
        datahelp.method<&DataHelp::getEvents>("getEvents");
        datahelp.method<&DataHelp::getAccFeeDue>("getAccFeeDue");

        datahelp.method<&DataHelp::getTemplate1>("getTemplate1");
        datahelp.method<&DataHelp::getTemplate2>("getTemplate2");

        Php::Class<AccountAudit> accountaudit("accountaudit");
        accountaudit.method<&AccountAudit::closeAccount>("closeAccount");
        
        Php::Class<SystemAi> systemai("systemai");
        systemai.method<&SystemAi::init>("init");
        systemai.method<&SystemAi::process>("process");

        extension.add(temptoken);
        extension.add(exam);
        extension.add(examrep);
        extension.add(accrep);
        extension.add(checkup);
        extension.add(corecard);
        extension.add(cright);
        extension.add(notification);
        extension.add(navigate);
        extension.add(server);
        extension.add(acchelp);
        extension.add(examhelp);
        extension.add(datahelp);
        extension.add(accountaudit);
        extension.add(systemai);
        extension.add("importbackup", importbackup);
        extension.add("getfilebackup", getfilebackup);
        extension.add("setfilebackup", setfilebackup);
        extension.add("mydecode", mydecode);
        extension.add("myencode", myencode);
        extension.add("mydecode2", mydecode2);
        extension.add("myencode2", myencode2);
        extension.add(Php::Constant("lockva", 1744262909));
        return extension;
    }
}
