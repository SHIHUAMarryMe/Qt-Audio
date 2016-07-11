#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

class QAction;
class QMenu;
class QMenuBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    std::shared_ptr<QAction> aboutAction;
    std::shared_ptr<QMenu> aboutMenu;
    std::shared_ptr<QMenuBar> menuBar;

    void addActionsToWindow()noexcept;
    void addMenusToWindow()noexcept;
    void addTabWidget()noexcept;

private slots:
    void showAbout()noexcept;

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow()=default;
};

#endif // MAINWINDOW_H
