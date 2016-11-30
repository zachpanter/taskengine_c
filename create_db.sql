CREATE SCHEMA `taskengine` DEFAULT CHARACTER SET utf8 ;

DROP TABLE IF EXISTS `taskrepo`;
DROP TABLE IF EXISTS `actionable`;
DROP TABLE IF EXISTS `reminder`;


CREATE TABLE `taskengine`.`taskrepo` (
  `taskrepo_id` INT NOT NULL AUTO_INCREMENT,
  `taskrepo_default` BIT(1) NOT NULL DEFAULT 0,
  `taskrepo_name` VARCHAR(45) NOT NULL,
  `taskrepo_createdate` DATETIME,
  `taskrepo_minimize_flag` BIT(1) NOT NULL DEFAULT 0,
  PRIMARY KEY (`taskrepo_id`),
  UNIQUE INDEX `taskrepo_id_UNIQUE` (`taskrepo_id` ASC));


CREATE TABLE `taskengine`.`actionable` (
  `actionable_id` INT NOT NULL AUTO_INCREMENT,
  `actionable_id_parent` INT NOT NULL,
  `taskrepo_id` INT NOT NULL,
  `actionable_title` VARCHAR(45) NOT NULL,
  `actionable_content` TEXT NULL,
  `actionable_createddate` DATETIME,
  `actionable_modifieddate` TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
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