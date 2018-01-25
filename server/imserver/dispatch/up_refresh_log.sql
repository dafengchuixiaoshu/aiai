DROP PROCEDURE IF EXISTS up_refresh_log;

CREATE PROCEDURE up_refresh_log(IN DAYID varchar(8),IN JOBNAME varchar(128),IN INTSTEP INTEGER,IN ERRMSG varchar(1024))
 
 TOP:Begin
 /**********************************************
            存储过程执行日志
/**********************************************/

    Declare intCount SmallInt;
    
    Select Count(*) Into intCount From information_schema.tables Where UPPER(trim(TABLE_NAME)) = 'odd_refresh_log';
    If intCount = 0 Then
        CREATE TABLE `odd_refresh_log` (
            `log_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
            `day_id` int(8) NOT NULL,
            `job_name` varchar(128) NOT NULL,
            `int_step` int(2) NOT NULL,
            `err_msg` varchar(1024) NOT NULL,
            PRIMARY KEY (`log_id`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8;
    End If;    
    
    INSERT INTO odd_refresh_log(day_id, job_name, int_step, err_msg)values(DAYID, JOBNAME, INTSTEP, ERRMSG);

End;