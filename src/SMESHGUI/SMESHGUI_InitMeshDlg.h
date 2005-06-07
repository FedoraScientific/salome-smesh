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
//  File   : SMESHGUI_InitMeshDlg.h
//  Author : Nicolas REJNERI
//  Module : SMESH
//  $Header$

#ifndef DIALOGBOX_INIT_MESH_H
#define DIALOGBOX_INIT_MESH_H

//#include "SMESH_TypeFilter.hxx"

#include "SUIT_SelectionFilter.h"
#include "SalomeApp_SelectionMgr.h"

// QT Includes
#include <qdialog.h>
#include <qstringlist.h>

// IDL Headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)
#include CORBA_SERVER_HEADER(SMESH_Gen)

class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class SMESHGUI;

//=================================================================================
// class    : SMESHGUI_InitMeshDlg
// purpose  :
//=================================================================================
class SMESHGUI_InitMeshDlg : public QDialog
{ 
    Q_OBJECT

public:
    SMESHGUI_InitMeshDlg ( SMESHGUI*,
			  const char* = 0,
			  bool modal = FALSE,
			  WFlags fl = 0 );
    ~SMESHGUI_InitMeshDlg();

protected:
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent( QEvent * ) ;

private:
    void Init();

    void UpdateControlState();
    QString SMESHGUI_InitMeshDlg::GetDefaultMeshName();

private:
    SMESHGUI*                  mySMESHGUI;
    SalomeApp_SelectionMgr*    mySelectionMgr;

    GEOM::GEOM_Object_var      myGeomShape;
    QLineEdit*                 myEditCurrentArgument; 

    //Handle(SALOME_TypeFilter)  myGeomFilter;
    SUIT_SelectionFilter*      myGeomFilter;
    //SMESH_TypeFilter*          myHypothesisFilter;
    //SMESH_TypeFilter*          myAlgorithmFilter;
    SUIT_SelectionFilter*      myHypothesisFilter;
    SUIT_SelectionFilter*      myAlgorithmFilter;

    QStringList                HypoList;
    QStringList                AlgoList;

    QGroupBox*    GroupButtons;
    QPushButton*  buttonOk;
    QPushButton*  buttonCancel;
    QPushButton*  buttonApply;

    QGroupBox*    GroupC1;
    QLabel*       TextLabel_NameMesh;
    QLineEdit*    LineEdit_NameMesh;
    QLabel*       TextLabelC1A1;
    QPushButton*  SelectButtonC1A1;
    QLineEdit*    LineEditC1A1;

    QLabel*       TextLabelC1A1Hyp;
    QPushButton*  SelectButtonC1A1Hyp;
    QLineEdit*    LineEditC1A1Hyp;

    QLabel*       TextLabelC1A1Algo;
    QPushButton*  SelectButtonC1A1Algo;
    QLineEdit*    LineEditC1A1Algo;

private slots:
    void ClickOnOk();
    bool ClickOnApply();
    void ClickOnCancel();
    void SetEditCurrentArgument();
    void SelectionIntoArgument();
    void DeactivateActiveDialog();
    void ActivateThisDialog();
};

#endif // DIALOGBOX_INIT_MESH_H
