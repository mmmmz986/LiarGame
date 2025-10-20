//Server_mainWindow.cpp

#include "server_mainwindow.h"
#include "ui_server_mainwindow.h"
#include "db_information.h" // DB 연결 정보를 포함하는 헤더
#include "game_data.h"
#include <QDebug>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QDataStream>
#include <QSqlError> // QSqlError 헤더 추가
#include <QSqlQuery>
#include <QRandomGenerator>

// GameData 클래스 초기화
const QMap<QString, QStringList> GameData::wordMap = {
    {"동물", {"고양이", "강아지", "코끼리", "호랑이", "사자", "기린", "펭귄", "토끼", "여우", "원숭이", "개미핥기", "하마"}},
    {"음식", {"피자", "치킨", "햄버거", "스파게티", "초밥", "김치", "라면", "짜장면", "냉면", "돈까스", "파스타"}},
    {"스포츠", {"축구", "농구", "야구", "수영", "테니스", "배구", "탁구", "골프", "배드민턴", "펜싱", "레슬링", "높이뛰기", "멀리뛰기", "봅슬레이", "족구", "피구", "하키"}},
    {"영화", {"어벤져스", "아바타", "기생충", "명량", "타이타닉", "겨울왕국", "해리포터", "신과함께", "친구", "친구2", "바람", "타짜", "타짜3", "타짜2", "서브스턴스"}},
    {"장소", {"학교", "도서관", "병원", "경찰서", "놀이공원", "공원", "카페", "극장", "공항", "바다", "산", "사막", "골목길", "집"}},
    {"직업", {"의사", "간호사", "경찰관", "소방관", "요리사", "선생님", "배우", "가수", "야구선수", "과학자", "군인", "기자", "백수", "학생", "변호사", "회계사", "변리사"}},
    {"사물", {"연필", "책상", "의자", "아이폰", "갤럭시", "컴퓨터", "냉장고", "시계", "안경", "가방", "열쇠", "우산", "신발", "전등", "이어폰", "카메라", "마우스", "칫솔", "침대", "에어컨", "자물쇠"}},
    {"브랜드/제품", {"콜라", "나이키", "아디다스", "애플", "삼성", "LG", "구글", "스타벅스", "맥도날드", "버거킹", "KFC", "BMW", "벤츠", "현대", "테슬라", "메가커피", "스탠리", "벤티"}},
    {"실제인물", {"이순신", "세종대왕", "유재석", "BTS", "손흥민", "김연아", "리사", "아이유", "봉준호", "박지성", "마이클 잭슨", "엘론 머스크", "스티브 잡스", "빌 게이츠", "호날두", "메시", "김도영(KIA 야구선수)", "오타니", "윙(도파민)"}},
    {"나라", {"미국", "이탈리아", "대한민국", "중국", "일본", "이수민", "이집트", "베트남", "태국", "필리핀", "인도", "영국", "프랑스", "러시아", "브라질", "아르헨티나", "베네수엘라", "캐나다"}},
    {"수도", {"도쿄", "서울", "베이징", "워싱턴D.C", "런던", "파리", "로마", "모스크바", "뉴델", "카이로", "마닐라", "방콕", "하노이", "캔버라", "베를린", "오타와", "멕시코시티", "브라질리아", "부에노스아이레스", "스리자야와르데네푸라코테"}},
    {"노래", {"Dynamite", "Gee", "LoveDive", "아무노래", "비행기", "NextLevel", "Sodapop", "Golden", "거짓말(빅뱅)", "Stay", "OMG", "피땀눈물", "DNA", "하루하루", "아리랑", "으르렁", "강남스타일", "Drowning", "LoveYourSelf", "Step", "Butterfly", "소원을 말해봐", "행복"}},
    {"애니메이션", {"나루토", "원피스", "블리치", "진격의 거인", "포켓몬스터", "도라에몽", "짱구는 못말려", "원펀맨", "드래곤볼", "케이팝데몬헌터스", "귀멸의 칼날", "체인소맨", "주술회전", "명탐정 코난", "도쿄구울", "메이저", "캐릭캐릭체인지", "꿈빛파티시엘", "강철의 연금술사", "배틀짱", "썬더일레븐", "세일러문", "꾸러기 수비대", "티니핑", "뽀로로"}},
    {"아이돌", {"빅뱅", "아이브", "헌트릭스", "사자보이즈", "BTS", "에스파", "엔믹스", "라이즈", "에이티즈", "비투비", "블랙핑크", "트와이스", "엑소", "세븐틴", "레드벨벳", "ITZY", "뉴진스", "NCT", "틴탑", "블락비"}},
    {"웹툰", {"폭풍의 전학생", "갓오브하이스쿨", "나혼자만레벨업", "외모지상주의", "마음의 소리", "노블레스", "와라 편의점!", "연애혁명", "전지적독자시점", "스위트홈", "여신강림", "내 ID는 강남미인", "신의 탑", "고수", "호랑이형님", "화산귀환", "조의 영역", "후레자식", "기기괴괴", "타인은 지옥이다", "유미의 세포들"}},
    {"LMS 7차", {"김신헤", "김재완", "김종관", "김영웅", "양승준", "이재은", "유진", "진리", "이왕신", "이명진", "최연주", "최상문", "이영진", "이승필", "임혜린", "이수민", "송권형", "마승수", "나윤성", "나현준", "이은서", "이주희", "박종찬", "김태형", "이상복"}},
    {"게임", {"배틀그라운드", "피파 온라인", "로스트아크", "리그오브레전드", "데이바이데이라이트", "이터널 리턴", "크레이지 아케이드", "카트라이더", "겟엠프드", "건즈", "서든어택", "문명", "메이플스토리", "어몽어스", "구스구스덕", "어둠의 전설", "마비노기", "바람의나라", "테일즈런너", "알투비트", "스페셜포스", "동물농장", "스타튜벨리", "고향만두", "리썰컴퍼니", "고군분투", "옷입히기게임", "배틀필드", "워록", "레프트4데드", "마리오카트", "레드데드리뎀션2", "GTA5", "젤다의전설:야생의숨결", "포켓몬스터 XY", "별의 커비"}}
};

// 생성자, 소멸자 -------------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // TCP 서버 객체 생성 및 시그널 연결
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &MainWindow::handleNewConnection);

    // DB 객체 초기화
    db = QSqlDatabase::addDatabase("QMYSQL");
} // MainWindow()

MainWindow::~MainWindow()
{
    if (db.isOpen()) {
        db.close();
    }
    delete tcpServer;
    delete ui;
} // ~MainWindow()
// 생성자, 소멸자 -------------------------------------------------------------------------------------------------


