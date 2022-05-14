<?php

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

namespace app\Object;

use Yii;

/**
 * Description of MarkGenerater
 *
 * @author krishna
 */

/**
 * This is the class for MarkGenerater.
 *
 * @property subjectData[] $subjectData
 * @property sheetResult[] $sheetResult
 * @property graceTrack[] $graceTrack
 * @property gradeSheet[] $gradeSheet
 * @property markSheet[] $markSheet
 */
class MarkGeneraternew {

//put your code here

    const ORDER_PROGRAM = 0;
    const ORDER_SECTION = 1;
    const PASSED = "PASSED";
    const FAILED = "FAILED";
    const SMS_STUDENT = "STUDENT";
    const SMS_PARENTS = "PARENTS";
    const SMS_BOTH = "BOTH";

    public $studentDetail = [];
    public $subjectData = [];
    public $gradeSheet = [];
    public $markSheet = [];
    public $resultSummary = [];
    public $order = [];
    public $orderType;
    public $sheetResult = [];
    private $formula = [];
    private $graceMark = [];
    private $graceTrack = [];
    private $messageTo;
    private $routineDetail = [];
    private $studentList = [];
    private $orginazation = [];
    private $comment = [];
    public $resultRemark = [];

    function __construct() {
        $this->resultSummary["total_pass"] = 0;
        $this->resultSummary["total_fail"] = 0;
        $this->resultSummary["total_student"] = 0;
        $this->resultSummary["grace"]["total"] = 0;
        $this->resultSummary["grace"]["student"] = [];
        $this->resultSummary["subject_count"] = 0;
        $this->resultSummary["subject_type_count"] = 0;
        $this->orderType = self::ORDER_PROGRAM;
        $this->messageTo = self::SMS_PARENTS;
        $this->resultRemark[\app\models\exa\ExaGradeSetting::GRADE]["N"] = "";
        $this->resultRemark[\app\models\exa\ExaGradeSetting::PERCENTAGE]["N"] = "";
    }

    public function setFormula($formula) {
        $this->formula = $formula;
    }

    public function setRoutine($routine) {
        $this->routineDetail = $routine;
    }

    public function setGraceMark($grace) {
        $this->graceMark = $grace;
    }

    public function setMessageTo($to) {
        $this->messageTo = $to;
    }

    public function setStudent($student) {
        $this->studentList = $student;
    }

    public function setOrganization($organization) {
        $this->orginazation = $organization;
    }

//    public function graceProcess() {
//        foreach ($this->sheetResult as $rkey => $sresult) {
//            if ($this->graceTrack[$rkey] && $this->studentData["remark"] == self::FAILED) {
//                $this->resultSummary["grace"]["student"][$rkey] = 0;
//                foreach ($sresult as $ckey => $scode) {
//                    if (array_key_exists(\app\models\ext\ExtSubjectType::TH, $scode)) {
//                        if ($scode[\app\models\ext\ExtSubjectType::TH]["mark"] < $scode[\app\models\ext\ExtSubjectType::TH]["pm"]) {
//                            $this->resultSummary["grace"]["total"] += $scode[\app\models\ext\ExtSubjectType::TH]["pm"] - $scode[\app\models\ext\ExtSubjectType::TH]["mark"];
//                            $this->sheetResult[$rkey][$ckey][\app\models\ext\ExtSubjectType::TH]["mark"] = $scode[\app\models\ext\ExtSubjectType::TH]["pm"];
//                            $this->resultSummary["grace"]["student"][$rkey] += $scode[\app\models\ext\ExtSubjectType::TH]["pm"] - $scode[\app\models\ext\ExtSubjectType::TH]["mark"];
//                        }
//                    }
//                }
//            }
//        }
//    }


