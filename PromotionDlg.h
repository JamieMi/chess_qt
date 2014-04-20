#ifndef PROMOTIONDLG_H
#define PROMOTIONDLG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QRadioButton>

class PromotionDlg : public QDialog{
    Q_OBJECT
public:
    PromotionDlg(QWidget *parent = 0);

private slots:
    Q_INVOKABLE void selectSlot() {selectClicked();}

private:
    char cChoice;
    QGroupBox* pGB;
    QGroupBox *createExclusiveGroup();
    QRadioButton *radioQueen;
    QRadioButton *radioBishop;
    QRadioButton *radioRook;
    QRadioButton *radioKnight;
    QPushButton *pbSelect;
    QLabel *label;
    void selectClicked();
public:
    char getPiece(){return cChoice;}
};

#endif // PROMOTIONDLG_H
