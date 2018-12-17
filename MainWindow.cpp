#include "MainWindow.h"

#include "LuaReplWidget.h"

#include "sol.hpp"

#include <QAction>
#include <QDebug>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QLabel>
#include <QVBoxLayout>

namespace
{
  sol::state * create_interpreter()
  {
    auto lua = new sol::state;

    lua->open_libraries();

    return lua;
  }
}

MainWindow::MainWindow()
  : SettingsWindow(QStringLiteral("LuaQtHotLoad"))
  , lua(create_interpreter())
  , watcher(new QFileSystemWatcher(this))
  , label(new QLabel(this))
  , replWidget(new LuaReplWidget(*lua, this))
{
  auto widget = new QWidget;
  auto layout = new QVBoxLayout;

  layout->addWidget(label);
  layout->addWidget(replWidget);
  widget->setLayout(layout);
  setCentralWidget(widget);

  auto fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction("&Open", this, &MainWindow::onOpen);
  fileMenu->addAction("&Run", this, &MainWindow::onRunAll);

  connect(watcher, &QFileSystemWatcher::directoryChanged,
    this, &MainWindow::onDirectoryChange);
  connect(watcher, &QFileSystemWatcher::fileChanged, this, &MainWindow::run);

  lua->set_function("set_label", [this](std::string const & text)
  {
    label->setText(QString::fromStdString(text));
  });

  lua->set_function("alert", [this](std::string const & text)
  {
    QMessageBox::information(this, "Alert!", QString::fromStdString(text));
  });

  loadWindowGeometry();
}

MainWindow::~MainWindow()
{
  delete lua;
}

void MainWindow::onDirectoryChange(QString const & path)
{
  QDir dir(path);

  dir.setNameFilters(QStringList() << "*.lua");

  for(auto filename : dir.entryList())
  {
    auto absolutePath = dir.filePath(filename);

    if(!watcher->files().contains(absolutePath))
    {
      watcher->addPath(absolutePath);
    }
  }
}

void MainWindow::onOpen()
{
  auto const caption = QStringLiteral("Select directory to watch.");
  auto path = QFileDialog::getExistingDirectory(this, caption);

  onDirectoryChange(path);
  watcher->addPath(path);
}

void MainWindow::onRunAll()
{
  for(auto path : watcher->files())
  {
    run(path);
  }
}

void MainWindow::run(QString const & path)
{
  qDebug() << "Run: " << path;
  auto result = lua->do_file(path.toStdString());
  
  if(!result.valid())
  {
    sol::error err = result;
    std::string what = err.what();

    label->setText(QString::fromStdString(what));
  }
}
