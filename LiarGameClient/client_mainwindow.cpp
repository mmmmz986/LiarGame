//Client_mainWindow.cpp

#include "client_mainwindow.h"
#include "ui_client_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QHostAddress>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDataStream>
#include <QFile>
#include <QGraphicsOpacityEffect>


// 생성자 및 소멸자------------------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) // ui 초기화
    , socket(new QTcpSocket(this)) // 소켓 초기화
    , nextBlockSize(0) // nextBlockSize 초기화
    , serverIp("10.10.21.122") // IP
    , serverPort(9806) // 포트 번호
    , currentRoomId(-1) // 초기 방 설정
    , currentGamePhase("") // 게임 단계 초기화
{
    // ui 시작(stackedWidget의 0번 인덱스부터 시작)
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

    // 애플리케이션 시작 시 서버 연결
    socket->connectToHost(serverIp,serverPort);

    // 배경화면 및 폰트 초기화
    setWallpaperOnScreen();
    setPontColor();

    // 비밀번호 입력 에코 모드(* 표시)
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    ui->signupConfirmPasswordLineEdit->setEchoMode(QLineEdit::Password);
    ui->signupPasswordLineEdit->setEchoMode(QLineEdit::Password);
    ui->pwChangeOldPasswordLineEdit->setEchoMode(QLineEdit::Password);
    ui->pwChangeNewPasswordLineEdit->setEchoMode(QLineEdit::Password);
    ui->pwChangeConfrimPasswordLineEdit->setEchoMode(QLineEdit::Password);
    ui->yourOldPasswordLineEdit->setEchoMode(QLineEdit::Password);
    ui->yourNewPasswordLineEdit->setEchoMode(QLineEdit::Password);
    ui->yourConfrimPasswordLineEdit->setEchoMode(QLineEdit::Password);

    // 회원 관리 관련 UI 시그널-슬롯 연결
    connect(ui->signupButton, &QPushButton::clicked, this, [this]{ ui->stackedWidget->setCurrentIndex(1); });
    connect(ui->findAccountButton, &QPushButton::clicked, this, [this]{ ui->stackedWidget->setCurrentIndex(2); });
    connect(ui->findPasswordButton, &QPushButton::clicked, this, [this]{ ui->stackedWidget->setCurrentIndex(3); });
    connect(ui->ChangePasswordButton, &QPushButton::clicked, this, [this]{ ui->stackedWidget->setCurrentIndex(4); });
    connect(ui->ChangeNicknameButton, &QPushButton::clicked, this, [this]{ ui->stackedWidget->setCurrentIndex(5); });
    connect(ui->OptionButton, &QPushButton::clicked, this, [this]{ ui->stackedWidget->setCurrentIndex(9); });

    // 게임 관련 UI 시그널-슬롯 연결
    connect(this, &MainWindow::roomCreated, this, [this](const QString& roomName) {
        QMessageBox::information(this, "방 생성", QString("'%1' 방이 생성되었습니다.").arg(roomName));
    });
    connect(ui->NormalChatEdit_2, &QLineEdit::returnPressed, this, &MainWindow::on_NormalChatEdit_returnPressed); // GamePage의 NormalChatEdit_2의 returnPressed를 NormalChatEdit과 연결

    // 뒤로가기 버튼 연결
    connect(ui->BackButton, &QPushButton::clicked, this, [this]{ ui->stackedWidget->setCurrentIndex(0); });
    connect(ui->BackButton_2, &QPushButton::clicked, this, [this]{ ui->stackedWidget->setCurrentIndex(0); });
    connect(ui->BackButton_3, &QPushButton::clicked, this, [this]{ ui->stackedWidget->setCurrentIndex(0); });
    connect(ui->BackButton_4, &QPushButton::clicked, this, [this]{ ui->stackedWidget->setCurrentIndex(9); });
    connect(ui->BackButton_5, &QPushButton::clicked, this, [this]{ ui->stackedWidget->setCurrentIndex(9); });
    connect(ui->BackButton_6, &QPushButton::clicked, this, [this]{ ui->stackedWidget->setCurrentIndex(6); });

    // 나가기 버튼 연결
    connect(ui->ExitButton, &QPushButton::clicked, this, &MainWindow::ExitButton);
    // connect(ui->ExitButton_2, &QPushButton::clicked, this, &MainWindow::ExitButton);

    // 소켓 시그널-슬롯 연결
    connect(socket, &QTcpSocket::connected, this, &MainWindow::handleConnected);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::handleDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::handleReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &MainWindow::handleError);
} // MainWindow()

MainWindow::~MainWindow()
{
    delete ui;
} // ~MainWindow()
// 생성자 및 소멸자 -----------------------------------------------------------------------------------------------------

