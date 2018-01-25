DROP PROCEDURE IF EXISTS up_historymsg;

CREATE PROCEDURE up_historymsg(IN IN_DATE VARCHAR(8))

TOP:Begin
/**********************************************
                历史消息备份
/**********************************************/ 
    
    DECLARE intStep INTEGER;
    Declare strCurDay VarChar(8);
    Declare strCurMonth VarChar(8);
    Declare dstTableName VarChar(64);
    Declare intCount SmallInt;

    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        CALL up_refresh_log(strCurDay,'up_historymsg',intStep,'fail');
    END;

    SET intStep = 0;
    Set strCurDay = replace(LEFT(date_add(LEFT(TRIM(In_Date),8), interval -3 month),10),'-','');
    Set strCurMonth = LEFT(TRIM(In_Date),6);
    Set dstTableName= CONCAT('message_timings_',strCurMonth);
    
    Select Count(*) Into intCount From information_schema.tables Where UPPER(trim(TABLE_NAME)) = dstTableName;
    If intCount = 0 Then
        SET @SQLSTR_CREATE = CONCAT('CREATE TABLE ',dstTableName,' LIKE message_timings');
        prepare s1 From @SQLSTR_CREATE;
        execute s1;
    End If;    

    SET intStep = intStep + 1;
    SET @SQLSTR_REPLACE = CONCAT('REPLACE INTO ',dstTableName,' SELECT * FROM message_timings WHERE sendflag <> 0 AND date_format(sendtime,''%Y%m%d'') <= ',strCurDay);
    prepare s1 From @SQLSTR_REPLACE;
    execute s1;
    
    SET intStep = intStep + 1;
    SET @SQLSTR_DELETE = CONCAT('DELETE FROM message_timings WHERE sendflag <> 0 AND date_format(sendtime,''%Y%m%d'') <= ',strCurDay);
    prepare s1 From @SQLSTR_DELETE;
    execute s1;

    commit;

    CALL up_refresh_log(strCurDay,'up_historymsg',intStep,'ok');
    
End;