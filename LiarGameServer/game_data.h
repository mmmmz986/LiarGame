// game_data.h

#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <QMap>
#include <QStringList>
#include <QRandomGenerator>
#include <QDebug>
#include <algorithm>

class GameData {
public:
    static QStringList getRandomWordsForCategory(const QString& category) {
        if (!wordMap.contains(category)) {
            qDebug() << "Error: Category" << category << "not found.";
            return QStringList();
        }

        QStringList categoryWords = wordMap.value(category);

        // 단어 목록을 무작위로 섞음
        std::shuffle(categoryWords.begin(), categoryWords.end(), *QRandomGenerator::global());

        // 두 단어가 있는지 확인하고 반환
        if (categoryWords.size() >= 2) {
            return {categoryWords.at(0), categoryWords.at(1)};
        }

        qDebug() << "Error: Not enough words in category" << category;
        return QStringList();
    }

private:
    // 모든 카테고리와 단어
    static const QMap<QString, QStringList> wordMap;
};

#endif // GAME_DATA_H