// 이미지 -------------------------------------------------------------------------------------------------------------
// 배경화면 설정
void MainWindow::setWallpaperOnScreen()
{
    QPixmap loginpaper(":/images/images/liargamewallpaper.png");

    // 로그인 배경화면
    ui->LoginWallPaperLabel->setPixmap(loginpaper);
    ui->LoginWallPaperLabel->setScaledContents(true); // 이미지가 라벨 위젯의 크기에 맞게 자동 조절됨.
    ui->LoginWallPaperLabel->lower();
    // 회원가입 배경화면
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(); // 밝기 조절
    opacityEffect->setOpacity(0.7);
    ui->RegisterWallPaperLabel->setPixmap(loginpaper);
    ui->RegisterWallPaperLabel->setScaledContents(true); // 이미지가 라벨 위젯의 크기에 맞게 자동 조절됨
    ui->RegisterWallPaperLabel->lower();
    ui->RegisterWallPaperLabel->setGraphicsEffect(opacityEffect);
    // 아이디 찾기 배경화면
    QGraphicsOpacityEffect *opacityEffect2 = new QGraphicsOpacityEffect();
    opacityEffect2->setOpacity(0.7);
    ui->findAccountWallPaperLabel->setPixmap(loginpaper);
    ui->findAccountWallPaperLabel->setScaledContents(true); // 이미지가 라벨 위젯의 크기에 맞게 자동 조절됨
    ui->findAccountWallPaperLabel->lower();
    ui->findAccountWallPaperLabel->setGraphicsEffect(opacityEffect2);
    // 비밀번호 찾기 배경화면
    QGraphicsOpacityEffect *opacityEffect3 = new QGraphicsOpacityEffect();
    opacityEffect3->setOpacity(0.7);
    ui->findPasswordWallPaperLabel->setPixmap(loginpaper);
    ui->findPasswordWallPaperLabel->setScaledContents(true); // 이미지가 라벨 위젯의 크기에 맞게 자동 조절됨
    ui->findPasswordWallPaperLabel->lower();
    ui->findPasswordWallPaperLabel->setGraphicsEffect(opacityEffect3);

    QPixmap mainpaper(":/images/images/roomwallpaper.png");
    // 메인 배경화면
    ui->MainRoomWallPaperLabel->setPixmap(mainpaper);
    ui->MainRoomWallPaperLabel->setScaledContents(true); // 이미지가 라벨 위젯의 크기에 맞게 자동 조절됨
    ui->MainRoomWallPaperLabel->lower();
    // 방 목록 투명도 조절
    QGraphicsOpacityEffect *opacityEffectMainList = new QGraphicsOpacityEffect();
    opacityEffectMainList->setOpacity(0.7);
    ui->roomListWidget->setGraphicsEffect(opacityEffectMainList);
    // 설정 배경화면
    QGraphicsOpacityEffect *opacityEffect4 = new QGraphicsOpacityEffect();
    opacityEffect4->setOpacity(0.7);
    ui->OptionWallPaperLabel->setPixmap(mainpaper);
    ui->OptionWallPaperLabel->setScaledContents(true); // 이미지가 라벨 위젯의 크기에 맞게 자동 조절됨
    ui->OptionWallPaperLabel->lower();
    ui->OptionWallPaperLabel->setGraphicsEffect(opacityEffect4);
    // 닉네임 변경 배경화면
    QGraphicsOpacityEffect *opacityEffect5 = new QGraphicsOpacityEffect();
    opacityEffect5->setOpacity(0.5);
    ui->changeNicknameWallPaperLabel->setPixmap(mainpaper);
    ui->changeNicknameWallPaperLabel->setScaledContents(true); // 이미지가 라벨 위젯의 크기에 맞게 자동 조절됨
    ui->changeNicknameWallPaperLabel->lower();
    ui->changeNicknameWallPaperLabel->setGraphicsEffect(opacityEffect5);
    // 비밀번호 변경 배경화면
    QGraphicsOpacityEffect *opacityEffect6 = new QGraphicsOpacityEffect();
    opacityEffect6->setOpacity(0.5);
    ui->changePasswordWallPaperLabel->setPixmap(mainpaper);
    ui->changePasswordWallPaperLabel->setScaledContents(true); // 이미지가 라벨 위젯의 크기에 맞게 자동 조절됨
    ui->changePasswordWallPaperLabel->lower();
    ui->changePasswordWallPaperLabel->setGraphicsEffect(opacityEffect6);

    QPixmap gamepaper(":/images/images/gamewallpaper.png");
    // 게임대기 배경화면
    ui->GameLobyWallPaperLabel->setPixmap(gamepaper);
    ui->GameLobyWallPaperLabel->setScaledContents(true); // 이미지가 라벨 위젯의 크기에 맞게 자동 조절됨.
    ui->GameLobyWallPaperLabel->lower();
    // 일반채팅 투명도 조절
    QGraphicsOpacityEffect *opacityEffectNormalChatList = new QGraphicsOpacityEffect();
    opacityEffectNormalChatList->setOpacity(0.7);
    ui->NormalChatBrowser->setGraphicsEffect(opacityEffectNormalChatList);
    // 게임 배경화면
    ui->GameWallPaperLabel->setPixmap(gamepaper);
    ui->GameWallPaperLabel->setScaledContents(true); // 이미지가 라벨 위젯의 크기에 맞게 자동 조절됨.
    ui->GameWallPaperLabel->lower();
    // 일반채팅2 투명도 조절
    QGraphicsOpacityEffect *opacityEffectNormalChatList2 = new QGraphicsOpacityEffect();
    opacityEffectNormalChatList2->setOpacity(0.7);
    ui->NormalChatBrowser_2->setGraphicsEffect(opacityEffectNormalChatList2);
    // 게임채팅 투명도 조절
    QGraphicsOpacityEffect *opacityEffectGameChatList = new QGraphicsOpacityEffect();
    opacityEffectGameChatList->setOpacity(0.7);
    ui->GameChatBrowser->setGraphicsEffect(opacityEffectGameChatList);
    // 시스템 메시지 투명도 조절
    QGraphicsOpacityEffect *opacityEffectSystemMessageList = new QGraphicsOpacityEffect();
    opacityEffectSystemMessageList->setOpacity(0.7);
    ui->SystemTextBrowser->setGraphicsEffect(opacityEffectSystemMessageList);
}
// 폰트 설정
void MainWindow::setPontColor()
{
    // 로그인
    ui->Id->setStyleSheet("color: white;");
    ui->Password->setStyleSheet("color: white;");
    // 회원가입
    ui->Id_2->setStyleSheet("color: white;");
    ui->Password_2->setStyleSheet("color: white;");
    ui->confrimPassword->setStyleSheet("color: white;");
    ui->Nickname->setStyleSheet("color: white;");
    ui->Email->setStyleSheet("color: white;");
    ui->passwordMessage->setStyleSheet("color: white;");
    // 아이디 찾기
    ui->Email_2->setStyleSheet("color: white;");
    // 비밀번호 찾기
    ui->Id_3->setStyleSheet("color: white;");
    ui->OldPassword->setStyleSheet("color: white;");
    ui->NewPassword->setStyleSheet("color: white;");
    ui->confrimPassword_2->setStyleSheet("color: white;");

}
// 이미지 -------------------------------------------------------------------------------------------------------------

// 클라이언트 전체 관리 --------------------------------------------------------------------------------------------------
// 버튼 활성화 및 비활성화
void MainWindow::setAllButtonsEnabled(bool enabled)
{
    // 각 버튼의 이름을 사용하여 일괄적으로 상태를 변경
    ui->loginButton->setEnabled(enabled);
    ui->findAccountFindButton->setEnabled(enabled);
    ui->passwordChangeButton->setEnabled(enabled);
    ui->changeNicknameButton->setEnabled(enabled);
    ui->logoutButton->setEnabled(enabled);
    ui->createRoomButton->setEnabled(enabled);
    ui->deleteRoomButton->setEnabled(enabled);
    ui->joinRoomButton->setEnabled(enabled);
    ui->ExitButton->setEnabled(enabled);
}
// 클라이언트 전체 관리 --------------------------------------------------------------------------------------------------


// 서버 연결 함수 -------------------------------------------------------------------------------------------------------
// 연결
void MainWindow::handleConnected()
{
    qDebug() << "서버에 연결되었습니다.";
}
// 연결 해제
void MainWindow::handleDisconnected()
{
    qDebug() << "서버 연결이 끊어졌습니다.";
    ui->stackedWidget->setCurrentIndex(0);
}
// 서버 데이터 읽기
void MainWindow::handleReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_0);

    while (socket->bytesAvailable() > 0) {
        if (nextBlockSize == 0) {
            if (socket->bytesAvailable() < (qint64)sizeof(quint32)) {
                return;
            }
            in >> nextBlockSize;
        }

        if (socket->bytesAvailable() < (qint64)nextBlockSize) {
            return;
        }

        // QDataStream을 사용하여 JSON 데이터를 읽습니다.
        QByteArray responseData;
        in >> responseData;

        // JSON 파싱
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);

        if (!doc.isNull() && doc.isObject() && parseError.error == QJsonParseError::NoError) {
            ResponseByServer(doc.object());
        } else {
            qDebug() << "잘못된 JSON 형식의 응답 수신";
            qDebug() << "파싱 오류:" << parseError.errorString();
        }

        nextBlockSize = 0;
    }
} // handleReadyRead()
// 에러
void MainWindow::handleError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    qDebug() << "소켓 에러 발생: " << socket->errorString();

    QMessageBox::critical(this, "연결 오류", "서버 접속에 실패했습니다: " + socket->errorString());

    // 버튼 재활성화
    ui->loginButton->setText("로그인");
    ui->loginButton->setEnabled(true);
}
// 서버 연결 함수 -------------------------------------------------------------------------------------------------------


