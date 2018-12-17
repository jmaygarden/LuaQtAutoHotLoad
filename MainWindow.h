#pragma once

#include "SettingsWindow.h"

class QFileSystemWatcher;
class QLabel;

namespace sol
{
  class state;
}

class MainWindow : public SettingsWindow
{
public:
  MainWindow();
  ~MainWindow();

private:
  void onDirectoryChange(QString const & path);
  void onFileChange(QString const & path);
  void onOpen();
  void onRunAll();

  void run(QString const & path);

  sol::state * lua;
  QFileSystemWatcher * watcher;
  QLabel * label;
  QWidget * replWidget;
};
