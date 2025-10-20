// selectCategoryDialog.cpp
#include "selectCategoryDialog.h"
#include <qlistwidget.h>

// 카테고리 다이얼로그
SelectCategoryDialog::SelectCategoryDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("카테고리 선택");
    setFixedSize(300, 150);

    // QComboBox 생성 및 카테고리 추가
    categoryComboBox = new QComboBox(this);
    categoryComboBox->addItems(categories);

    // 버튼 생성
    okButton = new QPushButton("확인", this);
    cancelButton = new QPushButton("취소", this);

    // 레이아웃 설정
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    mainLayout->addWidget(new QLabel("게임 카테고리를 선택하세요:", this));
    mainLayout->addWidget(categoryComboBox);
    mainLayout->addLayout(buttonLayout);

    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    // 시그널/슬롯 연결
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

QString SelectCategoryDialog::getSelectedCategory() const
{
    return categoryComboBox->currentText();
}

// 투표 다이얼로그
VoteDialog::VoteDialog(const QJsonArray& voters, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("투표할 대상을 선택하세요");
    setFixedSize(300, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("라이어로 의심되는 사람을 선택해주세요:", this));

    voterListWidget = new QListWidget(this);
    for (const QJsonValue& value : voters) {
        voterListWidget->addItem(value.toString());
    }
    layout->addWidget(voterListWidget);

    voteButton = new QPushButton("투표", this);
    layout->addWidget(voteButton);

    // 시그널/슬롯 연결
    connect(voteButton, &QPushButton::clicked, this, &VoteDialog::on_voteButton_clicked);
}

QString VoteDialog::getVotedUserNickname() const
{
    return votedUserNickname;
}

void VoteDialog::on_voteButton_clicked()
{
    QListWidgetItem* selectedItem = voterListWidget->currentItem();
    if (selectedItem) {
        votedUserNickname = selectedItem->text();
        accept(); // 다이얼로그 닫기 및 QDialog::Accepted 반환
    } else {
        // 선택된 항목이 없으면 경고 메시지 표시
        QMessageBox::warning(this, "선택 오류", "투표할 대상을 선택해주세요.");
    }
}
