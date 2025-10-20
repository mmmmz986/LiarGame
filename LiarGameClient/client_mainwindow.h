//Client_mainWindow.h

#ifndef CLIENT_MAINWINDOW_H
#define CLIENT_MAINWINDOW_H

#include "selectCategoryDialog.h"
#include <QMainWindow>
#include <QTcpSocket>
#include <QStackedWidget>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDataStream> // QDataStream을 사용하기 위해 헤더를 추가합니다.
#include <QInputDialog>
#include <QListWidgetItem>
#include <QJsonArray>
#include <QPixmap>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // 네트워크 연결 관리 멤버
    QTcpSocket* socket; // TCP 소켓 객체
    QString serverIp; // IP 주소
    quint16 serverPort; // 포트번호
    quint32 nextBlockSize; // 서버로부터 받을 데이터 패킷 크기

    // 게임, 사용자 정보 멤버
    QString currentUserId; // 현재 로그인한 사용자의 아이디
    QString currentNickname; // 현재 로그인한 사용자의 닉네임
    int currentRoomId;// 현재 입장한 방의 아이디
    QMap<int, QString> roomChatHistory; // 키는 방 ID, 값은 채팅 내용인 맵
    QJsonArray roomArray; // 방 배열
    QString currentGamePhase; // 현재 게임 단계

    // 서버 상태 관리, 유효성 검사 멤버
    bool isRequestInProgress = false; // 서버 버그 방지를 위한 플래그
    bool isIdChecked = false; // 아이디 중복 여부 확인
    bool isEmailChecked = false; // 이메링 중복 여부 확인

private:
    // 이미지 초기화
    void setWallpaperOnScreen();
    void setPontColor();
    // 서버에게 요청
    void SendRequestToServer(const QJsonObject& request); // 서버로 데이터를 전송하는 함수
    // 서버의 요청을 처리
    void ResponseByServer(const QJsonObject& json); // 서버로부터 받은 데이터를 처리
    void handleSignupResponse(const QJsonObject& json); // 회원가입
    void handleIdCheckResponse(const QJsonObject& json); // 아이디 중복확인
    void handleEmailCheckResponse(const QJsonObject& json); // 이메일 중복확인
    void handleLoginResponse(const QJsonObject& json); // 로그인
    void handleFindAccountResponse(const QJsonObject& json); // 아이디 찾기
    void handlePasswordChangeResponse(const QJsonObject& json); // 비밀번호 찾기
    void handleLogoutResponse(const QJsonObject& json); // 로그아웃
    void handleChangeNicknameResponse(const QJsonObject& json); // 닉네임 변경
    void handleChangePasswordResponse(const QJsonObject& json); // 비밀번호 변경
    void handleRoomListUpdate(); // 방 목록 업데이트
    void handleUserListUpdate(const QJsonObject& json); // 유저 업데이트
    void handleCreateRoomResponse(const QJsonObject& json); // 방 생성
    void handleDeleteRoomResponse(const QJsonObject& json); // 방 제거
    void handleJoinRoomResponse(const QJsonObject& json); // 방 입장
    void handleLeaveRoomResponse(const QJsonObject& json);// 방 나가기
    void handleChatResponse(const QJsonObject& json); // 일반채팅
    void handleStartGameResponse(const QJsonObject& json); // 게임시작
    void handleGameChat(const QJsonObject& json); // 게임 채팅
    void handleLiarAnswerPhase(const QJsonObject& json); // 라이어 정답
    void handleVotePhaseResponse(const QJsonObject& json); // 투표
    void handleGameEndResponse(const QJsonObject& json); // 게임종료
    void handleSystemMessage(const QJsonObject& json); // 서버 메시지
    // 클라이언트 관리 함수
    void setAllButtonsEnabled(bool enabled); // 모든 버튼 관리
    bool isPasswordValid(const QString& password); // 비밀번호 유효성 검사
    void setBackgroundForPage(int pageIndex);
signals:
    void roomCreated(const QString& roomName);
private slots:
    // 서버 연결 관련 함수
    void handleConnected(); // 소켓 연결
    void handleDisconnected(); // 소켓 연결 해제
    void handleReadyRead(); // 서버로부터의 데이터 읽기
    void handleError(QAbstractSocket::SocketError socketError);

    // 회원 관리 UI 시그널-슬롯 함수
    void on_loginButton_clicked(); // 로그인 버튼 클릭
    void on_passwordLineEdit_returnPressed(); // 로그인 엔터
    void SendLoginRequest(); // 로그인
    void on_signupRegisterButton_clicked(); // 회원가입
    void on_checkIdButton_clicked(); // 아이디 중복확인
    void on_checkEmailButton_clicked(); // 이메일 중복확인
    void on_signupIdLineEdit_textChanged(const QString& text); // 아이디 입력창 변경
    void on_signupEmailLineEdit_textChanged(const QString& text); // 이메일 입력창 변경
    void on_findAccountFindButton_clicked(); // 아이디 찾기
    void on_passwordChangeButton_clicked(); // 비밀번호 찾기(변경)
    void on_changePasswordButton_clicked(); // 비밀번호 변경
    void on_changeNicknameButton_clicked(); // 닉네임 변경
    void on_logoutButton_clicked(); // 로그아웃

    // 게임 UI 시그널-슬롯 함수
    void on_createRoomButton_clicked(); // 방 생성
    void on_deleteRoomButton_clicked(); // 방 삭제
    void on_joinRoomButton_clicked(); // 방 입장
    void on_roomListWidget_itemDoubleClicked(QListWidgetItem *item); // 방 더블클릭 입장
    void ExitButton(); // 방 나가기
    void on_NormalChatEdit_returnPressed(); // 채팅 엔터 전송
    void on_StartButton_clicked(); // 게임시작
    void sendStartGameRequest(const QString& category); // 게임시작 요청
    void on_GameChatEdit_returnPressed(); // 게임채팅
    void sendVoteRequest(const QString& votedNickname);

};

#endif // CLIENT_MAINWINDOW_H
