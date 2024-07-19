#include "appmainform.h"
#include "./ui_appmainform.h"
#include <string>
#include <map>
#include <stack>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QString>
#include <vector>
#include <list>

#include <string>
#include <iostream>
#include <fstream>

#include <QString>
#include <QFrame>
#include <QTextEdit>
#include <QPalette>

AppMainForm::AppMainForm(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AppMainForm)
{
    ui->setupUi(this);
    initMemState();  // Настройка состояния памяти перед запуском
}

AppMainForm::~AppMainForm()
{
    clearMemState(); // Очищаем память перед выходом
    delete ui;
}
  // Добавляет новук заметку в последний фрейм
void AppMainForm::addNote(){
    log_it( QString("Добавлена новая заметка"));
    // Не добавляем если не было фреймов
    if (frames.size() == 0) return ;

    // Текст заметки по умолчанию
    QString NEW_NOTE_TEXT("Новая заметка");
  
    // Добавили виджет
    QTextEdit* te = new QTextEdit();
    // Виджет в список
      notes.push_back(te);
    // Связали фрейм и виджет
      here[lastFrame].push_back(te);


    // меняем высоту текущего фрейма
    lastFrame->setFixedHeight((here[lastFrame].size()+1)*SIZE_NOTE_V);
    // если не первая заметка
    te->setGeometry(0, here[lastFrame].size()*SIZE_NOTE_V, SIZE_NOTE_W, SIZE_NOTE_V);


    te->setText(NEW_NOTE_TEXT);
    te->setParent(lastFrame);
    te->show();

}

// Добавляет виджет фрейма на форму
void AppMainForm::addList(){
    log_it( QString("Добавлен новый список заметок"));
    // Помещаем в список фреймов новый фрейм
    frames.push_back(new QFrame());
    // Берем ссылку на этот фрейм
    lastFrame = frames.at(frames.size()-1);

    lastFrame->setGeometry(SIZE_FRAME_W * frames.size(), 40, SIZE_FRAME_W, SIZE_FRAME_V);
    // Меняем цвет фрейма
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::blue);
    lastFrame->setPalette(palette);
    lastFrame->setAutoFillBackground(true);

    // Добавили к интерфейсу
    lastFrame->setParent(this);
    // Показали фрейм
    lastFrame->show();

}
   // Открывает проект из файла
void AppMainForm::openProject(const std::string &filename){
    initMemState();     // Подготавливаем память
    log_it( QString("Открываем проект с именем ") );
    log_it(  QString::fromStdString(filename) );
    // Конструируем имя файла проекта
    std::string projectFileName(filename);
    projectFileName += "project";
    std::ifstream projectFile(projectFileName);
    // хранит число фреймов
    int countOfFrames = 0;
    // считали число из файла
    projectFile >> countOfFrames;
    // закрыли файл
    projectFile.close();
      // пройдемся по номерам
    for (auto i=0;i<countOfFrames;i++){
         // добавили список
         QFrame* currentFrame = addListWhileLoading();
          // конструируем имя файла со списком заметок
        std::string frameFileName(filename);
        frameFileName += std::to_string(i);
         // открываем файл со списком заметок
        std::ifstream* frameStream = new std::ifstream(frameFileName);
        // считываем заметки в фрейм
        readFrameFromFile(frameStream, currentFrame);
        frameStream->close();     // закрыли файл
    }
}

  // Сохраняет проект в файл
void AppMainForm::saveProject(const std::string &filename){
    log_it( QString("Сохраняем проект с именем ") );
    log_it(  QString::fromStdString(filename) );
        // Номер фрейма
        int x = 0;
       // Для каждого фрейма
        for (const auto& frame : frames) {
            std::string frameFileName(filename);
            frameFileName += std::to_string(x);     // Конструируем имя файла для списка заметок
              // Создаем файловый поток вывода
            std::ofstream* outFile = new std::ofstream(frameFileName, std::ios::out);
              saveFrameToFile(outFile, frame);    // Сохраняем фрейм
              outFile->close();    //  Закрываем после записи
              x ++;   //  Увеличили счетчик фреймов
        };
        log_it(  QString( "Записано фреймов:") );
        log_it( std::to_string(x));
        std::string projectFileName(filename);
        projectFileName += "project";  // Конструируем имя файла проекта в целом
        std::ofstream outProject(projectFileName, std::ios::out);
        outProject  << x << std::endl;  //  Записываем число фреймов
        outProject.close();  //  Закрываем после записи
}

