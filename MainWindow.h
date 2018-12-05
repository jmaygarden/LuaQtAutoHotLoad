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

  QFileSystemWatcher * watcher;
  QLabel * label;
  sol::state * lua;
};