// 서버, DB 연결 로직----------------------------------------------------------------------------------------------
// 버튼과 서버시작 연결
void MainWindow::on_serverStartButton_clicked()
{
    // 서버가 현재 실행 중인지 확인
    if (tcpServer->isListening()) {
        // 서버가 실행 중이면 종료 로직 수행
        tcpServer->close();
        ui->logTextEdit->append("서버가 종료되었습니다.");
        ui->serverStatusLabel->setText("서버 상태: <font color='red'>종료됨</font>");
        ui->serverStartButton->setText("서버 시작");
        ui->dbStatusLabel->setText("DB 연결 상태: <font color='gray'>끊김</font>");
        if (db.isOpen()) {
            db.close();
        }
    } else {
        // 서버가 종료 상태면 시작 로직 수행
        // 1. DB 연결
        if (!connectDatabase()) {
            ui->logTextEdit->append("DB 연결 실패로 인해 서버를 시작할 수 없습니다.");
            QMessageBox::critical(this, "서버 시작 오류", "DB 연결에 실패했습니다.");
            return;
        }

        // 2. DB 연결 성공 시, 서버 시작
        if (tcpServer->listen(QHostAddress::Any, 9806)) {
            ui->logTextEdit->append("서버 시작 성공! 포트 9806에서 대기 중.");
            ui->serverStatusLabel->setText("서버 상태: <font color='green'>실행 중</font>");
            ui->serverStartButton->setText("서버 중지");
        } else {
            QString errorMsg = "서버 시작 실패: " + tcpServer->errorString();
            ui->logTextEdit->append(errorMsg);
            ui->serverStatusLabel->setText("서버 상태: <font color='red'>실패</font>");
            QMessageBox::critical(this, "서버 시작 오류", errorMsg);
        }
    }
} // on_serverStartButton_clicked()
// DB 연결
bool MainWindow::connectDatabase()
{
    // DB 연결 정보를 반복적으로 설정하지 않도록 한 번만 설정
    if (db.isOpen()) {
        db.close();
    }

    db.setHostName(DB_HOST_NAME);
    db.setDatabaseName(DB_DATABASE_NAME);
    db.setUserName(DB_USER_NAME);
    db.setPassword(DB_PASSWORD);

    if (db.open()) {
        ui->logTextEdit->append("DB 연결 성공!");
        ui->dbStatusLabel->setText("DB 연결 상태: <font color='green'>성공</font>");
        return true;
    } else {
        ui->logTextEdit->append("DB 연결 실패: " + db.lastError().text());
        ui->dbStatusLabel->setText("DB 연결 상태: <font color='red'>실패</font>");
        return false;
    }
} // connectDatabase()
// 새로운 클라이언트 관리
void MainWindow::handleNewConnection()
{
    // 새로운 클라이언트 소켓을 가져와서 리스트에 추가
    QTcpSocket* newClientSocket = tcpServer->nextPendingConnection();
    if (newClientSocket) {
        clientSockets.append(newClientSocket);
        ui->logTextEdit->append("새로운 클라이언트 접속: " + newClientSocket->peerAddress().toString());

        // 소켓의 시그널들을 슬롯에 연결
        connect(newClientSocket, &QTcpSocket::readyRead, this, &MainWindow::handleSocketReadyRead);
        connect(newClientSocket, &QTcpSocket::disconnected, this, &MainWindow::handleSocketDisconnected);

        // 새로운 클라이언트의 패킷 관련 정보 초기화
        clientDataBuffers[newClientSocket] = QByteArray();
        clientNextBlockSizes[newClientSocket] = 0;

        // 로그에 연결 정보 기록
        qDebug() << "새로운 클라이언트 연결:" << newClientSocket->peerAddress().toString();

        // 새로운 클라이언트가 연결되자마자 DB에 있는 방 목록을 전송
        sendRoomListToAllClients();
    }
} // handleNewConnection()
// 클라이언트의 전송 데이터 관리
void MainWindow::handleSocketReadyRead()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    // 소켓과 연결된 QDataStream을 생성합니다.
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_0);

    // 루프를 사용하여 소켓에 남은 모든 데이터 패킷을 처리합니다.
    while (socket->bytesAvailable() > 0) {
        quint32 &nextBlockSize = clientNextBlockSizes[socket];

        // 패킷 크기 아직 모르면 읽기 시도
        if (nextBlockSize == 0) {
            if (socket->bytesAvailable() < (int)sizeof(quint32))
                break; // 아직 헤더도 안 옴 → 다음 readyRead 때 이어서
            in >> nextBlockSize;
        }

        // 패킷 크기(4바이트)가 충분히 도착했는지 확인합니다.
        if (socket->bytesAvailable() < nextBlockSize) {
            break;
        }

        // 패킷 본문 데이터를 읽습니다.
        QByteArray jsonData;
        in >> jsonData;

        // JSON 데이터 처리
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

        if (!doc.isNull() && doc.isObject() && parseError.error == QJsonParseError::NoError) {
            HandleRequestByClient(socket, doc.object());
        } else {
            ui->logTextEdit->append("잘못된 JSON 형식의 요청 수신");
            qDebug() << "파싱 오류: " << parseError.errorString();
        }
        nextBlockSize = 0;
    }
} // handleSocketReadyRead()
// 클라이언트 연결 해제
void MainWindow::handleSocketDisconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        QString userId = clientToIdMap.value(socket);
        qDebug() << "클라이언트 연결 끊김:" << socket->peerAddress().toString() << " (User ID: " << userId << ")";
        ui->logTextEdit->append("클라이언트 연결 끊김: " + socket->peerAddress().toString());

        // 사용자 정보가 있는 경우에만 처리
        if (!userId.isEmpty()) {
            QSqlQuery query(db);

            // 1. 사용자가 속했던 방 ID를 찾습니다.
            int roomId = -1;
            query.prepare("SELECT room_id FROM room_members WHERE user_id = :user_id");
            query.bindValue(":user_id", userId);
            if (query.exec() && query.next()) {
                roomId = query.value("room_id").toInt();
            }

            // 2. 데이터베이스에서 사용자의 방 정보 삭제
            query.prepare("DELETE FROM room_members WHERE user_id = :user_id");
            query.bindValue(":user_id", userId);
            if (query.exec()) {
                qDebug() << "연결 끊김으로 인해 사용자 방 정보 정리 성공. 사용자 ID:" << userId;

                // 3. 만약 방 ID가 존재했다면, 방에 남은 멤버가 있는지 확인
                if (roomId != -1) {
                    QSqlQuery countQuery(db);
                    countQuery.prepare("SELECT COUNT(*) FROM room_members WHERE room_id = :roomId");
                    countQuery.bindValue(":roomId", roomId);
                    countQuery.exec();
                    countQuery.next();
                    int remainingMembers = countQuery.value(0).toInt();

                    // 4. 남은 멤버가 없으면 방을 삭제
                    if (remainingMembers == 0) {
                        // game_states 테이블 먼저 삭제
                        QSqlQuery deleteGameStateQuery(db);
                        deleteGameStateQuery.prepare("DELETE FROM game_states WHERE room_id = :roomId");
                        deleteGameStateQuery.bindValue(":roomId", roomId);
                        if (!deleteGameStateQuery.exec()) {
                            qDebug() << "게임 상태 삭제 실패:" << deleteGameStateQuery.lastError().text();
                            // 에러가 발생해도 다른 정리 로직은 계속 진행
                        }
                        // 이후 rooms 테이블 삭제
                        QSqlQuery deleteRoomQuery(db);
                        deleteRoomQuery.prepare("DELETE FROM rooms WHERE room_id = :roomId");
                        deleteRoomQuery.bindValue(":roomId", roomId);
                        if (deleteRoomQuery.exec()) {
                            qDebug() << "방에 남은 멤버가 없어 방" << roomId << "가 삭제되었습니다.";
                        }
                        // 수정된 부분: 서버 메모리에서 Room 객체 삭제
                        if (rooms.contains(roomId)) {
                            delete rooms.value(roomId);
                            rooms.remove(roomId);
                            qDebug() << "방 (ID:" << roomId << ")가 클라이언트 연결 끊김으로 인해 서버 메모리에서 삭제되었습니다.";
                        }
                    }
                }
            } else {
                qDebug() << "연결 끊김으로 인한 방 정보 정리 실패:" << query.lastError().text();
            }

            // 5. 서버 측 맵에서 해당 사용자 정보 정리
            clientToIdMap.remove(socket);
            idToClientMap.remove(userId);
            clientUserIds.remove(socket);

            // 6. 모든 클라이언트에게 방 목록 업데이트 전송
            sendRoomListToAllClients();

            // 7. 모든 클라이언트에게 보내는 대신, 방 멤버가 남아있다면 해당 방에만 메시지를 전송.
            if (roomId != -1) {
                // 특정 방 멤버에게 메시지를 보냅니다.
                sendSystemMessageToRoomMembers(roomId, QString("'%1'님이 방을 떠났습니다.").arg(getNicknameByUserId(userId)));
            } else {
                // 방에 속하지 않은 상태에서 연결이 끊긴 경우
                // 이 메시지는 모든 클라이언트에게 그대로 보내는 것이 일반적입니다.
                sendSystemMessageToRoomMembers(roomId, QString("'%1'님이 방을 떠났습니다.").arg(getNicknameByUserId(userId)));
            }
        }

        // 8. 소켓 목록에서 제거
        clientSockets.removeOne(socket);
        clientDataBuffers.remove(socket);
        clientNextBlockSizes.remove(socket);

        // 9. 소켓 객체를 안전하게 삭제
        socket->deleteLater();
    }
} // handleSocketDisconnected()
// 서버, DB 연결 로직 ---------------------------------------------------------------------------------------------

// 서버 클라이언트 전송 응답 -----------------------------------------------------------------------------------------
// 클라이언트로부터 요청
void MainWindow::HandleRequestByClient(QTcpSocket* socket, const QJsonObject& request)
{
    // JSON 객체에서 "type" 키를 읽음.
    QString requestType = request["type"].toString();

    // 로그에 요청 타입과 클라이언트 주소 기록
    ui->logTextEdit->append(QString("요청 수신 (%1): %2").arg(socket->peerAddress().toString(), requestType));

    // "type" 키와 대응된 값에 따라 처리 함수로 분기
    // 클라이언트와 일치해야 함
    if (requestType == "login") {
        handleLogin(socket, request);
    } else if (requestType == "signup") {
        handleSignUp(socket, request);
    } else if (requestType == "check_id") {
        handleCheckId(socket, request);
    } else if (requestType == "check_email") {
        handleCheckEmail(socket, request);
    } else if (requestType == "find_account") {
        handleFindAccount(socket, request);
    } else if (requestType == "password_change") {
        handlePasswordChange(socket, request);
    } else if (requestType == "logout") {
        handleLogout(socket, request);
    } else if (requestType == "change_nickname") {
        handleNicknameChange(socket, request);
    } else if (requestType == "create_room") {
        handleCreateRoom(socket, request);
    } else if (requestType == "join_room") {
        handleJoinRoom(socket, request);
    } else if (requestType == "leave_room") {
        handleExitRoom(socket, request);
    } else if (requestType == "delete_room") {
        handleDeleteRoom(socket, request);
    } else if (requestType == "chat") {
        handleChat(socket, request);
    } else if (requestType == "start_game") {
        handleStartGame(socket, request);
    } else if (requestType == "hint_message") {
        handleHintMessage(socket, request);
    } else if (requestType == "liar_answer") {
        handleLiarAnswer(socket, request);
    } else if (requestType == "vote") {
        handleVote(socket, request);
    }
} // HandleRequestByClient(QTcpSocket* socket, const QJsonObject& request)
// 클라이언트에게 전송
void MainWindow::SendResponseToClient(QTcpSocket* socket, const QJsonObject& response)
{
    // JSON 객체를 바이트 배열로 변환
    QByteArray jsonData = QJsonDocument(response).toJson(QJsonDocument::Compact);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);

    // 4바이트 헤더를 위한 공간을 확보합니다.
    out << (quint32)0;

    // JSON 데이터를 직렬화하여 담습니다.
    out << jsonData;

    // 헤더를 다시 쓰고, 전체 데이터 크기를 업데이트합니다.
    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));

    // 데이터 전송
    socket->write(block);
    socket->flush();
}
// 서버 클라이언트 전송 응답 -----------------------------------------------------------------------------------------


// 서버 유틸리티 ---------------------------------------------------------------------------------------------------
QString MainWindow::getNicknameBySocket(QTcpSocket* socket)
{
    if (clientToIdMap.contains(socket)) {
        QString userId = clientToIdMap[socket];
        QSqlQuery query(db);
        query.prepare("SELECT nickname FROM users WHERE user_id = :user_id");
        query.bindValue(":user_id", userId);
        if (query.exec() && query.next()) {
            return query.value("nickname").toString();
        }
    }
    return QString(); // 닉네임을 찾지 못한 경우 빈 문자열 반환
}
// 사용자 ID로 닉네임을 가져오는 함수
QString MainWindow::getNicknameByUserId(const QString& userId)
{
    QSqlQuery query(db);
    query.prepare("SELECT nickname FROM users WHERE user_id = :userId");
    query.bindValue(":userId", userId);
    if (query.exec() && query.next()) {
        return query.value("nickname").toString();
    }
    return "";
}
// 서버 유틸리티 ---------------------------------------------------------------------------------------------------


