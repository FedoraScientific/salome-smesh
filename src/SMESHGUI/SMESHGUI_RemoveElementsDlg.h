//  File      : SMESHGUI_RemoveElementsDlg.h
//  Created   : Fri Jun 07 11:18:42 2002
//  Author    : Nicolas REJNERI

//  Project   : SALOME
//  Module    : SMESH
//  Copyright : Open CASCADE 2002
//  $Header$

#ifndef DIALOGBOX_REMOVE_ELEMENTS_H
#define DIALOGBOX_REMOVE_ELEMENTS_H

#include "SALOME_Selection.h"

// QT Includes
#include <qvariant.h>
#include <qdialog.h>

// Open CASCADE Includes
#include <TColStd_MapOfInteger.hxx>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class SMESHGUI;

// IDL Headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SMESH_Mesh)

//=================================================================================
// class    : SMESHGUI_RemoveElementsDlg
// purpose  :
//=================================================================================
class SMESHGUI_RemoveElementsDlg : public QDialog
{ 
    Q_OBJECT

public:
    SMESHGUI_RemoveElementsDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SMESHGUI_RemoveElementsDlg();

private:

    void Init( SALOME_Selection* Sel ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;                          /* mouse enter the QWidget */

    SMESHGUI*                     mySMESHGUI ;              /* Current SMESHGUI object */
    SALOME_Selection*             mySelection ;             /* User shape selection */
    bool                          myOkElements ;               /* to check when arguments is defined */
    int                           myConstructorId ;         /* Current constructor id = radio button id */
    QLineEdit*                    myEditCurrentArgument;    /* Current  LineEdit */

    SMESH::SMESH_Mesh_var         myMesh;
    TColStd_MapOfInteger          myMapIndex;
    
    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QGroupBox* GroupButtons;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QPushButton* buttonApply;
    QGroupBox* GroupC1;
    QLabel* TextLabelC1A1;
    QPushButton* SelectButtonC1A1;
    QLineEdit* LineEditC1A1;

private slots:

    void ConstructorsClicked(int constructorId);
    void ClickOnOk();
    void ClickOnCancel();
    void ClickOnApply();
    void SetEditCurrentArgument() ;
    void SelectionIntoArgument() ;
    void DeactivateActiveDialog() ;
    void ActivateThisDialog() ;

protected:
    QGridLayout* SMESHGUI_RemoveElementsDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupC1Layout;
};

#endif // DIALOGBOX_REMOVE_ELEMENTS_H