// Сохранить фрейм с заметками в файл
void AppMainForm::saveFrameToFile(  std::ofstream* outFile, QFrame* currentFrame ){

        // получили список заметок
        const auto& notes = here[currentFrame];
        for (const auto& note : notes){ // по всем заметкам
            log_it(QString("Добавляется заметка с текстом"));
            log_it( note->toPlainText() );
            try {
            std::string s = note->toPlainText().toStdString();
            *outFile << s << std::endl; // записать в файл
            }
            catch (...) {
                log_it(QString("Не могу записать в файл!"));
            }
    }
}

// Считать фрейм с заметками из файла
void AppMainForm::readFrameFromFile( std::ifstream* inFile, QFrame* currentFrame ){

    std::string line; // очередная строка-буфер
    // если входной поток открыт
    try {
        // считываем построчно
        while (std::getline(*inFile, line)) {
            // добавляем заметку в фрейм
            addNoteToFrameWithText(line, currentFrame);
        }
        inFile->close(); // Закрыли список заметок
    } catch (...) {
        log_it( QString( "Не могу прочитать список заметок" ));
    }
}
    // Добавляет заметку с указанным текстом к фрейму
void AppMainForm::addNoteToFrameWithText(const std::string& note, QFrame* currentFrame){

    log_it( QString("Добавляем заметку к фрейму"));
    log_it( QString("Текст заметки"));
    log_it( note);

    // Текст заметки согласно файла
    QString NEW_NOTE_TEXT = QString::fromStdString(note);

    // Добавили виджет
    log_it( QString("Выделяем память под заметку"));
    QTextEdit* te = new QTextEdit();
    // Виджет в список
    log_it( QString("Пытаемся добавить виджет в список всех заметок"));
    notes.push_back(te);
     log_it( QString("Пытаемся связать заметку со списком заметок"));
    // Связали фрейм и виджет
     if (currentFrame != nullptr) {
         here[currentFrame].push_back(te);
         log_it( QString("Добавлена связь между заметкой и списком"));
     } else {
         log_it( QString("Указатель на текущий фрейм обнулен!"));
     }

    // меняем высоту текущего фрейма
    currentFrame->setFixedHeight((here[currentFrame].size()+1)*SIZE_NOTE_V);
    // если не первая заметка
    te->setGeometry(0, here[currentFrame].size()*SIZE_NOTE_V, SIZE_NOTE_W, SIZE_NOTE_V);
     log_it( QString("Подстроили геометрию"));

    te->setText(NEW_NOTE_TEXT);
    log_it( QString("Добавили текст к заметке"));
    te->setParent(currentFrame);
    log_it( QString("Прослушивание событий выполняется"));
    te->show();
    log_it( QString("Отобразили заметку на экране"));
}
    //  Размещает в интерфейсе новый фрейм
QFrame*  AppMainForm::addListWhileLoading(){

    log_it( QString("Помещаем в интерфейс новый фрейм при загрузке"));

    // Создаем новый фрейм и сохраняем на него указатель
    QFrame* currentFrame = new QFrame();
    log_it( QString("Новый фрейм был создан"));
    // Помещаем в список фреймов новый фрейм
    log_it( QString("Добавляю фрейм в список фреймов"));
    frames.push_back(currentFrame);
    log_it( QString("Фрейм был добавлен"));
    // Берем ссылку на этот фрейм
    log_it( QString("Получил ссылку на последний фрейм в списке"));
    lastFrame = frames.at(frames.size()-1);
    log_it( QString("Настройка геометрии фрейма"));
    lastFrame->setGeometry(SIZE_FRAME_W * frames.size(), 40, SIZE_FRAME_W, SIZE_FRAME_V);
    // Меняем цвет фрейма
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::blue);
    lastFrame->setPalette(palette);
    lastFrame->setAutoFillBackground(true);
    log_it( QString("Поменяли цвет нового фрейма"));
    // Добавили к интерфейсу
    log_it( QString("Подключаю к циклу событий"));
    lastFrame->setParent(this);
     log_it( QString("Есть прослушивание"));
    // Показали фрейм
    lastFrame->show();
    log_it( QString("Фрейм показан на экране"));
    return currentFrame;
}