// 서버로부터 받은 요청 응답 -----------------------------------------------------------------------------------------------
// 서버의 요청 응답
void MainWindow::ResponseByServer(const QJsonObject& json)
{
    // 응답 처리가 시작되면 isRequestInProgress를 리셋하고 버튼 활성화 함수 호출
    isRequestInProgress = false;
    setAllButtonsEnabled(true);

    QString type = json["type"].toString();
    // 회원 정보 관련 응답-------------------------------------------------------------------------
    // 회원가입 응답
    if (type == "signup_response") {
        handleSignupResponse(json);
    }
    // 아이디 중복검사 응답
    else if (type == "check_id_response") {
        handleIdCheckResponse(json);
    }
    // 이메일 중복검사 응답
    else if (type == "check_email_response") {
        handleEmailCheckResponse(json);
    }
    // 로그인 응답 처리 로직
    else if (type == "login_response") {
        handleLoginResponse(json);
    }
    // 아이디 찾기 응답
    else if (type == "find_account_response") {
        handleFindAccountResponse(json);
    }
    // 비밀번호 찾기시 변경 응답
    else if (type == "password_change_response") {
        handlePasswordChangeResponse(json);
    }
    // 로그아웃
    else if (type == "logout_response") {
        handleLogoutResponse(json);
    }
    // 닉네임 변경 응답
    else if (type == "change_nickname_response") {
        handleChangeNicknameResponse(json);
    }
    // 비밀번호 변경 응답
    else if (type == "change_password_response") {
        handleChangePasswordResponse(json);
    }
    // 회원 정보 관련 응답-------------------------------------------------------------------------

    // 게임 관련 응답-----------------------------------------------------------------------------
    // 서버에서 보낸 방 목록 업데이트 응답 처리
    else if (type == "room_list_update") {
        roomArray = json["room_list"].toArray();
        qDebug() << "서버로부터 방 목록 업데이트 받음. 방 개수:" << roomArray.size();

        // 방 목록이 보이는 페이지라면 즉시 위젯을 업데이트합니다.
        handleRoomListUpdate();
    }
    else if (type == "user_list_update") {
        handleUserListUpdate(json);
    }
    // 방 생성 응답 처리 로직
    else if (type == "create_room_response") {
        handleCreateRoomResponse(json);
    }
    // 방 제거 응답 처리 로직
    else if (type == "delete_room_response") {
        handleDeleteRoomResponse(json);
    }
    // 방 입장 응답
    else if (type == "join_room_response") {
        handleJoinRoomResponse(json);
    }
    // 방 나가기 응답 처리 로직
    else if (type == "leave_room_response") {
        handleLeaveRoomResponse(json);
    }
    // 채팅 메시지 응답 처리 로직
    else if (type == "chat_response") {
        handleChatResponse(json);
    }
    // 게임 시작 응답 처리 로직
    else if (type == "start_game_response") {
        handleStartGameResponse(json);
    }
    // 게임 채팅 응답 처리 로직
    else if (type == "game_chat") {
        handleGameChat(json);
    }
    else if (type == "liar_answer_phase") {
        handleLiarAnswerPhase(json);
    }
    else if (type == "vote_phase") {
        handleVotePhaseResponse(json);
    }
    else if (type == "game_end"){
        handleGameEndResponse(json);
    }
    // 서버 메시지 출력 응답 처리 로직
    else if (type == "system_message") {
        handleSystemMessage(json);
    }
}
void MainWindow::handleSignupResponse(const QJsonObject& json){
    QString result = json["result"].toString();
    QString message = json["message"].toString();
    if (result == "success") {
        // 회원가입 성공 메시지 박스 출력 후 첫 화면으로 이동
        QMessageBox::information(this, "회원가입 성공", "회원가입이 완료되었습니다. 로그인해 주세요.");
        ui->stackedWidget->setCurrentIndex(0);
    } else {
        QMessageBox::warning(this, "회원가입 실패", "회원가입에 실패했습니다: " + message);
    }
}
void MainWindow::handleIdCheckResponse(const QJsonObject& json){
    QString result = json["result"].toString();
    if (result != "failure") {
        QMessageBox::information(this, "아이디 중복확인", "사용 가능한 아이디입니다.");
        isIdChecked = true; // 중복확인 성공 플래그 설정
    } else {
        QString message = json["message"].toString();
        QMessageBox::warning(this, "아이디 중복확인", message);
        isIdChecked = false; // 중복확인 실패
    }
}
void MainWindow::handleEmailCheckResponse(const QJsonObject& json){
    QString result = json["result"].toString();
    if (result != "failure") {
        QMessageBox::information(this, "이메일 중복확인", "사용 가능한 이메일입니다.");
        isEmailChecked = true; // 중복확인 성공 플래그 설정
    } else {
        QString message = json["message"].toString();
        QMessageBox::warning(this, "이메일 중복확인", message);
        isEmailChecked = false; // 중복확인 실패
    }
}
void MainWindow::handleLoginResponse(const QJsonObject& json){
    QString result = json["result"].toString();
    QString message = json["message"].toString();

    if (result == "success") {
        // 로그인 성공
        // 서버 응답에서 닉네임 정보를 받았다면 사용
        QString nickname = json["nickname"].toString();
        QString userId = json["user_id"].toString();

        // 현재 닉네임 변수에 저장
        currentNickname = nickname;
        currentUserId = userId;

        QMessageBox::information(this, "로그인 성공", QString("%1님, 환영합니다.").arg(currentNickname));
        // 로그인 성공 시 원하는 UI로 전환
        ui->stackedWidget->setCurrentIndex(6);
        ui->ShowNickname->setText(nickname);
        ui->ShowNickname_2->setText(nickname);
        ui->ShowNickname_3->setText(nickname);
        ui->ShowNickname->setStyleSheet("border: 2px solid white; border-radius: 5px; color: white;");
        ui->ShowNickname_2->setStyleSheet("border: 2px solid white; border-radius: 5px; color: white;");
        ui->ShowNickname_3->setStyleSheet("border: 2px solid white; border-radius: 5px; color: white;");

        handleRoomListUpdate();
    } else {
        // 로그인 실패
        QMessageBox::warning(this, "로그인 실패", message);
    }
}
void MainWindow::handleFindAccountResponse(const QJsonObject& json){
    QString result = json["result"].toString();
    QString message = json["message"].toString();
    if (result == "success") {
        QString userId = json["user_id"].toString();
        QMessageBox::information(this, "아이디 찾기 성공", "회원님의 아이디는 '" + userId + "' 입니다.");
        ui->stackedWidget->setCurrentIndex(0);
    } else {
        QMessageBox::warning(this, "아이디 찾기 실패", message);
    }
}
void MainWindow::handlePasswordChangeResponse(const QJsonObject& json){
    QString result = json["result"].toString();
    QString message = json["message"].toString();
    if (result == "success") {
        QMessageBox::information(this, "비밀번호 변경 성공", "비밀번호가 성공적으로 변경되었습니다.");
        ui->stackedWidget->setCurrentIndex(0);
    } else {
        QMessageBox::warning(this, "비밀번호 변경 실패", message);
    }
}
void MainWindow::handleLogoutResponse(const QJsonObject& json){
    QString result = json["result"].toString();
    if (result == "success") {
        // 로그아웃 성공 시 상태 초기화
        currentNickname = "";
        currentUserId = "";
        QMessageBox::information(this, "로그아웃 성공", "성공적으로 로그아웃되었습니다.");
        ui->stackedWidget->setCurrentIndex(0);
        ui->ShowNickname->clear();
        ui->ShowNickname_2->clear();
        ui->ShowNickname_3->clear();
    }
}
void MainWindow::handleChangeNicknameResponse(const QJsonObject& json){
    QString result = json["result"].toString();
    QString message = json["message"].toString();
    if (result == "success") {
        QString newNickname = json["new_nickname"].toString();
        currentNickname = newNickname; // 새로운 닉네임으로 업데이트
        QMessageBox::information(this, "닉네임 변경 성공", message);
        ui->stackedWidget->setCurrentIndex(6); // 메인 페이지로 돌아감
        ui->ShowNickname->setText(newNickname);
        ui->ShowNickname_2->setText(newNickname);
        ui->ShowNickname_3->setText(newNickname);
    } else {
        QMessageBox::warning(this, "닉네임 변경 실패", message);
    }
}
void MainWindow::handleChangePasswordResponse(const QJsonObject& json){
    QString result = json["result"].toString();
    QString message = json["message"].toString();
    if (result == "success") {
        QMessageBox::information(this, "비밀번호 변경 성공", message);
        ui->stackedWidget->setCurrentIndex(6); // 메인 페이지로 돌아감
    } else {
        QMessageBox::warning(this, "비밀번호 변경 실패", message);
    }
}
void MainWindow::handleRoomListUpdate()
{
    if (ui->stackedWidget->currentIndex() == 6) {
        ui->roomListWidget->clear();

        for (const QJsonValue &value : roomArray) {
            QJsonObject roomJson = value.toObject();

            int roomId = roomJson["room_id"].toInt();
            QString roomName = roomJson["room_name"].toString();
            QString creatorNickname = roomJson["creator_nickname"].toString();
            int currentPlayers = roomJson["current_players"].toInt();
            QString status = roomJson["status"].toString();

            // QListWidgetItem 객체 생성
            QListWidgetItem* item = new QListWidgetItem();

            // 방 상태에 따라 텍스트에 " (게임 중)" 같은 정보를 추가
            QString statusText = "";
            if (status == "waiting") {
                statusText = " (대기 중)";
            } else {
                statusText = " (게임 중)";
            }

            // 아이템에 표시될 텍스트 설정
            QString roomInfo = QString("%1 (생성자 : %2, %3/10, %4)").arg(roomName).arg(creatorNickname).arg(currentPlayers).arg(statusText);
            item->setText(roomInfo);

            // 사용자 정의 역할(UserRole)에 방 ID 저장
            item->setData(Qt::UserRole, roomId);

            // roomListWidget에 아이템 추가
            ui->roomListWidget->addItem(item);
        }
        qDebug() << "방 목록 업데이트 완료.";
    }
}
void MainWindow::handleUserListUpdate(const QJsonObject& json)
{
    if (json.contains("members")) {
        QJsonArray membersArray = json["members"].toArray();
        ui->UserList->clear(); // 기존 목록을 지우고
        for (const QJsonValue& value : membersArray) {
            ui->UserList->addItem(value.toString()); // 새 목록으로 업데이트
        }
    }
}
void MainWindow::handleCreateRoomResponse(const QJsonObject& json){
    QString result = json["result"].toString();
    if (result == "success") {
        int roomId = json["room_id"].toInt();
        QString roomName = json["room_name"].toString();

        emit roomCreated(roomName);

        // 클라이언트의 현재 방 ID 상태를 업데이트합니다.
        currentRoomId = roomId;

        // 채팅 기록 초기화
        ui->NormalChatBrowser->clear();
        ui->NormalChatBrowser_2->clear();

        // 방 생성 성공 시 바로 로비 페이지로 이동
        ui->stackedWidget->setCurrentIndex(7);

    } else {
        QString message = json["message"].toString();
        QMessageBox::warning(this, "방 생성", message);
    }
}
void MainWindow::handleDeleteRoomResponse(const QJsonObject& json){
    QString result = json["result"].toString();
    QString message = json["message"].toString();
    if (result == "success") {
        QMessageBox::information(this, "방 삭제", message);
    } else {
        QMessageBox::warning(this, "방 삭제", message);
    }
}
// 입장 응답 로직 핸들러
void MainWindow::handleJoinRoomResponse(const QJsonObject& json)
{
    QString result = json["result"].toString();
    if (result == "success") {
        int roomId = json["room_id"].toInt();
        QString roomName = json["room_name"].toString();
        QString UserNickname = json["user_nickname"].toString();

        currentRoomId = roomId;
        qDebug() << "Successfully joined room. Current Room ID is now:" << currentRoomId;
        QMessageBox::information(this, "방 입장", QString("'%1' 방에 입장했습니다.").arg(roomName));

        ui->NormalChatBrowser->clear();
        ui->NormalChatBrowser_2->clear();
        ui->UserList->clear();

        // 서버에서 받은 전체 참가자 목록으로 UserList를 업데이트합니다.
        if (json.contains("members")) {
            QJsonArray membersArray = json["members"].toArray();
            for (const QJsonValue& value : membersArray) {
                ui->UserList->addItem(value.toString());
            }
        }

        // 방 입장 성공 시 게임 페이지로 이동
        ui->stackedWidget->setCurrentIndex(7);
    } else {
        QMessageBox::warning(this, "방 입장", json["message"].toString());
    }
}
// 나가기 응답 로직 핸들러
void MainWindow::handleLeaveRoomResponse(const QJsonObject& json)
{
    QString result = json["result"].toString();
    if (result == "success") {
        QMessageBox::information(this, "방 나가기", "방에서 나갔습니다.");
        ui->ShowJob->clear();
        ui->stackedWidget->setCurrentIndex(6); // 로비 페이지로 돌아갑니다.

        // 맵에서 해당 방의 채팅 기록을 삭제
        if (roomChatHistory.contains(currentRoomId)) {
            roomChatHistory.remove(currentRoomId);
            qDebug() << "채팅 기록 삭제: 방 ID" << currentRoomId;
        }

        // 방 관련 상태 초기화
        currentRoomId = -1;
        // 로비 페이지로 돌아간 후, 최신 방 목록 데이터로 위젯을 업데이트합니다.
        handleRoomListUpdate();
    } else {
        QString message = json["message"].toString();
        QMessageBox::warning(this, "방 나가기", message);
    }
}
// 채팅 메시지 응답을 처리하는 함수
void MainWindow::handleChatResponse(const QJsonObject& json)
{
    QString sender = json["sender"].toString();
    QString message = json["message"].toString();
    int roomId = json["room_id"].toInt();

    // 현재 방이 아닌 다른 방의 채팅 메시지도 맵에 저장
    QString formattedMessage = QString("<p style='margin:0;'><b>%1</b>: %2</p>").arg(sender).arg(message);
    if (!roomChatHistory.contains(roomId)) {
        roomChatHistory[roomId] = "";
    }
    roomChatHistory[roomId] += formattedMessage;

    // 현재 접속한 방의 메시지만 화면에 표시
    if (currentRoomId == roomId) {
        ui->NormalChatBrowser->append(formattedMessage);
        ui->NormalChatBrowser_2->append(formattedMessage);
    }
    isRequestInProgress = false;
    setAllButtonsEnabled(true);
}
// 게임 시작 응답 처리 함수
void MainWindow::handleStartGameResponse(const QJsonObject& json)
{
    QString result = json["result"].toString();
    QString message = json["message"].toString();
    ui->SystemTextBrowser->clear();
    ui->GameChatEdit->setEnabled(true);
    ui->GameChatEdit->setPlaceholderText("");

    if (result == "success") {
        QMessageBox::information(this, "게임 시작", "게임을 시작합니다.");
        // 서버의 성공 응답에 따라 게임 페이지로 이동
        currentGamePhase = "discussion";
        ui->ShowJob->setStyleSheet("border: 2px solid white; border-radius: 5px; color: white;");
        ui->stackedWidget->setCurrentIndex(8);
        ui->GameChatBrowser->clear();
    } else {
        QMessageBox::warning(this, "게임 시작 실패", message);
    }
}
void MainWindow::handleGameChat(const QJsonObject& json){
    QString sender = json["sender_nickname"].toString();
    QString hint = json["message"].toString();
    ui->GameChatBrowser->append(QString("<b>%1</b>: %2").arg(sender).arg(hint));
}
// 라이어의 정답 입력 단계 응답을 처리하는 함수
void MainWindow::handleLiarAnswerPhase(const QJsonObject& json)
{
    QString message = json["message"].toString();
    QString liarId = json["liar_id"].toString();

    // 시스템 메시지 출력
    ui->SystemTextBrowser->append(QString("<p style='color:blue;'>%1</p>").arg(message));

    // 현재 사용자가 라이어인지 확인
    if (currentUserId == liarId) {
        // 라이어인 경우 입력창 활성화
        ui->GameChatEdit->setPlaceholderText("정답을 입력하세요.");
        ui->GameChatEdit->setEnabled(true);
        currentGamePhase = "liar_answer"; // 게임 단계 업데이트
    } else {
        // 시민인 경우 입력창 비활성화
        ui->GameChatEdit->setPlaceholderText("라이어가 정답을 입력하고 있습니다.");
        ui->GameChatEdit->setEnabled(false);
        currentGamePhase = "liar_answer"; // 게임 단계 업데이트
    }
}
// 투표 단계 응답 처리 함수
void MainWindow::handleVotePhaseResponse(const QJsonObject& json)
{
    QString message = json["message"].toString();
    QJsonArray votersArray = json["voters"].toArray();
    QMessageBox::information(this, "투표 단계", message);

    // 투표 다이얼로그 생성
    VoteDialog dialog(votersArray, this);

    // 다이얼로그를 모달로 실행하고 결과 처리
    if (dialog.exec() == QDialog::Accepted) {
        QString votedNickname = dialog.getVotedUserNickname();
        if (!votedNickname.isEmpty()) {
            sendVoteRequest(votedNickname);
        } else {
            // 사용자가 투표를 취소하거나 아무도 선택하지 않은 경우
            QMessageBox::warning(this, "투표 실패", "투표할 대상을 선택하지 않았습니다.");
        }
    }

    currentGamePhase = "voting";
    qDebug() << "투표 대상자:" << votersArray;
}
// 게임 종료 응답 처리 함수
void MainWindow::handleGameEndResponse(const QJsonObject& json)
{
    QString winner = json["winner"].toString();
    QString message = json["message"].toString();
    QMessageBox::information(this, "게임 종료", message);
    // 게임 종료 후 게임 시작 버튼을 누르기 이전 화면으로 돌아감
    ui->stackedWidget->setCurrentIndex(7); // GameLobbyPage로 돌아간다고 가정
    ui->ShowJob->clear();
    currentGamePhase = ""; // 게임 단계 초기화
}
// 시스템 메시지 처리
void MainWindow::handleSystemMessage(const QJsonObject& json)
{
    QString message = json["message"].toString();
    // QtextBrowser에 메시지 추가
    ui->SystemTextBrowser->append(message);

    // 메시지 내용에 따라 'ShowJob' QLabel 업데이트
    if (message.contains("당신은 <font color='red'>라이어</font>입니다.")) {
        ui->ShowJob->setText("<span style='color:red;'>라이어</span>");
    } else if (message.contains("당신은 <font color='blue'>시민</font>입니다.")) {
        ui->ShowJob->setText("시민");
    }
    // '다음 턴은 **내 닉네임**님입니다.'와 같은 메시지인지 확인하여 입력창 활성화
    if (message.contains("이번 입력자는 <font color='blue'>\"%1\"</font>님입니다.")) {
        ui->GameChatEdit->setEnabled(true);
        ui->GameChatEdit->setPlaceholderText("힌트를 입력하세요.");
        currentGamePhase = "discussion";
    }
}
// 서버로부터 받은 요청 응답 -----------------------------------------------------------------------------------------------


