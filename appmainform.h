#ifndef APPMAINFORM_H
#define APPMAINFORM_H

#include <QMainWindow>
#include <map>
#include <stack>
#include <vector>
#include <list>
#include <QTextEdit>
#include <QFrame>
#include <string>
#include <iostream>
#include <ostream>

QT_BEGIN_NAMESPACE
namespace Ui {
class AppMainForm;
}
QT_END_NAMESPACE

class AppMainForm : public QMainWindow
{
    Q_OBJECT

public:
    AppMainForm(QWidget *parent = nullptr);
    ~AppMainForm();

    void addNote(); // Добавляет виджет заметки в последнюю группу
    void addList(); // Добавляет новую группу

    void saveProject(const std::string& filename);  // Сохранить проект в файл
    void openProject(const std::string& filename);  // Открыть проект из файла

    void saveFrameToFile(  std::ofstream* outFile, QFrame* currentFrame ); // Сохранить фрейм с заметками в файл
    void readFrameFromFile( std::ifstream* inFile,  QFrame* currentFrame ); // Считать фрейм с заметками из файла
    // Добавляет заметку с заданным текстом к фрейму
    void   addNoteToFrameWithText(const std::string& note, QFrame* currentFrame);
    // Добавляет список заметок при считывании
    QFrame*  addListWhileLoading();
    void log_it(const QString message);
    void log_it(const std::string message);

    void initMemState(); // инициализирует память под новый проект
    void clearMemState(); // очищает динамическук память после старого проекта


    std::vector<QTextEdit*> notes; // для простоты список заметок
    std::vector<QFrame*> frames; // для простоты список фреймов
    std::map<QFrame*, std::vector<QTextEdit *>> here; // хранит связь между группой
    // и заметкой
    QFrame* lastFrame;  // последний добавленный фрейм



private slots:
  // Слот по триггеру добавления фрейма
    void on_actAddList_triggered();
// Слот по триггеру добавления заметки
    void on_actAddNote_triggered();

    void on_actOpenFromFile_triggered();

    void on_actSaveToFile_triggered();

    void on_actClearMemory_triggered();

private:

    // Ширина и высота панели по умолчанию
    int SIZE_FRAME_V = 100;
    int SIZE_FRAME_W = 100;
    int SIZE_NOTE_V = 100;
    int SIZE_NOTE_W = 100;
    Ui::AppMainForm *ui;
};
#endif // APPMAINFORM_H