// 회원정보 관리 로직-----------------------------------------------------------------------------------------------
// 비밀번호를 해시하는 함수
QString MainWindow::hashPassword(const QString& password)
{
    QByteArray data = password.toUtf8();
    // SHA256 해싱
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
    return QString(hash.toHex());
}
// 회원가입 로직
void MainWindow::handleSignUp(QTcpSocket* socket, const QJsonObject& request)
{
    QString userId = request["user_id"].toString();
    QString password = request["password"].toString();
    QString nickname = request["nickname"].toString();
    QString email = request["email"].toString();

    // 입력값 유효성 검사
    if (userId.isEmpty() || password.isEmpty() || nickname.isEmpty() || email.isEmpty()) {
        QJsonObject response;
        response["type"] = "signup_response";
        response["result"] = "failure";
        response["message"] = "모든 필드를 입력해야 합니다.";
        SendResponseToClient(socket, response);
        return;
    }

    // 중복이 없을 경우, 회원가입 진행
    QString hashedPassword = hashPassword(password);
    QSqlQuery query(db);
    query.prepare("INSERT INTO users (user_id, password, nickname, email) VALUES (:user_id, :password, :nickname, :email)");
    query.bindValue(":user_id", userId);
    query.bindValue(":password", hashedPassword);
    query.bindValue(":nickname", nickname);
    query.bindValue(":email", email);

    QJsonObject response;
    response["type"] = "signup_response";

    if (query.exec()) {
        ui->logTextEdit->append(QString("회원가입 성공: %1").arg(userId));
        response["result"] = "success";
    } else {
        ui->logTextEdit->append(QString("회원가입 실패: %1, 이유: %2").arg(userId, query.lastError().text()));
        response["result"] = "failure";
        response["message"] = "회원가입에 실패했습니다. 데이터베이스 오류가 발생했습니다.";
    }

    SendResponseToClient(socket, response);
}
// 아이디 중복확인 로직
void MainWindow::handleCheckId(QTcpSocket* socket, const QJsonObject& request)
{
    QString userId = request["user_id"].toString();
    QJsonObject response;
    response["type"] = "check_id_response";

    QSqlQuery query(db);
    query.prepare("SELECT user_id FROM users WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);

    if (query.exec() && query.next()) {
        response["result"] = "failure";
        response["message"] = "이미 사용 중인 아이디입니다.";
    } else {
        response["result"] = "success";
        response["message"] = "사용 가능한 아이디입니다.";
    }

    SendResponseToClient(socket, response);
}

// 이메일 중복확인 로직
void MainWindow::handleCheckEmail(QTcpSocket* socket, const QJsonObject& request)
{
    QString email = request["email"].toString();
    QJsonObject response;
    response["type"] = "check_email_response";

    QSqlQuery query(db);
    query.prepare("SELECT email FROM users WHERE email = :email");
    query.bindValue(":email", email);

    if (query.exec() && query.next()) {
        response["result"] = "failure";
        response["message"] = "이미 사용 중인 이메일입니다.";
    } else {
        response["result"] = "success";
        response["message"] = "사용 가능한 이메일입니다.";
    }

    SendResponseToClient(socket, response);
}

// 로그인 로직
void MainWindow::handleLogin(QTcpSocket* socket, const QJsonObject& request)
{
    // 요청 JSON 객체에서 아이디와 비밀번호를 추출합니다.
    QString userId = request["user_id"].toString();
    QString password = request["password"].toString();

    // 응답 메시지를 담을 JSON 객체
    QJsonObject response;
    response["type"] = "login_response";

    // MySQL 데이터베이스에서 해당 아이디를 찾아 비밀번호를 검증합니다.
    QSqlQuery query(db);
    // 준비된 쿼리를 사용하여 SQL 인젝션 공격을 방지합니다.
    query.prepare("SELECT password, nickname FROM users WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);

    if (!query.exec()) {
        qWarning() << "로그인 쿼리 실행 실패:" << query.lastError().text();
        response["result"] = "failure";
        response["message"] = "데이터베이스 오류가 발생했습니다.";
        SendResponseToClient(socket, response);
        return;
    }

    if (query.next()) {
        // 데이터베이스에서 조회된 해시된 비밀번호를 가져옵니다.
        QString storedHash = query.value("password").toString();
        // 입력받은 비밀번호를 해시하여 저장된 비밀번호와 비교합니다.
        if (hashPassword(password) == storedHash) {
            // 로그인 성공
            ui->logTextEdit->append(QString("로그인 성공: %1").arg(userId));
            QString nickname = query.value("nickname").toString();
            QString userId = request["user_id"].toString();
            idToClientMap.insert(userId, socket);
            clientToIdMap[socket] = userId;
            response["result"] = "success";
            response["message"] = "Login successful.";
            response["user_id"] = userId;
            response["nickname"] = nickname;
            clientUserIds[socket] = userId;
        } else {
            // 비밀번호 불일치
            ui->logTextEdit->append(QString("로그인 실패: 비밀번호 불일치, ID: %1").arg(userId));
            response["result"] = "failure";
            response["message"] = "아이디 또는 비밀번호가 틀렸습니다.";
        }
    } else {
        // 아이디 없음
        ui->logTextEdit->append(QString("로그인 실패: 아이디 없음, ID: %1").arg(userId));
        response["result"] = "failure";
        response["message"] = "아이디 또는 비밀번호가 틀렸습니다.";
    }

    SendResponseToClient(socket, response);
} // handleLogin(QTcpSocket* socket, const QJsonObject& request)

// 회원정보 찾기 (아이디 찾기) 로직
void MainWindow::handleFindAccount(QTcpSocket* socket, const QJsonObject& request)
{
    QString email = request["email"].toString();

    QSqlQuery query(db);
    query.prepare("SELECT user_id FROM users WHERE email = :email");
    query.bindValue(":email", email);

    QJsonObject response;
    response["type"] = "find_account_response";

    if (!query.exec()) {
        qWarning() << "회원정보 찾기 쿼리 실행 실패:" << query.lastError().text();
        response["result"] = "failure";
        response["message"] = "데이터베이스 오류가 발생했습니다.";
        SendResponseToClient(socket, response);
        return;
    }

    if (query.next()) {
        QString userId = query.value(0).toString();
        ui->logTextEdit->append(QString("회원정보 찾기 성공: %1").arg(userId));
        response["result"] = "success";
        response["user_id"] = userId;
    } else {
        ui->logTextEdit->append(QString("회원정보 찾기 실패 (이메일 없음): %1").arg(email));
        response["result"] = "failure";
        response["message"] = "등록된 이메일이 없습니다.";
    }

    SendResponseToClient(socket, response);
} // handleFindAccount(QTcpSocket* socket, const QJsonObject& request)

// 비밀번호 변경 로직
void MainWindow::handlePasswordChange(QTcpSocket* socket, const QJsonObject& request)
{
    QString userId = request["user_id"].toString();
    QString oldPassword = request["old_password"].toString();
    QString newPassword = request["new_password"].toString();

    QString hashedOldPassword = hashPassword(oldPassword);
    QString hashedNewPassword = hashPassword(newPassword);

    QSqlQuery query(db);
    query.prepare("UPDATE users SET password = :new_password WHERE user_id = :user_id AND password = :old_password");
    query.bindValue(":new_password", hashedNewPassword);
    query.bindValue(":user_id", userId);
    query.bindValue(":old_password", hashedOldPassword);

    QJsonObject response;
    response["type"] = "password_change_response";

    if (!query.exec()) {
        qWarning() << "비밀번호 변경 쿼리 실행 실패:" << query.lastError().text();
        response["result"] = "failure";
        response["message"] = "데이터베이스 오류가 발생했습니다.";
        SendResponseToClient(socket, response);
        return;
    }

    if (query.numRowsAffected() > 0) {
        ui->logTextEdit->append(QString("비밀번호 변경 성공: %1").arg(userId));
        response["result"] = "success";
    } else {
        ui->logTextEdit->append(QString("비밀번호 변경 실패: %1, 이유: %2").arg(userId, query.lastError().text()));
        response["result"] = "failure";
        response["message"] = "기존 비밀번호가 일치하지 않습니다.";
    }

    SendResponseToClient(socket, response);
} // handlePasswordChange(QTcpSocket* socket, const QJsonObject& request)

// 로그아웃 요청 처리
void MainWindow::handleLogout(QTcpSocket* socket, const QJsonObject& request)
{
    Q_UNUSED(request); // request 매개변수 사용 안 함
    QJsonObject response;
    // 로그아웃 시 맵에서 해당 소켓 정보 삭제
    clientToIdMap.remove(socket);
    response["type"] = "logout_response";
    response["result"] = "success";
    SendResponseToClient(socket, response);
} // handleLogout(QTcpSocket* socket, const QJsonObject& request)

// 닉네임 변경 요청 처리
void MainWindow::handleNicknameChange(QTcpSocket* socket, const QJsonObject& request)
{
    QString userId = request["user_id"].toString();
    QString newNickname = request["new_nickname"].toString();

    QJsonObject response;
    response["type"] = "change_nickname_response";

    QSqlQuery query(db);
    query.prepare("UPDATE users SET nickname = :new_nickname WHERE user_id = :user_id");
    query.bindValue(":new_nickname", newNickname);
    query.bindValue(":user_id", userId);

    if (query.exec() && query.numRowsAffected() > 0) {
        ui->logTextEdit->append(QString("닉네임 변경 성공: %1 -> %2").arg(userId, newNickname));
        response["result"] = "success";
        response["new_nickname"] = newNickname;
        response["message"] = "닉네임이 성공적으로 변경되었습니다.";
    } else {
        ui->logTextEdit->append(QString("닉네임 변경 실패: %1, 이유: %2").arg(userId, query.lastError().text()));
        response["result"] = "failure";
        response["message"] = "새로운 닉네임을 입력하세요.";
    }
    SendResponseToClient(socket, response);
    sendRoomListToAllClients();
} // handleNicknameChange(QTcpSocket* socket, const QJsonObject& request)
// 회원정보 관리 로직-----------------------------------------------------------------------------------------------

// 게임 관리 로직--------------------------------------------------------------------------------------------------
// 모든 클라이언트에게 방 정보를 전송(목록 실시간 업데이트)
void MainWindow::sendRoomListToAllClients()
{
    if (!db.isOpen()) {
        qDebug() << "Database is not open.";
        return;
    }

    QJsonArray roomsArray;
    QSqlQuery query(db);

    // `HAVING` 절이 제거되어, 모든 방을 조회합니다.
    query.prepare(R"(
        SELECT
            r.room_id,
            r.room_name,
            r.creator_id,
            r.status,
            u.nickname AS creator_nickname,
            COUNT(rm.user_id) AS current_players
        FROM
            rooms AS r
        LEFT JOIN
            room_members AS rm ON r.room_id = rm.room_id
        LEFT JOIN
            users AS u ON r.creator_id = u.user_id
        GROUP BY
            r.room_id
        ORDER BY
            r.room_id
    )");

    if (!query.exec()) {
        qDebug() << "Query failed:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        QJsonObject roomObject;
        roomObject["room_id"] = query.value("room_id").toInt();
        roomObject["room_name"] = query.value("room_name").toString();
        roomObject["creator_nickname"] = query.value("creator_nickname").toString();
        roomObject["current_players"] = query.value("current_players").toInt();
        roomObject["status"] = query.value("status").toString();
        roomsArray.append(roomObject);
    }

    QJsonObject response;
    response["type"] = "room_list_update";
    response["room_list"] = roomsArray;

    for (QTcpSocket* client : clientSockets) {
        SendResponseToClient(client, response);
    }
    qDebug() << "모든 클라이언트에게 방 목록 업데이트 전송 완료.";
} // sendRoomListToAllClients()
// 유저 리스트 업데이트 전송
void MainWindow::sendUserListToRoom(int roomId)
{
    // DB에서 해당 방의 모든 참가자 닉네임을 조회
    QSqlQuery memberQuery(db);
    memberQuery.prepare("SELECT T2.nickname FROM room_members AS T1 JOIN users AS T2 ON T1.user_id = T2.user_id WHERE T1.room_id = :room_id");
    memberQuery.bindValue(":room_id", roomId);

    QJsonArray memberList;
    if (memberQuery.exec()) {
        while (memberQuery.next()) {
            memberList.append(memberQuery.value("nickname").toString());
        }
    }

    // 전송할 JSON 객체 생성
    QJsonObject updateData;
    updateData["type"] = "user_list_update";
    updateData["room_id"] = roomId;
    updateData["members"] = memberList;

    // 해당 방에 있는 모든 클라이언트에게 메시지 전송
    if (rooms.contains(roomId)) {
        Room* room = rooms.value(roomId);
        for (QTcpSocket* memberSocket : room->participants) {
            SendResponseToClient(memberSocket, updateData);
        }
    }
}
// 방 생성 로직
void MainWindow::handleCreateRoom(QTcpSocket* socket, const QJsonObject& request)
{
    QString roomName = request["room_name"].toString().trimmed();
    QString creatorId = request["creator_id"].toString();

    QJsonObject response;
    response["type"] = "create_room_response";

    if (roomName.isEmpty() || creatorId.isEmpty()) {
        response["result"] = "failure";
        response["message"] = "방 제목과 사용자 정보가 유효하지 않습니다.";
        SendResponseToClient(socket, response);
        return;
    }

    // 트랜잭션 시작
    if (!db.transaction()) {
        response["result"] = "failure";
        response["message"] = "서버 오류: 트랜잭션을 시작할 수 없습니다.";
        SendResponseToClient(socket, response);
        return;
    }

    QSqlQuery query(db);

    // 1. rooms 테이블에 방 정보 삽입
    query.prepare("INSERT INTO rooms (room_name, creator_id, status) VALUES (:room_name, :creator_id, :status)");
    query.bindValue(":room_name", roomName);
    query.bindValue(":creator_id", creatorId);
    query.bindValue(":status", "waiting");

    if (!query.exec()) {
        qDebug() << "방 생성 실패 (rooms 테이블):" << query.lastError().text();
        db.rollback(); // 실패 시 롤백
        response["result"] = "failure";
        response["message"] = "방 생성에 실패했습니다.";
        SendResponseToClient(socket, response);
        return;
    }

    int newRoomId = query.lastInsertId().toInt();

    // 2. room_members 테이블에 방장 정보 삽입
    QSqlQuery memberQuery(db);
    memberQuery.prepare("INSERT INTO room_members (room_id, user_id, join_time) VALUES (:room_id, :user_id, NOW())");
    memberQuery.bindValue(":room_id", newRoomId);
    memberQuery.bindValue(":user_id", creatorId);

    if (!memberQuery.exec()) {
        qDebug() << "방 멤버 추가 실패:" << memberQuery.lastError().text();
        db.rollback(); // 실패 시 롤백
        response["result"] = "failure";
        response["message"] = "방 멤버 추가에 실패했습니다.";
        SendResponseToClient(socket, response);
        return;
    }

    // 두 쿼리 모두 성공했을 경우 트랜잭션 커밋
    if (db.commit()) {
        // 서버 메모리 상태(in-memory) 업데이트
        Room* newRoom = new Room();
        newRoom->roomId = newRoomId;
        newRoom->roomName = roomName;
        newRoom->creatorId = creatorId;
        newRoom->participants.append(socket); // 방 생성자는 바로 참가자로 추가
        rooms[newRoomId] = newRoom;
        socketToRoomIdMap[socket] = newRoomId;

        QSqlQuery nicknameQuery(db);
        nicknameQuery.prepare("SELECT nickname FROM users WHERE user_id = :user_id");
        nicknameQuery.bindValue(":user_id", creatorId);

        QString creatorNickname = "Unknown";
        if (nicknameQuery.exec() && nicknameQuery.next()) {
            creatorNickname = nicknameQuery.value(0).toString();
        }

        ui->logTextEdit->append(QString("방 생성 성공: '%1' (ID: %2) by %3").arg(roomName).arg(newRoomId).arg(creatorId));

        response["result"] = "success";
        response["room_id"] = newRoomId;
        response["room_name"] = roomName;
        response["creator_nickname"] = creatorNickname;

        sendRoomListToAllClients(); // 모든 클라이언트 업데이트
        sendUserListToRoom(newRoomId);
        SendResponseToClient(socket, response);
    } else {
        qDebug() << "방 생성 트랜잭션 커밋 실패:" << db.lastError().text();
        db.rollback(); // 커밋 실패 시 롤백
        response["result"] = "failure";
        response["message"] = "서버 오류로 방 생성에 실패했습니다.";
        SendResponseToClient(socket, response);
    }
}
// 방 제거 로직
void MainWindow::handleDeleteRoom(QTcpSocket* socket, const QJsonObject& request)
{
    int roomId = request["room_id"].toInt();
    QString userId = request["user_id"].toString();

    QJsonObject response;
    response["type"] = "delete_room_response";

    // 1. 방의 생성자 ID를 조회하여 요청한 사용자와 일치하는지 확인
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT creator_id FROM rooms WHERE room_id = :room_id");
    checkQuery.bindValue(":room_id", roomId);

    if (!checkQuery.exec() || !checkQuery.next()) {
        response["result"] = "failure";
        response["message"] = "존재하지 않는 방입니다.";
        SendResponseToClient(socket, response);
        return;
    }

    QString creatorId = checkQuery.value("creator_id").toString();
    if (creatorId != userId) {
        // 생성자가 아닐 경우
        response["result"] = "failure";
        response["message"] = "방 생성자만 방을 삭제할 수 있습니다.";
        ui->logTextEdit->append(QString("방 삭제 실패: %1가 방 생성자가 아님.").arg(userId));
        SendResponseToClient(socket, response);
        return;
    }

    // 2. 방 생성자가 맞다면, 트랜잭션을 시작하여 삭제 작업의 원자성을 보장
    //    (room_members와 rooms 테이블에서 모두 삭제되도록)
    db.transaction();
    QSqlQuery deleteMembersQuery(db);
    deleteMembersQuery.prepare("DELETE FROM room_members WHERE room_id = :room_id");
    deleteMembersQuery.bindValue(":room_id", roomId);
    bool membersDeleted = deleteMembersQuery.exec();

    QSqlQuery deleteRoomQuery(db);
    deleteRoomQuery.prepare("DELETE FROM rooms WHERE room_id = :room_id");
    deleteRoomQuery.bindValue(":room_id", roomId);
    bool roomDeleted = deleteRoomQuery.exec();

    if (membersDeleted && roomDeleted) {
        db.commit();
        response["result"] = "success";
        response["message"] = "방이 성공적으로 삭제되었습니다.";
        ui->logTextEdit->append(QString("방 삭제 성공: 방 ID=%1").arg(roomId));

        // 수정된 부분: 서버 메모리에서 Room 객체 삭제
        if (rooms.contains(roomId)) {
            delete rooms.value(roomId);
            rooms.remove(roomId);
            qDebug() << "방 (ID:" << roomId << ")가 방장 삭제로 인해 서버 메모리에서 삭제되었습니다.";
        }
    } else {
        db.rollback();
        response["result"] = "failure";
        response["message"] = "방 삭제 중 오류가 발생했습니다. 데이터베이스 롤백.";
        ui->logTextEdit->append(QString("방 삭제 실패 (DB 오류): %1").arg(deleteRoomQuery.lastError().text()));
    }
    // 모든 클라이언트에게 방 목록 업데이트 전송
    sendRoomListToAllClients();
    SendResponseToClient(socket, response);
}
// 방 입장 로직
void MainWindow::handleJoinRoom(QTcpSocket* socket, const QJsonObject& request)
{
    qDebug() << "수신된 요청:" << request;
    int roomId = request["room_id"].toInt();
    QString userId = request["user_id"].toString();

    QJsonObject response;
    response["type"] = "join_room_response";
    response["room_id"] = roomId;

    QSqlQuery query(db);

    // 1. 이미 방에 입장해 있는지 확인
    query.prepare("SELECT COUNT(*) FROM room_members WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);
    if (query.exec() && query.next() && query.value(0).toInt() > 0) {
        response["result"] = "failure";
        response["message"] = "이미 다른 방에 입장해 있습니다.";
        ui->logTextEdit->append(QString("방 입장 실패: 사용자 '%1'은 이미 다른 방에 있습니다.").arg(userId));
        SendResponseToClient(socket, response);
        return;
    }

    // 2. 방이 가득 찼는지 확인 (정원 10명)
    query.prepare("SELECT COUNT(*) FROM room_members WHERE room_id = :room_id");
    query.bindValue(":room_id", roomId);
    if (query.exec() && query.next() && query.value(0).toInt() >= 10) {
        response["result"] = "failure";
        response["message"] = "방이 가득 찼습니다.";
        ui->logTextEdit->append(QString("방 입장 실패: 방 %1은 가득 찼습니다.").arg(QString::number(roomId)));
        SendResponseToClient(socket, response);
        return;
    }

    // 3. room_members 테이블에 기록 삽입
    query.prepare("INSERT INTO room_members (room_id, user_id, join_time) VALUES (:room_id, :user_id, NOW())");
    query.bindValue(":room_id", roomId);
    query.bindValue(":user_id", userId);

    if (query.exec()) {
        qDebug() << "DB에 사용자 추가 성공. 방 ID:" << roomId << ", 사용자 ID:" << userId;
        response["result"] = "success";

        //  서버 메모리 상태(in-memory) 업데이트
        if (rooms.contains(roomId)) {
            Room* room = rooms.value(roomId);
            room->participants.append(socket); // 참가자 목록에 소켓 추가
            socketToRoomIdMap[socket] = roomId; // 소켓-방 ID 맵에 추가
        } else {
            // 이 경우는 거의 없지만, 혹시 DB에는 방이 있는데 메모리에 없을 경우를 대비합니다.
            qDebug() << "오류: DB에는 방이 있지만 메모리에 없습니다. 방 입장을 취소합니다.";
            response["result"] = "failure";
            response["message"] = "서버 오류가 발생했습니다.";
            SendResponseToClient(socket, response);
            return;
        }

        // 방 이름 조회
        QSqlQuery roomNameQuery(db);
        roomNameQuery.prepare("SELECT room_name FROM rooms WHERE room_id = :room_id");
        roomNameQuery.bindValue(":room_id", roomId);
        if (roomNameQuery.exec() && roomNameQuery.next()) {
            response["room_name"] = roomNameQuery.value("room_name").toString();
        }
        SendResponseToClient(socket, response);
        ui->logTextEdit->append(QString("방 입장 성공: 방 ID=%1, 사용자 ID='%2'").arg(QString::number(roomId)).arg(userId));

        sendUserListToRoom(roomId);
        sendRoomListToAllClients();
    } else {
        qDebug() << "DB에 사용자 추가 실패:" << query.lastError().text();
        response["result"] = "failure";
        response["message"] = "방 입장에 실패했습니다.";
        ui->logTextEdit->append(QString("방 입장 실패: 방 ID=%1, 이유: %2").arg(QString::number(roomId)).arg(query.lastError().text()));
    }

} // handleJoinRoom()
// 방 나가기 로직
void MainWindow::handleExitRoom(QTcpSocket* socket, const QJsonObject& request)
{
    int roomId = request["room_id"].toInt();
    QString userId = request["user_id"].toString();

    QJsonObject response;
    response["type"] = "leave_room_response";

    //  트랜잭션 시작
    if (!db.transaction()) {
        response["result"] = "failure";
        response["message"] = "서버 오류: 트랜잭션을 시작할 수 없습니다.";
        SendResponseToClient(socket, response);
        return;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM room_members WHERE room_id = :room_id AND user_id = :user_id");
    query.bindValue(":room_id", roomId);
    query.bindValue(":user_id", userId);

    if (query.exec() && query.numRowsAffected() > 0) {
        // 방을 나간 사용자가 마지막 멤버인지 확인
        QSqlQuery countQuery(db);
        countQuery.prepare("SELECT COUNT(*) FROM room_members WHERE room_id = :roomId");
        countQuery.bindValue(":roomId", roomId);

        int remainingMembers = 0;
        if (countQuery.exec() && countQuery.next()) {
            remainingMembers = countQuery.value(0).toInt();
        }

        // 만약 방에 아무도 없다면 방 자체를 삭제
        if (remainingMembers == 0) {
            if (rooms.contains(roomId)) {
                delete rooms.value(roomId); // 동적 할당된 객체 해제
                rooms.remove(roomId);       // QMap에서 포인터 제거
                qDebug() << "방 (ID:" << roomId << ")가 마지막 멤버 퇴장으로 인해 서버 메모리에서 삭제되었습니다.";
            }
            // game_states 테이블 먼저 삭제
            QSqlQuery deleteGameStateQuery(db);
            deleteGameStateQuery.prepare("DELETE FROM game_states WHERE room_id = :roomId");
            deleteGameStateQuery.bindValue(":roomId", roomId);
            if (!deleteGameStateQuery.exec()) {
                db.rollback();
                response["result"] = "failure";
                response["message"] = "서버 오류: 게임 상태 삭제에 실패했습니다.";
                SendResponseToClient(socket, response);
                return;
            }
            // 이후 rooms 테이블 삭제
            QSqlQuery deleteRoomQuery(db);
            deleteRoomQuery.prepare("DELETE FROM rooms WHERE room_id = :roomId");
            deleteRoomQuery.bindValue(":roomId", roomId);
            if (!deleteRoomQuery.exec()) {
                db.rollback();
                response["result"] = "failure";
                response["message"] = "서버 오류로 방 삭제에 실패했습니다.";
                SendResponseToClient(socket, response);
                return;
            }
        }

        //  모든 데이터베이스 작업이 성공했을 때만 커밋
        if (db.commit()) {
            response["result"] = "success";
            response["message"] = "방에서 성공적으로 나갔습니다.";
            sendUserListToRoom(roomId);
            // 커밋 후 모든 클라이언트에게 방 목록 전송
            sendRoomListToAllClients();
            SendResponseToClient(socket, response);

            return;
        }
    }

    // 실패하거나 커밋에 실패한 경우
    db.rollback();
    response["result"] = "failure";
    response["message"] = "방에서 나가는 데 실패했습니다.";
    SendResponseToClient(socket, response);
}

// 일반채팅
void MainWindow::handleChat(QTcpSocket* socket, const QJsonObject& request)
{
    // 1. 발신자의 user_id와 닉네임을 찾습니다.
    QString senderUserId = clientToIdMap.value(socket);
    if (senderUserId.isEmpty()) {
        qDebug() << "ERROR: handleChat - 채팅 발신자의 사용자 ID를 찾을 수 없습니다.";
        return;
    }

    // 이 시점에서 senderUserId는 "marin0806"과 같이 올바른 값이어야 합니다.
    qDebug() << "DEBUG: handleChat - 채팅 발신자 ID: " << senderUserId;

    QString senderNickname = getNicknameBySocket(socket);
    if (senderNickname.isEmpty()) {
        qDebug() << "ERROR: handleChat - 채팅 발신자의 닉네임을 찾을 수 없습니다.";
        return;
    }

    QString message = request["message"].toString();
    int roomId = request["room_id"].toInt();

    // 2. 응답 메시지를 생성합니다.
    QJsonObject response;
    response["type"] = "chat_response";
    response["room_id"] = roomId;
    response["sender"] = senderNickname;
    response["message"] = message;

    // 3. 해당 방에 속한 모든 클라이언트 소켓에 메시지를 브로드캐스트합니다.
    // 이 쿼리는 DB에 쓰기 작업이 아닌 읽기 작업만 수행합니다.
    QSqlQuery membersQuery(db);
    membersQuery.prepare("SELECT user_id FROM room_members WHERE room_id = :room_id");
    membersQuery.bindValue(":room_id", roomId);

    if (membersQuery.exec()) {
        qDebug() << "DEBUG: handleChat - 방" << roomId << "의 멤버 목록을 가져옴.";
        while (membersQuery.next()) {
            QString memberUserId = membersQuery.value("user_id").toString();

            // user_id를 기반으로 소켓을 찾아 메시지를 보냅니다.
            for (QTcpSocket* clientSocket : clientSockets) {
                if (clientToIdMap.value(clientSocket) == memberUserId) {
                    SendResponseToClient(clientSocket, response);
                    qDebug() << "DEBUG: " << memberUserId << "에게 채팅 메시지 브로드캐스트 완료.";
                    break;
                }
            }
        }
    } else {
        qDebug() << "ERROR: handleChat - 멤버 목록 조회 쿼리 실패:" << membersQuery.lastError().text();
    }
    ui->logTextEdit->append(QString("방 %1에 채팅 메시지 전송: <%2> %3").arg(roomId).arg(senderNickname).arg(message));
}

// 게임시작
void MainWindow::handleStartGame(QTcpSocket* socket, const QJsonObject& request)
{
    // 1. 요청에서 방 ID 추출
    int roomId = request["room_id"].toInt();
    QString category = request["category"].toString();

    QSqlQuery query(db);

    // 2. 데이터베이스 쿼리: rooms 테이블의 상태를 'playing'으로 변경
    query.prepare("UPDATE rooms SET status = :status WHERE room_id = :roomId AND creator_id = :userId");
    query.bindValue(":status", "playing");
    query.bindValue(":roomId", roomId);
    // 방 생성자만 게임을 시작할 수 있도록 creator_id를 조건에 추가
    // 로그인 정보를 가져오기 위해 clientToIdMap을 사용합니다.
    query.bindValue(":userId", clientToIdMap.value(socket));

    QJsonObject response;
    response["type"] = "start_game_response";

    if (query.exec()) {
        if (query.numRowsAffected() > 0) {
            Room* room = rooms.value(roomId); //  Room 객체 가져오기

            qDebug() << "Room status changed to 'playing' for room ID:" << roomId;

            // 1. 방에 있는 모든 멤버를 가져옵니다.
            QSqlQuery membersQuery(db);
            membersQuery.prepare("SELECT user_id FROM room_members WHERE room_id = :roomId");
            membersQuery.bindValue(":roomId", roomId);

            QStringList userIds;
            if (membersQuery.exec()) {
                while (membersQuery.next()) {
                    userIds << membersQuery.value(0).toString();
                }
            }
            //  디버깅 코드 추가
            qDebug() << "Fetched user IDs from room_members:" << userIds;
            qDebug() << "Number of users fetched:" << userIds.size();

            // 2. 라이어 선정
            QString liarId = userIds.at(QRandomGenerator::global()->bounded(userIds.size()));
            std::shuffle(userIds.begin(), userIds.end(), *QRandomGenerator::global());

            // 3. GameData 클래스에서 무작위 단어 2개를 가져옵니다.
            QStringList words = GameData::getRandomWordsForCategory(category);
            if (words.size() < 2) {
                // 단어가 충분하지 않을 경우 처리
                response["result"] = "failure";
                response["message"] = "선택한 카테고리에 단어가 부족합니다.";
                SendResponseToClient(socket, response);
                return;
            }

            QString generalWord = words.at(0); // 시민 단어
            QString liarWord = words.at(1);     // 라이어 단어

            // 게임 상태 데이터(JSON) 생성 (예시)
            QJsonObject stateData;
            stateData["category"] = category;
            stateData["liar_id"] = liarId;
            stateData["general_word"] = generalWord;
            stateData["liar_word"] = liarWord;
            stateData["game_phase"] = "discussion";
            stateData["turn_order"] = QJsonArray::fromStringList(userIds);
            stateData["current_round"] = 1;

            //  2. Room 객체의 GameState 멤버 업데이트
            if (room) {
                room->gameState.liarId = liarId;
                room->gameState.currentWord = generalWord;
                room->gameState.liarWord = liarWord;
                room->gameState.playersInRoom = userIds;
                room->gameState.turnIndex = 0; // turnIndex 초기화
            }

            // 4. game_states 테이블에 INSERT
            QSqlQuery gameStateQuery(db);
            gameStateQuery.prepare("INSERT INTO game_states (room_id, current_turn_user_id, state_data) VALUES (:roomId, :currentTurnUserId, :stateData)");
            gameStateQuery.bindValue(":roomId", roomId);
            gameStateQuery.bindValue(":currentTurnUserId", userIds.first());
            gameStateQuery.bindValue(":stateData", QString(QJsonDocument(stateData).toJson(QJsonDocument::Compact)));

            if (gameStateQuery.exec()) {
                qDebug() << "게임 상태 초기화 및 DB 저장 완료.";
                response["result"] = "success";
                response["message"] = "Game started successfully.";

                for (const QString& memberUserId : userIds) {
                    QTcpSocket* memberSocket = idToClientMap.value(memberUserId);
                    if (memberSocket) {
                        // 사용자별로 다른 데이터(ex. 라이어 여부)를 담아 전송할 수도 있습니다.
                        // 현재는 동일한 성공 메시지를 보냅니다.
                        SendResponseToClient(memberSocket, response);
                    }
                }

                // 현재 턴인 플레이어의 닉네임 가져오기
                QSqlQuery userQuery(db);
                userQuery.prepare("SELECT nickname FROM users WHERE user_id = :userId");
                userQuery.bindValue(":userId", userIds.first());
                QString currentTurnNickname;
                if (userQuery.exec() && userQuery.next()) {
                    currentTurnNickname = userQuery.value("nickname").toString();
                }

                int delay = 1500;
                QTimer::singleShot(delay, [=]() {
                    QJsonObject msg;
                    msg["type"] = "system_message";
                    msg["message"] = "<b>게임을 시작합니다!</b>\n";
                    sendToAllRoomMembers(roomId, msg);
                });
                QTimer::singleShot(delay*2, [=]() {
                    QJsonObject msg;
                    msg["type"] = "system_message";
                    msg["message"] = "<b>총 3라운드로 진행되며 마지막에 라이어 투표를 진행합니다.</b>\n";
                    sendToAllRoomMembers(roomId, msg);
                });
                QTimer::singleShot(delay*3, [=]() {
                    QJsonObject msg;
                    msg["type"] = "system_message";
                    msg["message"] = QString("<b>카테고리는 <font color='blue'>'%1'</font>입니다.</b>\n").arg(category);
                    sendToAllRoomMembers(roomId, msg);
                });
                QTimer::singleShot(delay*4, [=]() {
                    sendRoleBasedMessagesToClients(roomId, stateData);
                });
                QTimer::singleShot(delay*5, [=]() {
                    QJsonObject msg;
                    msg["type"] = "system_message";
                    msg["message"] = "<br><b>&lt;1 라운드&gt; 게임채팅으로 힌트를 말해주세요.</b>";
                    sendToAllRoomMembers(roomId, msg);
                });
                QTimer::singleShot(delay*6, [=]() {
                    QJsonObject msg;
                    msg["type"] = "system_message";
                    msg["message"] = QString("<b>이번 입력자는 <font color='blue'>\"%1\"</font>님입니다.</b>\n").arg(currentTurnNickname);
                    sendToAllRoomMembers(roomId, msg);
                });

                // 4. UI 갱신을 위해 방 목록 업데이트 전송
                sendRoomListToAllClients();
            } else {
            // 해당 방이 존재하지 않거나, 현재 사용자가 생성자가 아닐 경우
            qDebug() << "게임 상태 삽입 실패:" << gameStateQuery.lastError().text();
            qDebug() << "Failed to start game: Room not found or user is not the creator.";
            response["result"] = "failure";
            response["message"] = "방이 존재하지 않거나 방장이 아닙니다.";
            SendResponseToClient(socket, response);
            }
    }   else {
        // 쿼리 실행 실패
        qDebug() << "Query failed to start game:" << query.lastError().text();
        response["result"] = "failure";
        response["message"] = "방장만 게임을 시작할 수 있습니다.";
        SendResponseToClient(socket, response);
        }
    }
}
// 힌트
void MainWindow::handleHintMessage(QTcpSocket* socket, const QJsonObject& request)
{
    qDebug() << "handleHintMessage called.";
    int roomId = request["room_id"].toInt();
    QString hintText = request["hint_text"].toString();
    QString userId = clientToIdMap.value(socket);

    if (userId.isEmpty()) {
        qDebug() << "Error: User ID not found for socket.";
        return;
    }

    // 1. DB에서 게임 상태 데이터 가져오기 (단 한 번만)
    QSqlQuery stateQuery(db);
    stateQuery.prepare("SELECT current_turn_user_id, state_data FROM game_states WHERE room_id = :roomId");
    stateQuery.bindValue(":roomId", roomId);

    if (!stateQuery.exec() || !stateQuery.next()) {
        qDebug() << "Error: Game state not found or query failed for room ID:" << roomId;
        return;
    }

    // 2. 현재 턴 확인
    QString currentTurnUserId = stateQuery.value("current_turn_user_id").toString();
    if (currentTurnUserId != userId) {
        sendSystemMessageToClient(socket, "<b>지금은 당신의 차례가 아닙니다.</b>");
        return;
    }

    // 3. 힌트 메시지 전송
    QByteArray jsonData = stateQuery.value("state_data").toByteArray();
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    QJsonObject stateData = doc.object();
    QJsonArray turnOrderArray = stateData["turn_order"].toArray();

    QSqlQuery nicknameQuery(db);
    nicknameQuery.prepare("SELECT nickname FROM users WHERE user_id = :userId");
    nicknameQuery.bindValue(":userId", userId);
    QString senderNickname;
    if (nicknameQuery.exec() && nicknameQuery.next()) {
        senderNickname = nicknameQuery.value(0).toString();
    }
    QJsonObject hintMessage;
    hintMessage["type"] = "game_chat";
    hintMessage["sender_nickname"] = senderNickname;
    hintMessage["message"] = hintText;
    sendToAllRoomMembers(roomId, hintMessage);

    // 4. 턴 및 라운드 업데이트
    //  turnOrderArray.indexOf(QJsonValue(userId)) 대신 이 코드를 사용하세요.
    int currentIndex = -1;
    for (int i = 0; i < turnOrderArray.size(); ++i) {
        if (turnOrderArray.at(i).toString() == userId) {
            currentIndex = i;
            break;
        }
    }
    if (currentIndex == -1) {
        qDebug() << "Error: User ID not found in turn order array.";
        return;
    }

    int nextIndex = (currentIndex + 1) % turnOrderArray.size();
    QString nextTurnUserId = turnOrderArray.at(nextIndex).toString();

    int currentRound = stateData["current_round"].toInt();

    if (nextIndex == 0) { // 라운드가 완료되면
        currentRound++;
        stateData["current_round"] = currentRound;
        qDebug() << "라운드" << currentRound << "완료.";
    }

    stateData["turn_index"] = nextIndex;

    // 5. DB에 업데이트
    QJsonDocument updatedDoc(stateData);
    QByteArray updatedJsonData = updatedDoc.toJson(QJsonDocument::Compact);
    QSqlQuery updateStateQuery(db);
    updateStateQuery.prepare("UPDATE game_states SET current_turn_user_id = :nextUserId, state_data = :stateData WHERE room_id = :roomId");
    updateStateQuery.bindValue(":nextUserId", nextTurnUserId);
    updateStateQuery.bindValue(":stateData", QString(updatedJsonData));
    updateStateQuery.bindValue(":roomId", roomId);

    if (!updateStateQuery.exec()) {
        qDebug() << "Query failed to update game state:" << updateStateQuery.lastError().text();
        return;
    }

    // 6. 게임 종료 또는 다음 턴 메시지 전송
    const int MAX_ROUNDS = 3;
    if (currentRound > MAX_ROUNDS) { // 3라운드까지 모두 완료되었을 때
        qDebug() << "모든 라운드가 완료되었습니다. 라이어의 정답 입력 단계로 넘어갑니다.";
        QString liarId = stateData["liar_id"].toString();

        QJsonArray membersArray = stateData["turn_order"].toArray();
        for (const QJsonValue& memberValue : membersArray) {
            QString memberUserId = memberValue.toString();
            QTcpSocket* memberSocket = idToClientMap.value(memberUserId);

            if (memberSocket) {
                QJsonObject response;
                response["type"] = "liar_answer_phase";
                response["liar_id"] = liarId;
                if (memberUserId == liarId) {
                    response["message"] = "<br><b>당신은 라이어입니다. 정답을 입력하세요.</b>";
                } else {
                    response["message"] = "<br><b>라이어가 정답을 입력하고 있습니다.</b>";
                }
                SendResponseToClient(memberSocket, response);
            }
        }
    } else {
        QString nextTurnNickname = getNicknameByUserId(nextTurnUserId);
        qDebug() << "다음 턴 플레이어 닉네임: " << nextTurnNickname;

        int delay = 1500;
        if (nextIndex == 0) { // 새 라운드 시작
            QTimer::singleShot(delay, [=]() {
                QJsonObject msg;
                msg["type"] = "system_message";
                msg["message"] = QString("<br><b>&lt;%1 라운드&gt; 게임채팅으로 힌트를 말해주세요.</b>").arg(currentRound);
                sendToAllRoomMembers(roomId, msg);
            });
            QTimer::singleShot(delay * 2, [=]() {
                QJsonObject msg;
                msg["type"] = "system_message";
                msg["message"] = QString("<b>이번 입력자는 <font color='blue'>\"%1\"</font>님입니다.</b>\n").arg(nextTurnNickname);
                sendToAllRoomMembers(roomId, msg);
            });
        } else { // 턴만 넘어감
            QTimer::singleShot(delay, [=]() {
                QJsonObject msg;
                msg["type"] = "system_message";
                msg["message"] = QString("<b>이번 입력자는 <font color='blue'>\"%1\"</font>님입니다.</b>\n").arg(nextTurnNickname);
                sendToAllRoomMembers(roomId, msg);
            });
        }
    }
}
//  새로 추가: 라이어의 정답을 처리하는 함수
void MainWindow::handleLiarAnswer(QTcpSocket* socket, const QJsonObject& request)
{
    qDebug() << "handleLiarAnswer called.";
    int roomId = request["room_id"].toInt();
    QString answer = request["answer_text"].toString();
    QString userId = clientToIdMap.value(socket);

    qDebug() << "Received liar answer. Room ID:" << roomId << "User ID:" << userId << "Answer:" << answer;

    // 1. 게임 상태 데이터 가져오기 (정답, 라이어 ID 확인)
    QSqlQuery stateQuery(db);
    stateQuery.prepare("SELECT state_data FROM game_states WHERE room_id = :roomId");
    stateQuery.bindValue(":roomId", roomId);
    if (!stateQuery.exec() || !stateQuery.next()) {
        qDebug() << "Error: Game state not found for room ID:" << roomId << "Query error:" << stateQuery.lastError().text();
        sendSystemMessageToClient(socket, "<b>게임 상태를 찾을 수 없습니다. 다시 시도해 주세요.</b>");
        return;
    }
    qDebug() << "Successfully retrieved game state from DB.";

    QJsonDocument doc = QJsonDocument::fromJson(stateQuery.value("state_data").toByteArray());
    QJsonObject stateData = doc.object();
    QString liarId = stateData["liar_id"].toString();
    QString citizensWord = stateData["general_word"].toString();

    qDebug() << "Game state data loaded. Liar ID:" << liarId << "Citizen's Word:" << citizensWord;



    // 2. 현재 요청한 유저가 라이어인지 확인
    if (userId != liarId) {
        qDebug() << "User" << userId << "is not the liar. Denying answer.";
        sendSystemMessageToClient(socket, "\n<b>당신은 라이어가 아닙니다. 정답을 입력할 수 없습니다.</b>");
        return;
    }
    qDebug() << "User is the liar. Checking answer.";

    // 3. 정답을 확인하고 게임 종료 또는 투표 단계로 전환
    QJsonObject response;

    //  정답이 일치하는지 확인
    if (answer.toLower().trimmed() == citizensWord.toLower().trimmed()) {
        qDebug() << "Liar's answer is CORRECT. Liar wins.";
        // 라이어 승리
        QString userNick = getNicknameByUserId(userId);
        endGame(roomId, "라이어", QString("라이어(%1)가 승리했습니다!\n정답 단어는 '%2'였습니다.").arg(userNick).arg(citizensWord));
    } else {
        qDebug() << "Liar's answer is INCORRECT. Proceeding to vote phase.";

        Room& currentRoom = *rooms[roomId];
        currentRoom.voters.clear();

        //  투표 대상자 목록을 닉네임으로 변환하여 전송
        QJsonArray userIdsArray = stateData["turn_order"].toArray();
        QJsonArray nicknamesArray;

        // user_id를 nickname으로 변환하여 새로운 배열에 추가
        for (const QJsonValue& value : userIdsArray) {
            QString userId = value.toString();
            QString nickname = getNicknameByUserId(userId);
            if (!nickname.isEmpty()) {
                nicknamesArray.append(nickname);
            }
        }

        // 라이어 패배, 투표 단계로 전환
        response["type"] = "vote_phase";
        response["message"] = "라이어가 정답을 맞히지 못했습니다. \n투표를 진행합니다.";
        // 투표 대상자 목록 추가 (턴 순서 배열 그대로 사용)
        response["voters"] = nicknamesArray;

        sendToAllRoomMembers(roomId, response);
    }
    qDebug() << "handleLiarAnswer finished.";
}
// 투표 요청 처리 함수
void MainWindow::handleVote(QTcpSocket* socket, const QJsonObject& request)
{
    int roomId = request["room_id"].toInt();
    QString votedUserNickname = request["voted_user_nickname"].toString();
    QString voterNickname = getNicknameBySocket(socket);

    if (rooms.contains(roomId)) {
        Room* room = rooms.value(roomId);

        //  이미 투표했는지 확인
        if (room->voters.contains(socket)) {
            qDebug() << "ERROR: " << voterNickname << "님은 이미 투표했습니다.";
            // 클라이언트에게 알림 메시지 전송
            sendSystemMessageToClient(socket, "<b>이미 투표에 참여했습니다.</b>");
            return;
        }

        // 투표 수 카운트
        voteCounts[votedUserNickname]++;
        qDebug() << voterNickname << "님이" << votedUserNickname << "님에게 투표했습니다. 현재 득표수: " << voteCounts[votedUserNickname];

        //  투표한 사람을 기록
        room->voters.insert(socket);

        // 투표가 모두 완료되었는지 확인
        int totalVotes = room->participants.size();

        // 투표를 마친 사람의 수가 전체 인원과 같은지 확인
        int totalParticipants = room->participants.size();
        if (room->voters.size() == totalParticipants) {
            qDebug() << "모든 투표가 완료되었습니다. /n투표 결과 확인.";
            handleVoteResult(roomId);
            voteCounts.clear();
        }
    }
}

// 투표 결과 확인 및 처리 함수
void MainWindow::handleVoteResult(int roomId)
{
    if (!rooms.contains(roomId)) {
        qDebug() << "방을 찾을 수 없습니다.";
        return;
    }

    Room* room = rooms.value(roomId);
    QString liarNickname = getNicknameByUserId(room->gameState.liarId);
    QString mostVotedNickname;
    int maxVotes = -1;
    QList<QString> tiedUsers; // 동률인 유저들을 저장

    // 최다 득표자 찾기 및 동률 확인
    QMapIterator<QString, int> i(voteCounts);
    while (i.hasNext()) {
        i.next();
        if (i.value() > maxVotes) {
            maxVotes = i.value();
            mostVotedNickname = i.key();
            tiedUsers.clear(); // 새로운 최다 득표자이므로 동률 리스트 초기화
            tiedUsers.append(i.key());
        } else if (i.value() == maxVotes) {
            tiedUsers.append(i.key()); // 동률인 경우 추가
        }
    }

    // 디버그용 로그 추가
    qDebug() << "--- 투표 결과 디버그 시작 ---";
    qDebug() << "라이어 닉네임:" << liarNickname;
    qDebug() << "최다 득표 닉네임:" << mostVotedNickname;
    qDebug() << "두 닉네임이 동일한가?" << (mostVotedNickname == liarNickname);
    qDebug() << "--- 투표 결과 디버그 종료 ---";

    // 투표 결과 메시지
    QString voteResultMsg = "투표 결과: ";
    QMapIterator<QString, int> j(voteCounts);
    while (j.hasNext()) {
        j.next();
        voteResultMsg += QString("%1 (%2표), ").arg(j.key()).arg(j.value());
    }
    voteResultMsg.chop(2); // 마지막 ", " 제거
    QJsonObject voteMsg;
    voteMsg["type"] = "system_message";
    voteMsg["message"] = voteResultMsg;
    sendToAllRoomMembers(roomId, voteMsg);

    // 무승부 처리
    if (tiedUsers.size() > 1) {
        QJsonObject tiedMsg;
        tiedMsg["type"] = "system_message";
        tiedMsg["message"] = "<b>투표수가 같으므로, 게임을 다시 진행합니다.</b>";
        sendToAllRoomMembers(roomId, tiedMsg);

        // 딜레이를 두고 다음 라운드를 시작
        QTimer::singleShot(2000, [this, roomId]() {
            this->startNextRound(roomId);
        });
        return;
    }

    // 게임 결과 판정
    if (mostVotedNickname == liarNickname) {
        // 라이어에게 가장 많은 표를 던졌을 경우 시민 승리
        endGame(roomId, "시민", QString("시민이 승리했습니다!\n정답 단어는 '%1'였습니다.\n라이어의 정체는 %2 님입니다!").arg(room->gameState.currentWord).arg(liarNickname));
    } else {
        // 라이어가 아닌 다른 사람에게 가장 많은 표를 던졌을 경우 라이어 승리
        endGame(roomId, "라이어", QString("라이어(%1)가 승리했습니다!\n정답 단어는 '%2'였습니다.").arg(liarNickname).arg(room->gameState.currentWord));
    }
}

// 게임 종료 함수
// 함수가 이제 최종 메시지를 직접 인자로 받습니다.
void MainWindow::endGame(int roomId, const QString& winner, const QString& message)
{
    qDebug() << "endGame 함수 호출. roomId: " << roomId;
    qDebug() << "DB 연결 상태: " << db.isOpen();

    QJsonObject response;
    response["type"] = "game_end";
    response["winner"] = winner;
    response["message"] = message;
    sendToAllRoomMembers(roomId, response);

    // 방 상태 초기화
    if (rooms.contains(roomId)) {
        Room* room = rooms.value(roomId);
        room->isGameInProgress = false;
        room->gameState = GameState();

        // 투표 기록 초기화
        voteCounts.clear();
        room->voters.clear();

        // DB의 game_states 테이블에서 해당 방의 게임 상태 삭제
        // QSqlQuery 객체 생성 시 db 객체를 명시적으로 전달
        QSqlQuery query(db);
        query.prepare("DELETE FROM game_states WHERE room_id = :room_id");
        query.bindValue(":room_id", roomId);

        if (!query.exec()) {
            qDebug() << "게임 상태 삭제 실패:" << query.lastError().text();
        } else {
            qDebug() << "게임 상태(room_id:" << roomId << ")가 DB에서 삭제되었습니다. 삭제된 행 수: " << query.numRowsAffected();
        }

        // rooms 테이블의 status를 'waiting'으로 변경
        // QSqlQuery 객체 생성 시 db 객체를 명시적으로 전달
        QSqlQuery updateQuery(db);
        updateQuery.prepare("UPDATE rooms SET status = 'waiting' WHERE room_id = :room_id");
        updateQuery.bindValue(":room_id", roomId);

        if (!updateQuery.exec()) {
            qDebug() << "방 상태 변경 실패:" << updateQuery.lastError().text();
        } else {
            qDebug() << "방 상태(room_id:" << roomId << ")가 'waiting'으로 변경되었습니다.";
        }
    }
    sendRoomListToAllClients();
}
// 다음 라운드 시작 함수
void MainWindow::startNextRound(int roomId)
{
    qDebug() << "startNextRound called for room:" << roomId;

    QSqlQuery stateQuery(db);
    stateQuery.prepare("SELECT state_data FROM game_states WHERE room_id = :roomId");
    stateQuery.bindValue(":roomId", roomId);

    if (!stateQuery.exec() || !stateQuery.next()) {
        qDebug() << "Error: Game state not found for room ID:" << roomId;
        return;
    }

    QByteArray jsonData = stateQuery.value("state_data").toByteArray();
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    QJsonObject stateData = doc.object();

    // 1. 라운드 및 투표 결과 초기화
    stateData["current_round"] = 1;
    voteCounts.clear(); // 투표 결과 초기화
    Room& currentRoom = *rooms[roomId];
    currentRoom.voters.clear();

    // 기존 단어 및 라이어 정보 불러오기
    QString existingCategory = stateData["category"].toString();
    QString existingGeneralWord = stateData["general_word"].toString();
    QString existingLiarWord = stateData["liar_word"].toString();
    QString existingLiarId = stateData["liar_id"].toString();

    // 2. 턴 순서를 섞되, 라이어는 유지
    QJsonArray membersArray = stateData["turn_order"].toArray();
    QStringList nonLiarMembers;
    for (const QJsonValue& memberValue : membersArray) {
        if (memberValue.toString() != existingLiarId) {
            nonLiarMembers << memberValue.toString();
        }
    }
    unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
    std::default_random_engine rng(seed);

    // QList를 std::vector로 변환하거나, QList의 iterator를 직접 사용
    std::shuffle(nonLiarMembers.begin(), nonLiarMembers.end(), rng);

    nonLiarMembers.append(existingLiarId);
    QJsonArray turnOrderArray = QJsonArray::fromStringList(nonLiarMembers);

    stateData["turn_order"] = turnOrderArray;
    stateData["liar_id"] = existingLiarId;
    stateData["current_turn_user_id"] = turnOrderArray.at(0).toString();

    // 게임 단계와 턴 인덱스 초기화
    stateData["game_phase"] = "discussion";
    stateData["turn_index"] = 0;

    // 3. 업데이트된 게임 상태를 DB에 저장
    QJsonDocument updatedDoc(stateData);
    QByteArray updatedJsonData = updatedDoc.toJson(QJsonDocument::Compact);

    QSqlQuery updateStateQuery(db);
    updateStateQuery.prepare("UPDATE game_states SET current_turn_user_id = :currentTurnId, state_data = :stateData WHERE room_id = :roomId");
    updateStateQuery.bindValue(":currentTurnId", stateData["current_turn_user_id"].toString());
    updateStateQuery.bindValue(":stateData", QString(updatedJsonData));
    updateStateQuery.bindValue(":roomId", roomId);

    if (!updateStateQuery.exec()) {
        qDebug() << "Failed to update game state for new round:" << updateStateQuery.lastError().text();
        return;
    }

    // 4. 모든 플레이어에게 게임 시작 및 역할 메시지 전송
    QSqlQuery membersQuery(db);
    membersQuery.prepare("SELECT user_id FROM room_members WHERE room_id = :roomId");
    membersQuery.bindValue(":roomId", roomId);

    if (membersQuery.exec()) {
        while (membersQuery.next()) {
            QString memberUserId = membersQuery.value(0).toString();
            QTcpSocket* memberSocket = idToClientMap.value(memberUserId);

            if (memberSocket) {
                QJsonObject response;
                response["type"] = "start_game_response";
                response["result"] = "success";
                response["current_turn_user_id"] = turnOrderArray.at(0).toString();
                if (memberUserId == existingLiarId) {
                    response["message"] = QString("<b>당신은 <font color='red'라이어</font>입니다.\n 제시된 단어는 <font color='blue'>'%1'</font>입니다.</b>\n").arg(existingLiarWord); // 원래 라이어임
                } else {
                    response["message"] = QString("<b>당신은 <font color='blue'>시민</font>입니다.\n 제시된 단어는 <font color='blue'>'%1'</font>입니다.</b>\n").arg(existingGeneralWord);
                }
                SendResponseToClient(memberSocket, response);
            }
        }
    }

    // 5. QTimer를 이용해 시스템 메시지 순차 전송 (handleStartGame과 동일한 순서)
    QString currentTurnNickname = getNicknameByUserId(turnOrderArray.at(0).toString());
    int currentRound = stateData["current_round"].toInt();
    int delay = 1500;
    QTimer::singleShot(delay, [=]() {
        QJsonObject msg;
        msg["type"] = "system_message";
        msg["message"] = "<b>무승부로 인해 게임을 다시 시작합니다.</b>\n";
        sendToAllRoomMembers(roomId, msg);
    });
    QTimer::singleShot(delay * 2, [=]() {
        QJsonObject msg;
        msg["type"] = "system_message";
        msg["message"] = QString("<br><b>&lt;%1 라운드&gt; 게임채팅으로 힌트를 말해주세요.</b>").arg(currentRound);
        sendToAllRoomMembers(roomId, msg);
    });
    QTimer::singleShot(delay * 3, [=]() {
        QJsonObject msg;
        msg["type"] = "system_message";
        msg["message"] = QString("<b>이번 입력자는 <font color='blue'>\"%1\"</font>님입니다.</b>\n").arg(currentTurnNickname);
        sendToAllRoomMembers(roomId, msg);
    });
}

// 특정 방의 모든 멤버에게 JSON 응답을 보내는 함수
void MainWindow::sendToAllRoomMembers(int roomId, const QJsonObject& response)
{
    QSqlQuery query(db);
    query.prepare("SELECT user_id FROM room_members WHERE room_id = :roomId");
    query.bindValue(":roomId", roomId);

    if (query.exec()) {
        while (query.next()) {
            QString memberUserId = query.value("user_id").toString();
            QTcpSocket* memberSocket = idToClientMap.value(memberUserId);
            if (memberSocket) {
                SendResponseToClient(memberSocket, response);
            }
        }
    } else {
        qDebug() << "sendToAllRoomMembers query failed:" << query.lastError().text();
    }
}
// 해당 클라이언트에게만 보내는 메시지
void MainWindow::sendSystemMessageToClient(QTcpSocket* socket, const QString& message)
{
    QJsonObject response;
    response["type"] = "system_message";
    response["message"] = message;
    SendResponseToClient(socket, response);
}
// 특정 방의 모든 멤버에게 시스템 메시지를 보내는 함수
void MainWindow::sendSystemMessageToRoomMembers(int roomId, const QString& message)
{
    // 1. 해당 방의 모든 멤버 user_id를 DB에서 가져옵니다.
    QSqlQuery membersQuery(db);
    membersQuery.prepare("SELECT user_id FROM room_members WHERE room_id = :roomId");
    membersQuery.bindValue(":roomId", roomId);

    if (membersQuery.exec()) {
        QJsonObject systemMessage;
        systemMessage["type"] = "system_message";
        systemMessage["message"] = message;

        while (membersQuery.next()) {
            QString memberUserId = membersQuery.value(0).toString();
            QTcpSocket* memberSocket = idToClientMap.value(memberUserId);

            // 2. 소켓이 유효하면 메시지를 전송합니다.
            if (memberSocket && memberSocket->isValid()) {
                SendResponseToClient(memberSocket, systemMessage);
            }
        }
    } else {
        qDebug() << "Failed to get room members:" << membersQuery.lastError().text();
    }
}
// 직업에 따라 다르게 보내는 메시지
void MainWindow::sendRoleBasedMessagesToClients(int roomId, const QJsonObject& stateData)
{
    QString liarId = stateData["liar_id"].toString();
    QString liarWord = stateData["liar_word"].toString();
    QString generalword = stateData["general_word"].toString();

    QJsonObject liarMessage;
    liarMessage["type"] = "system_message";
    liarMessage["message"] = QString("<b>당신은 <font color='red'>라이어</font>입니다.</b>\n"); // 원래 red, 라이어임

    QJsonObject nonLiarMessage;
    nonLiarMessage["type"] = "system_message";
    nonLiarMessage["message"] = QString("<b>당신은 <font color='blue'>시민</font>입니다.<br>제시된 단어는 <font color='blue'>'%1'</font>입니다.</b>\n").arg(generalword);

    // 현재 방의 참가자 목록을 가져옵니다.
    if (rooms.contains(roomId)) {
        Room* room = rooms.value(roomId);
        for (QTcpSocket* client : room->participants) {
            if (clientToIdMap.value(client) == liarId) {
                SendResponseToClient(client, liarMessage);
            } else {
                SendResponseToClient(client, nonLiarMessage);
            }
        }
    }
}
// 게임 관리 로직--------------------------------------------------------------------------------------------------
