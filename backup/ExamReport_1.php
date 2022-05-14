<?php

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

namespace app\Object;

class ExamReport_1 {

    public $gradeFormula;
    public $markFormula;
    public $resultReport;
    public $routine = [];
    public $queryData;

    function __construct($formula, $routine) {
        $this->resultReport = [];
        $this->routine = $routine;
        $this->gradeFormula = \app\models\exa\ExaGradeSetting::find()->where(["fk_program_id" => $formula, "mark_type" => \app\models\exa\ExaGradeSetting::GRADE])->all();
    }

    private function score() {
        $routineMod = new \app\models\exa\ExaRoutine();
        $markGenerator = new \app\Object\MarkGenerater();
        $markGenerator->orderType = 0;
        $markGenerator->process($routineMod->getMarkLedger($this->routine));
        $markGenerator->setFormula($this->gradeFormula);
        $markGenerator->gradeCalculate(false);
        return $markGenerator->gradeSheet;
    }

    public function process() {
        $trackSubject = [];
        $trackStudent = [];
        $remarkFlag = TRUE;
        if (empty($this->queryData)) {
            $modelReport = new \app\models\exa\AdvanceReport();
            $this->queryData = $modelReport->getReport($this->routine);
        }
        foreach ($this->queryData as $rd) {
            if (empty($trackStudent[$rd["student_id"]])) {
                $trackStudent[$rd["student_id"]]["pg"] = $rd["pg"];
                $trackStudent[$rd["student_id"]]["name"] = $rd["student_name"];
                $trackStudent[$rd["student_id"]]["gender"] = $rd["gender"];
                $trackStudent[$rd["student_id"]]["ethnic"] = $rd["ethnic"];
            }
            if (empty($trackSubject[$rd["student_id"]][$rd["subject_name"]])) {
                if ($rd["att_status"] == 0) {
                    $trackSubject[$rd["student_id"]][$rd["subject_name"]] = 'ABSENT';
                } elseif ($rd["pm"] <= $rd["mark"]) {
                    $trackSubject[$rd["student_id"]][$rd["subject_name"]] = 'PASSED';
                } else {
                    $trackSubject[$rd["student_id"]][$rd["subject_name"]] = 'FAILED';
                }
            } else {
                if ($rd["pm"] > $rd["mark"]) {
                    $trackSubject[$rd["student_id"]][$rd["subject_name"]] = 'FAILED';
                }
            }
        }
        $gradeScore = $this->score();
        foreach ($gradeScore as $sid => $gs) {
            $pgid = $trackStudent[$sid]["pg"];
            $this->resultReport[$pgid]["grade"][] = [
                "id" => $sid,
                "name" => $trackStudent[$sid]["name"],
                "GPA" => $gs["GPA"],
                "AG" => $gs["AG"]
            ];
        }
        foreach ($trackSubject as $sid => $sub) {
            $remarkFlag = "PASSED";
            $pgid = $trackStudent[$sid]["pg"];
            $gender = $trackStudent[$sid]["gender"];
            $ethnic = $trackStudent[$sid]["ethnic"];
            foreach ($sub as $subname => $suba) {
                if (empty($this->resultReport[$pgid]["subject"][$subname])) {
                    $this->resultReport[$pgid]["subject"][$subname]["PASSED"] = 0;
                    $this->resultReport[$pgid]["subject"][$subname]["FAILED"] = 0;
                    $this->resultReport[$pgid]["subject"][$subname]["ABSENT"] = 0;
                }
                if ($suba == 'ABSENT') {
                    $this->resultReport[$pgid]["subject"][$subname]["ABSENT"] += 1;
                } elseif ($suba == 'PASSED') {
                    $this->resultReport[$pgid]["subject"][$subname]["PASSED"] += 1;
                } else {
                    $this->resultReport[$pgid]["subject"][$subname]["FAILED"] += 1;
                }
                if ($remarkFlag == 'ABSENT' || $remarkFlag == 'PASSED') {
                    $remarkFlag = $suba;
                }
            }
            if (empty($this->resultReport[$pgid]["ethnic"][$ethnic])) {
                $this->resultReport[$pgid]["ethnic"][$ethnic]["PASSED"] = 0;
                $this->resultReport[$pgid]["ethnic"][$ethnic]["FAILED"] = 0;
                $this->resultReport[$pgid]["ethnic"][$ethnic]["ABSENT"] = 0;
            }
            if (empty($this->resultReport[$pgid]["gender"])) {
                $this->resultReport[$pgid]["gender"]["PASSED"]["boys"] = 0;
                $this->resultReport[$pgid]["gender"]["FAILED"]["boys"] = 0;
                $this->resultReport[$pgid]["gender"]["ABSENT"]["boys"] = 0;
                $this->resultReport[$pgid]["gender"]["PASSED"]["girls"] = 0;
                $this->resultReport[$pgid]["gender"]["FAILED"]["girls"] = 0;
                $this->resultReport[$pgid]["gender"]["ABSENT"]["girls"] = 0;
                $this->resultReport[$pgid]["gender"]["PASSED"]["other"] = 0;
                $this->resultReport[$pgid]["gender"]["FAILED"]["other"] = 0;
                $this->resultReport[$pgid]["gender"]["ABSENT"]["other"] = 0;
            }
            if ($remarkFlag == 'ABSENT') {
                if ($gender == 0) {
                    $this->resultReport[$pgid]["gender"]["ABSENT"]["girls"] += 1;
                } elseif ($gender == 1) {
                    $this->resultReport[$pgid]["gender"]["ABSENT"]["boys"] += 1;
                } else {
                    $this->resultReport[$pgid]["gender"]["ABSENT"]["other"] += 1;
                }
                $this->resultReport[$pgid]["ethnic"][$ethnic]["ABSENT"] += 1;
            } elseif ($remarkFlag == 'PASSED') {
                if ($gender == 0) {
                    $this->resultReport[$pgid]["gender"]["PASSED"]["girls"] += 1;
                } elseif ($gender == 1) {
                    $this->resultReport[$pgid]["gender"]["PASSED"]["boys"] += 1;
                } else {
                    $this->resultReport[$pgid]["gender"]["PASSED"]["other"] += 1;
                }
                $this->resultReport[$pgid]["ethnic"][$ethnic]["PASSED"] += 1;
            } else {
                if ($gender == 0) {
                    $this->resultReport[$pgid]["gender"]["FAILED"]["girls"] += 1;
                } elseif ($gender == 1) {
                    $this->resultReport[$pgid]["gender"]["FAILED"]["boys"] += 1;
                } else {
                    $this->resultReport[$pgid]["gender"]["FAILED"]["other"] += 1;
                }
                $this->resultReport[$pgid]["ethnic"][$ethnic]["FAILED"] += 1;
            }
        }
        return $this->resultReport;
    }

}