// 서버로 클라이언트 요청 전송 ---------------------------------------------------------------------------------------------
// 서버에게 요청 전송
void MainWindow::SendRequestToServer(const QJsonObject& request)
{
    // QByteArray에 데이터를 직렬화합니다.
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);

    // 4바이트 크기 헤더를 위한 공간을 확보합니다.
    out << (quint32)0;

    // JSON 객체를 바이트 배열로 직렬화합니다.
    QJsonDocument doc(request);
    out << doc.toJson(QJsonDocument::Compact);

    // 헤더를 다시 쓰고, 전체 데이터 크기를 업데이트합니다.
    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));

    // 최종 데이터를 소켓으로 전송합니다.
    socket->write(block);

    qDebug() << "JSON 데이터 전송 성공 (총 크기:" << block.size() << "바이트)";
}
// 서버로 클라이언트 요청 전송 ---------------------------------------------------------------------------------------------
// 회원가입, 로그인, 회원정보 변경 슬롯 함수 ---------------------------------------------------------------------------------
// 로그인 버튼 클릭시
void MainWindow::on_loginButton_clicked()
{
    // 로그인 핵심 로직을 별도 함수로 분리
    SendLoginRequest();
}

// 로그인 엔터 키 입력 시
void MainWindow::on_passwordLineEdit_returnPressed()
{
    SendLoginRequest();
}

