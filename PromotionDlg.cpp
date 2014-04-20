#include <QtGui>
#include "PromotionDlg.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QIcon>// TO DO: remove?

#include <iostream> //TO DO : remove this
PromotionDlg::PromotionDlg(QWidget *parent) : QDialog(parent){
    label = new QLabel(tr("The pawn has been promoted. What\npiece do you want to convert it to?"));
    pbSelect = new QPushButton(tr("Promote"));

    cChoice = '-';

    connect(pbSelect, SIGNAL(clicked()), this, SLOT(selectSlot()));

    QGridLayout *grid = new QGridLayout;
    pGB = createExclusiveGroup();
    grid->addWidget(pGB, 0, 0);
    grid->addWidget(label);

    QHBoxLayout *labelLayout = new QHBoxLayout;
    labelLayout->addWidget(label);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addLayout(labelLayout);
    leftLayout->addLayout(grid);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(pbSelect);
    rightLayout->addStretch();

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Promotion"));
    setFixedHeight(sizeHint().height());
}

QGroupBox *PromotionDlg::createExclusiveGroup(){
     QGroupBox *groupBox = new QGroupBox(tr(""));

     radioQueen = new QRadioButton(tr("Queen"));
     radioBishop = new QRadioButton(tr("Bishop"));
     radioRook = new QRadioButton(tr("Rook"));
     radioKnight = new QRadioButton(tr("Knight"));

     radioQueen->setChecked(true);

     QVBoxLayout *vbox = new QVBoxLayout;
     vbox->addWidget(radioQueen);
     vbox->addWidget(radioBishop);
     vbox->addWidget(radioRook);
     vbox->addWidget(radioKnight);
     vbox->addStretch(1);
     groupBox->setLayout(vbox);
     return groupBox;
}

void PromotionDlg::selectClicked(){
    if (radioQueen->isChecked()) cChoice = 'Q';
    else if (radioBishop->isChecked()) cChoice = 'B';
    else if (radioRook->isChecked()) cChoice = 'R';
    else if (radioKnight->isChecked()) cChoice = 'N';
    close();
}