    public function process($markBasket) {
        foreach ($markBasket as $sl) {
            if ($sl["fm"] < 1) {
                throw new \yii\web\BadRequestHttpException("Full marks cannot be zero of subject " . $sl["subject_name"]);
            }

            $this->studentToAca[$sl["student_aca"]] = $sl["student_aca"];
//collection subject detail
            $this->subjectData[$sl["subject_code"]]["type"][$sl["subject_type"]]["type"] = $sl["subject_type"];
            $this->subjectData[$sl["subject_code"]]["type"][$sl["subject_type"]]["fm"] = $sl["fm"];
            $this->subjectData[$sl["subject_code"]]["type"][$sl["subject_type"]]["pm"] = $sl["pm"];
            $this->subjectData[$sl["subject_code"]]["name"] = $sl["subject_name"];
            $this->subjectData[$sl["subject_code"]]["code"] = $sl["subject_code"];
            $this->subjectData[$sl["subject_code"]]["credit"] = $sl["credit"];
            $this->subjectData[$sl["subject_code"]]["s_name"] = $sl["subject_sname"];
            $this->resultSummary["subject_type"][$sl["subject_type"]] = $sl["subject_type"];

//collection mark detail
            $this->sheetResult[$sl["student_id"]][$sl["subject_code"]][$sl["subject_type"]]["mark"] = $sl["mark"];
            $this->sheetResult[$sl["student_id"]][$sl["subject_code"]][$sl["subject_type"]]["pm"] = $sl["pm"];
            $this->sheetResult[$sl["student_id"]][$sl["subject_code"]][$sl["subject_type"]]["att_status"] = $sl["att_status"] == 1 ? "" : "ABS";
            $this->sheetResult[$sl["student_id"]][$sl["subject_code"]][$sl["subject_type"]]["fm"] = $sl["fm"]; //grace
//grace mark filter
            if (!empty($this->graceMarks)) {
                if (!array_key_exists($sl["student_id"], $this->graceTrack)) {
                    $this->graceTrack[$sl["student_id"]]["fail"] = 0;
                    $this->graceTrack[$sl["student_id"]]["grace"] = true;
                }
            }
            if (!isset($this->studentDetail[$sl["student_id"]]["remark"])) {
                $this->studentDetail[$sl["student_id"]]["remark"] = self::PASSED;
            }
            if ($sl["pm"] > $sl["mark"]) {
                $this->sheetResult[$sl["student_id"]][$sl["subject_code"]][$sl["subject_type"]]["att_status"] = $sl["att_status"] == 0 ? "ABS" : "F";
                $this->studentDetail[$sl["student_id"]]["remark"] = self::FAILED;
                if (!empty($this->graceMarks)) {
                    $graceRatio = $sl["fm"] * ($this->graceMark["min_mark"] / 100);
                    if ($sl["subject_type"] == \app\models\ext\ExtCourseType::PR || $this->graceTrack[$sl["student_id"]]["fail"] > $this->graceMark["max_subjects"] || $sl["mark"] < $graceRatio) {
                        $this->graceTrack[$sl["student_id"]]["grace"] = false;
                    }
                    $this->graceTrack[$sl["student_id"]]["fail"] += 1; //..grace 
                }
            }
        }
        $this->resultSummary["subject_count"] = count($this->subjectData);
    }

