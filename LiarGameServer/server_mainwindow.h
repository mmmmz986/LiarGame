//Server_mainWindow.h

#ifndef SERVER_MAINWINDOW_H
#define SERVER_MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonObject>
#include <QMap>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// 게임 상태 구조체
struct GameState {
    int turnIndex = 0;
    int currentRound;
    QString currentWord;
    QString liarWord;
    QString liarId;
    QList<QString> playersInRoom;
};

// 방 상태 구조체
struct Room {
    int roomId;
    QString roomName;
    QList<QTcpSocket*> participants;
    bool isGameInProgress = false;
    QString creatorId;
    GameState gameState;
    QSet<QTcpSocket*> voters;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;

    // 클라이어트와 연결
    QTcpServer* tcpServer; // 클라이언트 연결 요청을 기다리는 TCP 서버 객체
    QList<QTcpSocket*> clientSockets; // 현재 서버에 연결된 모든 클라이언트 소켓들을 저장하는 리스트
    QMap<QTcpSocket*, QByteArray> clientDataBuffers; // 각 클라이언트 소켓으로부터 불완전하게 수신된 데이터를 임시로 저장하는 버퍼 맵
    QMap<QTcpSocket*, quint32> clientNextBlockSizes; // 각 클라이언트 소켓으로부터 다음에 수신될 데이터 블록의 크기를 저장하는 맵
    // DB 연결
    QSqlDatabase db; // MySQL 데이터베이스와 연결을 관리하는 객체
    // 클라이언트와 방 매핑
    QHash<QTcpSocket*, QString> clientUserIds; // 소켓을 키로, 해당 소켓에 연결된 사용자 ID를 값으로 저장하는 해시맵
    QMap<QTcpSocket*, QString> clientToIdMap; // 소켓 포인터를 키로, 사용자의 ID를 값으로 매핑하여 빠르게 찾기 위한 맵
    QMap<QString, QTcpSocket*> idToClientMap; // 사용자 ID를 키로, 해당 사용자의 소켓 포인터를 값으로 매핑하여 빠르게 찾기 위한 맵
    QMap<int, Room*> rooms; // 방 ID를 키로, 방의 정보를 담고 있는 Room 객체를 값으로 저장하는 맵
    int nextRoomId = 1; // 새로 생성될 방에 할당할 다음 고유 ID
    QMap<QTcpSocket*, int> socketToRoomIdMap; // 특정 소켓이 현재 어떤 방에 속해 있는지 매핑하는 맵
    // 투표자와 투표수 매핑
    QMap<QString, int> voteCounts; // 투표 시스템에서 각 항목별 투표 수를 저장하는 맵

private:
    // 서버 유틸리티 함수
    bool connectDatabase(); // DB 연결
    QString hashPassword(const QString& password); // 비밀번호 암호화
    QString getNicknameBySocket(QTcpSocket* socket); // 소켓과 연결된 닉네임을 반환
    QString getNicknameByUserId(const QString& userId); // 아이디와 연결된 닉네임을 반환

    // 응답 전송 및 브로드캐스트 함수
    void SendResponseToClient(QTcpSocket* socket, const QJsonObject& response); // 특정 클라이언트에게 JSON 형식 응답을 전송
    void sendToAllRoomMembers(int roomId, const QJsonObject& response); // 특정 방에 속한 클라이언트에게 JSON 형식 응답을 전송

    // 클라이언트 요청 핸들러 함수
    void HandleRequestByClient(QTcpSocket* socket, const QJsonObject& request); // 클라이언트로부터 수신된 모든 요청을 최초로 받음
    // 회원 관련 요청 핸들러 함수
    void handleSignUp(QTcpSocket* socket, const QJsonObject& request); // 회원가입 처리
    void handleCheckId(QTcpSocket* socket, const QJsonObject& request); // 아이디 중복확인 처리
    void handleCheckEmail(QTcpSocket* socket, const QJsonObject& request); // 이메일 중복확인 처리
    void handleLogin(QTcpSocket* socket, const QJsonObject& request); // 로그인 처리
    void handleFindAccount(QTcpSocket* socket, const QJsonObject& request); // 아이디 찾기 처리
    void handlePasswordChange(QTcpSocket* socket, const QJsonObject& request); // 비밀번호 변경 처리
    void handleLogout(QTcpSocket* socket, const QJsonObject& request); // 로그아웃 처리
    void handleNicknameChange(QTcpSocket* socket, const QJsonObject& request); // 닉네임 변경 처리
    // 게임 관련 요청 핸들러 함수
    void sendRoomListToAllClients(); // 방 목록 모든 클라이언트에게 전송
    void sendUserListToRoom(int roomId); // 특정 방에 있는 사용자 목록을 모든 클라이언트에게 전송
    void handleCreateRoom(QTcpSocket* socket, const QJsonObject& request); // 방 생성 처리
    void handleDeleteRoom(QTcpSocket* socket, const QJsonObject& request); // 방 삭제 처리
    void handleJoinRoom(QTcpSocket* socket, const QJsonObject& request); // 방 입장 처리
    void handleExitRoom(QTcpSocket* socket, const QJsonObject& request); // 방 나가기 처리
    void handleChat(QTcpSocket* socket, const QJsonObject& request); // 일반채팅 처리
    void handleStartGame(QTcpSocket* socket, const QJsonObject& request); // 게임시작 처리
    void handleHintMessage(QTcpSocket* socket, const QJsonObject& request); // 게임채팅 처리
    void handleLiarAnswer(QTcpSocket* socket, const QJsonObject& request); // 라이어 답변 처리
    void sendVotePhase(int roomId); // 투표시작 처리
    void handleVote(QTcpSocket* socket, const QJsonObject& request); // 투표 요청 처리
    void handleVoteResult(int roomId); // 투표 결과 확인 및 처리
    void startNextRound(int roomId); // 다음 라운드 시작 처리
    void endGame(int roomId, const QString& winner, const QString& message); // 게임 종료 처리
    void sendSystemMessageToClient(QTcpSocket* socket, const QString& message); // 특정 클라이언트에게 시스템 메시지 전송
    void sendSystemMessageToRoomMembers(int roomId, const QString& message); // 특정 방 모든 멤버에게 시스템 메시지 전송
    void sendRoleBasedMessagesToClients(int roomId, const QJsonObject& stateData); // 역할에 따라 시스템 메시지 전송

private slots:
    void on_serverStartButton_clicked(); // 서버 시작 버튼
    void handleNewConnection(); // newConnection 시그널 처리
    void handleSocketReadyRead(); // readyRead 시그널 처리
    void handleSocketDisconnected(); // disconnected 시그널 처리
};

#endif // SERVER_MAINWINDOW_H
