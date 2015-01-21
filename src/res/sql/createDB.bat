cd C:\workspace_galileo\cutefarm\cutefarm\res\sql
DEL C:\workspace_galileo\cutefarm\cutefarm\res\sql\de.sqlite3
echo .read 'C:\workspace_galileo\cutefarm\cutefarm\res\sql\cutefarm-2-schema.sql' | sqlite3_6_19.exe C:\workspace_galileo\cutefarm\cutefarm\res\sql\de.sqlite3
echo .read 'C:\workspace_galileo\cutefarm\cutefarm\res\sql\cutefarm-2-views.sql' | sqlite3_6_19.exe C:\workspace_galileo\cutefarm\cutefarm\res\sql\de.sqlite3
echo .read 'C:\workspace_galileo\cutefarm\cutefarm\res\sql\data\de_crop.sql' | sqlite3_6_19.exe C:\workspace_galileo\cutefarm\cutefarm\res\sql\de.sqlite3
echo .read 'C:\workspace_galileo\cutefarm\cutefarm\res\sql\data\de_pltr.sql' | sqlite3_6_19.exe C:\workspace_galileo\cutefarm\cutefarm\res\sql\de.sqlite3
echo .read 'C:\workspace_galileo\cutefarm\cutefarm\res\sql\data\de_fild_use.sql' | sqlite3_6_19.exe C:\workspace_galileo\cutefarm\cutefarm\res\sql\de.sqlite3
echo .read 'C:\workspace_galileo\cutefarm\cutefarm\res\sql\data\de_fert.sql' | sqlite3_6_19.exe C:\workspace_galileo\cutefarm\cutefarm\res\sql\de.sqlite3
echo .read 'C:\workspace_galileo\cutefarm\cutefarm\res\sql\data\de_anim_typ.sql' | sqlite3_6_19.exe C:\workspace_galileo\cutefarm\cutefarm\res\sql\de.sqlite3
DEL C:\workspace_galileo\cutefarm\cutefarm\res\sql\fr.sqlite3
echo .read 'C:\workspace_galileo\cutefarm\cutefarm\res\sql\cutefarm-2-schema.sql' | sqlite3_6_19.exe C:\workspace_galileo\cutefarm\cutefarm\res\sql\fr.sqlite3
echo .read 'C:\workspace_galileo\cutefarm\cutefarm\res\sql\cutefarm-2-views.sql' | sqlite3_6_19.exe C:\workspace_galileo\cutefarm\cutefarm\res\sql\fr.sqlite3
echo .read 'C:\workspace_galileo\cutefarm\cutefarm\res\sql\data\fr_crop.sql' | sqlite3_6_19.exe C:\workspace_galileo\cutefarm\cutefarm\res\sql\fr.sqlite3
echo .read 'C:\workspace_galileo\cutefarm\cutefarm\res\sql\data\fr_pltr.sql' | sqlite3_6_19.exe C:\workspace_galileo\cutefarm\cutefarm\res\sql\fr.sqlite3
echo .read 'C:\workspace_galileo\cutefarm\cutefarm\res\sql\data\fr_fild_use.sql' | sqlite3_6_19.exe C:\workspace_galileo\cutefarm\cutefarm\res\sql\fr.sqlite3
echo .read 'C:\workspace_galileo\cutefarm\cutefarm\res\sql\data\fr_fert.sql' | sqlite3_6_19.exe C:\workspace_galileo\cutefarm\cutefarm\res\sql\fr.sqlite3
echo .read 'C:\workspace_galileo\cutefarm\cutefarm\res\sql\data\fr_anim_typ.sql' | sqlite3_6_19.exe C:\workspace_galileo\cutefarm\cutefarm\res\sql\fr.sqlite3

DEL C:\workspace_galileo\cutefarm\cutefarm\res\sql\en.sqlite3
echo .read 'C:\workspace_galileo\cutefarm\cutefarm\res\sql\cutefarm-2-schema.sql' | sqlite3_6_19.exe C:\workspace_galileo\cutefarm\cutefarm\res\sql\en.sqlite3
echo .read 'C:\workspace_galileo\cutefarm\cutefarm\res\sql\cutefarm-2-views.sql' | sqlite3_6_19.exe C:\workspace_galileo\cutefarm\cutefarm\res\sql\en.sqlite3
echo .read 'C:\workspace_galileo\cutefarm\cutefarm\res\sql\data\en_crop.sql' | sqlite3_6_19.exe C:\workspace_galileo\cutefarm\cutefarm\res\sql\en.sqlite3
echo .read 'C:\workspace_galileo\cutefarm\cutefarm\res\sql\data\en_pltr.sql' | sqlite3_6_19.exe C:\workspace_galileo\cutefarm\cutefarm\res\sql\en.sqlite3
echo .read 'C:\workspace_galileo\cutefarm\cutefarm\res\sql\data\en_fild_use.sql' | sqlite3_6_19.exe C:\workspace_galileo\cutefarm\cutefarm\res\sql\en.sqlite3
echo .read 'C:\workspace_galileo\cutefarm\cutefarm\res\sql\data\en_fert.sql' | sqlite3_6_19.exe C:\workspace_galileo\cutefarm\cutefarm\res\sql\en.sqlite3
echo .read 'C:\workspace_galileo\cutefarm\cutefarm\res\sql\data\en_anim_typ.sql' | sqlite3_6_19.exe C:\workspace_galileo\cutefarm\cutefarm\res\sql\en.sqlite3
pause