// 로그인 요청
void MainWindow::SendLoginRequest()
{
    if (isRequestInProgress) return;
    isRequestInProgress = true;
    setAllButtonsEnabled(false);

    QString userId = ui->idLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text().trimmed();

    if (!userId.isEmpty() && !password.isEmpty()) {
        QJsonObject request;
        request["type"] = "login";
        request["user_id"] = userId;
        request["password"] = password;
       SendRequestToServer(request);

        // 입력창 비우기
        ui->idLineEdit->clear();
        ui->passwordLineEdit->clear();
    } else {
        QMessageBox::warning(this, "로그인", "아이디와 비밀번호를 입력하세요.");
        isRequestInProgress = false;
        setAllButtonsEnabled(true);
    }
}

// 회원가입 버튼 클릭시
void MainWindow::on_signupRegisterButton_clicked()
{
    // 중복확인 플래그를 확인하여 중복확인을 강제.
    if (!isIdChecked) {
        QMessageBox::warning(this, "중복확인", "아이디 중복확인을 먼저 해주세요.");
        return;
    }
    if (!isEmailChecked) {
        QMessageBox::warning(this, "중복확인", "이메일 중복확인을 먼저 해주세요.");
        return;
    }

    QString userId = ui->signupIdLineEdit->text().trimmed();
    QString password = ui->signupPasswordLineEdit->text().trimmed();
    QString confrimPassword = ui->signupConfirmPasswordLineEdit->text().trimmed();
    QString nickname = ui->signupNicknameLineEdit->text().trimmed();
    QString email = ui->signupEmailLineEdit->text().trimmed();

    if (userId.isEmpty() || password.isEmpty() || confrimPassword.isEmpty() || nickname.isEmpty() || email.isEmpty()) {
        QMessageBox::warning(this, "회원가입", "모든 필드를 채워주세요.");
        return;
    }
    if(!isPasswordValid(password)){
        QMessageBox::warning(this, "비밀번호", "비밀번호는 최소 8자 이상, 대문자, 특수문자를 각각 1개씩 포함해야 합니다.");
        return;
    }
    if (password != confrimPassword){
        QMessageBox::warning(this, "비밀번호", "입력한 비밀번호가 서로 다릅니다.");
        return;
    }
    QJsonObject request;
    request["type"] = "signup";
    request["user_id"] = userId;
    request["password"] = password;
    request["nickname"] = nickname;
    request["email"] = email;
   SendRequestToServer(request);
} // on_signupRegisterButton_clicked()

