#include "accountdialog.h"

#include "sql/usertable.h"
#include "global.h"
#include <QGridLayout>
#include <QLabel>
#include <QDateTime>
#include <QGroupBox>
#include <QPushButton>


#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>

using namespace evernote::edam;

AccountDialog::AccountDialog(QWidget *parent) :
    QDialog(parent)
{
    // Setup window layout, title, & icon
    this->setWindowTitle(tr("Account Information"));
    //setWindowIcon(new QIcon(new QIcon(iconPath+"account.png")));
    QGridLayout *grid = new QGridLayout();
    setLayout(grid);
    QLabel *premium = new QLabel(tr("Free"));
    User user;
    UserTable userTable;
    userTable.getUser(user);
    if (user.__isset.privilege) {
        if (user.privilege == PrivilegeLevel::PREMIUM)
            premium->setText(tr("Premium"));
        if (user.privilege == PrivilegeLevel::MANAGER)
            premium->setText(tr("Manager"));
        if (user.privilege == PrivilegeLevel::SUPPORT)
            premium->setText(tr("Support"));
        if (user.privilege == PrivilegeLevel::ADMIN)
            premium->setText(tr("Admin"));
    }
    QString username = "*unknown*";
    if (user.__isset.username)
        username = QString::fromStdString(user.username);
    qlonglong uploadAmt = 0;
    qlonglong uploadLimit = 0;
    qlonglong uploadLimitEnd = 0;
    int pct;
    if (user.__isset.accounting && user.accounting.__isset.uploadLimit)
        uploadLimit = user.accounting.uploadLimit;
    if (user.__isset.accounting && user.accounting.__isset.uploadLimitEnd)
        uploadLimitEnd = user.accounting.uploadLimitEnd;
    uploadAmt = userTable.getUploadAmt();
    if (uploadLimit > 0)
        pct = uploadAmt*100/uploadLimit;
    else
        pct = 0;
    QString unit = tr(" Bytes");

    if (uploadAmt >= 1024) {
        uploadAmt = uploadAmt/1024;
        unit = tr(" KB");
    }
    if (uploadAmt >= 1024) {
        uploadAmt = uploadAmt / 1024;
        unit = tr(" MB");
    }
    QDateTime dt;
    dt = QDateTime::fromMSecsSinceEpoch(uploadLimitEnd);
    QString ds = dt.toString();

    //		// Show limits
    QGroupBox *limitGroup = new QGroupBox(tr("Account:"));

    QGridLayout *textGrid = new QGridLayout();
    textGrid->addWidget(new QLabel(tr("User Name:")),1,1);
    textGrid->addWidget(new QLabel(username), 1,2);
    textGrid->addWidget(new QLabel(tr("Account Type:")), 2,1);
    textGrid->addWidget(premium, 2, 2);
    textGrid->addWidget(new QLabel(tr("Limit:")), 3,1);
    textGrid->addWidget(new QLabel(QString::number(uploadLimit/1024/1024) +QString(" MB")),3,2);
    textGrid->addWidget(new QLabel(tr("Uploaded In This Period:")), 4,1);
    if (uploadAmt > 0)
        textGrid->addWidget(new QLabel(QString::number(uploadAmt)+unit +" ("+QString::number(pct)+"%)"),4,2);
    else
        textGrid->addWidget(new QLabel(tr("Less than 1MB")),4,2);
    textGrid->addWidget(new QLabel(tr("Current Cycle Ends:")), 5,1);
    textGrid->addWidget(new QLabel(ds),5,2);
    limitGroup->setLayout(textGrid);

    grid->addWidget(limitGroup, 1, 1);


    QHBoxLayout *buttonLayout = new QHBoxLayout();
    ok = new QPushButton(tr("OK"),this);
    connect(ok, SIGNAL(clicked()), this, SLOT(okPushed()));
    buttonLayout->addStretch();
    buttonLayout->addWidget(ok);
    buttonLayout->addStretch();
    grid->addLayout(buttonLayout,3,1);

}


// OK button pushed, close the window
void AccountDialog::okPushed() {
    this->close();
}
