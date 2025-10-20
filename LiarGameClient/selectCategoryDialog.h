#ifndef SELECTCATEGORYDIALOG_H
#define SELECTCATEGORYDIALOG_H

// selectCategoryDialog.h
#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget> // QListWidget 사용을 위해 추가
#include <QJsonArray> // QJsonArray 사용을 위해 추가
#include <QJsonValue>
#include <QMessageBox>

// 카테고리 다이얼로그 클래스
class SelectCategoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectCategoryDialog(QWidget *parent = nullptr);
    QString getSelectedCategory() const;

private:
    QComboBox *categoryComboBox;
    QPushButton *okButton;
    QPushButton *cancelButton;

    // 카테고리 목록
    QStringList categories = {"동물", "음식", "스포츠", "영화", "장소", "직업", "사물", "브랜드/제품", "실제인물", "나라", "수도", "노래", "애니메이션", "아이돌", "웹툰", "LMS 7차", "게임"};

signals:
    void categorySelected(const QString& category);

private slots:
};

// 투표 다이얼로그 클래스
class VoteDialog : public QDialog
{
    Q_OBJECT

public:
    // QJsonArray를 인자로 받아 투표 대상자 목록을 초기화합니다.
    explicit VoteDialog(const QJsonArray& voters, QWidget *parent = nullptr);
    QString getVotedUserNickname() const;

private slots:
    void on_voteButton_clicked();

private:
    QListWidget* voterListWidget;
    QPushButton* voteButton;
    QString votedUserNickname;
};

#endif // SELECTCATEGORYDIALOG_H