// 비밀번호 유효성을 검사하는 도우미 함수
bool MainWindow::isPasswordValid(const QString& password)
{
    // 비밀번호는 최소 8자 이상
    if (password.length() < 8) {
        return false;
    }

    // 대문자, 특수문자 포함 여부를 확인할 플래그
    bool hasUpperCase = false;
    bool hasSpecialChar = false;

    // 모든 문자를 순회하며 조건 확인
    for (const QChar& c : password) {
        if (c.isUpper()) {
            hasUpperCase = true;
        }
        // isSymbol(), isPunct()를 사용하여 특수문자 판별
        if (c.isSymbol() || c.isPunct()) {
            hasSpecialChar = true;
        }
    }

    return hasUpperCase && hasSpecialChar;
}

// 아이디 중복확인 버튼 클릭 시
void MainWindow::on_checkIdButton_clicked()
{
    // 중복확인 버튼 클릭 시, 플래그 초기화 후 서버에 요청을 보냅니다.
    isIdChecked = false;
    QString userId = ui->signupIdLineEdit->text().trimmed();

    if (userId.isEmpty()) {
        QMessageBox::warning(this, "아이디", "아이디를 입력해주세요.");
        return;
    }

    // 아이디 유효성 검사 (예: 길이, 특수문자 등)
    if (userId.length() < 5) {
        QMessageBox::warning(this, "아이디", "아이디는 5자 이상이어야 합니다.");
        return;
    }

    QJsonObject request;
    request["type"] = "check_id";
    request["user_id"] = userId;
   SendRequestToServer(request);
}

// 이메일 중복확인 버튼 클릭 시
void MainWindow::on_checkEmailButton_clicked()
{
    // 중복확인 버튼 클릭 시, 플래그 초기화 후 서버에 요청을 보냅니다.
    isEmailChecked = false;
    QString email = ui->signupEmailLineEdit->text().trimmed();

    if (email.isEmpty()) {
        QMessageBox::warning(this, "이메일", "이메일을 입력해주세요.");
        return;
    }

    // 이메일 유효성 검사 (정규식 사용 등)
    // 간단한 예시: @ 기호 포함 여부
    if (!email.contains("@")) {
        QMessageBox::warning(this, "이메일", "올바른 이메일 형식이 아닙니다.");
        return;
    }

    QJsonObject request;
    request["type"] = "check_email";
    request["email"] = email;
   SendRequestToServer(request);
}

// 아이디 입력창 내용 변경 시 플래그 초기화
void MainWindow::on_signupIdLineEdit_textChanged(const QString& text)
{
    isIdChecked = false;
}

// 이메일 입력창 내용 변경 시 플래그 초기화
void MainWindow::on_signupEmailLineEdit_textChanged(const QString& text)
{
    isEmailChecked = false;
}

// 아이디 찾기
void MainWindow::on_findAccountFindButton_clicked()
{
    if (isRequestInProgress) return;
    isRequestInProgress = true;
    // 모든 버튼 비활성화 함수 호출
    setAllButtonsEnabled(false);

    QString email = ui->findAccountEmailLineEdit->text().trimmed();
    ui->findAccountEmailLineEdit->clear();

    if (email.isEmpty()) {
        QMessageBox::warning(this, "아이디 찾기", "이메일을 입력하세요.");
        return;
    }

    QJsonObject request;
    request["type"] = "find_account";
    request["email"] = email;
   SendRequestToServer(request);
}

