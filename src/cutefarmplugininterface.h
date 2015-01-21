/* 	CuteFarm
    Copyright (C) 2008-2011  Jan Vaillant

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation License 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see http://www.gnu.org/licenses/.
*/

#ifndef CUTEFARMPLUGININTERFACE_H_
#define CUTEFARMPLUGININTERFACE_H_

#include <QtPlugin>
#include <QAction>
#include <QObject>


class CuteFarmPluginInterface
{

public:
	    
    virtual ~CuteFarmPluginInterface() {}
    
public: 
	virtual QAction* action(QObject* parent = 0) const = 0;
	virtual QObject* actnMonitor(QObject* parent = 0) const = 0;   
};


Q_DECLARE_INTERFACE(CuteFarmPluginInterface,
                    "cutefarm.Plugin.CuteFarmPluginInterface/1.0");

#endif /*CUTEFARMPLUGININTERFACE_H_*/
