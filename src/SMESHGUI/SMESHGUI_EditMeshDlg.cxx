
#include "SMESHGUI_EditMeshDlg.h"

#include "SMESHGUI.h"
#include "SMESHGUI_Utils.h"
#include "SMESHGUI_VTKUtils.h"

#include "SMESH_TypeFilter.hxx"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"

#include "SALOME_ListIO.hxx"

#include "utilities.h"

// QT Includes
#include <qapplication.h>
#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qpixmap.h>

//=================================================================================
// class    : SMESHGUI_EditMeshDlg()
// purpose  :
//=================================================================================
SMESHGUI_EditMeshDlg::SMESHGUI_EditMeshDlg (SMESHGUI* theModule,
                                            const char* title, const char* icon,
                                            int theAction)
  : QDialog(SMESH::GetDesktop(theModule), "SMESHGUI_EditMeshDlg", false, WStyle_Customize |
            WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu | Qt::WDestructiveClose),
    mySMESHGUI(theModule),
    mySelectionMgr(SMESH::GetSelectionMgr(theModule)),
    myAction(theAction)
{
  resize(303, 185);
  setCaption(tr(title));

  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("SMESH", tr(icon)));
  QPixmap image1 (aResMgr->loadPixmap("SMESH", tr("ICON_SELECT")));

  setSizeGripEnabled(TRUE);
  DlgLayout = new QGridLayout (this);
  DlgLayout->setSpacing(6);
  DlgLayout->setMargin(11);

  /***************************************************************/
  GroupConstructors = new QButtonGroup (this, "GroupConstructors");
  GroupConstructors->setExclusive(TRUE);
  GroupConstructors->setColumnLayout(0, Qt::Vertical);
  GroupConstructors->layout()->setSpacing(0);
  GroupConstructors->layout()->setMargin(0);
  GroupConstructorsLayout = new QGridLayout (GroupConstructors->layout());
  GroupConstructorsLayout->setAlignment(Qt::AlignTop);
  GroupConstructorsLayout->setSpacing(6);
  GroupConstructorsLayout->setMargin(11);
  Constructor1 = new QRadioButton (GroupConstructors, "Constructor1");
  Constructor1->setText(tr(""));
  Constructor1->setPixmap(image0);
  Constructor1->setChecked(TRUE);
  Constructor1->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)1,
                                          (QSizePolicy::SizeType)0,
                                          Constructor1->sizePolicy().hasHeightForWidth()));
  Constructor1->setMinimumSize(QSize(50, 0));
  GroupConstructorsLayout->addWidget(Constructor1, 0, 0);
  QSpacerItem* spacer = new QSpacerItem (20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  GroupConstructorsLayout->addItem(spacer, 0, 1);
  DlgLayout->addWidget(GroupConstructors, 0, 0);

  /***************************************************************/
  GroupButtons = new QGroupBox (this, "GroupButtons");
  GroupButtons->setGeometry(QRect(10, 10, 281, 48));
  GroupButtons->setTitle(tr("" ));
  GroupButtons->setColumnLayout(0, Qt::Vertical);
  GroupButtons->layout()->setSpacing(0);
  GroupButtons->layout()->setMargin(0);
  GroupButtonsLayout = new QGridLayout (GroupButtons->layout());
  GroupButtonsLayout->setAlignment(Qt::AlignTop);
  GroupButtonsLayout->setSpacing(6);
  GroupButtonsLayout->setMargin(11);
  buttonCancel = new QPushButton (GroupButtons, "buttonCancel");
  buttonCancel->setText(tr("SMESH_BUT_CLOSE" ));
  buttonCancel->setAutoDefault(TRUE);
  GroupButtonsLayout->addWidget(buttonCancel, 0, 3);
  buttonApply = new QPushButton (GroupButtons, "buttonApply");
  buttonApply->setText(tr("SMESH_BUT_APPLY" ));
  buttonApply->setAutoDefault(TRUE);
  GroupButtonsLayout->addWidget(buttonApply, 0, 1);
  QSpacerItem* spacer_9 = new QSpacerItem (20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  GroupButtonsLayout->addItem(spacer_9, 0, 2);
  buttonOk = new QPushButton (GroupButtons, "buttonOk");
  buttonOk->setText(tr("SMESH_BUT_OK" ));
  buttonOk->setAutoDefault(TRUE);
  buttonOk->setDefault(TRUE);
  GroupButtonsLayout->addWidget(buttonOk, 0, 0);
  DlgLayout->addWidget(GroupButtons, 2, 0);

  /***************************************************************/
  GroupMesh = new QGroupBox (this, "GroupMesh");
  GroupMesh->setTitle(tr("SMESH_MESH" ));
  GroupMesh->setMinimumSize(QSize(0, 0));
  GroupMesh->setFrameShape(QGroupBox::Box);
  GroupMesh->setFrameShadow(QGroupBox::Sunken);
  GroupMesh->setColumnLayout(0, Qt::Vertical);
  GroupMesh->layout()->setSpacing(0);
  GroupMesh->layout()->setMargin(0);
  GroupMeshLayout = new QGridLayout (GroupMesh->layout());
  GroupMeshLayout->setAlignment(Qt::AlignTop);
  GroupMeshLayout->setSpacing(6);
  GroupMeshLayout->setMargin(11);
  TextLabelMesh = new QLabel (GroupMesh, "TextLabelMesh");
  TextLabelMesh->setText(tr("SMESH_MESH"));
  TextLabelMesh->setMinimumSize(QSize(50, 0));
  TextLabelMesh->setFrameShape(QLabel::NoFrame);
  TextLabelMesh->setFrameShadow(QLabel::Plain);
  GroupMeshLayout->addWidget(TextLabelMesh, 0, 0);
  SelectButton = new QPushButton (GroupMesh, "SelectButton");
  SelectButton->setText(tr(""));
  SelectButton->setPixmap(image1);
  SelectButton->setToggleButton(FALSE);
  GroupMeshLayout->addWidget(SelectButton, 0, 1);
  LineEditMesh = new QLineEdit (GroupMesh, "LineEditMesh");
  LineEditMesh->setReadOnly(true);
  GroupMeshLayout->addWidget(LineEditMesh, 0, 2);
  DlgLayout->addWidget(GroupMesh, 1, 0);

  Init(); // Initialisations
}

//=================================================================================
// function : ~SMESHGUI_EditMeshDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
SMESHGUI_EditMeshDlg::~SMESHGUI_EditMeshDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void SMESHGUI_EditMeshDlg::Init()
{
  GroupMesh->show();
  mySMESHGUI->SetActiveDialogBox((QDialog*)this);

  myMesh = SMESH::SMESH_Mesh::_nil();

  myMeshFilter = new SMESH_TypeFilter (MESH);

  // signals and slots connections
  connect(buttonOk    , SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));
  connect(buttonApply , SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(SelectButton, SIGNAL(clicked()), this, SLOT(SelectionIntoArgument()));

  connect(mySelectionMgr, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  connect(mySMESHGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(mySMESHGUI, SIGNAL(SignalCloseAllDialogs())       , this, SLOT(ClickOnCancel()));

  // Move widget on the bottom right corner of main widget
  int x, y;
  mySMESHGUI->DefineDlgPosition(this, x, y);
  this->move(x, y);
  this->show(); // displays Dialog

  LineEditMesh->setFocus();
  mySelectionMgr->clearFilters();
  mySelectionMgr->installFilter(myMeshFilter);

  SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void SMESHGUI_EditMeshDlg::ClickOnApply()
{
  if (!myMesh->_is_nil()) {
    try	{
      QApplication::setOverrideCursor(Qt::waitCursor);

      if (myAction == 1) {
        SMESH::SMESH_MeshEditor_var aMeshEditor = myMesh->GetMeshEditor();
        aMeshEditor->MergeEqualElements();
      }

      QApplication::restoreOverrideCursor();
    } catch(...) {
    }

    mySelectionMgr->clearSelected();
    SMESH::UpdateView();
  }
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void SMESHGUI_EditMeshDlg::ClickOnOk()
{
  ClickOnApply();
  ClickOnCancel();
}

//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void SMESHGUI_EditMeshDlg::ClickOnCancel()
{
  mySelectionMgr->clearSelected();
  mySelectionMgr->clearFilters();
  disconnect(mySelectionMgr, 0, this, 0);
  mySMESHGUI->ResetState();
  reject();
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void SMESHGUI_EditMeshDlg::SelectionIntoArgument()
{
  if (!GroupButtons->isEnabled()) // inactive
    return;

  QString aString = "";

  SALOME_ListIO aList;
  mySelectionMgr->selectedObjects(aList);
  int nbSel = SMESH::GetNameOfSelectedIObjects(mySelectionMgr, aString);

  // mesh
  if (nbSel != 1) {
    myMesh = SMESH::SMESH_Mesh::_nil();
    aString = "";
  } else {
    Handle(SALOME_InteractiveObject) IO = aList.First();
    myMesh = SMESH::IObjectToInterface<SMESH::SMESH_Mesh>(IO);
    if (myMesh->_is_nil())
      aString = "";
  }

  LineEditMesh->setText(aString);

  bool isEnabled = (!myMesh->_is_nil());
  buttonOk->setEnabled(isEnabled);
  buttonApply->setEnabled(isEnabled);
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void SMESHGUI_EditMeshDlg::DeactivateActiveDialog()
{
  if (GroupConstructors->isEnabled()) {
    GroupConstructors->setEnabled(false);
    GroupMesh->setEnabled(false);
    GroupButtons->setEnabled(false);
    mySMESHGUI->ResetState();
    mySMESHGUI->SetActiveDialogBox(0);
  }
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void SMESHGUI_EditMeshDlg::ActivateThisDialog()
{
  /* Emit a signal to deactivate the active dialog */
  mySMESHGUI->EmitSignalDeactivateDialog();
  GroupConstructors->setEnabled(true);
  GroupMesh->setEnabled(true);
  GroupButtons->setEnabled(true);

  mySMESHGUI->SetActiveDialogBox((QDialog*)this);
  SelectionIntoArgument();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void SMESHGUI_EditMeshDlg::enterEvent(QEvent* e)
{
  if (GroupConstructors->isEnabled())
    return;
  ActivateThisDialog();
}

//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void SMESHGUI_EditMeshDlg::closeEvent(QCloseEvent* e)
{
  /* same than click on cancel button */
  this->ClickOnCancel();
}

//=======================================================================
//function : hideEvent
//purpose  : caused by ESC key
//=======================================================================
void SMESHGUI_EditMeshDlg::hideEvent (QHideEvent * e)
{
  if (!isMinimized())
    ClickOnCancel();
}