void AppMainForm::on_actAddList_triggered()
{
    // Добавляет новый фрейм
    addList();
}


void AppMainForm::on_actAddNote_triggered()
{
    // Добавляет в текущий фрейм очередную заметку
    if (frames.size() > 0) addNote();
}

 // Открывает файл проекта
void AppMainForm::on_actOpenFromFile_triggered()
{
    // Получить полное имя файла
    QString projectFileName = QFileDialog::getOpenFileName(this,
                                                       tr("Открыть файл"),
                                                       "/home", tr("Проект Канбан (*.project)"));
    log_it(QString("Выбран файл (открытие файла)"));
    log_it(projectFileName);
    // Если пустое сразу возврат
    if (projectFileName.isEmpty()) {
        log_it( QString("Пустое имя файла! Прерываем..."));
        return;
    }
    // Получаем сведения о файле
    QFileInfo fileInfo(projectFileName);
    // Получаем путь до файла без расширения
    std::string dirOfFile = fileInfo.absoluteDir().absolutePath().toStdString();
    std::string stdFileName = fileInfo.baseName().toStdString();
    std::string usedNameWithoutExt = dirOfFile + '/' + stdFileName + '.';
    // Открываем проект
    openProject(usedNameWithoutExt);
}


void AppMainForm::on_actSaveToFile_triggered()
{

    // Получить полное имя файла
    QString projectFileName = QFileDialog::getSaveFileName(this,
                                                           tr("Сохранить файл"),
                                                           "/home", tr("Проект канбан (*.project)"));
    log_it(QString("Выбран файл (сохранение файла)"));
    log_it(projectFileName);

    // Если пустое сразу возврат
    if (projectFileName.isEmpty()) {
        log_it( QString("Пустое имя файла! Прерываем..."));
        return ;
    }
    // Получаем сведения о файле
    QFileInfo fileInfo(projectFileName);
    // Получаем путь до файла без расширения
    std::string dirOfFile = fileInfo.absoluteDir().absolutePath().toStdString();
    std::string stdFileName = fileInfo.baseName().toStdString();
    std::string usedNameWithoutExt = dirOfFile + '/' + stdFileName + '.';
    //  Сохраняем проект
    saveProject(usedNameWithoutExt);
}

void AppMainForm::log_it(const QString message){
    std::ofstream outputLog("/home/artem/00_test/test.log", std::ios::app);
    outputLog << message.toStdString() << std::endl;
    outputLog.close();
}
void AppMainForm::log_it(const std::string message){
    std::ofstream outputLog("/home/artem/00_test/test.log", std::ios::app);
    outputLog << message << std::endl;
    outputLog.close();
}
// Очищает динамическук память
void AppMainForm::clearMemState() {
    // Для каждого указателя в списке
    for (auto & note : notes){
        note->hide(); // Скрыли заметку
        note->setParent(nullptr); // Вытащили из цикла событий
        delete note; // Удаление элемента из оперативки
    }
   notes.clear();  //  Удаляем список указателей
    for (auto & frame : frames){
        frame->hide(); // Скрыли фрейм
        frame->setParent(nullptr); // Вытащили из цикла событий
        delete frame; // Удаление элемента из оперативки
    }
     //  Чистим список фреймов
    frames.clear();
     //  Чистим связи
    here.clear();
     //  Зануляем указатель на фрейм
    lastFrame = nullptr;
}
// Настройка памяти под новый проект
void AppMainForm::initMemState(){
     clearMemState(); // просто почистим память
}
// Действие для очистки памяти
void AppMainForm::on_actClearMemory_triggered()
{
    initMemState();
}

