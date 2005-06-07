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
//  File   : SMESHGUI_GroupOpDlg.h
//  Author : Sergey LITONIN
//  Module : SMESH


#ifndef SMESHGUI_GroupOpDlg_H
#define SMESHGUI_GroupOpDlg_H

#include <qdialog.h>

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SMESH_Group)

class QCloseEvent;
class QLabel;
class QFrame;
class QPushButton;
class SalomeApp_SelectionMgr;
class QLineEdit;

/*
  Class       : SMESHGUI_GroupOpDlg
  Description : Perform boolean operations on groups
*/

class SMESHGUI_GroupOpDlg : public QDialog
{ 
  Q_OBJECT
    
public:
  enum { UNION, INTERSECT, CUT };
    
public:
                            SMESHGUI_GroupOpDlg( QWidget*, SalomeApp_SelectionMgr*, const int );
  virtual                   ~SMESHGUI_GroupOpDlg();

  void                      Init( SalomeApp_SelectionMgr* ) ;
  
private:

  void                      closeEvent( QCloseEvent* e ) ;
  void                      enterEvent ( QEvent * ) ;            
  
private slots:

  void                      onOk();
  bool                      onApply();
  void                      onClose();

  void                      onDeactivate();
  void                      onSelectionDone();
  void                      onFocusChanged();

private:

  QFrame*                   createButtonFrame( QWidget* );
  QFrame*                   createMainFrame  ( QWidget* );
  bool                      isValid();
  void                      reset();
  
private:

  QPushButton*              myOkBtn;
  QPushButton*              myApplyBtn;
  QPushButton*              myCloseBtn;
  
  QLineEdit*                myNameEdit;
  QLineEdit*                myEdit1;
  QLineEdit*                myEdit2;
  QPushButton*              myBtn1;
  QPushButton*              myBtn2;
  
  SalomeApp_SelectionMgr*   mySelectionMgr;
  int                       myMode;
  
  QLineEdit*                myFocusWg;
  
  SMESH::SMESH_GroupBase_var    myGroup1;
  SMESH::SMESH_GroupBase_var    myGroup2;
  
};

#endif