    public function markCalculate() {
        foreach ($this->formula as $div) {
            $this->resultRemark[\app\models\exa\ExaGradeSetting::PERCENTAGE][$div["grade"]] = $div["remark"];
        }
        foreach ($this->sheetResult as $rkey => $sresult) {
            $this->markSheet[$rkey]["total_marks"] = 0;
            $this->markSheet[$rkey]["total_points"] = 0;
            $total_fm = 0;
            foreach ($sresult as $ckey => $scode) {
                $grade = 0;
                $gradeCount = 0;
                $pmcount = 0;
                $countOM = $countFM = 0;
                $this->markSheet[$rkey]["subject"][$ckey]["AM"] = 0;
                $this->markSheet[$rkey]["subject"][$ckey]["FG"] = 0;
                foreach ($scode as $tkey => $stype) {
                    $countFM += $stype["fm"];
                    $countOM += $stype["mark"];
                    $pmcount += $stype["pm"];
                    $gradeCount = round(($countOM / $countFM) * $this->formula[0]["base"], 2);
                    $this->markSheet[$rkey]["subject"][$ckey][$tkey] = $stype["mark"];
                    $this->markSheet[$rkey]["subject"][$ckey]["AM"] += $stype["mark"];
                    $this->markSheet[$rkey]["subject"][$ckey]["FG"] += $stype["mark"];
                    $this->markSheet[$rkey]["total_marks"] += $stype["mark"];
                }
                $total_fm += $countFM;
                $this->markSheet[$rkey]["total_points"] += $gradeCount;
                $this->subjectData[$ckey]["fm"] = $countFM;
                $this->subjectData[$ckey]["pm"] = $pmcount;
                if (empty($this->subjectData[$ckey]["HIGNSCORE"])) {
                    $this->subjectData[$ckey]["HIGNSCORE"] = $this->markSheet[$rkey]["subject"][$ckey]["AM"];
                } else {
                    if ($this->subjectData[$ckey]["HIGNSCORE"] < $this->markSheet[$rkey]["subject"][$ckey]["AM"]) {
                        $this->subjectData[$ckey]["HIGNSCORE"] = $this->markSheet[$rkey]["subject"][$ckey]["AM"];
                    }
                }
            }
            $this->markSheet[$rkey]["GPA"] = "N";
            $this->markSheet[$rkey]["AG"] = "N";
            $grade = round(($this->markSheet[$rkey]["total_marks"] / $total_fm) * 100, 2);
            $this->markSheet[$rkey]["GPA"] = $grade;
            foreach ($this->formula as $div) {
                if ($div["value_from"] <= $grade && $div["value_to"] >= $grade) {
                    $this->markSheet[$rkey]["AG"] = $div["grade"];
                    break;
                }
            }
        }
        if (empty($this->order)) {
            $this->order();
        }
    }

    public function gradeCalculate() {
        foreach ($this->formula as $div) {
            $this->resultRemark[\app\models\exa\ExaGradeSetting::GRADE][$div["code"]] = $div["remark"];
        }
        foreach ($this->sheetResult as $rkey => $sresult) {
            $this->gradeSheet[$rkey]["total_points"] = 0;
            $this->gradeSheet[$rkey]["total_credit"] = 0;
            foreach ($sresult as $ckey => $scode) {
                $gradePoint = 0;
                $countOM = $countFM = 0;
                $gradeCount = 0;
                $finalGrade = "N";
                $pmcount = 0;
                foreach ($scode as $tkey => $stype) {
                    $grade = round(($stype["mark"] / $stype["fm"]) * $this->formula[0]["base"], 2);
                    $countOM += $stype["mark"];
                    $countFM += $stype["fm"];
                    $pmcount += $stype["pm"];
                    $gradeCount = round(($countOM / $countFM) * $this->formula[0]["base"], 2);
                    foreach ($this->formula as $div) {
                        if ($div["value_from"] <= $gradeCount && $div["value_to"] >= $gradeCount) {
                            $gradePoint = $div["value_to"];
                            $finalGrade = $div["code"];
                        }
                        if ($div["value_from"] <= $grade && $div["value_to"] >= $grade) {
                            $this->gradeSheet[$rkey]["subject"][$ckey][$tkey] = $div["code"];
                        }
                    }
                }
                $this->gradeSheet[$rkey]["subject"][$ckey]["AM"] = $gradePoint;
                $this->gradeSheet[$rkey]["subject"][$ckey]["FG"] = $finalGrade;
                $this->gradeSheet[$rkey]["total_credit"] = $this->gradeSheet[$rkey]["total_credit"] + $this->subjectData[$ckey]["credit"];
                $this->gradeSheet[$rkey]["total_points"] += ($gradePoint * $this->subjectData[$ckey]["credit"]);
                $this->subjectData[$ckey]["fm"] = $countFM;
                $this->subjectData[$ckey]["pm"] = $pmcount;
            }
            $this->gradeSheet[$rkey]["GPA"] = "N";
            $this->gradeSheet[$rkey]["AG"] = "N";
            $grade = round($this->gradeSheet[$rkey]["total_points"] / $this->gradeSheet[$rkey]["total_credit"], 2);
            $this->gradeSheet[$rkey]["GPA"] = $grade;
            foreach ($this->formula as $div) {
                if ($div["value_from"] <= $grade && $div["value_to"] >= $grade) {
                    $this->gradeSheet[$rkey]["AG"] = $div["grade"];
                    break;
                }
            }
        }
        if (empty($this->order)) {
            $this->order();
        }
    }

