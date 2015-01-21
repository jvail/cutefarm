OBJECTS_DIR = ../tmp/obj
MOC_DIR = ../tmp/moc
RCC_DIR = ../tmp/qrc
DESTDIR = ../bin
CONFIG += release
#CONFIG += console
QT += core \
    gui \
    sql \
    xml
TARGET = CuteFarm
HEADERS += cutefarm.h \
    mainwindow.h \
    aboutdlg/aboutdlg.h \
    setupwizard/progressbar.h \
    label.h \
    actionsview.h \
    actionsviewmodel.h \
    fertbalanceframe.h \
    helpdock.h \
    setupwizard/setupwizard.h \
    initdbdialog.h \
    highlighter.h \
    newharvestyeardialog.h \
    reportfiledialog.h \
    reportviewwidget.h \
    sqlrelationaldelegate.h \
    cultivardialog.h \
    cropusedialog.h \
    numberinputdialog.h \
    lotdialog.h \
    filddialog.h \
    lotxfilddialog.h \
    proddialog.h \
    animgrpdialog.h \
    workdialog.h \
    actionsviewwidget.h \
    calendarviewwidget.h \
    evntprgdialog.h \
    fertdialog.h \
    pltrdialog.h \
    textinputdialog.h \
    cropdialog.h \
    sqlrelationaltablemodel.h \
    cutefarmplugininterface.h \
    sqlquerymodel.h \
    calendarwidget.h \
    actionwizard.h \
    fieldviewwidget.h \
    spinboxdelegate.h \
    csvimportdialog.h \
    sqltablemodel.h \
    settingsdialog.h \
    farmdatadialog.h \
    basicdatadialog.h \
    ../3rdparty/wwwidgets/qwwrichtextedit.h \
    ../3rdparty/wwwidgets/qwwcolorbutton.h \
    ../3rdparty/wwwidgets/wwglobal.h \
    ../3rdparty/wwwidgets/wwglobal_p.h \
    ../3rdparty/wwwidgets/colormodel.h
SOURCES += cutefarm.cpp \
    mainwindow.cpp \
    aboutdlg/aboutdlg.cpp \
    setupwizard/progressbar.cpp \
    label.cpp \
    actionsview.cpp \
    actionsviewmodel.cpp \
    fertbalanceframe.cpp \
    helpdock.cpp \
    setupwizard/setupwizard.cpp \
    initdbdialog.cpp \
    highlighter.cpp \
    newharvestyeardialog.cpp \
    reportfiledialog.cpp \
    reportviewwidget.cpp \
    cultivardialog.cpp \
    cropusedialog.cpp \
    numberinputdialog.cpp \
    lotdialog.cpp \
    filddialog.cpp \
    lotxfilddialog.cpp \
    proddialog.cpp \
    animgrpdialog.cpp \
    workdialog.cpp \
    actionsviewwidget.cpp \
    calendarviewwidget.cpp \
    evntprgdialog.cpp \
    fertdialog.cpp \
    pltrdialog.cpp \
    textinputdialog.cpp \
    cropdialog.cpp \
    sqlrelationaltablemodel.cpp \
    sqlquerymodel.cpp \
    calendarwidget.cpp \
    actionwizard.cpp \
    fieldviewwidget.cpp \
    spinboxdelegate.cpp \
    csvimportdialog.cpp \
    sqltablemodel.cpp \
    settingsdialog.cpp \
    farmdatadialog.cpp \
    basicdatadialog.cpp \
    main.cpp \
    ../3rdparty/wwwidgets/qwwcolorbutton.cpp \
    ../3rdparty/wwwidgets/qwwrichtextedit.cpp \
    ../3rdparty/wwwidgets/wwglobal_p.cpp \
    ../3rdparty/wwwidgets/colormodel.cpp
FORMS += mainwindow.ui \
    aboutdlg/aboutdlg.ui \
    setupwizard/setupwizardstartpage.ui \
    setupwizard/setupwizardimportpage.ui \
    helpdock.ui \
    actionwizardsoiltestpage.ui \
    initdbdialog.ui \
    newharvestyeardialog.ui \
    reportfiledialog.ui \
    reportviewwidget.ui \
    cultivardialog.ui \
    cropusedialog.ui \
    numberinputdialog.ui \
    lotdialog.ui \
    filddialog.ui \
    lotxfilddialog.ui \
    proddialog.ui \
    animgrpdialog.ui \
    workdialog.ui \
    actionsviewwidget.ui \
    calendarviewwidget.ui \
    evntprgdialog.ui \
    fertdialog.ui \
    pltrdialog.ui \
    textinputdialog.ui \
    cropdialog.ui \
    actionwizardfildpage.ui \
    actionwizardactnpage.ui \
    actionwizardanimpage.ui \
    actionwizardharvpage.ui \
    actionwizarddrilpage.ui \
    actionwizardfertpage.ui \
    actionwizardpltrpage.ui \
    actionwizardcomtpage.ui \
    fieldviewwidget.ui \
    csvimportdialog.ui \
    settingsdialog.ui \
    farmdatadialog.ui \
    basicdatadialog.ui
RESOURCES += res/res.qrc
CODECFORTR = UTF-8
TRANSLATIONS = res/trs/cutefarm_de.ts \
    res/trs/cutefarm_fr.ts \
    res/trs/cutefarm_pl.ts \
    res/trs/cutefarm_it.ts \
    res/trs/cutefarm_fi.ts \
    res/trs/cutefarm_pt.ts \
    res/trs/cutefarm_tr.ts \
    res/trs/cutefarm_ru.ts
RC_FILE = res/ico/win/ico.rc
ICON = res/ico/osx/cutefarm.icns
!isEmpty(TRANSLATIONS) { 
    isEmpty(QMAKE_LRELEASE) { 
        win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
        else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
    }
    TSQM.name = lrelease \
        ${QMAKE_FILE_IN}
    TSQM.input = TRANSLATIONS
    TSQM.output = $$TS_DIR/${QMAKE_FILE_BASE}.qm
    TSQM.commands = $$QMAKE_LRELEASE \
        ${QMAKE_FILE_IN}
    TSQM.CONFIG = no_link
    QMAKE_EXTRA_COMPILERS += TSQM
    PRE_TARGETDEPS += compiler_TSQM_make_all
}
else:message(No translation files in project)
