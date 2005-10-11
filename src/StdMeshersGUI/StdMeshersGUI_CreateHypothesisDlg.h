//  SMESH SMESHGUI : GUI for SMESH component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : StdMeshersGUI_CreateHypothesisDlg.h
//  Module : SMESH
//  $Header$

#ifndef DIALOGBOX_CreateHypothesisDlg_H
#define DIALOGBOX_CreateHypothesisDlg_H

// QT Includes
#include <qdialog.h>
#include <qmap.h>
#include <qpair.h>

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SMESH_BasicHypothesis)

#include "SMESHGUI_aParameter.h"

#include <list>

class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class SMESHGUI;
class SMESHGUI_SpinBox;

//=================================================================================
// class    : StdMeshersGUI_CreateHypothesisDlg
// purpose  :
//=================================================================================
class StdMeshersGUI_CreateHypothesisDlg : public QDialog
{ 
    Q_OBJECT

public:
    StdMeshersGUI_CreateHypothesisDlg (const QString& hypType,
				       QWidget*       parent = 0,
				       const char*    name = 0,
				       bool           modal = FALSE,
				       WFlags         fl = 0);
    ~StdMeshersGUI_CreateHypothesisDlg ();

protected:

    void CreateDlgLayout(const QString & caption,
			 const QPixmap & hypIcon,
			 const QString & hypTypeName);

    virtual void GetParameters( const QString& hypType,
                                std::list<SMESHGUI_aParameterPtr>& ) = 0;

    virtual bool SetParameters( SMESH::SMESH_Hypothesis_ptr theHyp,
			                          const std::list<SMESHGUI_aParameterPtr> & params ) = 0;

protected slots:
    virtual void onValueChanged();
    
private:
    void UpdateShown( const SMESHGUI_aParameterPtr );
    void Init() ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;

    SMESHGUI*  mySMESHGUI;
    QString    myHypType;

    typedef struct 
    {
      QWidget* editor;
      QLabel*  label;
      int      order;
      
    } ParamInfo;
    
    typedef QMap< SMESHGUI_aParameterPtr, ParamInfo > ParameterMap;
    ParameterMap      myParamMap;

    QLabel*           iconLabel;
    QLabel*           typeLabel;
    QGroupBox*        GroupC1;
    QLabel*           TextLabel_NameHypothesis ;
    QLineEdit*        LineEdit_NameHypothesis ;
    QLabel*           TextLabel_Length ;
    QGroupBox*        GroupButtons;
    QPushButton*      buttonOk;
    QPushButton*      buttonApply;
    QPushButton*      buttonCancel;

private slots:

    void ClickOnOk();
    void ClickOnCancel();
    bool ClickOnApply();
    void DeactivateActiveDialog() ;
    void ActivateThisDialog() ;
};

#endif // DIALOGBOX_LOCAL_LENGTH_H