// 비밀번호 찾기의 변경
void MainWindow::on_passwordChangeButton_clicked()
{
    if (isRequestInProgress) return;
    isRequestInProgress = true;
    // 모든 버튼 비활성화 함수 호출
    setAllButtonsEnabled(false);

    QString userId = ui->pwChangeIdLineEdit->text().trimmed();
    QString oldPassword = ui->pwChangeOldPasswordLineEdit->text().trimmed();
    QString newPassword = ui->pwChangeNewPasswordLineEdit->text().trimmed();
    QString confrimPassword = ui->pwChangeConfrimPasswordLineEdit->text().trimmed();
    ui->pwChangeIdLineEdit->clear();
    ui->pwChangeOldPasswordLineEdit->clear();
    ui->pwChangeNewPasswordLineEdit->clear();
    ui->pwChangeConfrimPasswordLineEdit->clear();

    if (userId.isEmpty() || oldPassword.isEmpty() || newPassword.isEmpty() || confrimPassword.isEmpty()) {
        QMessageBox::warning(this, "비밀번호 변경", "모든 필드를 채워주세요.");
        return;
    }
    if (newPassword != confrimPassword){
        QMessageBox::warning(this, "비밀번호 변경", "입력한 비밀번호가 서로 다릅니다.");
        return;
    }
    QJsonObject request;
    request["type"] = "password_change";
    request["user_id"] = userId;
    request["old_password"] = oldPassword;
    request["new_password"] = newPassword;
   SendRequestToServer(request);
} // on_passwordChangeButton_clicked()

// 닉네임 변경
void MainWindow::on_changeNicknameButton_clicked()
{
    if (isRequestInProgress) return;
    isRequestInProgress = true;
    // 모든 버튼 비활성화 함수 호출
    setAllButtonsEnabled(false);

    QString newNickname = ui->yourNewNicknameLineEdit->text();
    ui->yourNewNicknameLineEdit->clear();

    if (newNickname.isEmpty()) {
        QMessageBox::warning(this, "닉네임 변경", "새로운 닉네임을 입력하세요.");
        return;
    }

    QJsonObject request;
    request["type"] = "change_nickname";
    request["user_id"] = currentUserId; // 저장해둔 user_id 사용
    request["new_nickname"] = newNickname;
   SendRequestToServer(request);
}

// 설정의 비밀번호 변경
void MainWindow::on_changePasswordButton_clicked()
{
    if (isRequestInProgress) return;
    isRequestInProgress = true;
    // 모든 버튼 비활성화 함수 호출
    setAllButtonsEnabled(false);

    QString oldPassword = ui->yourOldPasswordLineEdit->text();
    QString newPassword = ui->yourNewPasswordLineEdit->text();
    QString confrimPassword = ui->yourConfrimPasswordLineEdit->text();
    ui->yourOldPasswordLineEdit->clear();
    ui->yourNewPasswordLineEdit->clear();
    ui->yourConfrimPasswordLineEdit->clear();

    if (oldPassword.isEmpty() || newPassword.isEmpty() || confrimPassword.isEmpty()) {
        QMessageBox::warning(this, "비밀번호 변경", "모든 필드를 채워주세요.");
        return;
    }
    if (newPassword != confrimPassword){
        QMessageBox::warning(this, "비밀번호 변경", "입력한 비밀번호가 서로 다릅니다.");
        return;
    }
    QJsonObject request;
    request["type"] = "password_change";
    request["user_id"] = currentUserId;
    request["old_password"] = oldPassword;
    request["new_password"] = newPassword;
   SendRequestToServer(request);
} // on_changePasswordButton_clicked()

// 로그아웃
void MainWindow::on_logoutButton_clicked()
{
    if (isRequestInProgress) return;
    isRequestInProgress = true;
    // 모든 버튼 비활성화 함수 호출
    setAllButtonsEnabled(false);

    // 로그아웃 요청을 보낼 JSON 객체 생성
    QJsonObject request;
    request["type"] = "logout";
   SendRequestToServer(request);
}
// 회원가입, 로그인, 회원정보 변경 슬롯 함수 ---------------------------------------------------------------------------------


// 게임 관련 슬롯 함수 ---------------------------------------------------------------------------------------------------
// 방 만들기
void MainWindow::on_createRoomButton_clicked()
{
    if (isRequestInProgress) return;
    isRequestInProgress = true;
    // 모든 버튼 비활성화 함수 호출
    setAllButtonsEnabled(false);

    bool ok;
    QString roomName = QInputDialog::getText(this, tr("방 생성"),
                                             tr("방 이름을 입력하세요:"), QLineEdit::Normal,
                                             QString(), &ok);
    if (ok && !roomName.isEmpty()) {
        QJsonObject request;
        request["type"] = "create_room";
        request["room_name"] = roomName;
        request["creator_id"] = currentUserId; // 현재 로그인된 사용자 ID를 보냅니다.
       SendRequestToServer(request);
    } else{
        isRequestInProgress = false;
        // 모든 버튼 비활성화 함수 호출
        setAllButtonsEnabled(true);
    }
} // on_createRoomButton_clicked()

// 방 삭제
void MainWindow::on_deleteRoomButton_clicked()
{
    if (isRequestInProgress) return;
    isRequestInProgress = true;
    // 모든 버튼 비활성화 함수 호출
    setAllButtonsEnabled(false);

    // 1. 방 목록에서 현재 선택된 아이템이 있는지 확인합니다.
    QListWidgetItem* selectedItem = ui->roomListWidget->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "방 삭제", "삭제할 방을 선택해주세요.");
        isRequestInProgress = false;
        // 모든 버튼 비활성화 함수 호출
        setAllButtonsEnabled(true);
        return;
    }

    // 2. 사용자에게 삭제 여부를 확인하는 메시지 박스를 띄웁니다.
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "방 삭제", "정말로 이 방을 삭제하시겠습니까?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // 3. 사용자가 '예'를 선택한 경우, 방 ID를 가져옵니다.
        int roomId = selectedItem->data(Qt::UserRole).toInt();

        // 4. 서버에 방 삭제 요청을 보낼 JSON 객체를 만듭니다.
        QJsonObject request;
        request["type"] = "delete_room";
        request["room_id"] = roomId;
        request["user_id"] = currentUserId; // 현재 로그인된 사용자 ID를 함께 보냄
       SendRequestToServer(request);
    } else{
        isRequestInProgress = false;
        // 모든 버튼 비활성화 함수 호출
        setAllButtonsEnabled(true);
    }
} // on_deleteRoomButton_clicked()