    public function getStudentDetail() {
        $student = \app\models\std\StudentToAca::find()
                ->joinWith(["fkStudent", "fkProgram", "fkPeriod", "fkBatch", "fkSection", "fkProgramGroup.fkShift", "fkProgramGroup.fkLangMedium"])
                ->where(["student_to_aca.id" => $this->studentToAca])
                ->asArray()
                ->all();
        foreach ($student as $std) {
            $this->studentDetail[$std["fk_student_id"]] = array_merge($this->studentDetail[$std["fk_student_id"]], $std);
        }
    }

    public function studentSelection($studentList) {
        $this->studentList = $studentList;
        foreach ($this->order as $seckey => $order) {
            foreach ($order as $skey => $order) {
                if (!in_array($skey, $studentList)) {
                    unset($this->order[$seckey][$skey]);
                }
            }
        }
    }

    public function order() {
        $this->getStudentDetail();
        if (count($this->markSheet) == 0) {
            if ($this->orderType == self::ORDER_PROGRAM) {
                foreach ($this->studentDetail as $gskey => $sd) {
                    if ($this->studentDetail[$gskey]["remark"] == self::PASSED) {
                        $this->order["A"][$gskey] = $this->gradeSheet[$gskey]["GPA"];
                    } else {
                        $this->order["A"][$gskey] = $this->gradeSheet[$gskey]["GPA"];
                    }
                }
            } else {
                foreach ($this->studentDetail as $gskey => $sd) {
                    if ($this->studentDetail[$gskey]["remark"] == self::PASSED) {
                        $this->order[$sd["fkSection"]["name"]][$gskey] = $this->gradeSheet[$gskey]["GPA"];
                    } else {
                        $this->order[$sd["fkSection"]["name"]][$gskey] = $this->gradeSheet[$gskey]["GPA"];
                    }
                }
            }
        } else {
            if ($this->orderType == self::ORDER_PROGRAM) {
                foreach ($this->studentDetail as $gskey => $sd) {
                    if ($this->studentDetail[$gskey]["remark"] == self::PASSED) {
                        $this->order["A"][$gskey] = $this->markSheet[$gskey]["GPA"];
                    } else {
                        $this->order["A"][$gskey] = $this->markSheet[$gskey]["GPA"] / 5000;
                    }
                }
            } else {
                foreach ($this->studentDetail as $gskey => $sd) {
                    if ($this->studentDetail[$gskey]["remark"] == self::PASSED) {
                        $this->order[$sd["fkSection"]["name"]][$gskey] = $this->markSheet[$gskey]["GPA"];
                    } else {
                        $this->order[$sd["fkSection"]["name"]][$gskey] = $this->markSheet[$gskey]["GPA"] / 5000;
                    }
                }
            }
        }
        foreach ($this->order as $key => $order) {
            uasort($this->order[$key], [$this, "cmp"]);
        }
        ksort($this->order);
        foreach ($this->order as $secKey => $order) {
            $rank = 0;
            $mark = 0;
            $this->resultSummary[$secKey]["total_pass"] = 0;
            $this->resultSummary[$secKey]["total_fail"] = 0;
            $this->resultSummary[$secKey]["total_student"] = 0;
            foreach ($order as $skey => $or) {
                if ($mark != $or) {
                    $mark = $or;
                    if ($this->studentDetail[$skey]["remark"] == self::PASSED || count($this->markSheet) == 0) {
                        $rank++;
                    }
                }
                $this->studentDetail[$skey]["rank"] = $rank;
                if ($this->studentDetail[$skey]["remark"] == self::PASSED) {
                    $this->resultSummary[$secKey]["total_pass"] += 1;
                } else {
                    $this->resultSummary[$secKey]["total_fail"] += 1;
                }
                $this->resultSummary[$secKey]["total_student"] += 1;
            }
        }
        $this->getStudentDetail();
    }

