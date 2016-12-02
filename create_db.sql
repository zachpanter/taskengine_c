CREATE SCHEMA `taskengine` DEFAULT CHARACTER SET utf8 ;

DROP TABLE IF EXISTS `taskrepo`;
DROP TABLE IF EXISTS `actionable`;
DROP TABLE IF EXISTS `reminder`;


CREATE TABLE `taskengine`.`taskrepo` (
  `taskrepo_id` INT NOT NULL AUTO_INCREMENT,
  `taskrepo_default` BIT(1) NOT NULL DEFAULT 0,
  `taskrepo_name` VARCHAR(45) NOT NULL,
  #`taskrepo_createdate` DATETIME,
  #`taskrepo_minimize_flag` BIT(1) NOT NULL DEFAULT 0,
  PRIMARY KEY (`taskrepo_id`),
  UNIQUE INDEX `taskrepo_id_UNIQUE` (`taskrepo_id` ASC));


CREATE TABLE `taskengine`.`actionable` (
  `actionable_id` INT NOT NULL AUTO_INCREMENT,
  `actionable_id_parent` INT NULL,
  `taskrepo_id` INT NOT NULL,
  `actionable_title` VARCHAR(45) NOT NULL,
  `actionable_content` TEXT NULL,
  `actionable_createddate` DATETIME,
  #`actionable_modifieddate` TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  `actionable_duedate` DATETIME NULL,
  PRIMARY KEY (`actionable_id`),
  UNIQUE INDEX `actionable_id_UNIQUE` (`actionable_id` ASC));


CREATE TABLE `taskengine`.`reminder` (
  `reminder_id` INT NOT NULL AUTO_INCREMENT,
  `actionable_id` INT NOT NULL,
  `reminder_rules` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`reminder_id`),
  UNIQUE INDEX `reminder_id_UNIQUE` (`reminder_id` ASC));


ALTER TABLE `actionable` ADD CONSTRAINT `FK_Actionable_taskrepo_id` FOREIGN KEY (`taskrepo_id`) REFERENCES `taskrepo` (`taskrepo_id`) ON DELETE NO ACTION ON UPDATE NO ACTION;
ALTER TABLE `reminder` ADD CONSTRAINT `FK_Reminder_actionable_id` FOREIGN KEY (`actionable_id`) REFERENCES `actionable` (`actionable_id`) ON DELETE NO ACTION ON UPDATE NO ACTION;

#INSERT STATEMENTS*************************************************************************

# taskrepo
INSERT INTO taskengine.taskrepo (taskrepo_id, taskrepo_default, taskrepo_name) VALUES ('1', b'1','Inbox');
INSERT INTO taskengine.taskrepo (taskrepo_id, taskrepo_default, taskrepo_name) VALUES ('2', b'0','Triage');
INSERT INTO taskengine.taskrepo (taskrepo_id, taskrepo_default, taskrepo_name) VALUES ('3', b'0','Work');
INSERT INTO taskengine.taskrepo (taskrepo_id, taskrepo_default,taskrepo_name) VALUES ('4', b'0','Home');
INSERT INTO taskengine.taskrepo (taskrepo_id, taskrepo_default,taskrepo_name) VALUES ('5', b'0','School');

# actionable
INSERT INTO taskengine.actionable (taskrepo_id, actionable_title, actionable_content, actionable_createddate, actionable_duedate) VALUES ('1', 'Bring Maria Dr. Pepper', 'She asked for some caffeine', NOW(), DATE_ADD(CURDATE(), INTERVAL 1 DAY));
INSERT INTO taskengine.actionable (taskrepo_id, actionable_title, actionable_content, actionable_createddate, actionable_duedate) VALUES ('3', 'Verify DASH data for CCM', 'Necessary to move ahead with DASH work', NOW(), '2016-12-02 17:00:00');
INSERT INTO taskengine.actionable (taskrepo_id, actionable_title, actionable_content, actionable_createddate, actionable_duedate) VALUES ('5', 'Submit 3210 Final Project', 'Atta boy, Zach. Atta boy.', NOW(), '2016-12-10 23:59:59');
INSERT INTO taskengine.actionable (taskrepo_id, actionable_title, actionable_createddate, actionable_duedate) VALUES ('5', 'Submit 4230 Summary Report', NOW(), '2016-12-14 23:59:59');
INSERT INTO taskengine.actionable (taskrepo_id, actionable_title, actionable_createddate, actionable_duedate) VALUES ('5','Submit 4230 Design Document', NOW(), '2016-12-14 23:59:59');
INSERT INTO taskengine.actionable (taskrepo_id, actionable_title, actionable_createddate, actionable_duedate) VALUES ('5', 'Submit 4230 User Guide', NOW(), '2016-12-14 23:59:59');
INSERT INTO taskengine.actionable (taskrepo_id, actionable_title, actionable_createddate, actionable_duedate) VALUES ('5', 'Complete 4230 HW4 Peer Review', NOW(), '2016-12-02 23:59:59');
INSERT INTO taskengine.actionable (taskrepo_id, actionable_title, actionable_createddate, actionable_duedate) VALUES ('5', 'Complete 4230 Team Evaluation', NOW(), '2016-12-14 23:59:59');
INSERT INTO taskengine.actionable (taskrepo_id, actionable_title, actionable_createddate) VALUES ('3','Fix L3 categories on PRR', NOW());
INSERT INTO taskengine.actionable (taskrepo_id, actionable_title, actionable_createddate) VALUES ('4', 'Set up budgets in mint', NOW());
INSERT INTO taskengine.actionable (taskrepo_id, actionable_title, actionable_createddate) VALUES ('4', 'Fix Wanderookie Carousel', NOW());
INSERT INTO taskengine.actionable (taskrepo_id, actionable_title, actionable_createddate) VALUES ('4', 'Sew Columbia Beanie', NOW());
INSERT INTO taskengine.actionable (taskrepo_id, actionable_title, actionable_createddate) VALUES ('4', 'Put Loctite on sunglasses', NOW());
INSERT INTO taskengine.actionable (taskrepo_id, actionable_title, actionable_createddate) VALUES ('2', 'Read Pro Git 2', NOW());
INSERT INTO taskengine.actionable (taskrepo_id, actionable_title, actionable_createddate) VALUES ('4', 'Replace TravisCI with Jenkins', NOW());