// 방 입장
void MainWindow::on_joinRoomButton_clicked()
{
    if (isRequestInProgress) return;
    isRequestInProgress = true;
    // 모든 버튼 비활성화 함수 호출
    setAllButtonsEnabled(false);

    // 현재 로그인된 사용자가 있는지 확인
    if (!currentUserId.isEmpty()) {
        // 현재 선택된 리스트 아이템을 가져옵니다.
        QListWidgetItem* selectedItem = ui->roomListWidget->currentItem();

        // 아이템이 선택되었는지 확인
        if (selectedItem) {
            // QListWidgetItem의 사용자 정의 데이터(UserRole)에 저장된 방 ID를 가져옵니다.
            // 이전에 ResponseByServer()에서 setData(Qt::UserRole, roomId)를 사용해 저장했습니다.
            int roomId = selectedItem->data(Qt::UserRole).toInt();

            // 방 ID가 유효한지 확인
            if (roomId > 0) {
                qDebug() << "입장 버튼으로 방 입장 요청. 방 ID:" << roomId;

                // 서버로 보낼 JSON 요청 생성
                QJsonObject request;
                request["type"] = "join_room";
                request["room_id"] = roomId;
                request["user_id"] = currentUserId;

                qDebug() << "요청 전송:" << request;
               SendRequestToServer(request);
            } else {
                // 방 ID가 유효하지 않은 경우
                QMessageBox::warning(this, "입장", "선택된 방의 ID가 유효하지 않습니다.");
                isRequestInProgress = false;
                // 모든 버튼 비활성화 함수 호출
                setAllButtonsEnabled(true);
            }
        } else {
            // 선택된 아이템이 없는 경우
            QMessageBox::warning(this, "입장", "먼저 입장할 방을 선택해주세요.");
            isRequestInProgress = false;
            // 모든 버튼 비활성화 함수 호출
            setAllButtonsEnabled(true);
        }
    } else {
        // 로그인되지 않은 경우
        QMessageBox::warning(this, "입장", "먼저 로그인을 해주세요.");
        isRequestInProgress = false;
        // 모든 버튼 비활성화 함수 호출
        setAllButtonsEnabled(true);
    }
} // on_joinRoomButton_clicked()
// 더블클릭 시 방 입장
void MainWindow::on_roomListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    if (isRequestInProgress) {
        qDebug() << "A request is already in progress. Ignoring room selection double-click.";
        return;
    }
    // 이 시점에서 요청 시작 플래그를 true로 설정합니다.
    isRequestInProgress = true;
    setAllButtonsEnabled(false); // 모든 버튼 비활성화

    // 현재 로그인된 사용자가 있는지 확인하고 아이템이 유효한지 확인
    if (!currentUserId.isEmpty() && item) {
        // QListWidgetItem의 사용자 정의 데이터(UserRole)에 저장된 방 ID를 가져옵니다.
        // 이 데이터는 방 목록을 새로고침하거나 방을 생성할 때 setData() 함수로 저장되어 있어야 합니다.
        int roomId = item->data(Qt::UserRole).toInt();

        // 방 ID가 유효한지 확인
        if (roomId > 0) {
            qDebug() << "더블클릭으로 방 입장 요청. 방 ID:" << roomId;

            // 서버로 보낼 JSON 요청 생성
            QJsonObject request;
            request["type"] = "join_room";
            request["room_id"] = roomId;
            request["user_id"] = currentUserId;

            qDebug() << "요청 전송:" << request;
           SendRequestToServer(request);
        } else {
            // 방 ID가 유효하지 않은 경우
            qDebug() << "오류: 선택된 방의 ID가 유효하지 않습니다.";
        }
    }
} // on_roomListWidget_itemDoubleClicked()

// 방 나가기 버튼 클릭 시
void MainWindow::ExitButton()
{
    if (isRequestInProgress) return;
    isRequestInProgress = true;
    // 모든 버튼 비활성화 함수 호출
    setAllButtonsEnabled(false);

    if (currentRoomId != -1) {
        QJsonObject request;
        request["type"] = "leave_room";
        request["room_id"] = currentRoomId;
        request["user_id"] = currentUserId;
       SendRequestToServer(request);
    } else {
        QMessageBox::warning(this, "방 나가기", "현재 방에 입장해 있지 않습니다.");
    }
}
// 채팅 메시지 엔터 클릭 시
void MainWindow::on_NormalChatEdit_returnPressed()
{
    // 현재 활성화된 페이지에 따라 다른 QLineEdit 사용
    QLineEdit* chatEdit = nullptr;
    if (ui->stackedWidget->currentWidget() == ui->GameLobbyPage) {
        chatEdit = ui->NormalChatEdit;
    } else if (ui->stackedWidget->currentWidget() == ui->GamePage) {
        chatEdit = ui->NormalChatEdit_2;
    }

    if (!chatEdit) {
        return;
    }

    QString message = chatEdit->text();
    if (message.isEmpty() || isRequestInProgress || currentRoomId == -1) {
        return;
    }

    chatEdit->clear(); // 텍스트를 먼저 지웁니다.

    isRequestInProgress = true;
    setAllButtonsEnabled(false);

    QJsonObject request;
    request["type"] = "chat";
    request["room_id"] = currentRoomId; // 현재 방 ID를 전송
    request["message"] = message;
   SendRequestToServer(request);
}
// StartButton을 누르면 GamePage로 이동하는 로직을 담당하는 함수
void MainWindow::on_StartButton_clicked()
{
    // 게임 시작 시, 카테고리 선택 다이얼로그를 먼저 띄웁니다.
    SelectCategoryDialog dialog(this);

    // 카테고리 선택 시그널 연결: 카테고리를 선택하면 서버에 요청을 보냅니다.
    if (dialog.exec() == QDialog::Accepted) {
        // '확인' 버튼을 눌렀을 때만 카테고리 정보 가져오기
        QString category = dialog.getSelectedCategory();

        // 선택된 카테고리를 서버에 전송하는 요청을 보냅니다.
        qDebug() << "선택된 카테고리:" << category;
        sendStartGameRequest(category);
    } else {
        // '취소' 버튼을 눌렀거나 다이얼로그가 닫혔을 때, 아무것도 하지 않습니다.
        qDebug() << "게임 시작이 취소되었습니다.";
        return;
    }

    // 다이얼로그를 모달로 실행하여 사용자가 선택할 때까지 대기합니다.
}
// 게임 시작 요청
void MainWindow::sendStartGameRequest(const QString& category)
{
    QJsonObject request;
    request["type"] = "start_game";
    request["room_id"] = currentRoomId;
    request["category"] = category; // 선택된 카테고리 정보를 JSON에 포함

   SendRequestToServer(request);
}
// 게임 채팅 전송 (힌트 또는 정답 입력)
void MainWindow::on_GameChatEdit_returnPressed()
{
    // 이미 요청이 진행 중인 경우, 중복 전송 방지
    if (isRequestInProgress) return;

    QString message = ui->GameChatEdit->text();
    if (message.isEmpty()) {
        return;
    }
    QJsonObject request;
    request["room_id"] = currentRoomId;

    if (currentGamePhase == "discussion") {
        // 힌트 단계
        request["type"] = "hint_message";
        request["hint_text"] = message;
    } else if (currentGamePhase == "liar_answer") {
        // 라이어의 정답 입력 단계
        request["type"] = "liar_answer";
        request["answer_text"] = message;
    } else {
        // 다른 게임 단계에서는 아무것도 하지 않습니다.
        return;
    }

    // 요청이 시작되었음을 알리는 플래그 설정
    isRequestInProgress = true;

   SendRequestToServer(request);

    // 전송 후 입력창 초기화 및 비활성화
    ui->GameChatEdit->clear();
}
// 투표 요청 전송 함수
void MainWindow::sendVoteRequest(const QString& votedNickname)
{
    QJsonObject request;
    request["type"] = "vote";
    request["room_id"] = currentRoomId;
    request["voted_user_nickname"] = votedNickname;

    SendRequestToServer(request);
}

// 게임 관련 슬롯 함수 -------------------------------------------------------------------------------------------------