    private function cmp($a, $b) {
        if ($a == $b) {
            return 0;
        }
        return ($a > $b) ? -1 : 1;
    }

    public function sms($templateMessage) {

        if (empty($this->orginazation)) {
            Yii::$app->message->flush("Orginazation detail not set", 2);
            return false;
        }
        if (empty($this->routineDetail)) {
            Yii::$app->message->flush("Routine not found", 2);
            return false;
        }
        if (!(strpos($_POST["templateMessage"], "{{message}}") !== FALSE)) {
            Yii::$app->message->flush("Message template not found", 2);
            return false;
        }
        $StudentToAca = new \app\models\std\StudentToAca();
        $phoneData = $StudentToAca->getStudentParentPhone($this->studentList);
        $phone = [];
        foreach ($phoneData as $sta) {
//skip if there is no marksheet of that student
            if (empty($this->studentDetail[$sta["student_id"]])) {
                continue;
            }
            if ($this->messageTo == self::SMS_STUDENT) {
//skip on empty number
                if (empty($sta["student_phone"])) {

                    continue;
                }
//check if number has been already added
                if (empty($phone[$sta["student_id"]]["number"]) || !in_array($sta["student_phone"], $phone[$sta["student_id"]]["number"])) {
                    $phone[$sta["student_id"]]["number"][] = $sta["student_phone"];
                }
            } elseif ($this->messageTo == self::SMS_PARENTS) {
//skip on empty number
                if (empty($sta["parent_phone"])) {
                    continue;
                }
//check if number has been already added
                if (empty($phone[$sta["student_id"]]["number"]) || !in_array($sta["parent_phone"], $phone[$sta["student_id"]]["number"])) {
                    $phone[$sta["student_id"]]["number"][] = $sta["parent_phone"];
                }
            } elseif ($this->messageTo == self::SMS_BOTH) {
//skip on empty number
                if (empty($sta["student_phone"]) && empty($sta["parent_phone"])) {
                    continue;
                }
                if (!empty($sta["student_phone"])) {
                    //check if number has been already added
                    if (empty($phone[$sta["student_id"]]["number"]) || !in_array($sta["student_phone"], $phone[$sta["student_id"]]["number"])) {
                        $phone[$sta["student_id"]]["number"][] = $sta["student_phone"];
                    }
                }
                if (!empty($sta["parent_phone"])) {
                    //check if number has been already added
                    if (empty($phone[$sta["student_id"]]["number"]) || !in_array($sta["parent_phone"], $phone[$sta["student_id"]]["number"])) {
                        $phone[$sta["student_id"]]["number"][] = $sta["parent_phone"];
                    }
                }
            }
            $phone[$sta["student_id"]]["aca"] = $this->studentDetail[$sta["student_id"]]["student_aca"];
            $phone[$sta["student_id"]]["student_id"] = $this->studentDetail[$sta["student_id"]]["student_id"];
        }
        $smsStudentRecord = [];
        $smsGroup = new \app\models\sms\SmsGroup();
        $smsGroup->create_time = time();
        $smsGroup->group_id = "GROUP_" . $smsGroup->create_time;
        $smsGroup->title = $this->routineDetail["fkExamType"]["name"] . "(" . $this->routineDetail["routine_id"] . ")";
        if (!empty($_POST["smsTitle"])) {
            $smsGroup->title = $_POST["smsTitle"];
        }
        $smsGroup->type = \app\models\sms\SmsStudent::SMS_MARKSHEET;
        $msgCounter = 0;
        $msgNumber = 0;
        foreach ($phone as $phn) {
            $message = $this->studentDetail[$phn["student_id"]]["name"] . "'s" . PHP_EOL;
            $message .= $this->routineDetail["fkProgramGroup"]["fkPeriod"]["name"] . " ";
            $message .= $this->routineDetail["fkExamType"]["name"] . PHP_EOL;
            $message .= "Marksheet :-" . PHP_EOL;
            foreach ($this->markSheet[$phn["student_id"]]["subject"] as $scode => $subject) {
                foreach ($subject as $typekey => $subtype) {
                    if (in_array($typekey, $this->resultSummary["subject_type"])) {
                        $message .= $this->subjectData[$scode]["name"] . "(" . $typekey . "):" . $subtype . " out of " . $this->subjectData[$scode]["type"][$typekey]["fm"] . PHP_EOL;
                    }
                }
            }
            $message .= "Remarks: " . $this->studentDetail[$phn["student_id"]]["remark"] . "(" . $this->markSheet[$phn["student_id"]]["GPA"] . "%)" . PHP_EOL;
//            $message .= "Position : " . $this->studentDetail[$phn["student_id"]]["rank"];
            $message = str_replace("{{message}}", $message, $templateMessage);
            $message .= PHP_EOL . " - " . $this->orginazation["code"];
            foreach ($phn["number"] as $phnonbb) {
                $smsStudentRecord[$msgCounter][] = $this->studentDetail[$phn["student_id"]]["name"];
                $smsStudentRecord[$msgCounter][] = $phn["student_id"];
                $smsStudentRecord[$msgCounter][] = $phn["aca"];
                $smsStudentRecord[$msgCounter][] = Yii::$app->user->id;
                $smsStudentRecord[$msgCounter][] = 1;
                $smsStudentRecord[$msgCounter][] = $phnonbb;
                $smsStudentRecord[$msgCounter][] = $message;
                $smsStudentRecord[$msgCounter][] = "";
                $smsStudentRecord[$msgCounter][] = $smsGroup->group_id;
                $smsStudentRecord[$msgCounter][] = $smsGroup->create_time;
                $smsStudentRecord[$msgCounter][] = 2;
            }
            $this->comment[$msgCounter][] = $phn["student_id"];
            $this->comment[$msgCounter][] = Yii::$app->user->id;
            $this->comment[$msgCounter][] = $message;
            $this->comment[$msgCounter][] = 0;
            $this->comment[$msgCounter][] = 0;
            $this->comment[$msgCounter][] = $smsGroup->create_time;
            $msgCounter++;
            $msgNumber += count($phn["number"]);
        }
        file_put_contents(Yii::$app->basePath . "/cronjob/crontab.txt", "* * * * * php " . Yii::$app->basePath . "/yii sms/index " . PHP_EOL);
        shell_exec('crontab ' . Yii::$app->basePath . '/cronjob/crontab.txt');
        $smsGroup->detail = $this->routineDetail["fkExamType"]["name"] . "(" . $this->routineDetail["routine_id"] . ")" . ", " . PHP_EOL
                . $this->routineDetail["fkProgramGroup"]["fkProgram"]["code"] . "/"
                . $this->routineDetail["fkProgramGroup"]["fkPeriod"]["name"] . "/"
                . $this->routineDetail["fkProgramGroup"]["fkLangMedium"]["name"] . "/"
                . $this->routineDetail["fkProgramGroup"]["fkShift"]["name"] . "/"
                . $this->routineDetail["fkBatch"]["name"] . PHP_EOL
                . "Total Messages : " . $msgNumber . PHP_EOL;
        $messageFlag = false;
        try {
            $smsGroup->save(false);
            Yii::$app->db->createCommand()
                    ->batchInsert('sms_student', ['name', 'fk_student_id', 'fk_stu_aca_id', 'fk_emp_id', 'type', 'number', 'message', 'response', 'sms_group', 'create_date', 'status'], $smsStudentRecord)
                    ->execute();
            Yii::$app->db->createCommand()
                    ->batchInsert('student_comment', ['fk_student_id', 'fk_emp_id', 'comment', 'flow', 'seen_status', 'create_date'], $this->comment)
                    ->execute();
            Yii::$app->message->flush("Message to " . $msgNumber . " contacts.");
            $messageFlag = true;
        } catch (Exception $ex) {
            Yii::$app->message->flush("Message not sent", 1);
        }
        return $messageFlag;
    }

}
